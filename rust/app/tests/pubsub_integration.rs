use app_util::pubsub::{
    generate_self_signed_tls_config, PubSubBroker, SymmetricKey, TransportMode,
};
use std::net::{SocketAddr, SocketAddrV4};
use std::time::Duration;
use tokio::time::timeout;

#[tokio::test]
async fn test_lan_udp_unicast_integration() {
    let receiver_addr: SocketAddr = "127.0.0.1:18801".parse().unwrap();
    let sender_addr: SocketAddr = "127.0.0.1:18802".parse().unwrap();

    let receiver_broker = PubSubBroker::builder()
        .node_id("receiver-node")
        .bind_udp(receiver_addr)
        .build()
        .await
        .expect("Failed to build receiver broker");

    let sender_broker = PubSubBroker::builder()
        .node_id("sender-node")
        .bind_udp(sender_addr)
        .build()
        .await
        .expect("Failed to build sender broker");

    let mut sub = receiver_broker.subscribe("devices/+/status").await;

    tokio::time::sleep(Duration::from_millis(50)).await;

    sender_broker
        .publish(
            "devices/sensor1/status",
            b"ONLINE".to_vec(),
            TransportMode::LanUnicast {
                target_addr: receiver_addr,
            },
        )
        .await
        .expect("Failed to publish unicast message");

    let msg = timeout(Duration::from_secs(2), sub.rx.recv())
        .await
        .expect("Timeout waiting for unicast message")
        .expect("Channel closed");

    assert_eq!(msg.topic, "devices/sensor1/status");
    assert_eq!(msg.payload, b"ONLINE");
    assert_eq!(msg.sender_id, "sender-node");
}

#[tokio::test]
async fn test_encrypted_lan_udp_unicast_integration() {
    let receiver_addr: SocketAddr = "127.0.0.1:18811".parse().unwrap();
    let sender_addr: SocketAddr = "127.0.0.1:18812".parse().unwrap();

    let shared_key = SymmetricKey::generate();

    let receiver_broker = PubSubBroker::builder()
        .node_id("enc-receiver-node")
        .bind_udp(receiver_addr)
        .udp_symmetric_key(shared_key.0)
        .build()
        .await
        .expect("Failed to build receiver broker");

    let sender_broker = PubSubBroker::builder()
        .node_id("enc-sender-node")
        .bind_udp(sender_addr)
        .udp_symmetric_key(shared_key.0)
        .build()
        .await
        .expect("Failed to build sender broker");

    let mut sub = receiver_broker.subscribe("secure/lan/data").await;

    tokio::time::sleep(Duration::from_millis(50)).await;

    sender_broker
        .publish(
            "secure/lan/data",
            b"TOP_SECRET_PAYLOAD".to_vec(),
            TransportMode::LanUnicast {
                target_addr: receiver_addr,
            },
        )
        .await
        .expect("Failed to publish encrypted unicast message");

    let msg = timeout(Duration::from_secs(2), sub.rx.recv())
        .await
        .expect("Timeout waiting for encrypted unicast message")
        .expect("Channel closed");

    assert_eq!(msg.topic, "secure/lan/data");
    assert_eq!(msg.payload, b"TOP_SECRET_PAYLOAD");
    assert_eq!(msg.sender_id, "enc-sender-node");
}

#[tokio::test]
async fn test_lan_udp_multicast_integration() {
    let mcast_group: SocketAddrV4 = "239.255.0.1:18803".parse().unwrap();
    let mcast_socket_addr: SocketAddr = SocketAddr::V4(mcast_group);

    let broker_a = PubSubBroker::builder()
        .node_id("mcast-broker-a")
        .bind_udp("0.0.0.0:18803".parse().unwrap())
        .udp_multicast(mcast_group)
        .build()
        .await
        .expect("Failed to build broker A");

    let broker_b = PubSubBroker::builder()
        .node_id("mcast-broker-b")
        .bind_udp("0.0.0.0:18804".parse().unwrap())
        .udp_multicast(mcast_group)
        .build()
        .await
        .expect("Failed to build broker B");

    let mut sub_a = broker_a.subscribe("cluster/events/#").await;

    tokio::time::sleep(Duration::from_millis(50)).await;

    broker_b
        .publish(
            "cluster/events/node_joined",
            b"Node-B Active".to_vec(),
            TransportMode::LanMulticast {
                group_addr: mcast_socket_addr,
            },
        )
        .await
        .expect("Failed to publish multicast message");

    let msg = timeout(Duration::from_secs(2), sub_a.rx.recv())
        .await
        .expect("Timeout waiting for multicast message")
        .expect("Channel closed");

    assert_eq!(msg.topic, "cluster/events/node_joined");
    assert_eq!(msg.payload, b"Node-B Active");
}

