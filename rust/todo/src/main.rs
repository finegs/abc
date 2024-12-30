mod model;
mod schema;
mod handler;

use std::sync::Arc;

use axum::{response::IntoResponse, routing::get, Json, Router};
use tokio::net::TcpListener;

use dotenv::dotenv;
use sqlx::{database, mysql::{MySqlPool, MySqlPoolOptions}};

pub struct AppState {
    db: MySqlPool,
}

#[tokio::main]
async fn main()  {
    dotenv().ok();

    println!("### Rest API Service ###");


    let database_url =  std::env::var("DATABASE_URL").expect("DATABASE_URL must set");
    let pool = match MySqlPoolOptions::new()
        .max_connections(10)
        .connect(&database_url)
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
    };

    let app =  Router::new()
                .route("/api/healthcheck", get(health_check_handler))
                .with_state(Arc::new(AppState{db:  pool.clone() }));

    println!("✅  Server started successfully at 0.0.0.0:8080");

    let listener  =  TcpListener::bind("0.0.0.0:8080").await.unwrap();
    axum::serve(listener, app.into_make_service())
        .await
        .unwrap();
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
