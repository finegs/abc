use serde::{Deserialize, Serialize};
use sqlx::prelude::FromRow;

#[derive(Debug, Serialize, Deserialize, FromRow)]
pub(crate) struct Book {
    pub id: i32,
    pub title: String,
    pub author: String,
}

#[derive(Debug, Deserialize)]
#[allow(dead_code)]
pub(crate) struct CreateBook {
    pub title: String,
    pub author: String,
}