#[tokio::test]
async fn test_lan_udp_broadcast_integration() {
    let broadcast_port: u16 = 18805;
    let bind_addr: SocketAddr = format!("0.0.0.0:{}", broadcast_port).parse().unwrap();

    let listener_broker = PubSubBroker::builder()
        .node_id("bcast-listener")
        .bind_udp(bind_addr)
        .enable_broadcast(true)
        .build()
        .await
        .expect("Failed to build listener broker");

    let sender_broker = PubSubBroker::builder()
        .node_id("bcast-sender")
        .bind_udp("0.0.0.0:0".parse().unwrap())
        .enable_broadcast(true)
        .build()
        .await
        .expect("Failed to build sender broker");

    let mut sub = listener_broker.subscribe("system/alerts").await;

    tokio::time::sleep(Duration::from_millis(50)).await;

    sender_broker
        .publish(
            "system/alerts",
            b"CRITICAL_EVENT".to_vec(),
            TransportMode::LanBroadcast { broadcast_port },
        )
        .await
        .expect("Failed to publish broadcast message");

    let msg = timeout(Duration::from_secs(2), sub.rx.recv())
        .await
        .expect("Timeout waiting for broadcast message")
        .expect("Channel closed");

    assert_eq!(msg.topic, "system/alerts");
    assert_eq!(msg.payload, b"CRITICAL_EVENT");
}

#[tokio::test]
async fn test_cross_lan_tcp_cluster_integration() {
    let gateway_a_addr: SocketAddr = "127.0.0.1:19921".parse().unwrap();
    let gateway_b_addr: SocketAddr = "127.0.0.1:19922".parse().unwrap();

    let broker_lan1 = PubSubBroker::builder()
        .node_id("gateway-lan-1")
        .bind_tcp_cluster(gateway_a_addr)
        .add_tcp_cluster_peer(gateway_b_addr)
        .build()
        .await
        .expect("Failed to build LAN-1 Gateway");

    let broker_lan2 = PubSubBroker::builder()
        .node_id("gateway-lan-2")
        .bind_tcp_cluster(gateway_b_addr)
        .add_tcp_cluster_peer(gateway_a_addr)
        .build()
        .await
        .expect("Failed to build LAN-2 Gateway");

    let mut sub_lan2 = broker_lan2.subscribe("global/market/data").await;

    tokio::time::sleep(Duration::from_millis(100)).await;

    broker_lan1
        .publish(
            "global/market/data",
            b"BTC/USD=65000".to_vec(),
            TransportMode::TcpCluster,
        )
        .await
        .expect("Failed to publish TCP cluster message");

    let msg = timeout(Duration::from_secs(3), sub_lan2.rx.recv())
        .await
        .expect("Timeout waiting for cross-LAN TCP cluster message")
        .expect("Channel closed");

    assert_eq!(msg.topic, "global/market/data");
    assert_eq!(msg.payload, b"BTC/USD=65000");
    assert_eq!(msg.sender_id, "gateway-lan-1");
}

#[tokio::test]
async fn test_encrypted_cross_lan_tls_cluster_integration() {
    let gateway_tls_a_addr: SocketAddr = "127.0.0.1:19911".parse().unwrap();
    let gateway_tls_b_addr: SocketAddr = "127.0.0.1:19912".parse().unwrap();

    let shared_tls_config = generate_self_signed_tls_config("localhost").unwrap();

    let broker_tls_lan1 = PubSubBroker::builder()
        .node_id("tls-gateway-lan-1")
        .bind_tcp_cluster(gateway_tls_a_addr)
        .add_tcp_cluster_peer(gateway_tls_b_addr)
        .tls_config(shared_tls_config.clone())
        .build()
        .await
        .expect("Failed to build TLS LAN-1 Gateway");

    let broker_tls_lan2 = PubSubBroker::builder()
        .node_id("tls-gateway-lan-2")
        .bind_tcp_cluster(gateway_tls_b_addr)
        .add_tcp_cluster_peer(gateway_tls_a_addr)
        .tls_config(shared_tls_config)
        .build()
        .await
        .expect("Failed to build TLS LAN-2 Gateway");

    let mut sub_lan2 = broker_tls_lan2.subscribe("secure/cross_lan/orders").await;

    tokio::time::sleep(Duration::from_millis(150)).await;

    broker_tls_lan1
        .publish(
            "secure/cross_lan/orders",
            b"ENCRYPTED_ORDER_DATA".to_vec(),
            TransportMode::TcpCluster,
        )
        .await
        .expect("Failed to publish TLS cluster message");

    let msg = timeout(Duration::from_secs(3), sub_lan2.rx.recv())
        .await
        .expect("Timeout waiting for TLS cross-LAN cluster message")
        .expect("Channel closed");

    assert_eq!(msg.topic, "secure/cross_lan/orders");
    assert_eq!(msg.payload, b"ENCRYPTED_ORDER_DATA");
    assert_eq!(msg.sender_id, "tls-gateway-lan-1");
}
