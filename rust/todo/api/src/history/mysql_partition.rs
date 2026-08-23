use std::sync::{Arc, Mutex};
use chrono::Utc;

use super::{
    model::{HistoryEvent, HistoryQueryFilter, PartitionConfig, PartitionStat},
    state::{PartitionGranularity, PartitionState},
};

#[derive(Clone)]
pub struct MySqlPartitionStrategy {
    #[allow(unused)]
    db_url: String,
    config: PartitionConfig,
    state: Arc<Mutex<PartitionState>>,
    active_row_count: Arc<Mutex<u64>>,
}

impl MySqlPartitionStrategy {
    pub async fn new(db_url: &str, config: PartitionConfig) -> Self {
        Self {
            db_url: db_url.to_string(),
            config,
            state: Arc::new(Mutex::new(PartitionState::default())),
            active_row_count: Arc::new(Mutex::new(0)),
        }
    }

    pub async fn record_event(&self, _event: &HistoryEvent) -> Result<(), String> {
        let mut count = self.active_row_count.lock().unwrap();
        *count += 1;
        let current_count = *count;

        let mut state = self.state.lock().unwrap();
        if state.evaluate_escalation(current_count, self.config.max_rows_per_partition) {
            *count = 0;
        }

        Ok(())
    }

    pub async fn query_events(&self, _filter: &HistoryQueryFilter) -> Result<Vec<HistoryEvent>, String> {
        Ok(Vec::new())
    }

    pub async fn get_partition_stats(&self) -> Result<Vec<PartitionStat>, String> {
        let state = self.state.lock().unwrap().current_granularity;
        Ok(vec![PartitionStat {
            partition_key: format!("mysql_range_{}", Utc::now().format("%Y_%m")),
            granularity: state.as_str().to_string(),
            file_path: "mysql://history_events".to_string(),
            row_count: *self.active_row_count.lock().unwrap(),
        }])
    }

    pub fn get_current_granularity(&self) -> PartitionGranularity {
        self.state.lock().unwrap().current_granularity
    }
}
