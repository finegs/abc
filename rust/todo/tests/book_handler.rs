use axum::{Extension, Json};
use axum::response::IntoResponse;
use sqlx::{Sqlite, SqlitePool};
use hyper::StatusCode;

pub(crate) mod book_models;

use book_models::{ Book, CreateBook };

async fn create_book(
    Extension(pool): Extension<SqlitePool>,
    Json(payload): Json<CreateBook>,
) -> Result<impl IntoResponse, impl IntoResponse> {
    let rec = sqlx::query_as::<_, Book>(
        "INSERT INTO books (title, author) VALUES ($1, $2) RETURNING id, title, author"
    )
    .bind(&payload.title)
    .bind(&payload.author)
    .fetch_one(&pool)
    .await
    .map_err(|e| {
        (
            StatusCode::INTERNAL_SERVER_ERROR,
            format!("데이터베이스 오류: {}", e),
        )
    })?;

    Ok((StatusCode::CREATED, Json(rec)))
}

async fn get_books(
    Extension(pool): Extension<PgPool>,
) -> Result<impl IntoResponse, impl IntoResponse> {
    let recs = sqlx::query_as::<_, Book>(
        "SELECT id, title, author FROM books"
    )
    .fetch_all(&pool)
    .await
    .map_err(|e| {
        (
            StatusCode::INTERNAL_SERVER_ERROR,
            format!("데이터베이스 오류: {}", e),
        )
    })?;

    Ok(Json(recs))
}
