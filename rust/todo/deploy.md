# 🚀 Deployment & Architecture Guide (`deploy.md`)

This document provides a comprehensive overview of the **Rust Axum + SQLx Todo REST API** architecture, database options, container setup, CI/CD pipeline, and production deployment procedures.

---

## 🔄 Docker CI/CD Pipeline (`.github/workflows/ci-cd.yml`)

The repository includes a production-ready GitHub Actions CI/CD pipeline automating test verification and container image deployment:

```mermaid
flowchart LR
    Push["Git Push / PR"] --> CI["Job 1: Test & Lint<br/>cargo check & cargo test"]
    CI -->|Pass| CD["Job 2: Docker Buildx<br/>Build multi-stage image"]
    CD --> Registry["GHCR Registry<br/>ghcr.io/finegs/abc"]
    CD --> Health["Container Health Check<br/>curl http://localhost:8080/api/healthcheck"]
```

### Pipeline Jobs & Workflow
1. **`test-and-check`**: Runs `cargo check` and `cargo test --all-targets --all-features` with Cargo caching (`Swatinem/rust-cache`).
2. **`docker-build`**: Uses `docker/build-push-action` to build a multi-stage Docker image, tests running the container, verifies healthcheck endpoint, and publishes image tags (`latest`, `<sha>`) to GHCR.

---

## 🏗️ System Architecture (Mermaid Diagram)

```mermaid
graph TD
    subgraph Clients["🌐 Clients & API Consumers"]
        Client["HTTP Client / Browser / test.http"]
        TestSuite["Cargo Test Suite"]
    end

    subgraph AxumServer["⚡ Axum Web Server (0.0.0.0:8080)"]
        Router["Router (src/main.rs)"]
        HealthRoute["GET /api/healthcheck"]
        NoteListRoute["GET /api/notes"]
        NoteCreateRoute["POST /api/notes"]
        NoteGetRoute["GET /api/notes/:id"]
        NoteEditRoute["PATCH /api/notes/:id"]
        NoteDeleteRoute["DELETE /api/notes/:id"]
    end

    subgraph HandlerLayer["🧠 Application Core (src/)"]
        AppState["AppState (src/appstate.rs)<br/>Arc<AppState>"]
        Handlers["Request Handlers (src/handler.rs)<br/>• health_check_handler<br/>• note_list_handle<br/>• create_note_handler<br/>• get_note_handler<br/>• edit_note_handler<br/>• delete_note_handler"]
        Schemas["Schemas (src/schema.rs)<br/>• CreateNoteSchema<br/>• UpdateNoteSchema<br/>• FilterOptions"]
        Models["Models (src/model.rs)<br/>• NoteModel (FromRow)<br/>• NoteModelResponse"]
    end

    subgraph DBLayer["🗄️ Persistence Layer (src/db_imp/)"]
        direction LR
        SQLitePool["SqlitePool (db_sqlite.rs)<br/>Default Feature"]
        MySQLPool["MySqlPool (db_mysql.rs)<br/>MySQL Feature"]
    end

    subgraph Databases["💾 Database Infrastructure"]
        SQLiteDB[("SQLite Database<br/>sqlite://db/test.db")]
        MySQLDB[("MySQL Database<br/>docker-compose.yml:3306")]
    end

    %% Client Connections
    Client -->|HTTP Request| Router
    TestSuite -->|In-Memory Test| Router

    %% Route Mappings
    Router --> HealthRoute
    Router --> NoteListRoute
    Router --> NoteCreateRoute
    Router --> NoteGetRoute
    Router --> NoteEditRoute
    Router --> NoteDeleteRoute

    HealthRoute --> Handlers
    NoteListRoute --> Handlers
    NoteCreateRoute --> Handlers
    NoteGetRoute --> Handlers
    NoteEditRoute --> Handlers
    NoteDeleteRoute --> Handlers

    %% Handler Interactions
    Handlers --> Schemas
    Handlers --> Models
    Handlers --> AppState

    %% State to DB
    AppState -->|cfg sqlite| SQLitePool
    AppState -->|cfg mysql| MySQLPool

    SQLitePool -->|SQL Queries| SQLiteDB
    MySQLPool -->|SQL Queries| MySQLDB
```

