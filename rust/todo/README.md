# 📝 Rust Axum + SQLx Todo REST API

A high-performance, asynchronous RESTful API service built in Rust using **Axum** and **SQLx**. This project provides full CRUD operations for Notes/Todos and Books, featuring dual-database support for **SQLite** (default) and **MySQL**, comprehensive schema validations, and a full unit & integration test suite.

---

## ✨ Features

- **🏗️ 5 Major Design Patterns**: Built using Repository, Factory, Builder, Strategy, and Singleton/Shared State patterns.
- **⚡ Fast & Async**: Built on top of [Tokio](https://tokio.rs/) runtime and [Axum](https://github.com/tokio-rs/axum) web framework.
- **🗄️ Multi-Database Support**: Cargo feature flags allowing easy switching between **SQLite** (default) and **MySQL**.
- **📋 RESTful CRUD Endpoints**: Full CRUD for Notes (Create, Read, Update, Delete, List with pagination) and Health Checks.
- **🛡️ Type-Safe Querying**: SQL query execution powered by [SQLx](https://github.com/launchbadge/sqlx).
- **🧪 Complete Test Suite**: Comprehensive unit tests and integration tests with in-memory SQLite support.
- **📦 Docker Ready**: Includes `docker-compose.yml` for local MySQL database setup.

---

## 🏛️ 5 Major Design Patterns

| Pattern | Module | Purpose & Implementation |
| :--- | :--- | :--- |
| **1. Repository Pattern** | [`src/patterns/repository.rs`](file:///home/user01/dev/git/abc/rust/todo/src/patterns/repository.rs) | Abstracts database persistence behind `NoteRepository` trait (`SqliteNoteRepository`), decoupling business logic from raw SQL execution. |
| **2. Factory Pattern** | [`src/patterns/factory.rs`](file:///home/user01/dev/git/abc/rust/todo/src/patterns/factory.rs) | Encapsulates repository creation logic via `RepositoryFactory::create_sqlite_repository()`. |
| **3. Builder Pattern** | [`src/patterns/builder.rs`](file:///home/user01/dev/git/abc/rust/todo/src/patterns/builder.rs) | Provides fluent builder API (`NoteBuilder`, `CreateNoteSchemaBuilder`) for constructing entities and request DTOs. |
| **4. Strategy Pattern** | [`src/patterns/strategy.rs`](file:///home/user01/dev/git/abc/rust/todo/src/patterns/strategy.rs) | Encapsulates pagination calculation algorithms (`DefaultPaginationStrategy`) behind `PaginationStrategy` interface. |
| **5. Singleton / Shared State** | [`src/appstate.rs`](file:///home/user01/dev/git/abc/rust/todo/src/appstate.rs) | Manages thread-safe global application state (`Arc<AppState>`) injected into Axum web handlers. |

---

## 🛠️ Tech Stack

| Component | Technology | Description |
| :--- | :--- | :--- |
| **Language** | Rust (2021 Edition) | Safe, concurrent, and high-performance system programming |
| **Web Framework** | [Axum](https://github.com/tokio-rs/axum) | Ergonomic and modular web framework built with Tower and Hyper |
| **Async Runtime** | [Tokio](https://tokio.rs) | Event-driven non-blocking I/O platform |
| **Database & ORM** | [SQLx](https://github.com/launchbadge/sqlx) | Async, compile-time checked SQL toolkit |
| **Serialization** | Serde / Serde JSON | JSON request parsing and response rendering |
| **Utilities** | Chrono, UUID, dotenv | Timestamp handling, UUID v4 generation, environment configs |

---

## 📁 Project Structure

```text
rust/todo/
├── src/
│   ├── main.rs          # Server initialization & Axum router configuration
│   ├── appstate.rs      # Application state container (AppState)
│   ├── handler.rs       # HTTP request handlers (Healthcheck & Note CRUD)
│   ├── model.rs         # Database entities & JSON response models
│   ├── schema.rs        # Request payload & filter schemas
│   ├── db.rs            # Database abstraction module
│   └── db_imp/          # Database connection implementations
│       ├── db_sqlite.rs # SQLite connection pool factory
│       └── db_mysql.rs  # MySQL connection pool factory
├── migrations/          # SQL migration files for SQLx
│   ├── 20241230040552_create_notes_table.up.sql
│   └── 20250202034205_create_books_table.up.sql
├── tests/               # Test Suite
│   ├── note_integration_test.rs # Note API integration tests
│   ├── book_handler.rs          # Book API integration tests
│   ├── book_models.rs           # Book models & structures
│   ├── book_test.rs             # Database migration tests
│   └── codetest.rs              # String parser unit tests
├── .env                 # Environment variables configuration
├── test.http            # HTTP request samples for REST clients
├── docker-compose.yml   # Docker setup for local MySQL instance
├── Cargo.toml           # Package dependencies and feature flags
└── tasks.md             # Task log and verification results
```

---

## ⚙️ Environment Configuration (`.env`)

Create or update the `.env` file in the root directory:

```env
# --- SQLite Configuration (Default) ---
DATABASE_URL=sqlite://db/test.db

# --- MySQL Configuration (Optional Docker Setup) ---
MYSQL_DATABASE=rust_axum_sqlx
MYSQL_USER=user
MYSQL_PASSWORD=password123
MYSQL_ROOT_PASSWORD=password123
# DATABASE_URL=mysql://user:password123@127.0.0.1:3306/rust_axum_sqlx
```

---

## 📡 REST API Reference

### Health Check

| Method | Endpoint | Description |
| :--- | :--- | :--- |
| `GET` | `/api/healthcheck` | Returns service status |

**Response Example (`200 OK`)**:
```json
{
  "status": "ok",
  "message": "API Services"
}
```

---

### Notes API (`/api/notes`)

| Method | Endpoint | Query / Body | Description |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/notes` | `?page=1&limit=10` | Retrieve paginated list of notes |
| `POST` | `/api/notes` | `CreateNoteSchema` | Create a new note |
| `GET` | `/api/notes/:id` | `Path(id)` | Get note details by UUID |
| `PATCH` | `/api/notes/:id` | `UpdateNoteSchema` | Update an existing note |
| `DELETE` | `/api/notes/:id` | `Path(id)` | Delete a note by UUID |

#### Request & Response Examples

- **Create Note (`POST /api/notes`)**:
  ```json
  {
    "title": "Learn Rust Axum",
    "content": "Build RESTful services with Axum and SQLx",
    "is_published": true
  }
  ```

- **Update Note (`PATCH /api/notes/:id`)**:
  ```json
  {
    "title": "Mastering Rust Axum",
    "is_published": true
  }
  ```

- **Get Note List Response (`GET /api/notes?page=1&limit=10`)**:
  ```json
  {
    "status": "ok",
    "count": 1,
    "notes": [
      {
        "id": "c56a4180-65aa-42ec-a945-5fd21dec0538",
        "title": "Learn Rust Axum",
        "content": "Build RESTful services with Axum and SQLx",
        "is_published": 1,
        "created_at": "2026-08-07T12:00:00Z",
        "updated_at": "2026-08-07T12:00:00Z"
      }
    ]
  }
  ```

---

## 🚀 Getting Started

### 1. Prerequisites

- [Rust & Cargo](https://www.rust-lang.org/tools/install) (1.75+ recommended)
- (Optional) [sqlx-cli](https://crates.io/crates/sqlx-cli):
  ```bash
  cargo install sqlx-cli
  ```
- (Optional) [Docker & Docker Compose](https://docs.docker.com/compose/) (if using MySQL)

### 2. Run Database Migrations

For SQLite:
```bash
sqlx migrate run
```

### 3. Run Application

#### Default (SQLite Mode)
```bash
cargo run
```
The server will start on `http://0.0.0.0:8080`.

#### MySQL Mode
1. Start MySQL Docker container:
   ```bash
   docker-compose up -d
   ```
2. Update `.env` to point `DATABASE_URL` to MySQL.
3. Run with MySQL feature flag:
   ```bash
   cargo run --no-default-features --features mysql
   ```

---

## 🧪 Running Tests

The repository includes comprehensive unit tests and integration tests.

### Run All Tests
```bash
cargo test
```

### Run Specific Test Suite
- **Note Integration Tests**:
  ```bash
  cargo test --test note_integration_test
  ```
- **Book Integration Tests**:
  ```bash
  cargo test --test book_handler
  ```
- **Code Utility Tests**:
  ```bash
  cargo test --test codetest t01 -- --nocapture
  ```

---

## 📄 License

This project is open-source and available under the MIT License.
