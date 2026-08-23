use serde::Serialize;
use std::collections::HashMap;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::Arc;
use tokio::sync::RwLock;

/// Per-topic statistics entry.
#[derive(Debug, Clone, Serialize, Default)]
pub struct TopicStats {
    pub topic: String,
    pub sent_count: u64,
    pub recv_count: u64,
    pub total_bytes: u64,
    pub last_active_timestamp_ms: u64,
}

/// Dynamic metrics collector tracking throughput and connectivity.
#[derive(Clone, Default)]
pub struct MetricsCollector {
    pub sent_messages: Arc<AtomicU64>,
    pub recv_messages: Arc<AtomicU64>,
    pub sent_bytes: Arc<AtomicU64>,
    pub recv_bytes: Arc<AtomicU64>,
    topics: Arc<RwLock<HashMap<String, TopicStats>>>,
}

impl MetricsCollector {
    pub fn new() -> Self {
        Self::default()
    }

    /// Record a message send event.
    pub async fn record_send(&self, topic: &str, byte_len: usize) {
        self.sent_messages.fetch_add(1, Ordering::Relaxed);
        self.sent_bytes.fetch_add(byte_len as u64, Ordering::Relaxed);
        self.update_topic_stats(topic, byte_len, true).await;
    }

    /// Record a message receive event.
    pub async fn record_recv(&self, topic: &str, byte_len: usize) {
        self.recv_messages.fetch_add(1, Ordering::Relaxed);
        self.recv_bytes.fetch_add(byte_len as u64, Ordering::Relaxed);
        self.update_topic_stats(topic, byte_len, false).await;
    }

    async fn update_topic_stats(&self, topic: &str, byte_len: usize, is_send: bool) {
        let now_ms = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .map(|d| d.as_millis() as u64)
            .unwrap_or(0);

        let mut map = self.topics.write().await;
        let entry = map.entry(topic.to_string()).or_insert_with(|| TopicStats {
            topic: topic.to_string(),
            ..Default::default()
        });

        if is_send {
            entry.sent_count += 1;
        } else {
            entry.recv_count += 1;
        }
        entry.total_bytes += byte_len as u64;
        entry.last_active_timestamp_ms = now_ms;
    }

    /// Get snapshot of all topic statistics.
    pub async fn get_topic_stats(&self) -> Vec<TopicStats> {
        let map = self.topics.read().await;
        let mut list: Vec<TopicStats> = map.values().cloned().collect();
        list.sort_by(|a, b| (b.sent_count + b.recv_count).cmp(&(a.sent_count + a.recv_count)));
        list
    }
}

/// Connectivity status snapshot for LAN UDP and TCP Cluster peers.
#[derive(Debug, Clone, Serialize)]
pub struct ConnectivityStatus {
    pub node_id: String,
    pub udp_bind_addr: String,
    pub udp_multicast_group: Option<String>,
    pub udp_broadcast_enabled: bool,
    pub udp_encryption: String,
    pub tcp_cluster_listen_addr: Option<String>,
    pub tcp_cluster_peers: Vec<PeerStatus>,
    pub tcp_encryption: String,
}

#[derive(Debug, Clone, Serialize)]
pub struct PeerStatus {
    pub peer_addr: String,
    pub status: String, // "Connected" | "Configured"
    pub protocol: String,
}

/// Global System Summary Statistics.
#[derive(Debug, Clone, Serialize)]
pub struct SystemStatsSummary {
    pub node_id: String,
    pub sent_messages_total: u64,
    pub recv_messages_total: u64,
    pub sent_bytes_total: u64,
    pub recv_bytes_total: u64,
    pub active_topics_count: usize,
    pub topics: Vec<TopicStats>,
    pub connectivity: ConnectivityStatus,
}
