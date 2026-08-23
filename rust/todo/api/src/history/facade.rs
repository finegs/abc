use std::sync::Arc;
use chrono::Utc;

use super::{
    factory::HistoryEngineBackend,
    model::{HistoryEvent, HistoryQueryFilter, PartitionStat},
    observer::AuditEventPublisher,
    state::PartitionGranularity,
};

#[derive(Clone)]
pub struct HistoryFacade {
    publisher: Arc<AuditEventPublisher>,
    backend: Arc<HistoryEngineBackend>,
}

impl HistoryFacade {
    pub fn new(backend: Arc<HistoryEngineBackend>) -> Self {
        let publisher = Arc::new(AuditEventPublisher::new(backend.clone()));
        Self { publisher, backend }
    }

    pub fn record_action(&self, event_type: &str, entity_type: &str, entity_id: &str, payload: Option<String>) {
        let event = HistoryEvent {
            id: uuid::Uuid::new_v4().to_string(),
            event_type: event_type.to_string(),
            entity_type: entity_type.to_string(),
            entity_id: entity_id.to_string(),
            payload,
            created_at: Utc::now().to_rfc3339(),
        };

        self.publisher.publish(event);
    }

    pub async fn query_trail(&self, filter: HistoryQueryFilter) -> Result<Vec<HistoryEvent>, String> {
        self.backend.query_events(&filter).await
    }

    pub async fn get_stats(&self) -> Result<Vec<PartitionStat>, String> {
        self.backend.get_partition_stats().await
    }

    pub fn current_granularity(&self) -> PartitionGranularity {
        self.backend.get_current_granularity()
    }
}
