
use sqlx::mysql::{self, MySqlPool, MySqlPoolOptions};

async fn create_conn_pool(db_url: &str) -> sqlx::mysql::MySqlPool {

    match MySqlPoolOptions::new()
        .max_connections(10)
        .connect(&db_url)
        .await
    {
        Ok(pool) => {
            println!("✅ Connection to the database is successful!");
            pool
        },
        Err(err) => {
            eprintln!("❌ Failed to connect to the database: {:?}", err);
            std::process::exit(1);
        }
    }
}
