use std::sync::Arc;
use tokio::sync::mpsc;

use super::{factory::HistoryEngineBackend, model::HistoryEvent};

pub struct AuditEventPublisher {
    sender: mpsc::UnboundedSender<HistoryEvent>,
}

impl AuditEventPublisher {
    pub fn new(backend: Arc<HistoryEngineBackend>) -> Self {
        let (sender, mut receiver) = mpsc::unbounded_channel::<HistoryEvent>();

        tokio::spawn(async move {
            while let Some(event) = receiver.recv().await {
                if let Err(e) = backend.record_event(&event).await {
                    eprintln!("❌ Failed to store audit history event: {}", e);
                }
            }
        });

        Self { sender }
    }

    pub fn publish(&self, event: HistoryEvent) {
        let _ = self.sender.send(event);
    }
}
