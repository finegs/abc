use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, Serialize, Deserialize, sqlx::FromRow)]
pub struct HistoryEvent {
    pub id: String,
    pub event_type: String,
    pub entity_type: String,
    pub entity_id: String,
    pub payload: Option<String>,
    pub created_at: String,
}

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
pub struct HistoryQueryFilter {
    pub entity_type: Option<String>,
    pub entity_id: Option<String>,
    pub event_type: Option<String>,
    pub from_date: Option<String>,
    pub to_date: Option<String>,
    pub page: Option<usize>,
    pub limit: Option<usize>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PartitionStat {
    pub partition_key: String,
    pub granularity: String,
    pub file_path: String,
    pub row_count: u64,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PartitionConfig {
    pub max_rows_per_partition: u64,
}

impl Default for PartitionConfig {
    fn default() -> Self {
        Self {
            max_rows_per_partition: 1_000_000,
        }
    }
}
