use std::sync::Arc;
use sqlx::SqlitePool;

use super::repository::{NoteRepository, SqliteNoteRepository};

/// 🔹 4. FACTORY PATTERN
/// Factory to instantiate repositories without exposing creation logic.
pub struct RepositoryFactory;

impl RepositoryFactory {
    pub fn create_sqlite_repository(pool: SqlitePool) -> Arc<dyn NoteRepository> {
        Arc::new(SqliteNoteRepository::new(pool))
    }
}
