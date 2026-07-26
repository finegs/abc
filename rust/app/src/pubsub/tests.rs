use crate::pubsub::cross_lan_tcp::{
    read_framed_message, write_framed_message,
};
use crate::pubsub::patterns::{
    DeduplicationFilter, HopLimitFilter, MessageFactory, MessagePipeline, ObserverRegistry,
};
use crate::pubsub::protocol::{Message, TransportMode};
use crate::pubsub::routing_rules::{RuleAction, RuleEngine, RoutingRule};
use crate::pubsub::security::{
    decrypt_payload, encrypt_payload, generate_self_signed_tls_config, MessageDeduplicator,
    SymmetricKey,
};
use crate::pubsub::stats::MetricsCollector;
use crate::pubsub::topic_matcher::matches_topic;
use crate::pubsub::web_manager::start_web_manager;
use crate::pubsub::PubSubBroker;
use std::sync::atomic::AtomicU64;
use std::sync::Arc;
use tokio::net::{TcpListener, TcpStream};
use tokio::sync::mpsc;

#[test]
fn test_message_serialization() {
    let msg = MessageFactory::create(
        "sensors/temperature/room1",
        b"23.5C".to_vec(),
        "node-test",
        TransportMode::LanUnicast {
            target_addr: "127.0.0.1:8080".parse().unwrap(),
        },
    );

    let bytes = msg.to_bytes().expect("Failed to serialize message");
    let decoded = Message::from_bytes(&bytes).expect("Failed to deserialize message");

    assert_eq!(msg.id, decoded.id);
    assert_eq!(msg.topic, decoded.topic);
    assert_eq!(msg.payload, decoded.payload);
    assert_eq!(msg.sender_id, decoded.sender_id);
    assert_eq!(msg.transport_mode, decoded.transport_mode);
}

#[tokio::test]
async fn test_metrics_collector() {
    let collector = MetricsCollector::new();

    collector.record_send("sensors/temp", 100).await;
    collector.record_send("sensors/temp", 50).await;
    collector.record_recv("sensors/temp", 50).await;
    collector.record_recv("orders/created", 200).await;

    let topics = collector.get_topic_stats().await;
    assert_eq!(topics.len(), 2);

    let temp_stat = topics.iter().find(|t| t.topic == "sensors/temp").unwrap();
    assert_eq!(temp_stat.sent_count, 2);
    assert_eq!(temp_stat.recv_count, 1);
    assert_eq!(temp_stat.total_bytes, 200);
}

#[tokio::test]
async fn test_broker_system_stats() {
    let broker = PubSubBroker::builder()
        .node_id("stats-node-1")
        .bind_udp("127.0.0.1:18900".parse().unwrap())
        .enable_broadcast(true)
        .build()
        .await
        .unwrap();

    broker
        .publish(
            "metrics/test",
            b"Hello Metrics".to_vec(),
            TransportMode::LanUnicast {
                target_addr: "127.0.0.1:18900".parse().unwrap(),
            },
        )
        .await
        .unwrap();

    let stats = broker.get_system_stats().await;
    assert_eq!(stats.node_id, "stats-node-1");
    assert_eq!(stats.sent_messages_total, 1);
    assert_eq!(stats.connectivity.udp_bind_addr, "127.0.0.1:18900");
    assert!(stats.connectivity.udp_broadcast_enabled);
}

#[test]
fn test_pattern_1_factory() {
    let msg = MessageFactory::create(
        "factory/topic",
        b"payload".to_vec(),
        "factory-node",
        TransportMode::TcpCluster,
    );
    assert_eq!(msg.topic, "factory/topic");
    assert_eq!(msg.sender_id, "factory-node");
    assert_eq!(msg.hops, 0);

    let msg_hops = MessageFactory::create_with_hops(
        "factory/topic",
        b"payload".to_vec(),
        "factory-node",
        TransportMode::TcpCluster,
        3,
    );
    assert_eq!(msg_hops.hops, 3);
}

#[tokio::test]
async fn test_pattern_2_builder() {
    let broker = PubSubBroker::builder()
        .node_id("builder-node-123")
        .bind_udp("0.0.0.0:0".parse().unwrap())
        .enable_broadcast(true)
        .build()
        .await
        .expect("Builder failed");

    assert_eq!(broker.node_id(), "builder-node-123");
}

