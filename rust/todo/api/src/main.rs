mod appstate;
mod db;
mod handler;
mod history;
mod history_handler;
#[allow(unused)]
mod model;
mod patterns;
mod schema;

use std::sync::Arc;

use axum::{
    response::IntoResponse,
    routing::get,
    Json, Router,
};
use tokio::net::TcpListener;

use dotenv::dotenv;

use appstate::AppState;
use db::create_conn_pool;
use history::{HistoryFacade, HistoryStoreFactory, PartitionConfig};

#[tokio::main]
async fn main() {
    dotenv().ok();

    println!("### Rest API Service ###");

    let database_url = std::env::var("DATABASE_URL").expect("DATABASE_URL must be set");

    let pool = create_conn_pool(database_url.as_str()).await;

    let history_backend_path = std::env::var("HISTORY_DB_PATH").unwrap_or_else(|_| "db/history".to_string());
    let history_backend = HistoryStoreFactory::create_backend(&history_backend_path, PartitionConfig::default()).await;
    let history_facade = Arc::new(HistoryFacade::new(history_backend));

    let mut state = AppState::new(pool);
    state.history_facade = Some(history_facade);

    let app = Router::new()
        .route("/api/healthcheck", get(health_check_handler))
        .route(
            "/api/notes",
            get(handler::note_list_handle).post(handler::create_note_handler),
        )
        .route(
            "/api/notes/:id",
            get(handler::get_note_handler)
                .patch(handler::edit_note_handler)
                .delete(handler::delete_note_handler),
        )
        .route("/api/history", get(history_handler::history_list_handler))
        .route("/api/history/stats", get(history_handler::history_stats_handler))
        .with_state(Arc::new(state));

    println!("✅ Server started successfully at 0.0.0.0:8080");

    let listener = TcpListener::bind("0.0.0.0:8080").await.unwrap();
    axum::serve(listener, app.into_make_service())
        .await
        .unwrap();
}

pub async fn health_check_handler() -> impl IntoResponse {
    const MESSAGE: &str = "API Services";

    let json_response = serde_json::json!({
        "status": "ok",
        "message": MESSAGE
    });

    Json(json_response)
}
