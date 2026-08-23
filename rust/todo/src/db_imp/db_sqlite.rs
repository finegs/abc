use sqlx::sqlite::SqlitePoolOptions;

pub async fn create_conn_pool(db_url: &str) -> sqlx::SqlitePool {
    match SqlitePoolOptions::new()
        .max_connections(5)
        .connect(db_url)
        .await
    {
        Ok(pool) => {
            println!("✅ Connection to SQLite database is successful!");
            pool
        }
        Err(err) => {
            eprintln!("❌ Failed to connect to SQLite database: {}", err);
            std::process::exit(1);
        }
    }
}
