#![allow(dead_code)]

use axum::{Extension, Json};
use sqlx::SqlitePool;
use hyper::StatusCode;

pub(crate) mod book_models;

use book_models::{ Book, CreateBook };

async fn create_book(
    Extension(pool): Extension<SqlitePool>,
    Json(payload): Json<CreateBook>,
) -> Result<(StatusCode, Json<Book>), (StatusCode, String)> {
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
    Extension(pool): Extension<SqlitePool>,
) -> Result<Json<Vec<Book>>, (StatusCode, String)> {
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

#[tokio::test]
async fn test_book_handler_flow() {
    let pool = sqlx::sqlite::SqlitePool::connect(":memory:").await.unwrap();
    sqlx::query(
        "CREATE TABLE books (id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT NOT NULL, author TEXT NOT NULL);"
    )
    .execute(&pool)
    .await
    .unwrap();

    let create_req = CreateBook {
        title: "Rust Programming".to_string(),
        author: "Steve Klabnik".to_string(),
    };

    let (status, Json(book)) = create_book(Extension(pool.clone()), Json(create_req)).await.unwrap();
    assert_eq!(status, StatusCode::CREATED);
    assert_eq!(book.title, "Rust Programming");
    assert_eq!(book.author, "Steve Klabnik");

    let Json(books) = get_books(Extension(pool.clone())).await.unwrap();
    assert_eq!(books.len(), 1);
    assert_eq!(books[0].title, "Rust Programming");
}
