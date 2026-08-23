use async_trait::async_trait;
use sqlx::SqlitePool;

use crate::model::NoteModel;
use crate::schema::{CreateNoteSchema, UpdateNoteSchema};

/// 🔹 1. REPOSITORY PATTERN
/// Abstracts persistence mechanisms behind a clean, testable trait interface.
#[async_trait]
pub trait NoteRepository: Send + Sync {
    async fn find_all(&self, limit: i32, offset: i32) -> Result<Vec<NoteModel>, sqlx::Error>;
    async fn find_by_id(&self, id: &str) -> Result<Option<NoteModel>, sqlx::Error>;
    async fn create(&self, id: &str, schema: &CreateNoteSchema) -> Result<NoteModel, sqlx::Error>;
    async fn update(
        &self,
        id: &str,
        schema: &UpdateNoteSchema,
    ) -> Result<Option<NoteModel>, sqlx::Error>;
    async fn delete(&self, id: &str) -> Result<bool, sqlx::Error>;
}

/// Concrete SQLite Repository implementation
#[derive(Clone)]
pub struct SqliteNoteRepository {
    pool: SqlitePool,
}

impl SqliteNoteRepository {
    pub fn new(pool: SqlitePool) -> Self {
        Self { pool }
    }
}

#[async_trait]
impl NoteRepository for SqliteNoteRepository {
    async fn find_all(&self, limit: i32, offset: i32) -> Result<Vec<NoteModel>, sqlx::Error> {
        sqlx::query_as::<_, NoteModel>("SELECT * FROM notes ORDER BY id LIMIT $1 OFFSET $2")
            .bind(limit)
            .bind(offset)
            .fetch_all(&self.pool)
            .await
    }

    async fn find_by_id(&self, id: &str) -> Result<Option<NoteModel>, sqlx::Error> {
        sqlx::query_as::<_, NoteModel>("SELECT * FROM notes WHERE id = $1")
            .bind(id)
            .fetch_optional(&self.pool)
            .await
    }

    async fn create(&self, id: &str, schema: &CreateNoteSchema) -> Result<NoteModel, sqlx::Error> {
        sqlx::query("INSERT INTO notes (id, title, content) VALUES ($1, $2, $3)")
            .bind(id)
            .bind(&schema.title)
            .bind(&schema.content)
            .execute(&self.pool)
            .await?;

        self.find_by_id(id)
            .await?
            .ok_or(sqlx::Error::RowNotFound)
    }

    async fn update(
        &self,
        id: &str,
        schema: &UpdateNoteSchema,
    ) -> Result<Option<NoteModel>, sqlx::Error> {
        let existing = match self.find_by_id(id).await? {
            Some(note) => note,
            None => return Ok(None),
        };

        let is_published = schema.is_published.unwrap_or(existing.is_published != 0);
        let i8_published = is_published as i8;

        let title = schema.title.as_deref().unwrap_or(&existing.title);
        let content = schema.content.as_deref().unwrap_or(&existing.content);

        let result = sqlx::query(
            "UPDATE notes SET title = $1, content = $2, is_published = $3 WHERE id = $4",
        )
        .bind(title)
        .bind(content)
        .bind(i8_published)
        .bind(id)
        .execute(&self.pool)
        .await?;

        if result.rows_affected() == 0 {
            return Ok(None);
        }

        self.find_by_id(id).await
    }

    async fn delete(&self, id: &str) -> Result<bool, sqlx::Error> {
        let result = sqlx::query("DELETE FROM notes WHERE id = $1")
            .bind(id)
            .execute(&self.pool)
            .await?;

        Ok(result.rows_affected() > 0)
    }
}
