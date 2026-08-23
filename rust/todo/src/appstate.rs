use std::sync::Arc;
use crate::history::HistoryFacade;

#[cfg(feature = "sqlite")]
pub type DbPool = sqlx::SqlitePool;

#[cfg(all(feature = "mysql", not(feature = "sqlite")))]
pub type DbPool = sqlx::MySqlPool;

#[derive(Clone)]
pub struct AppState {
    pub conn_pool: DbPool,
    pub history_facade: Arc<HistoryFacade>,
}
