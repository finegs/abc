#[path = "../src/client.rs"]
mod client;
#[path = "../src/models.rs"]
mod models;

use axum::{routing::get, Json, Router};
use client::ApiClient;
use models::{NoteItem, NoteListResponse};
use std::net::SocketAddr;
use tokio::net::TcpListener;

async fn start_mock_api_server() -> String {
    let app = Router::new()
        .route(
            "/api/healthcheck",
            get(|| async { Json(serde_json::json!({"status": "ok", "message": "API Services"})) }),
        )
        .route(
            "/api/notes",
            get(|| async {
                Json(NoteListResponse {
                    status: "ok".to_string(),
                    count: 1,
                    notes: vec![NoteItem {
                        id: "mock-id-100".to_string(),
                        title: "Mock API Note".to_string(),
                        content: "Testing ApiClient integration".to_string(),
                        is_published: 1,
                        created_at: None,
                        updated_at: None,
                    }],
                })
            }),
        );

    let listener = TcpListener::bind("127.0.0.1:0").await.unwrap();
    let addr: SocketAddr = listener.local_addr().unwrap();
    tokio::spawn(async move {
        axum::serve(listener, app).await.unwrap();
    });

    format!("http://{}", addr)
}

#[tokio::test]
async fn test_api_client_healthcheck() {
    let server_url = start_mock_api_server().await;
    let client = ApiClient::new(&server_url);

    let healthy = client.healthcheck().await.expect("Healthcheck failed");
    assert!(healthy);
}

#[tokio::test]
async fn test_api_client_fetch_notes() {
    let server_url = start_mock_api_server().await;
    let client = ApiClient::new(&server_url);

    let notes = client.fetch_notes().await.expect("Fetch notes failed");
    assert_eq!(notes.len(), 1);
    assert_eq!(notes[0].id, "mock-id-100");
    assert_eq!(notes[0].title, "Mock API Note");
}