---

## 🧩 Architectural Components Breakdown

| Layer | Component | File Path | Description |
| :--- | :--- | :--- | :--- |
| **Web Server** | Axum Router | [`src/main.rs`](file:///home/user01/dev/git/abc/rust/todo/src/main.rs) | Initializes Tokio TCP listener, binds routes, and injects `AppState`. |
| **State Management** | AppState | [`src/appstate.rs`](file:///home/user01/dev/git/abc/rust/todo/src/appstate.rs) | Thread-safe state container wrapped in `Arc` holding the active connection pool. |
| **Request Processing** | Handlers | [`src/handler.rs`](file:///home/user01/dev/git/abc/rust/todo/src/handler.rs) | Validates input, executes SQL queries, and converts models to JSON responses. |
| **Schemas** | Data Transfer Objects | [`src/schema.rs`](file:///home/user01/dev/git/abc/rust/todo/src/schema.rs) | Input validation schemas (`CreateNoteSchema`, `UpdateNoteSchema`, `FilterOptions`). |
| **Models** | Entities | [`src/model.rs`](file:///home/user01/dev/git/abc/rust/todo/src/model.rs) | Internal database structures (`NoteModel`) and public API responses (`NoteModelResponse`). |
| **Database Pool** | Factory | [`src/db_imp/`](file:///home/user01/dev/git/abc/rust/todo/src/db_imp) | Conditional compilation modules (`db_sqlite.rs` or `db_mysql.rs`) based on features. |

---

## 🛠️ Production Deployment Options

### Prerequisites
- Linux Server (Ubuntu 22.04 LTS or Debian 12 recommended)
- Rust Toolchain or Docker installed
- Open Port `8080` (or reverse-proxy via Nginx/Caddy)

---

### Option A: Native Linux Service (Systemd + SQLite)

#### 1. Compile Release Binary
```bash
cargo build --release
```
The optimized binary will be created at `target/release/todo`.

#### 2. Create Production Environment File (`/opt/todo/.env`)
```bash
sudo mkdir -p /opt/todo/db
sudo cp target/release/todo /opt/todo/
sudo cp -r migrations /opt/todo/
```

Create `/opt/todo/.env`:
```env
DATABASE_URL=sqlite:///opt/todo/db/todo.db
```

#### 3. Run Database Migrations
```bash
cd /opt/todo
sqlx migrate run
```

#### 4. Configure Systemd Service (`/etc/systemd/system/todo.service`)
```ini
[Unit]
Description=Rust Axum Todo Service
After=network.target

[Service]
Type=simple
User=www-data
WorkingDirectory=/opt/todo
ExecStart=/opt/todo/todo
Restart=always
RestartSec=5
EnvironmentFile=/opt/todo/.env

[Install]
WantedBy=multi-user.target
```

#### 5. Enable and Start Service
```bash
sudo systemctl daemon-reload
sudo systemctl enable todo
sudo systemctl start todo
sudo systemctl status todo
```

---

### Option B: Docker Containerized Deployment (MySQL Mode)

#### 1. Start MySQL Database Container
```bash
docker-compose up -d
```

#### 2. Build Release Binary with MySQL Support
```bash
cargo build --release --no-default-features --features mysql
```

#### 3. Run Migrations & Start Binary
```bash
export DATABASE_URL="mysql://user:password123@127.0.0.1:3306/rust_axum_sqlx"
sqlx migrate run
./target/release/todo
```

---

## 🏥 Health & Monitoring Checks

Verify deployment status via curl or automated healthchecks:

```bash
curl -i http://localhost:8080/api/healthcheck
```

Expected Response (`HTTP/1.1 200 OK`):
```json
{
  "status": "ok",
  "message": "API Services"
}
```

---

## 🛡️ Backup & Security Recommendations

1. **Database Backups (SQLite)**:
   ```bash
   sqlite3 /opt/todo/db/todo.db ".backup '/opt/todo/db/todo_backup.db'"
   ```
2. **Reverse Proxy Configuration**:
   Use Nginx or Caddy to terminate TLS/SSL certificates (Let's Encrypt) and forward requests to `http://127.0.0.1:8080`.
