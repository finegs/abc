use std::sync::Arc;

use axum::{
    extract::{Query, State},
    http::StatusCode,
    response::IntoResponse,
    Json,
};
use serde_json::json;

use crate::{
    history::{
        HistoryDiffMergeEngine, HistoryDiffRequest, HistoryMergeRequest, HistoryQueryFilter,
        MergeResolutionStrategy, RetentionConfig, RetentionManager,
    },
    AppState,
};

pub async fn history_list_handler(
    opts: Query<HistoryQueryFilter>,
    State(data): State<Arc<AppState>>,
) -> Result<impl IntoResponse, (StatusCode, Json<serde_json::Value>)> {
    let Query(filter) = opts;

    if let Some(ref facade) = data.history_facade {
        match facade.query_trail(filter).await {
            Ok(events) => Ok(Json(json!({
                "status": "ok",
                "count": events.len(),
                "events": events
            }))),
            Err(e) => Err((
                StatusCode::INTERNAL_SERVER_ERROR,
                Json(json!({
                    "status": "error",
                    "message": format!("Failed to query history: {}", e)
                })),
            )),
        }
    } else {
        Ok(Json(json!({
            "status": "ok",
            "count": 0,
            "events": []
        })))
    }
}

pub async fn history_stats_handler(
    State(data): State<Arc<AppState>>,
) -> Result<impl IntoResponse, (StatusCode, Json<serde_json::Value>)> {
    if let Some(ref facade) = data.history_facade {
        match facade.get_stats().await {
            Ok(stats) => {
                let active_granularity = facade.current_granularity();
                Ok(Json(json!({
                    "status": "ok",
                    "active_scale_level": active_granularity.as_str(),
                    "partitions_count": stats.len(),
                    "partitions": stats
                })))
            }
            Err(e) => Err((
                StatusCode::INTERNAL_SERVER_ERROR,
                Json(json!({
                    "status": "error",
                    "message": format!("Failed to get history stats: {}", e)
                })),
            )),
        }
    } else {
        Ok(Json(json!({
            "status": "ok",
            "active_scale_level": "None",
            "partitions_count": 0,
            "partitions": []
        })))
    }
}

pub async fn history_retention_get_handler() -> impl IntoResponse {
    let config = RetentionConfig::default();
    let manager = RetentionManager::new("db/history", config.clone());
    let summary = manager.get_summary();

    Json(json!({
        "status": "ok",
        "retention_config": config,
        "summary": summary
    }))
}

pub async fn history_retention_purge_handler() -> impl IntoResponse {
    let config = RetentionConfig::default();
    let manager = RetentionManager::new("db/history", config);
    let summary = manager.purge_expired_partitions();

    Json(json!({
        "status": "ok",
        "message": "Retention purge completed",
        "summary": summary
    }))
}

pub async fn history_diff_handler(
    State(data): State<Arc<AppState>>,
    Json(payload): Json<HistoryDiffRequest>,
) -> Result<impl IntoResponse, (StatusCode, Json<serde_json::Value>)> {
    let facade = data.history_facade.as_ref().ok_or_else(|| {
        (
            StatusCode::SERVICE_UNAVAILABLE,
            Json(json!({"status": "error", "message": "History engine not available"})),
        )
    })?;

    let filter1 = HistoryQueryFilter {
        limit: Some(100),
        ..Default::default()
    };
    let events = facade.query_trail(filter1).await.map_err(|e| {
        (
            StatusCode::INTERNAL_SERVER_ERROR,
            Json(json!({"status": "error", "message": format!("{:?}", e)})),
        )
    })?;

    let e1 = events.iter().find(|e| e.id == payload.event_id_1);
    let e2 = events.iter().find(|e| e.id == payload.event_id_2);

    match (e1, e2) {
        (Some(ev1), Some(ev2)) => {
            let diff = HistoryDiffMergeEngine::compute_diff(ev1, ev2);
            Ok(Json(json!({
                "status": "ok",
                "diff": diff
            })))
        }
        _ => Err((
            StatusCode::NOT_FOUND,
            Json(json!({"status": "fail", "message": "One or both history events not found"})),
        )),
    }
}

pub async fn history_merge_handler(
    State(data): State<Arc<AppState>>,
    Json(payload): Json<HistoryMergeRequest>,
) -> Result<impl IntoResponse, (StatusCode, Json<serde_json::Value>)> {
    let facade = data.history_facade.as_ref().ok_or_else(|| {
        (
            StatusCode::SERVICE_UNAVAILABLE,
            Json(json!({"status": "error", "message": "History engine not available"})),
        )
    })?;

    let filter = HistoryQueryFilter {
        limit: Some(100),
        ..Default::default()
    };
    let events = facade.query_trail(filter).await.map_err(|e| {
        (
            StatusCode::INTERNAL_SERVER_ERROR,
            Json(json!({"status": "error", "message": format!("{:?}", e)})),
        )
    })?;

    let e1 = events.iter().find(|e| e.id == payload.event_id_1);
    let e2 = events.iter().find(|e| e.id == payload.event_id_2);

    match (e1, e2) {
        (Some(ev1), Some(ev2)) => {
            let strategy = payload
                .strategy
                .unwrap_or(MergeResolutionStrategy::CombineText);
            let merge_res = HistoryDiffMergeEngine::merge_events(ev1, ev2, strategy);
            Ok(Json(json!({
                "status": "ok",
                "merge_result": merge_res
            })))
        }
        _ => Err((
            StatusCode::NOT_FOUND,
            Json(json!({"status": "fail", "message": "One or both history events not found"})),
        )),
    }
}
