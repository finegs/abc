use sqlx::sqlite::{self as sqlite, SqlitePool, SqlitePoolOptions};

async fn create_conn_pool(db_url: &str) -> sqlite::SqlitePool {
    // url :  \"sqlite::mybd.db\"
    let  result = SqlitePoolOptions::new()
        .max_connections(5)
        .connect(db_url).await;

    match result {
        Ok(pool) => pool,
        Err(err) => {
            eprintln!("fail to  create_sqlite_conn_pool. err = {}", err);
            std::process::exit(1);
        }
    }
}
