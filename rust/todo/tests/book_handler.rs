use axum::{response::IntoResponse, Extension, Json};
use hyper::StatusCode;
use sqlx::SqlitePool;

pub(crate) mod book_models;

use book_models::{Book, CreateBook};

#[allow(unused)]
async fn create_book(
    Extension(pool): Extension<SqlitePool>,
    Json(payload): Json<CreateBook>,
) -> Result<(StatusCode, Json<Book>), (StatusCode, String)> {
    let rec = sqlx::query_as::<_, Book>(
        "INSERT INTO books (title, author) VALUES (?, ?) RETURNING id, title, author",
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

#[allow(unused)]
async fn get_books(
    Extension(pool): Extension<SqlitePool>,
) -> Result<Json<Vec<Book>>, (StatusCode, String)> {
    let recs = sqlx::query_as::<_, Book>("SELECT id, title, author FROM books")
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
