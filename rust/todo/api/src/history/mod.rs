pub mod diff_merge;
pub mod facade;
pub mod factory;
pub mod model;

#[cfg(feature = "sqlite")]
pub mod sqlite_partition;

#[cfg(feature = "mysql")]
pub mod mysql_partition;

pub mod observer;
pub mod retention;
pub mod state;

pub use diff_merge::{HistoryDiffMergeEngine, HistoryDiffRequest, HistoryMergeRequest, MergeResolutionStrategy};
pub use facade::HistoryFacade;
pub use factory::HistoryStoreFactory;
pub use model::{HistoryEvent, HistoryQueryFilter, PartitionConfig, PartitionStat};
pub use retention::{RetentionConfig, RetentionManager};
pub use state::PartitionGranularity;
