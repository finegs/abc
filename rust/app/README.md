# ⚡ Rust Hybrid Network Pub/Sub Library & Web Manager

A high-performance, low-latency **Rust Message Pub/Sub Library** with dual-layer networking:
- **Intra-LAN Messaging**: UDP socket transport supporting **Unicast**, **IPv4 Multicast**, and **Subnet Broadcast** with **AES-256-GCM AEAD** payload encryption.
- **Inter-LAN Messaging**: Reliable **TCP Cluster Gateway Bridges** with **TLS 1.3** stream encryption and 4-byte length-prefixed framing.
- **Web Manager Dashboard**: Embedded Rust `axum` REST API and modern TypeScript/HTML Web UI for dynamic topic message routing rules, statistics monitoring, and manual message dispatching.

---

## 📚 Documentation Index

To explore detailed step-by-step guides, refer to the separated documentation topics:

- 🛠️ [**Installation Guide**](docs/INSTALLATION.md): Requirements, dependency setup, and compilation instructions (`cargo build`, `cargo test`).
- ⚙️ [**Configuration Guide**](docs/CONFIGURATION.md): Configuring UDP sockets, AES-256-GCM keys, TCP TLS 1.3 certificates, and broker options via `PubSubBrokerBuilder`.
- 📊 [**Monitoring Guide**](docs/MONITORING.md): Real-time metrics, per-topic performance tracking, network connectivities status, and REST API endpoints (`/api/stats`, `/api/status`).
- 🎮 [**Management Guide**](docs/MANAGEMENT.md): Managing dynamic topic routing rules, using the Web Manager UI, and sending messages via API/CLI.

---

## 🚀 Quick Start Example

### 1. Build and Run Tests
```bash
cargo build --release
cargo test
```

### 2. Basic Code Example
```rust
use app_util::pubsub::{PubSubBroker, TransportMode, SymmetricKey, generate_self_signed_tls_config};
use std::net::SocketAddr;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Generate AES key for Intra-LAN UDP payload encryption
    let shared_key = SymmetricKey::generate();

    // Construct broker using the Builder Pattern
    let broker = PubSubBroker::builder()
        .node_id("gateway-node-1")
        .bind_udp("0.0.0.0:18801".parse()?)
        .udp_multicast("239.255.0.1:18803".parse()?)
        .enable_broadcast(true)
        .udp_symmetric_key(shared_key.0)
        .bind_tcp_cluster("0.0.0.0:19901".parse()?)
        .build()
        .await?;

    // Subscribe to matching topics (Observer Pattern)
    let mut sub = broker.subscribe("sensors/+/temperature").await;

    // Publish a message over LAN Multicast
    broker.publish(
        "sensors/room1/temperature",
        b"24.5C".to_vec(),
        TransportMode::LanMulticast {
            group_addr: "239.255.0.1:18803".parse()?,
        },
    ).await?;

    // Receive reactive event
    if let Some(msg) = sub.rx.recv().await {
        println!("Received on topic {}: {}", msg.topic, String::from_utf8_lossy(&msg.payload));
    }

    Ok(())
}
```

---

## 🏛️ Architecture & 5 Applied Design Patterns

1. **Builder Pattern** (`PubSubBrokerBuilder`): Step-by-step broker construction.
2. **Factory Pattern** (`MessageFactory`): Centralized creation of standard `Message` frames.
3. **Strategy Pattern** (`TransportStrategy` & `TransportContext`): Runtime selection between UDP and TCP transport strategies.
4. **Chain of Responsibility** (`MessagePipeline` & `MessageFilter`): Sequential processing (`HopLimitFilter` -> `DeduplicationFilter`).
5. **Observer Pattern** (`ObserverRegistry` & `TopicObserver`): Reactive topic subscriber event notification.

---

## 📄 License
MIT License.
