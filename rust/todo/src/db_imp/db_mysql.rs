use sqlx::mysql::MySqlPoolOptions;

pub async fn create_conn_pool(db_url: &str) -> sqlx::MySqlPool {
    match MySqlPoolOptions::new()
        .max_connections(10)
        .connect(db_url)
        .await
    {
        Ok(pool) => {
            println!("✅ Connection to MySQL database is successful!");
            pool
        }
        Err(err) => {
            eprintln!("❌ Failed to connect to MySQL database: {:?}", err);
            std::process::exit(1);
        }
    }
}
