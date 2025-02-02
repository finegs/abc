use serde::{Serialize, Deserialize};
use sqlx::prelude::FromRow;

#[derive(Debug, Serialize, Deserialize, FromRow)]
pub(crate) struct Book {
    id: i32,
    title: String,
    author: String,
}

#[derive(Debug, Deserialize)]
pub(crate) struct CreateBook {
    title: String,
    author: String,
}
