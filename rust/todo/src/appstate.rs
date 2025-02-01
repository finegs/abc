#[allow(unused)]

#[cfg(feature="sqlite")]
use sqlx::SqlitePool;

// #[cfg(feature="mysql")]
// use sqlx::MySqlPool;

#[derive(Debug)]
pub  struct AppState {

    // #[cfg(feature="mysql")]
    // pub conn_pool: sqlx::MySqlPool,

    #[cfg(feature="sqlite")]
    pub conn_pool: sqlx::SqlitePool,
}
