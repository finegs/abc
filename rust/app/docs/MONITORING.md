# 📊 Monitoring Guide

This guide describes how to monitor node performance, topic metrics, message throughput, and connectivity status via the REST API and Web Manager UI.

---

## 1. REST API Endpoints

The embedded Rust HTTP server exposes real-time JSON metrics endpoints:

### `GET /api/status`
Returns general node health:
```json
{
  "node_id": "production-node-1",
  "active_rules_count": 3,
  "total_messages_processed": 1420
}
```

### `GET /api/stats`
Returns full system metrics, topic performance, and connectivity status:
```json
{
  "node_id": "production-node-1",
  "sent_messages_total": 1250,
  "recv_messages_total": 1180,
  "sent_bytes_total": 184200,
  "recv_bytes_total": 165400,
  "active_topics_count": 2,
  "topics": [
    {
      "topic": "sensors/room1/temperature",
      "sent_count": 800,
      "recv_count": 750,
      "total_bytes": 120000,
      "last_active_timestamp_ms": 1722000000000
    }
  ],
  "connectivity": {
    "node_id": "production-node-1",
    "udp_bind_addr": "0.0.0.0:18801",
    "udp_multicast_group": "239.255.0.1:18803",
    "udp_broadcast_enabled": true,
    "udp_encryption": "AES-256-GCM AEAD Enabled",
    "tcp_cluster_listen_addr": "0.0.0.0:19901",
    "tcp_cluster_peers": [
      {
        "peer_addr": "192.168.2.10:19902",
        "status": "Configured Peer",
        "protocol": "TCP/TLS 1.3"
      }
    ],
    "tcp_encryption": "TLS 1.3 Standard"
  }
}
```

---

## 2. Web Manager Dashboard Overview

Access `http://<node-ip>:28888` in your web browser:

1. **Header Bar**: Displays Node ID, AES-256-GCM status, and TLS 1.3 indicator.
2. **Top Metrics Cards**: Displays real-time message send/recv counters and total bytes.
3. **Active Topic Performance Table**: Displays per-topic breakdown of traffic and volume.
4. **Network Connectivities Panel**: Displays active UDP bind address, Multicast group, TCP listening address, and configured peer gateways.
