use crate::pubsub::cross_lan_tcp::CrossLanTcpCluster;
use crate::pubsub::lan_udp::{LanUdpConfig, LanUdpTransport};
use crate::pubsub::patterns::{
    DeduplicationFilter, HopLimitFilter, MessageFactory, MessagePipeline, ObserverRegistry,
    TransportContext,
};
use crate::pubsub::protocol::{Message, TransportMode};
use crate::pubsub::security::{MessageDeduplicator, TlsConfig, AES_KEY_SIZE};
use crate::pubsub::stats::{ConnectivityStatus, MetricsCollector, PeerStatus, SystemStatsSummary};
use std::io;
use std::net::{SocketAddr, SocketAddrV4};
use std::sync::atomic::Ordering;
use std::sync::Arc;
use tokio::sync::{mpsc, RwLock};

/// Subscription handle returned to caller.
pub struct Subscription {
    pub id: usize,
    pub pattern: String,
    pub rx: mpsc::Receiver<Message>,
}

/// Central Pub/Sub Broker managing transport strategies, observers, pipeline filters, and real-time metrics.
pub struct PubSubBroker {
    node_id: String,
    transport_context: TransportContext,
    observer_registry: ObserverRegistry,
    pipeline: MessagePipeline,
    metrics: MetricsCollector,
    udp_bind_addr: SocketAddr,
    udp_multicast_group: Option<SocketAddrV4>,
    udp_broadcast_enabled: bool,
    has_aes_key: bool,
    tcp_listen_addr: Option<SocketAddr>,
    tcp_peers: Vec<SocketAddr>,
    has_tls: bool,
    next_sub_id: Arc<RwLock<usize>>,
    #[allow(dead_code)]
    internal_tx: mpsc::Sender<Message>,
}

impl PubSubBroker {
    pub fn builder() -> PubSubBrokerBuilder {
        PubSubBrokerBuilder::default()
    }

    pub async fn subscribe(&self, pattern: impl Into<String>) -> Subscription {
        let pattern_str = pattern.into();
        let (tx, rx) = mpsc::channel(100);

        let mut sub_id_guard = self.next_sub_id.write().await;
        let id = *sub_id_guard;
        *sub_id_guard += 1;

        self.observer_registry
            .register(id, pattern_str.clone(), tx)
            .await;

        Subscription {
            id,
            pattern: pattern_str,
            rx,
        }
    }

    pub async fn unsubscribe(&self, sub_id: usize) {
        self.observer_registry.unregister(sub_id).await;
    }

    pub async fn publish(
        &self,
        topic: impl Into<String>,
        payload: Vec<u8>,
        transport_mode: TransportMode,
    ) -> io::Result<String> {
        let topic_str = topic.into();
        let payload_len = payload.len();
        let msg = MessageFactory::create(&topic_str, payload, &self.node_id, transport_mode);
        let msg_id = msg.id.clone();

        self.metrics.record_send(&topic_str, payload_len).await;

        if let Some(processed_msg) = self.pipeline.execute(msg.clone()).await {
            self.observer_registry.notify(&processed_msg).await;
        }

        self.transport_context.dispatch(&msg).await?;

        Ok(msg_id)
    }

    pub async fn handle_incoming(&self, msg: Message) {
        self.metrics.record_recv(&msg.topic, msg.payload.len()).await;
        if let Some(processed_msg) = self.pipeline.execute(msg).await {
            self.observer_registry.notify(&processed_msg).await;
        }
    }

    /// Return comprehensive system metrics, topic statistics, and connectivity state.
    pub async fn get_system_stats(&self) -> SystemStatsSummary {
        let topic_stats = self.metrics.get_topic_stats().await;
        let peer_statuses = self
            .tcp_peers
            .iter()
            .map(|p| PeerStatus {
                peer_addr: p.to_string(),
                status: "Configured Peer".to_string(),
                protocol: if self.has_tls { "TCP/TLS 1.3".to_string() } else { "TCP Plain".to_string() },
            })
            .collect();

        let connectivity = ConnectivityStatus {
            node_id: self.node_id.clone(),
            udp_bind_addr: self.udp_bind_addr.to_string(),
            udp_multicast_group: self.udp_multicast_group.map(|g| g.to_string()),
            udp_broadcast_enabled: self.udp_broadcast_enabled,
            udp_encryption: if self.has_aes_key { "AES-256-GCM AEAD Enabled".to_string() } else { "Plaintext".to_string() },
            tcp_cluster_listen_addr: self.tcp_listen_addr.map(|a| a.to_string()),
            tcp_cluster_peers: peer_statuses,
            tcp_encryption: if self.has_tls { "TLS 1.3 Standard".to_string() } else { "Plaintext".to_string() },
        };

        SystemStatsSummary {
            node_id: self.node_id.clone(),
            sent_messages_total: self.metrics.sent_messages.load(Ordering::Relaxed),
            recv_messages_total: self.metrics.recv_messages.load(Ordering::Relaxed),
            sent_bytes_total: self.metrics.sent_bytes.load(Ordering::Relaxed),
            recv_bytes_total: self.metrics.recv_bytes.load(Ordering::Relaxed),
            active_topics_count: topic_stats.len(),
            topics: topic_stats,
            connectivity,
        }
    }

