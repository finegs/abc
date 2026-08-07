use std::sync::Arc;

#[cfg(feature = "sqlite")]
use sqlx::SqlitePool;

use crate::patterns::{
    DefaultPaginationStrategy, NoteRepository, PaginationStrategy, RepositoryFactory,
};

/// 🔹 5. SINGLETON / SHARED STATE PATTERN
/// Thread-safe shared application state injected into Axum router via Arc<AppState>.
#[derive(Clone)]
pub struct AppState {
    #[cfg(feature = "sqlite")]
    #[allow(dead_code)]
    pub conn_pool: SqlitePool,
    pub repository: Arc<dyn NoteRepository>,
    pub pagination_strategy: Arc<dyn PaginationStrategy>,
}

impl AppState {
    pub fn new(conn_pool: SqlitePool) -> Self {
        let repository = RepositoryFactory::create_sqlite_repository(conn_pool.clone());
        let pagination_strategy = Arc::new(DefaultPaginationStrategy::default());

        Self {
            conn_pool,
            repository,
            pagination_strategy,
        }
    }
}
