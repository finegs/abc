use crate::pubsub::broker::PubSubBroker;
use crate::pubsub::protocol::TransportMode;
use crate::pubsub::routing_rules::{RuleAction, RuleEngine, RoutingRule};
use axum::{
    extract::{
        ws::{Message as WsMessage, WebSocket, WebSocketUpgrade},
        Path, State,
    },
    http::{HeaderValue, StatusCode},
    response::{Html, IntoResponse, Json},
    routing::{delete, get, post},
    Router,
};
use serde::{Deserialize, Serialize};
use std::net::SocketAddr;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::Arc;
use tower_http::cors::CorsLayer;

/// Shared application state for Web Manager REST & WebSocket API.
pub struct WebManagerState {
    pub broker: Arc<PubSubBroker>,
    pub rule_engine: RuleEngine,
    pub message_counter: Arc<AtomicU64>,
}

#[derive(Serialize)]
pub struct ServerStatusResponse {
    pub node_id: String,
    pub active_rules_count: usize,
    pub total_messages_processed: u64,
}

#[derive(Deserialize)]
pub struct CreateRuleRequest {
    pub name: String,
    pub source_topic_pattern: String,
    pub target_topic: Option<String>,
    pub target_transport: TransportMode,
    pub priority: u32,
}

#[derive(Deserialize)]
pub struct PublishMessageRequest {
    pub topic: String,
    pub payload: String,
    pub transport_mode: TransportMode,
}

/// WebSocket payload sent to client for real-time updates.
#[derive(Serialize)]
pub struct RealtimeWsUpdate {
    pub stats: crate::pubsub::stats::SystemStatsSummary,
    pub rules: Vec<RoutingRule>,
}

/// Start the Rust Web Manager HTTP REST & WebSocket API and UI dashboard.
pub async fn start_web_manager(
    addr: SocketAddr,
    broker: Arc<PubSubBroker>,
    rule_engine: RuleEngine,
    message_counter: Arc<AtomicU64>,
) -> Result<(), Box<dyn std::error::Error>> {
    let state = Arc::new(WebManagerState {
        broker,
        rule_engine,
        message_counter,
    });

    let cors = CorsLayer::new()
        .allow_origin("*".parse::<HeaderValue>().unwrap())
        .allow_headers(tower_http::cors::Any)
        .allow_methods(tower_http::cors::Any);

    let app = Router::new()
        .route("/", get(serve_dashboard_html))
        .route("/ws", get(ws_handler))
        .route("/api/status", get(get_status))
        .route("/api/stats", get(get_stats))
        .route("/api/rules", get(get_rules).post(create_rule))
        .route("/api/rules/:id", delete(delete_rule))
        .route("/api/rules/:id/toggle", post(toggle_rule))
        .route("/api/publish", post(publish_message))
        .layer(cors)
        .with_state(state);

    let listener = tokio::net::TcpListener::bind(addr).await?;
    tokio::spawn(async move {
        axum::serve(listener, app).await.unwrap();
    });

    Ok(())
}

async fn ws_handler(
    ws: WebSocketUpgrade,
    State(state): State<Arc<WebManagerState>>,
) -> impl IntoResponse {
    ws.on_upgrade(move |socket| handle_websocket(socket, state))
}

async fn handle_websocket(mut socket: WebSocket, state: Arc<WebManagerState>) {
    let mut interval = tokio::time::interval(tokio::time::Duration::from_millis(500));

    loop {
        tokio::select! {
            _ = interval.tick() => {
                let stats = state.broker.get_system_stats().await;
                let rules = state.rule_engine.get_rules().await;

                let update = RealtimeWsUpdate { stats, rules };
                if let Ok(json_str) = serde_json::to_string(&update) {
                    if socket.send(WsMessage::Text(json_str)).await.is_err() {
                        break; // Client disconnected
                    }
                }
            }
            msg = socket.recv() => {
                match msg {
                    Some(Ok(WsMessage::Text(_text))) => {
                        // Received command from frontend WS client
                    }
                    Some(Ok(WsMessage::Close(_))) | None => break,
                    _ => {}
                }
            }
        }
    }
}

async fn get_status(State(state): State<Arc<WebManagerState>>) -> Json<ServerStatusResponse> {
    let rules = state.rule_engine.get_rules().await;
    Json(ServerStatusResponse {
        node_id: state.broker.node_id().to_string(),
        active_rules_count: rules.len(),
        total_messages_processed: state.message_counter.load(Ordering::Relaxed),
    })
}

async fn get_stats(
    State(state): State<Arc<WebManagerState>>,
) -> Json<crate::pubsub::stats::SystemStatsSummary> {
    let stats = state.broker.get_system_stats().await;
    Json(stats)
}

async fn get_rules(State(state): State<Arc<WebManagerState>>) -> Json<Vec<RoutingRule>> {
    let rules = state.rule_engine.get_rules().await;
    Json(rules)
}

async fn create_rule(
    State(state): State<Arc<WebManagerState>>,
    Json(req): Json<CreateRuleRequest>,
) -> impl IntoResponse {
    let id = format!(
        "rule-{}",
        std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .map(|d| d.as_millis())
            .unwrap_or(0)
    );

    let action = if let Some(target_topic) = req.target_topic {
        if !target_topic.trim().is_empty() {
            RuleAction::TransformTopic {
                new_topic: target_topic,
            }
        } else {
            RuleAction::Forward
        }
    } else {
        RuleAction::Forward
    };

    let rule = RoutingRule {
        id,
        name: req.name,
        source_topic_pattern: req.source_topic_pattern,
        action,
        target_transport: req.target_transport,
        enabled: true,
        priority: req.priority,
    };

    state.rule_engine.add_rule(rule.clone()).await;
    (StatusCode::CREATED, Json(rule))
}

async fn delete_rule(
    Path(id): Path<String>,
    State(state): State<Arc<WebManagerState>>,
) -> impl IntoResponse {
    if state.rule_engine.remove_rule(&id).await {
        StatusCode::NO_CONTENT
    } else {
        StatusCode::NOT_FOUND
    }
}

async fn toggle_rule(
    Path(id): Path<String>,
    State(state): State<Arc<WebManagerState>>,
) -> impl IntoResponse {
    if let Some(enabled) = state.rule_engine.toggle_rule(&id).await {
        (
            StatusCode::OK,
            Json(serde_json::json!({ "id": id, "enabled": enabled })),
        )
    } else {
        (
            StatusCode::NOT_FOUND,
            Json(serde_json::json!({ "error": "Rule not found" })),
        )
    }
}

async fn publish_message(
    State(state): State<Arc<WebManagerState>>,
    Json(req): Json<PublishMessageRequest>,
) -> impl IntoResponse {
    match state
        .broker
        .publish(req.topic, req.payload.into_bytes(), req.transport_mode)
        .await
    {
        Ok(msg_id) => (
            StatusCode::OK,
            Json(serde_json::json!({ "status": "published", "message_id": msg_id })),
        ),
        Err(e) => (
            StatusCode::INTERNAL_SERVER_ERROR,
            Json(serde_json::json!({ "error": e.to_string() })),
        ),
    }
}

/// Serves the single-page HTML / TS dashboard.
async fn serve_dashboard_html() -> Html<&'static str> {
    Html(include_str!("../../web/index.html"))
}