    pub fn node_id(&self) -> &str {
        &self.node_id
    }
}

pub fn builder() -> PubSubBrokerBuilder {
    PubSubBrokerBuilder::default()
}

#[derive(Default)]
pub struct PubSubBrokerBuilder {
    node_id: Option<String>,
    udp_bind_addr: Option<SocketAddr>,
    udp_multicast_group: Option<SocketAddrV4>,
    udp_enable_broadcast: bool,
    udp_symmetric_key: Option<[u8; AES_KEY_SIZE]>,
    tcp_listen_addr: Option<SocketAddr>,
    tcp_peers: Vec<SocketAddr>,
    tls_config: Option<TlsConfig>,
}

impl PubSubBrokerBuilder {
    pub fn node_id(mut self, id: impl Into<String>) -> Self {
        self.node_id = Some(id.into());
        self
    }

    pub fn bind_udp(mut self, addr: SocketAddr) -> Self {
        self.udp_bind_addr = Some(addr);
        self
    }

    pub fn udp_multicast(mut self, group_addr: SocketAddrV4) -> Self {
        self.udp_multicast_group = Some(group_addr);
        self
    }

    pub fn enable_broadcast(mut self, enable: bool) -> Self {
        self.udp_enable_broadcast = enable;
        self
    }

    pub fn udp_symmetric_key(mut self, key: [u8; AES_KEY_SIZE]) -> Self {
        self.udp_symmetric_key = Some(key);
        self
    }

    pub fn bind_tcp_cluster(mut self, addr: SocketAddr) -> Self {
        self.tcp_listen_addr = Some(addr);
        self
    }

    pub fn add_tcp_cluster_peer(mut self, peer_addr: SocketAddr) -> Self {
        self.tcp_peers.push(peer_addr);
        self
    }

    pub fn tls_config(mut self, config: TlsConfig) -> Self {
        self.tls_config = Some(config);
        self
    }

    pub async fn build(self) -> io::Result<Arc<PubSubBroker>> {
        let node_id = self
            .node_id
            .unwrap_or_else(|| format!("node-{}", std::process::id()));

        let udp_bind_addr = self.udp_bind_addr.unwrap_or_else(|| "0.0.0.0:0".parse().unwrap());
        let has_aes_key = self.udp_symmetric_key.is_some();
        let has_tls = self.tls_config.is_some();

        let udp_config = LanUdpConfig {
            bind_addr: udp_bind_addr,
            multicast_group: self.udp_multicast_group,
            enable_broadcast: self.udp_enable_broadcast,
            symmetric_key: self.udp_symmetric_key,
        };

        let udp_transport = Arc::new(LanUdpTransport::new(&udp_config)?);
        let dedup = MessageDeduplicator::new();
        let (internal_tx, mut internal_rx) = mpsc::channel::<Message>(500);

        LanUdpTransport::start_receive_loop(
            udp_transport.socket(),
            internal_tx.clone(),
            udp_config.symmetric_key,
        );

        let tcp_cluster = if let Some(tcp_addr) = self.tcp_listen_addr {
            let cluster = Arc::new(CrossLanTcpCluster::new(
                tcp_addr,
                self.tcp_peers.clone(),
                self.tls_config,
                internal_tx.clone(),
            ));
            cluster.clone().start().await?;
            Some(cluster)
        } else {
            None
        };

        let transport_context = TransportContext::new(udp_transport, tcp_cluster);
        let observer_registry = ObserverRegistry::new();
        let metrics = MetricsCollector::new();

        let pipeline = MessagePipeline::new()
            .add_filter(Arc::new(DeduplicationFilter {
                dedup: dedup.clone(),
            }))
            .add_filter(Arc::new(HopLimitFilter { max_hops: 5 }));

        let broker = Arc::new(PubSubBroker {
            node_id,
            transport_context,
            observer_registry,
            pipeline,
            metrics,
            udp_bind_addr,
            udp_multicast_group: self.udp_multicast_group,
            udp_broadcast_enabled: self.udp_enable_broadcast,
            has_aes_key,
            tcp_listen_addr: self.tcp_listen_addr,
            tcp_peers: self.tcp_peers,
            has_tls,
            next_sub_id: Arc::new(RwLock::new(1)),
            internal_tx,
        });

        let broker_clone = broker.clone();
        tokio::spawn(async move {
            while let Some(msg) = internal_rx.recv().await {
                broker_clone.handle_incoming(msg).await;
            }
        });

        Ok(broker)
    }
}