#[tokio::test]
async fn test_pattern_4_chain_of_responsibility() {
    let dedup = MessageDeduplicator::new();

    let pipeline = MessagePipeline::new()
        .add_filter(Arc::new(HopLimitFilter { max_hops: 2 }))
        .add_filter(Arc::new(DeduplicationFilter { dedup }));

    let valid_msg = MessageFactory::create(
        "valid/topic",
        b"data".to_vec(),
        "sender-1",
        TransportMode::LanBroadcast { broadcast_port: 8000 },
    );

    let res1 = pipeline.execute(valid_msg.clone()).await;
    assert!(res1.is_some());

    let res2 = pipeline.execute(valid_msg.clone()).await;
    assert!(res2.is_none());

    let high_hop_msg = MessageFactory::create_with_hops(
        "high/hop",
        b"data".to_vec(),
        "sender-2",
        TransportMode::TcpCluster,
        5,
    );
    let res3 = pipeline.execute(high_hop_msg).await;
    assert!(res3.is_none());
}

#[tokio::test]
async fn test_pattern_5_observer() {
    let registry = ObserverRegistry::new();
    let (tx1, mut rx1) = mpsc::channel(10);
    let (tx2, mut rx2) = mpsc::channel(10);

    registry.register(1, "sensors/+/temperature".to_string(), tx1).await;
    registry.register(2, "sensors/#".to_string(), tx2).await;

    let msg = MessageFactory::create(
        "sensors/bedroom/temperature",
        b"22C".to_vec(),
        "sensor-node",
        TransportMode::LanUnicast { target_addr: "127.0.0.1:9000".parse().unwrap() },
    );

    registry.notify(&msg).await;

    let notification1 = rx1.recv().await.unwrap();
    let notification2 = rx2.recv().await.unwrap();

    assert_eq!(notification1.topic, "sensors/bedroom/temperature");
    assert_eq!(notification2.topic, "sensors/bedroom/temperature");
}

#[test]
fn test_topic_matcher() {
    assert!(matches_topic("a/b/c", "a/b/c"));
    assert!(!matches_topic("a/b/c", "a/b/d"));
    assert!(!matches_topic("a/b/c", "a/b"));

    assert!(matches_topic("a/+/c", "a/b/c"));
    assert!(matches_topic("a/+/c", "a/x/c"));
    assert!(!matches_topic("a/+/c", "a/b/d/e"));

    assert!(matches_topic("a/#", "a/b/c"));
    assert!(matches_topic("a/#", "a/b"));
    assert!(matches_topic("a/#", "a"));
    assert!(matches_topic("#", "anything/goes/here"));
    assert!(!matches_topic("a/b/#", "a/c/d"));
}

#[tokio::test]
async fn test_tcp_framing() {
    let listener = TcpListener::bind("127.0.0.1:0").await.unwrap();
    let addr = listener.local_addr().unwrap();

    let msg = MessageFactory::create(
        "cluster/test",
        b"hello tcp cluster".to_vec(),
        "node-tcp",
        TransportMode::TcpCluster,
    );

    let msg_clone = msg.clone();
    tokio::spawn(async move {
        let (mut stream, _) = listener.accept().await.unwrap();
        let received = read_framed_message(&mut stream).await.unwrap();
        assert_eq!(msg_clone, received);
    });

    let mut client_stream = TcpStream::connect(addr).await.unwrap();
    write_framed_message(&mut client_stream, &msg).await.unwrap();
}

#[tokio::test]
async fn test_message_deduplicator() {
    let dedup = MessageDeduplicator::new();
    let msg_id = "unique-msg-123";

    assert!(dedup.check_and_insert(msg_id).await);
    assert!(!dedup.check_and_insert(msg_id).await);
}

