use std::sync::Arc;
use axum::{
    http::{HeaderMap, HeaderValue, StatusCode},
    Json,
};
use sqlx::sqlite::SqlitePool;

use todo_api::{
    appstate::AppState,
    history::{
        UpdateRetentionConfigSchema, UserRole,
    },
    history_handler::{
        history_retention_get_handler, history_retention_put_handler,
    },
};

#[test]
fn test_user_role_admin_privilege() {
    let admin_role = UserRole::from_header(Some("admin"));
    let user_role = UserRole::from_header(Some("user"));
    let none_role = UserRole::from_header(None);

    assert!(admin_role.is_admin());
    assert!(!user_role.is_admin());
    assert!(!none_role.is_admin());
}

#[tokio::test]
async fn test_admin_retention_configuration_flow() {
    let pool = SqlitePool::connect(":memory:").await.unwrap();
    let state = Arc::new(AppState::new(pool));

    // 1. Non-Admin User attempt -> Should return 403 Forbidden
    let non_admin_headers = HeaderMap::new();
    let update_body = UpdateRetentionConfigSchema {
        history_retention_days: Some(730),
        notes_retention_days: None,
        stats_retention_days: None,
        auto_purge_enabled: None,
    };

    let non_admin_res = history_retention_put_handler(
        axum::extract::State(state.clone()),
        non_admin_headers,
        Json(update_body.clone()),
    )
    .await;

    assert!(non_admin_res.is_err());
    let (status, err_json) = non_admin_res.err().unwrap();
    assert_eq!(status, StatusCode::FORBIDDEN);
    assert!(err_json.0.to_string().contains("Admin privilege required"));

    // 2. Admin user request -> Should return 200 OK & update retention config to 730 days
    let mut admin_headers = HeaderMap::new();
    admin_headers.insert("x-user-role", HeaderValue::from_static("admin"));

    let admin_res = history_retention_put_handler(
        axum::extract::State(state.clone()),
        admin_headers,
        Json(update_body),
    )
    .await;

    assert!(admin_res.is_ok());
    let Json(ok_json) = admin_res.unwrap();
    assert!(ok_json.to_string().contains("730"));

    // 3. Verify GET settings reflects updated 730 days config
    let Json(get_val) = history_retention_get_handler(axum::extract::State(state.clone())).await;
    assert!(get_val.to_string().contains("730"));
}
