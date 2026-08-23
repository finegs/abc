# 🌐 How to Launch Intra/Inter Clustered Tier via CLI (`launch_clusterd_tier_cli.md`)

This guide provides step-by-step instructions and command lines to start and operate the **Intra-LAN (UDP)** and **Inter-LAN (TCP Cluster Gateway Bridge)** clustered tiers directly from the Command Line Interface (CLI).

---

## 🏛️ 1. Cluster Tier Networking Overview

The Pub/Sub engine features dual-layer clustered networking:

| Clustered Tier | Transport Layer | Protocol & Security | Use Cases | Default Sockets |
| :--- | :--- | :--- | :--- | :--- |
| **Intra-LAN Tier** | UDP Unicast | Direct Socket + AES-256-GCM AEAD | High-speed point-to-point node messaging | `127.0.0.1:18801` |
| **Intra-LAN Tier** | UDP IPv4 Multicast | IGMP Group (`239.255.0.1:18803`) | Efficient one-to-many LAN streaming | `239.255.0.1:18803` |
| **Intra-LAN Tier** | UDP Broadcast | Subnet Broadcast (`18805`) | Local node auto-discovery & network sync | Port `18805` |
| **Inter-LAN Tier** | TCP Cluster Bridge | TLS 1.3 Encryption + Length-Prefixed Frame | Cross-LAN router gateway bridging | `0.0.0.0:19901` |

---

## 🚀 2. Start Central Broker Node & Web Manager

Before launching CLI publisher nodes, start the central broker daemon which binds both Intra-LAN UDP sockets and Inter-LAN TCP TLS 1.3 cluster gateways:

```bash
cargo run --bin app_main
```
* **Web Dashboard**: [http://localhost:28888](http://localhost:28888)
* **Broker Ports**: UDP Unicast (`18801`), UDP Multicast (`18803`), UDP Broadcast (`18805`), TCP Cluster TLS 1.3 (`19901`).

---

## 📡 3. Intra-LAN Clustered Tier CLI Launch Commands

### 3.1 UDP Unicast Tier (Point-to-Point)
Launch a direct UDP unicast publisher stream targeting the central broker:
```bash
cargo run --bin cli_client -- \
  --transport unicast \
  --target-addr 127.0.0.1:18801 \
  --topic "sensors/unicast/temperature" \
  --payload "temp_data_seq_#" \
  --period 100 \
  --unit ms \
  --end-type count \
  --end-val 50
```

### 3.2 UDP IPv4 Multicast Tier (Group Stream)
Launch an IPv4 multicast stream targeting multicast group `239.255.0.1:18803`:
```bash
cargo run --bin cli_client -- \
  --transport multicast \
  --topic "lan/multicast/alerts" \
  --payload "multicast_alert_payload_#" \
  --period 500 \
  --unit ms \
  --end-type none
```

### 3.3 UDP Subnet Broadcast Tier (Subnet Discovery)
Launch a subnet broadcast stream targeting port `18805`:
```bash
cargo run --bin cli_client -- \
  --transport broadcast \
  --topic "discovery/subnet/nodes" \
  --payload "heartbeat_beacon_#" \
  --period 1 \
  --unit sec \
  --end-type count \
  --end-val 10
```

---

## 🛡️ 4. Inter-LAN Cross-LAN Clustered Tier CLI Commands

### 4.1 TCP Cluster Gateway Bridge (TLS 1.3 Encryption)
Launch a cross-LAN TCP cluster publisher encrypted with **TLS 1.3**:
```bash
cargo run --bin cli_client -- \
  --transport tcp_cluster \
  --topic "cross_lan/gateway/transactions" \
  --payload "encrypted_tx_record_#" \
  --period 50 \
  --unit ms \
  --end-type count \
  --end-val 200
```

### 4.2 Keyword Termination Stream Command
Launch a TCP cluster stream that automatically terminates when a specific keyword payload is published:
```bash
cargo run --bin cli_client -- \
  --transport tcp_cluster \
  --topic "cross_lan/orders" \
  --payload "order_status_#" \
  --period 200 \
  --unit ms \
  --end-type keyword \
  --end-val "order_status_25"
```

---

## ⚡ 5. High-Frequency Benchmark Sequence Stream CLI

To test maximum message throughput (10ms period / 100 msg/sec up to 1,000,000 sequence messages):
```bash
cargo run --bin seq_client
```

---

## 📋 6. CLI Parameter Reference Guide

| Parameter | Short | Description | Default Value | Example |
| :--- | :--- | :--- | :--- | :--- |
| `--topic` | `-t` | Pub/Sub topic string | `"t#1"` | `-t "sensors/room1"` |
| `--payload` | `-p` | Payload template (`#` = auto-incrementing seq) | `"m#"` | `-p "message_seq_#"` |
| `--period` | N/A | Stream publishing interval number | `10` | `--period 50` |
| `--unit` | N/A | Interval unit (`ms` or `sec`) | `"ms"` | `--unit sec` |
| `--transport` | N/A | Transport mode (`unicast`, `multicast`, `broadcast`, `tcp_cluster`) | `"unicast"` | `--transport tcp_cluster` |
| `--target-addr` | N/A | Target IP:PORT for UDP Unicast | `"127.0.0.1:18801"` | `--target-addr 127.0.0.1:18801` |
| `--end-type` | N/A | Stream termination rule (`none`, `count`, `keyword`, `datetime`) | `"none"` | `--end-type count` |
| `--end-val` | N/A | Target value for end condition | `""` | `--end-val 500` |

---
*Created for terminal workflow & cluster tier operation.*
