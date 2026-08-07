use std::sync::Arc;
use axum::{
    extract::{Path, Query, State},
    http::StatusCode,
    response::IntoResponse,
    Json,
};
use sqlx::sqlite::SqlitePool;

// Include main src modules for integration testing
#[path = "../src/appstate.rs"]
mod appstate;
#[path = "../src/handler.rs"]
mod handler;
#[path = "../src/model.rs"]
mod model;
#[path = "../src/patterns/mod.rs"]
mod patterns;
#[path = "../src/schema.rs"]
mod schema;

use appstate::AppState;
use handler::{
    create_note_handler, delete_note_handler, edit_note_handler, get_note_handler,
    health_check_handler, note_list_handle,
};
use schema::{CreateNoteSchema, FilterOptions, UpdateNoteSchema};

async fn setup_test_db() -> SqlitePool {
    let pool = SqlitePool::connect(":memory:").await.unwrap();
    sqlx::query(
        r#"
        CREATE TABLE IF NOT EXISTS notes (
            id CHAR(36) PRIMARY KEY NOT NULL,
            title VARCHAR(255) NOT NULL UNIQUE,
            content TEXT NOT NULL,
            is_published BOOLEAN NOT NULL DEFAULT FALSE,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
        "#,
    )
    .execute(&pool)
    .await
    .unwrap();

    pool
}

#[tokio::test]
async fn test_healthcheck() {
    let response = health_check_handler().await.into_response();
    assert_eq!(response.status(), StatusCode::OK);
}

#[tokio::test]
async fn test_note_crud_lifecycle() {
    let pool = setup_test_db().await;
    let state = Arc::new(AppState::new(pool));

    // 1. Create Note
    let create_body = CreateNoteSchema {
        title: "Integration Test Note".to_string(),
        content: "Testing full CRUD flow".to_string(),
        is_published: Some(true),
    };
    let _res = create_note_handler(State(state.clone()), Json(create_body))
        .await
        .expect("Create note failed");

    // 2. List Notes
    let _list_res = note_list_handle(Query(FilterOptions::default()), State(state.clone()))
        .await
        .expect("List notes failed");

    // 3. Get Note List via DB directly to find ID
    let notes = sqlx::query_as::<_, model::NoteModel>("SELECT * FROM notes")
        .fetch_all(&state.conn_pool)
        .await
        .unwrap();
    assert_eq!(notes.len(), 1);
    let note_id = notes[0].id.clone();
    assert_eq!(notes[0].title, "Integration Test Note");

    // 4. Get Note by ID
    let _get_res = get_note_handler(Path(note_id.clone()), State(state.clone()))
        .await
        .expect("Get note failed");

    // 5. Update Note
    let update_body = UpdateNoteSchema {
        title: Some("Updated Note Title".to_string()),
        content: Some("Updated Content".to_string()),
        is_published: Some(false),
    };
    let _edit_res = edit_note_handler(
        Path(note_id.clone()),
        State(state.clone()),
        Json(update_body),
    )
    .await
    .expect("Edit note failed");

    // Verify DB update
    let updated_note = sqlx::query_as::<_, model::NoteModel>("SELECT * FROM notes WHERE id = $1")
        .bind(&note_id)
        .fetch_one(&state.conn_pool)
        .await
        .unwrap();
    assert_eq!(updated_note.title, "Updated Note Title");
    assert_eq!(updated_note.content, "Updated Content");

    // 6. Delete Note
    let _delete_res = delete_note_handler(Path(note_id.clone()), State(state.clone()))
        .await
        .expect("Delete note failed");

    // 7. Verify Note deleted (Get should return 404)
    let get_deleted = get_note_handler(Path(note_id.clone()), State(state.clone())).await;
    if let Err((err_status, _)) = get_deleted {
        assert_eq!(err_status, StatusCode::NOT_FOUND);
    } else {
        panic!("Expected 404 NOT_FOUND error, got Ok");
    }
}
