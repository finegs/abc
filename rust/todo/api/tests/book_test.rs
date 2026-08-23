use dotenv::dotenv;
use sqlx::sqlite::SqlitePoolOptions;

pub mod book_models;

#[tokio::test]
async fn sqlite_db_create_test() -> Result<(), Box<dyn std::error::Error>> {
    dotenv().ok();

    // Use isolated in-memory sqlite database for test
    let pool = SqlitePoolOptions::new()
        .connect("sqlite::memory:")
        .await?;

    sqlx::migrate!().run(&pool).await?;

    Ok(())
}
