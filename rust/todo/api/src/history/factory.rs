use std::sync::Arc;

use super::{
    model::{HistoryEvent, HistoryQueryFilter, PartitionConfig, PartitionStat},
    state::PartitionGranularity,
};

#[cfg(feature = "sqlite")]
use super::sqlite_partition::SqlitePartitionStrategy;

#[cfg(feature = "mysql")]
use super::mysql_partition::MySqlPartitionStrategy;

#[derive(Clone)]
pub enum HistoryEngineBackend {
    #[cfg(feature = "sqlite")]
    Sqlite(SqlitePartitionStrategy),
    #[cfg(feature = "mysql")]
    MySql(MySqlPartitionStrategy),
}

impl HistoryEngineBackend {
    pub async fn record_event(&self, event: &HistoryEvent) -> Result<(), String> {
        match self {
            #[cfg(feature = "sqlite")]
            HistoryEngineBackend::Sqlite(s) => s.record_event(event).await,
            #[cfg(feature = "mysql")]
            HistoryEngineBackend::MySql(m) => m.record_event(event).await,
        }
    }

    pub async fn query_events(&self, filter: &HistoryQueryFilter) -> Result<Vec<HistoryEvent>, String> {
        match self {
            #[cfg(feature = "sqlite")]
            HistoryEngineBackend::Sqlite(s) => s.query_events(filter).await,
            #[cfg(feature = "mysql")]
            HistoryEngineBackend::MySql(m) => m.query_events(filter).await,
        }
    }

    pub async fn get_partition_stats(&self) -> Result<Vec<PartitionStat>, String> {
        match self {
            #[cfg(feature = "sqlite")]
            HistoryEngineBackend::Sqlite(s) => s.get_partition_stats().await,
            #[cfg(feature = "mysql")]
            HistoryEngineBackend::MySql(m) => m.get_partition_stats().await,
        }
    }

    pub fn get_current_granularity(&self) -> PartitionGranularity {
        match self {
            #[cfg(feature = "sqlite")]
            HistoryEngineBackend::Sqlite(s) => s.get_current_granularity(),
            #[cfg(feature = "mysql")]
            HistoryEngineBackend::MySql(m) => m.get_current_granularity(),
        }
    }
}

pub struct HistoryStoreFactory;

impl HistoryStoreFactory {
    pub async fn create_backend(base_path_or_url: &str, config: PartitionConfig) -> Arc<HistoryEngineBackend> {
        #[cfg(feature = "sqlite")]
        {
            let strategy = SqlitePartitionStrategy::new(base_path_or_url, config).await;
            Arc::new(HistoryEngineBackend::Sqlite(strategy))
        }

        #[cfg(all(feature = "mysql", not(feature = "sqlite")))]
        {
            let strategy = MySqlPartitionStrategy::new(base_path_or_url, config).await;
            Arc::new(HistoryEngineBackend::MySql(strategy))
        }
    }
}
