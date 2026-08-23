use crate::pubsub::cross_lan_tcp::CrossLanTcpCluster;
use crate::pubsub::lan_udp::LanUdpTransport;
use crate::pubsub::protocol::{Message, TransportMode};
use crate::pubsub::security::MessageDeduplicator;
use crate::pubsub::topic_matcher::matches_topic;
use async_trait::async_trait;
use std::io;
use std::sync::Arc;
use tokio::sync::mpsc;

// ============================================================================
// 1. FACTORY PATTERN: MessageFactory
// ============================================================================
/// Factory for constructing standardized Message frames.
pub struct MessageFactory;

impl MessageFactory {
    /// Creates a standard Message frame with default hop count 0.
    pub fn create(
        topic: impl Into<String>,
        payload: Vec<u8>,
        sender_id: impl Into<String>,
        transport_mode: TransportMode,
    ) -> Message {
        Message::new(topic, payload, sender_id, transport_mode)
    }

    /// Creates a Message frame with a specific initial hop count.
    pub fn create_with_hops(
        topic: impl Into<String>,
        payload: Vec<u8>,
        sender_id: impl Into<String>,
        transport_mode: TransportMode,
        hops: u8,
    ) -> Message {
        let mut msg = Message::new(topic, payload, sender_id, transport_mode);
        msg.hops = hops;
        msg
    }
}

// ============================================================================
// 2. STRATEGY PATTERN: TransportStrategy
// ============================================================================
/// Strategy interface defining transport execution algorithm.
#[async_trait]
pub trait TransportStrategy: Send + Sync {
    async fn send(&self, msg: &Message) -> io::Result<()>;
}

/// Concrete Strategy for LAN UDP Transport (Unicast, Multicast, Broadcast).
pub struct UdpTransportStrategy {
    pub udp_transport: Arc<LanUdpTransport>,
}

#[async_trait]
impl TransportStrategy for UdpTransportStrategy {
    async fn send(&self, msg: &Message) -> io::Result<()> {
        self.udp_transport.send_message(msg).await
    }
}

/// Concrete Strategy for Cross-LAN TCP Cluster Transport (TLS 1.3 Bridge).
pub struct TcpClusterTransportStrategy {
    pub tcp_cluster: Arc<CrossLanTcpCluster>,
}

#[async_trait]
impl TransportStrategy for TcpClusterTransportStrategy {
    async fn send(&self, msg: &Message) -> io::Result<()> {
        self.tcp_cluster.forward_to_peers(msg.clone()).await;
        Ok(())
    }
}

/// Context for invoking the active TransportStrategy dynamically.
pub struct TransportContext {
    udp_strategy: Arc<UdpTransportStrategy>,
    tcp_strategy: Option<Arc<TcpClusterTransportStrategy>>,
}

impl TransportContext {
    pub fn new(
        udp_transport: Arc<LanUdpTransport>,
        tcp_cluster: Option<Arc<CrossLanTcpCluster>>,
    ) -> Self {
        Self {
            udp_strategy: Arc::new(UdpTransportStrategy { udp_transport }),
            tcp_strategy: tcp_cluster.map(|tc| Arc::new(TcpClusterTransportStrategy { tcp_cluster: tc })),
        }
    }

    pub async fn dispatch(&self, msg: &Message) -> io::Result<()> {
        match &msg.transport_mode {
            TransportMode::LanUnicast { .. }
            | TransportMode::LanMulticast { .. }
            | TransportMode::LanBroadcast { .. } => {
                self.udp_strategy.send(msg).await
            }
            TransportMode::TcpCluster => {
                if let Some(ref tcp) = self.tcp_strategy {
                    tcp.send(msg).await
                } else {
                    Err(io::Error::new(
                        io::ErrorKind::NotConnected,
                        "TCP Cluster transport not configured on this broker",
                    ))
                }
            }
        }
    }
}

// ============================================================================
// 3. CHAIN OF RESPONSIBILITY PATTERN: MessagePipeline & MessageFilter
// ============================================================================
/// Handler trait in the Chain of Responsibility for processing incoming messages.
#[async_trait]
pub trait MessageFilter: Send + Sync {
    async fn process(&self, msg: Message) -> Option<Message>;
}

/// Concrete Handler 1: Filters out duplicate messages using Deduplicator cache.
pub struct DeduplicationFilter {
    pub dedup: MessageDeduplicator,
}

#[async_trait]
impl MessageFilter for DeduplicationFilter {
    async fn process(&self, msg: Message) -> Option<Message> {
        if self.dedup.check_and_insert(&msg.id).await {
            Some(msg)
        } else {
            None // Drop duplicate message
        }
    }
}

/// Concrete Handler 2: Filters out messages exceeding max hop limit (prevents infinite loops).
pub struct HopLimitFilter {
    pub max_hops: u8,
}

#[async_trait]
impl MessageFilter for HopLimitFilter {
    async fn process(&self, msg: Message) -> Option<Message> {
        if msg.hops <= self.max_hops {
            Some(msg)
        } else {
            None // Drop message exceeding hop limit
        }
    }
}

/// Sequential Pipeline executing the Chain of Responsibility.
#[derive(Clone)]
pub struct MessagePipeline {
    filters: Vec<Arc<dyn MessageFilter>>,
}

impl MessagePipeline {
    pub fn new() -> Self {
        Self { filters: Vec::new() }
    }

    pub fn add_filter(mut self, filter: Arc<dyn MessageFilter>) -> Self {
        self.filters.push(filter);
        self
    }

    pub async fn execute(&self, mut msg: Message) -> Option<Message> {
        for filter in &self.filters {
            match filter.process(msg).await {
                Some(next_msg) => msg = next_msg,
                None => return None, // Short-circuit pipeline
            }
        }
        Some(msg)
    }
}

// ============================================================================
// 4. OBSERVER PATTERN: TopicObserver & ObserverRegistry
// ============================================================================
/// Observer handle receiving reactive topic event notifications.
pub struct TopicObserver {
    pub id: usize,
    pub pattern: String,
    pub tx: mpsc::Sender<Message>,
}

/// Subject / Registry managing active TopicObservers.
#[derive(Default, Clone)]
pub struct ObserverRegistry {
    observers: Arc<tokio::sync::RwLock<Vec<TopicObserver>>>,
}

impl ObserverRegistry {
    pub fn new() -> Self {
        Self {
            observers: Arc::new(tokio::sync::RwLock::new(Vec::new())),
        }
    }

    /// Register a new Observer for a topic pattern.
    pub async fn register(&self, id: usize, pattern: String, tx: mpsc::Sender<Message>) {
        let mut obs = self.observers.write().await;
        obs.push(TopicObserver { id, pattern, tx });
    }

    /// Unregister an Observer by ID.
    pub async fn unregister(&self, id: usize) {
        let mut obs = self.observers.write().await;
        obs.retain(|o| o.id != id);
    }

    /// Notify all matching observers of a new published Message.
    pub async fn notify(&self, msg: &Message) {
        let obs = self.observers.read().await;
        for observer in obs.iter() {
            if matches_topic(&observer.pattern, &msg.topic) {
                let _ = observer.tx.send(msg.clone()).await;
            }
        }
    }
}