#[tokio::test]
async fn test_aead_encryption_decryption() {
    let key = SymmetricKey::generate();
    let plaintext = b"Secret payload message over UDP".to_vec();

    let encrypted = encrypt_payload(&key.0, &plaintext).unwrap();
    assert_ne!(plaintext, encrypted);

    let decrypted = decrypt_payload(&key.0, &encrypted).unwrap();
    assert_eq!(plaintext, decrypted);

    let wrong_key = SymmetricKey::generate();
    assert!(decrypt_payload(&wrong_key.0, &encrypted).is_err());
}

#[test]
fn test_tls_config_generation() {
    let tls_config = generate_self_signed_tls_config("localhost").unwrap();
    assert!(tls_config.server_config.is_some());
    assert!(tls_config.client_config.is_some());
    assert_eq!(tls_config.server_name, "localhost");
}

#[tokio::test]
async fn test_routing_rule_engine() {
    let engine = RuleEngine::new();

    let rule1 = RoutingRule {
        id: "rule-1".to_string(),
        name: "Sensor Forwarder".to_string(),
        source_topic_pattern: "sensors/+/temperature".to_string(),
        action: RuleAction::TransformTopic {
            new_topic: "alerts/high_temp".to_string(),
        },
        target_transport: TransportMode::LanBroadcast { broadcast_port: 8888 },
        enabled: true,
        priority: 10,
    };

    engine.add_rule(rule1).await;
    assert_eq!(engine.get_rules().await.len(), 1);

    let msg = MessageFactory::create(
        "sensors/room1/temperature",
        b"35C".to_vec(),
        "sensor-1",
        TransportMode::LanUnicast {
            target_addr: "127.0.0.1:9000".parse().unwrap(),
        },
    );

    let routed = engine.evaluate_and_route(&msg).await;
    assert_eq!(routed.len(), 1);
    assert_eq!(routed[0].0.topic, "alerts/high_temp");
    assert_eq!(
        routed[0].1,
        TransportMode::LanBroadcast { broadcast_port: 8888 }
    );

    assert_eq!(engine.toggle_rule("rule-1").await, Some(false));
    let routed_disabled = engine.evaluate_and_route(&msg).await;
    assert_eq!(routed_disabled.len(), 0);

    assert!(engine.remove_rule("rule-1").await);
    assert_eq!(engine.get_rules().await.len(), 0);
}

#[tokio::test]
async fn test_web_manager_api() {
    let broker = PubSubBroker::builder()
        .node_id("web-manager-node")
        .build()
        .await
        .unwrap();

    let rule_engine = RuleEngine::new();
    let counter = Arc::new(AtomicU64::new(42));
    let addr = "127.0.0.1:28890".parse().unwrap();

    start_web_manager(addr, broker, rule_engine, counter)
        .await
        .expect("Failed to start web manager");

    tokio::time::sleep(tokio::time::Duration::from_millis(100)).await;

    let client = reqwest_or_hyper_get("http://127.0.0.1:28890/api/status").await;
    assert!(client.contains("web-manager-node"));
}

async fn reqwest_or_hyper_get(_url: &str) -> String {
    use tokio::io::AsyncWriteExt;
    let stream = tokio::net::TcpStream::connect("127.0.0.1:28890").await.unwrap();
    let (mut reader, mut writer) = stream.into_split();

    let req = format!("GET /api/status HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n");
    writer.write_all(req.as_bytes()).await.unwrap();

    let mut buf = vec![0u8; 1024];
    let len = tokio::io::AsyncReadExt::read(&mut reader, &mut buf).await.unwrap();
    String::from_utf8_lossy(&buf[..len]).to_string()
}

#[tokio::test]
async fn test_local_pubsub_broker() {
    let broker = PubSubBroker::builder()
        .node_id("test-local-node")
        .build()
        .await
        .unwrap();

    let mut sub = broker.subscribe("orders/+").await;

    broker
        .publish(
            "orders/created",
            b"Order #1001".to_vec(),
            TransportMode::LanUnicast {
                target_addr: "127.0.0.1:9999".parse().unwrap(),
            },
        )
        .await
        .unwrap();

    let received = tokio::time::timeout(tokio::time::Duration::from_secs(1), sub.rx.recv())
        .await
        .unwrap()
        .unwrap();

    assert_eq!(received.topic, "orders/created");
    assert_eq!(received.payload, b"Order #1001");
}
