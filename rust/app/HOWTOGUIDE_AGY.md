# 🚀 Network Pub/Sub & Web Manager - HOW-TO GUIDE (`HOWTOGUIDE_AGY.md`)

This compact reference guide documents all features, architecture patterns, layout modes, filter profile CRUD workflows, real-time metrics plot charts, and commands for the **Network Pub/Sub Engine & Web Manager Dashboard**.

---

## 🛠️ 1. Quick Start & Terminal Commands

### Run Unit & Integration Test Suite (23 Tests)
```bash
cargo test
```

### Launch Web Manager Dashboard & Backend Server
```bash
cargo run --bin app_main
```
* **Web Dashboard URL**: [http://localhost:28888](http://localhost:28888) or [http://127.0.0.1:28888](http://127.0.0.1:28888)
* **LAN UDP Socket**: `0.0.0.0:18801` (Multicast: `239.255.0.1:18803`, Broadcast: Port `18805`, Encryption: AES-256-GCM)
* **Cross-LAN TCP Cluster**: `0.0.0.0:19901` (TLS 1.3 Enabled)

---

## 💬 2. Client Tab Dynamic Grid & Task Tree Layout Modes

### 📂 Task Tree Hierarchy (`#sidebar-task-tree-view`)
* **`📂 Running (N)`**: Active background stream tasks. Includes `📋` (Table Layout) and `田` (Grid Layout) quick view icons.
* **`📂 Named Tasks (N)`**: Saved reusable stream task configurations (Icon-only `🚀` direct run button).
* **`📂 Profiled Tasks (N)`**: Grouped transport profile tasks (`Unicast`, `Multicast`, `Broadcast`, `TcpCluster`).

### ⚡ Click Reactivity Enhancements
* **Instant Mouse Press (`onmousedown`)**: Tree items react instantly on mouse press without waiting for mouse release.
* **Throttled DOM Tear-Down**: Sidebar tree DOM remains stable during fast streaming pulses (e.g. 10ms ticks) to prevent destroyed DOM elements mid-click.

### 📐 Dynamic Grid State Rules (`#client-tab-grid`)

| Selected Node Type | Publish/Send Area (`#publish-send-panel`) | Scrolled Console (`#scrolled-console-panel`) | Layout Grid State Class |
| :--- | :--- | :--- | :--- |
| **Running Task** | **Deleted / Hidden** (`display: none !important`) | **100% Full Width** (`display: block !important`) | `.running-task-active` |
| **Show All Running Tasks** | **Deleted / Hidden** (`display: none !important`) | **100% Full Width** (`display: block !important`) | `.running-task-active` |
| **Named / Profiled Task** | **100% Full Width** (`display: block !important`) | **Hidden** (`display: none !important`) | `.console-hidden` |
| **Deselected (Default)** | **Restored Full View** | **Collapsed / Default View** | `Default Grid` |

---

## 📊 3. Show All Running Tasks (Table & Grid Layouts)

* **Trigger**: Click `📋` or `田` next to `Running` in the left sidebar, or toggle view buttons inside `#client-inline-task-manager`.
* **Table View Layout (`📋`)**: Displays all background stream tasks in a full-width responsive HTML data table (Task Name, Topic, Transport, Period, Sent Count, End Condition, Status, Controls).
* **Grid View Layout (`田`)**: Displays active tasks as interactive visual cards in a CSS grid (`grid-template-columns: repeat(auto-fill, minmax(280px, 1fr))`).

---

## 🎨 4. Profile Selector & Name Input Field

* **Ultra-Compact Pulldown (`▾`)**: `#main-profile-select` and `#overlay-profile-select` compacted to 36px width showing a compact dropdown arrow.
* **Editable Profile Name Field**: Text input (`#main-profile-name-input` / `#overlay-profile-name-input`) positioned adjacent to the pulldown selector. Pre-fills on selection and syncs live updates to `localStorage` (`pubsub_send_profiles`).

---

## 📟 5. Global Audit & Performance Console Filter Profiles CRUD

The bottom log console toolbar (`#console-filter-toolbar`) includes persistent **Filter Profiles CRUD**:

### Preset & User Filter Profiles
* `f_all`: `🌐 All Logs (No Filter)` (`keyword: '', unit: 'ALL'`)
* `f_tasks`: `📋 Background Stream Tasks` (`unit: 'TASK_MANAGER'`)
* `f_rules`: `⚙️ Topic Routing Rules` (`unit: 'RULE_ENGINE'`)
* `f_pub`: `📤 Client Publications` (`unit: 'CLIENT_PUB'`)
* `f_errors`: `🚨 System Alerts & Errors` (`keyword: 'ERROR'`, `unit: 'ALL'`)
* `user_f_*`: Custom user-created filter profiles.

### Filter Profile CRUD Controls
* **Select Profile**: Pulldown menu (`#console-filter-profile-select`).
* **Edit Profile Name**: Live text field (`#console-filter-profile-name-input`).
* **`💾 Save Profile`**: Saves current keyword search and unit filter to `localStorage` (`pubsub_console_filter_profiles`).
* **`🗑️ Delete Profile`**: Removes selected custom profile.

---

## 📈 6. Real-Time Dashboard Plot Chart Specifications

Located on the **Dashboard Tab** (`#tab-dashboard`):

* **X-Axis**: Time in seconds (`HH:MM:SS`) over a 60-second rolling window.
* **Left Y-Axis (Send Throughput)**:
  * **Send Rate (`send msg cnt / 1 sec`)**: Solid Cyan line (`#00F2FE`) with gradient area fill.
  * **Avg Send Rate (`avg send msg cnt / 1 sec`)**: Dotted Purple line (`#C084FC`).
* **Right Y-Axis (Receive Throughput)**:
  * **Recv Rate (`recv msg cnt / 1 sec`)**: Solid Emerald Green line (`#10B981`) with gradient area fill.
  * **Avg Recv Rate (`avg recv msg cnt / 1 sec`)**: Dotted Amber line (`#F59E0B`).
* **Controls**: `🔄 Reset Chart` button to reset cumulative sample averages on demand.

---

## 🎨 7. Dashboard Top Status Widgets

High-readability themed widget cards on the Dashboard landing page:

1. **Outbound Stream (`📤 Sent`)**: Cyan glassmorphism card (`#00F2FE` gradient top accent border, bold sent message count, bytes transmitted).
2. **Inbound Traffic (`📥 Received`)**: Emerald Green glassmorphism card (`#10B981` gradient top accent border, bold received message count, bytes received).
3. **Pub/Sub Channels (`🎯 Topics`)**: Purple glassmorphism card (`#C084FC` gradient top accent border, active recorded topics count).
4. **Pipeline Engine (`⚙️ Rules`)**: Amber glassmorphism card (`#FBBF24` gradient top accent border, active pipeline routing rules count).

---

## 💾 8. LocalStorage Persistence Keys Summary

| LocalStorage Key | Purpose |
| :--- | :--- |
| `pubsub_send_profiles` | User-defined custom sending profiles |
| `pubsub_named_tasks` | Saved named task definitions |
| `pubsub_console_filter_profiles` | Custom console log filter profiles |
| `pubsub_active_console_filter_profile_id` | Currently selected console filter profile ID |
| `pubsub_console_filters` | Keyword search text, unit filter, and filter toolbar visibility |
| `pubsub_topology_node_positions` | Custom interactive topology drag positions |
| `pubsub_topology_profiles` | Interactive topology layout profiles |

---
*Created for future reuse & system maintenance.*
