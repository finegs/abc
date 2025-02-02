
use dotenv::dotenv;
use std::env;

use sqlx::{sqlite::SqlitePoolOptions};

pub mod book_models;

#[tokio::test]
// #[tokio::test(flavor = "multi_thread", worker_threads = 2)]
async fn sqlite_db_create_test() -> Result<(), sqlx::Error> {
    dotenv().ok();

    // read DATABASE_URL
    let database_url = env::var("DATABASE_URL").expect("DATABASE_URL must be set");

    // Create Connection Pool
    let  pool = SqlitePoolOptions::new()
        .connect(&database_url)
        .await?;


    // Migrate
    sqlx::migrate!().run(&pool).await?;

    Ok(())
}
