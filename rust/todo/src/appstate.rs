#[cfg(feature="sqlite")]
use sqlx::SqlitePool;

#[derive(Debug)]
#[cfg(feature="sqlite")]
struct AppState {
    db_conn: sqlx::SqlitePool,
}

#[cfg(feature="mysql")]
use sqlx::MySqlPool;

#[derive(Debug)]
#[cfg(feature="mysql")]
struct AppState {
    db_conn: sqlx::MySqlPool,
}
