use sqlx::sqlite::{SqliteConnectOptions, SqlitePool, SqlitePoolOptions};
use std::str::FromStr;

#[allow(dead_code)]
pub async fn create_sqlite_conn_pool(db_url: &str) -> SqlitePool {
    let options = SqliteConnectOptions::from_str(db_url)
        .unwrap_or_else(|_| SqliteConnectOptions::new())
        .create_if_missing(true);

    let result = SqlitePoolOptions::new()
        .max_connections(5)
        .connect_with(options)
        .await;

    match result {
        Ok(pool) => pool,
        Err(err) => {
            eprintln!("fail to create_sqlite_conn_pool. err = {}", err);
            std::process::exit(1);
        }
    }
}
