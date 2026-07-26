# 🛠️ Installation Guide

This guide describes the system requirements, Cargo dependencies, compilation process, and installation of the Rust Pub/Sub Library & Web Manager.

---

## 📋 System Requirements

- **Operating System**: Linux (Ubuntu 20.04+, Debian, Fedora, RHEL), macOS, or Windows (10/11 / Server).
- **Rust Toolchain**: Rust 1.75.0 or newer with `cargo` installed.
- **Networking**:
  - Open UDP ports for LAN Unicast/Multicast/Broadcast (e.g. `18801-18805`).
  - Open TCP ports for Inter-LAN TLS Cluster Gateways (e.g. `19901-19902`).
  - Open TCP port for Web Manager Dashboard (e.g. `28888` / `8080`).

---

## 📦 Dependencies Setup

Add the required crate dependencies to your project's `Cargo.toml`:

```toml
[dependencies]
tokio = { version = "1.0", features = ["full"] }
serde = { version = "1.0", features = ["derive"] }
serde_json = "1.0"
socket2 = "0.5"
rustls = { version = "0.23", features = ["ring"] }
tokio-rustls = { version = "0.26", features = ["ring"] }
rustls-pki-types = "1.0"
rcgen = "0.13"
aes-gcm = "0.10"
axum = "0.7"
tower-http = { version = "0.5", features = ["cors", "fs"] }
async-trait = "0.1"
```

---

## 🔨 Building and Compiling

### 1. Clone & Build Library
```bash
# Check code syntax and dependencies
cargo check

# Build debug binary & library
cargo build

# Build optimized release build
cargo build --release
```

### 2. Running Test Suite
Execute the full test suite comprising unit tests and integration tests:

```bash
# Run all unit and integration tests
cargo test

# Run tests with output logs enabled
cargo test -- --nocapture
```

---

## 🌐 Running the Web Manager Standalone

To launch a node with the embedded Web Manager interface:

```rust
use app_util::pubsub::{start_web_manager, RuleEngine, PubSubBroker};
use std::sync::atomic::AtomicU64;
use std::sync::Arc;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let broker = PubSubBroker::builder()
        .node_id("production-node-1")
        .bind_udp("0.0.0.0:18801".parse()?)
        .bind_tcp_cluster("0.0.0.0:19901".parse()?)
        .build()
        .await?;

    let rule_engine = RuleEngine::new();
    let counter = Arc::new(AtomicU64::new(0));

    // Start Web Manager on port 28888
    start_web_manager("0.0.0.0:28888".parse()?, broker, rule_engine, counter).await?;

    println!("Web Manager UI running at http://localhost:28888");
    tokio::signal::ctrl_c().await?;
    Ok(())
}
```
