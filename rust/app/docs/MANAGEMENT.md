# 🎮 Management Guide

This guide describes how to manage topic routing rules, trigger test message publications, and toggle pipeline execution via the Web UI and REST API.

---

## 1. Managing Routing Rules via Web UI

1. Open `http://<node-ip>:28888` in your browser.
2. Click **"+ Add Rule"**.
3. Fill in the modal fields:
   - **Rule Name**: e.g. `Filter High Temp Alerts`.
   - **Source Pattern**: e.g. `sensors/+/temp` (supports single `+` and multi `#` level wildcards).
   - **Target Topic**: (Optional) e.g. `alerts/temperature`.
   - **Target Transport**: Select `LAN UDP Unicast`, `Multicast`, `Broadcast`, or `Cross-LAN TCP Cluster Bridge`.
   - **Priority**: Higher numeric value evaluates earlier in the pipeline.
4. Click **Save Routing Rule**.

---

## 2. Managing Routing Rules via REST API

### Create Rule (`POST /api/rules`)
```bash
curl -X POST http://localhost:28888/api/rules \
  -H "Content-Type: application/json" \
  -d '{
    "name": "Cluster Forwarder",
    "source_topic_pattern": "global/#",
    "target_topic": null,
    "target_transport": "TcpCluster",
    "priority": 100
  }'
```

### Toggle Rule Enable/Disable (`POST /api/rules/:id/toggle`)
```bash
curl -X POST http://localhost:28888/api/rules/rule-1722000000/toggle
```

### Delete Rule (`DELETE /api/rules/:id`)
```bash
curl -X DELETE http://localhost:28888/api/rules/rule-1722000000
```

---

## 3. Dispatching Messages via API

You can trigger a message publish from the Web Manager REST API:

```bash
curl -X POST http://localhost:28888/api/publish \
  -H "Content-Type: application/json" \
  -d '{
    "topic": "sensors/bedroom/temperature",
    "payload": "24.5C",
    "transport_mode": {
      "LanMulticast": {
        "group_addr": "239.255.0.1:18803"
      }
    }
  }'
```
