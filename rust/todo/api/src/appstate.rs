use std::sync::{Arc, RwLock};

use crate::history::{HistoryFacade, RetentionConfig};
use crate::patterns::{
    DefaultPaginationStrategy, NoteRepository, PaginationStrategy, RepositoryFactory,
};

#[cfg(feature = "sqlite")]
pub type DbPool = sqlx::SqlitePool;

#[cfg(all(feature = "mysql", not(feature = "sqlite")))]
pub type DbPool = sqlx::MySqlPool;

#[derive(Clone)]
pub struct AppState {
    pub conn_pool: DbPool,
    pub repository: Arc<dyn NoteRepository>,
    pub pagination_strategy: Arc<dyn PaginationStrategy>,
    pub history_facade: Option<Arc<HistoryFacade>>,
    pub retention_config: Arc<RwLock<RetentionConfig>>,
}

impl AppState {
    pub fn new(conn_pool: DbPool) -> Self {
        let repository = RepositoryFactory::create_sqlite_repository(conn_pool.clone());
        let pagination_strategy = Arc::new(DefaultPaginationStrategy::default());

        Self {
            conn_pool,
            repository,
            pagination_strategy,
            history_facade: None,
            retention_config: Arc::new(RwLock::new(RetentionConfig::default())),
        }
    }
}
