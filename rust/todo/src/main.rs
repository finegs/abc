mod model;
mod schema;
mod handler;
mod db;

use std::sync::Arc;

use axum::{response::IntoResponse, routing::get, Json, Router};
use tokio::net::TcpListener;

use dotenv::dotenv;
use sqlx::{database, mysql::{MySqlPool, MySqlPoolOptions}, Database};

pub struct AppState<DB> 
where 
    DB: sqlx::Database
{
    conn_pool: sqlx::Pool<DB>
}

#[tokio::main]
async fn main()  {
    dotenv().ok();

    println!("### Rest API Service ###");

    let database_url =  std::env::var("DATABASE_URL").expect("DATABASE_URL must set");

    #[cfg(any(feature = "mysql", feature = "sqlite"))] 
    {
        #[cfg(feature = "mysql")]
        let pool = create_mysql_conn_pool(database_url.as_str()).await;

        #[cfg(feature = "sqlite")]
        let pool = create_sqlite_conn_pool(database_url.as_str()).await;

        #[cfg(any(feature = "mysql", feature = "sqlite"))]
        let app =  Router::new()
                    .route("/api/healthcheck", get(health_check_handler))
                    .with_state(Arc::new(AppState{ conn_pool: pool.clone() }));

        println!("✅  Server started successfully at 0.0.0.0:8080");

        let listener  =  TcpListener::bind("0.0.0.0:8080").await.unwrap();
        axum::serve(listener, app.into_make_service())
            .await
            .unwrap();
    }
}

pub async fn health_check_handler() -> impl  IntoResponse {
    const MESSAGE: &str =  "API Services";

    let json_response =  serde_json::json!(
        {
            "status": "ok",
            "message": MESSAGE
        }
    );

    Json(json_response)
}

async fn create_mysql_conn_pool(db_url: &str) -> sqlx::mysql::MySqlPool {

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

async fn create_sqlite_conn_pool(db_url: &str) -> sqlx::sqlite::SqlitePool {
    match sqlx::sqlite::SqlitePool::connect("sqlite:mydb.db").await {
        Ok(pool) => pool,
        Err(err) => {
            eprintln!("fail to  create_sqlite_conn_pool. err = {}", err);
            std::process::exit(1);
        }
    }
}
