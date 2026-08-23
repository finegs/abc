use std::sync::Arc;

use axum::{
    extract::{Query, State},
    http::StatusCode,
    response::IntoResponse,
    Json,
};
use serde_json::json;

use crate::{history::HistoryQueryFilter, AppState};

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
