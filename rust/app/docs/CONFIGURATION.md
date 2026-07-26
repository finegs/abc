# ⚙️ Configuration Guide

This guide details how to configure LAN UDP transports, Cross-LAN TCP Cluster Gateways, AES-256-GCM AEAD encryption keys, TLS 1.3 certificates, and topic routing rules.

---

## 1. Broker Configuration (`PubSubBrokerBuilder`)

The `PubSubBrokerBuilder` uses the **Builder Pattern** to configure network parameters:

```rust
use app_util::pubsub::{PubSubBroker, SymmetricKey, generate_self_signed_tls_config};

let symmetric_key = SymmetricKey::generate();
let tls_config = generate_self_signed_tls_config("localhost")?;

let broker = PubSubBroker::builder()
    .node_id("gateway-east-1")
    .bind_udp("0.0.0.0:18801".parse()?)
    .udp_multicast("239.255.0.1:18803".parse()?)
    .enable_broadcast(true)
    .udp_symmetric_key(symmetric_key.0)
    .bind_tcp_cluster("0.0.0.0:19901".parse()?)
    .add_tcp_cluster_peer("192.168.2.10:19902".parse()?)
    .tls_config(tls_config)
    .build()
    .await?;
```

---

## 2. Intra-LAN UDP Transports

| Mode | Function | Code Example |
| :--- | :--- | :--- |
| **Unicast** | Point-to-point UDP delivery to target IP:Port | `TransportMode::LanUnicast { target_addr: "192.168.1.20:18801".parse()? }` |
| **Multicast** | IGMP group delivery without subnet flooding | `TransportMode::LanMulticast { group_addr: "239.255.0.1:18803".parse()? }` |
| **Broadcast** | Subnet-wide UDP broadcast | `TransportMode::LanBroadcast { broadcast_port: 18805 }` |

### Payload Encryption (AES-256-GCM AEAD)
- **Key Size**: 256-bit (32 bytes).
- **Nonce**: 96-bit (12 bytes) cryptographically secure random per datagram.
- **Config**: `.udp_symmetric_key(key_bytes)`.

---

## 3. Cross-LAN TCP Cluster Gateways

- **Protocol**: Length-prefixed 4-byte big-endian header framing.
- **TLS 1.3**: Configured with `rustls` X.509 server certificate and client root certificate store.
- **Peer Mesh**: Configured via `.add_tcp_cluster_peer(peer_socket_addr)`.

---

## 4. Topic Message Routing Rules

Routing rules dynamically inspect incoming message topics matching wildcards (`+` or `#`), transform topics, or redirect transport modes.

```rust
use app_util::pubsub::{RuleEngine, RoutingRule, RuleAction, TransportMode};

let engine = RuleEngine::new();

let rule = RoutingRule {
    id: "rule-sensor-redirect".to_string(),
    name: "Redirect Critical Temp Alerts".to_string(),
    source_topic_pattern: "sensors/+/temperature".to_string(),
    action: RuleAction::TransformTopic {
        new_topic: "alerts/critical_temp".to_string(),
    },
    target_transport: TransportMode::TcpCluster,
    enabled: true,
    priority: 10,
};

engine.add_rule(rule).await;
```
