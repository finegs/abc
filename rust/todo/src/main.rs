mod appstate;
mod handler;
#[allow(unused)]
mod model;
mod schema;

use std::sync::Arc;

use axum::{routing::get, Router};
use tokio::net::TcpListener;

use dotenv::dotenv;

use appstate::AppState;
use handler::{
    create_note_handler, delete_note_handler, edit_note_handler, get_note_handler,
    health_check_handler, note_list_handle,
};

// TODO : check crud with  db (CRUD  BOOK)[https://wolfy.tistory.com/335]
// FIXME:

#[tokio::main]
async fn main() {
    dotenv().ok();

    println!("### Rest API Service ###");

    let database_url = std::env::var("DATABASE_URL").expect("DATABASE_URL must set");

    #[cfg(any(feature = "mysql", feature = "sqlite"))]
    {
        #[cfg(feature = "mysql")]
        let pool = create_mysql_conn_pool(database_url.as_str()).await;

        #[cfg(feature = "sqlite")]
        let pool = create_sqlite_conn_pool(database_url.as_str()).await;

        #[cfg(any(feature = "mysql", feature = "sqlite"))]
        let app = Router::new()
            .route("/api/healthcheck", get(health_check_handler))
            .route("/api/notes", get(note_list_handle).post(create_note_handler))
            .route(
                "/api/notes/:id",
                get(get_note_handler)
                    .patch(edit_note_handler)
                    .delete(delete_note_handler),
            )
            .with_state(Arc::new(AppState {
                conn_pool: pool.clone(),
            }));

        println!("✅  Server started successfully at 0.0.0.0:8080");

        let listener = TcpListener::bind("0.0.0.0:8080").await.unwrap();
        axum::serve(listener, app.into_make_service())
            .await
            .unwrap();
    }
}

#[cfg(feature = "mysql")]
async fn create_mysql_conn_pool(db_url: &str) -> sqlx::mysql::MySqlPool {
    match sqlx::mysql::MySqlPoolOptions::new()
        .max_connections(10)
        .connect(&db_url)
        .await
    {
        Ok(pool) => {
            println!("✅ Connection to the database is successful!");
            pool
        }
        Err(err) => {
            eprintln!("❌ Failed to connect to the database: {:?}", err);
            std::process::exit(1);
        }
    }
}

#[cfg(feature = "sqlite")]
async fn create_sqlite_conn_pool(db_url: &str) -> sqlx::sqlite::SqlitePool {
    // let db_url =  "sqlite::mydb.db";
    match sqlx::sqlite::SqlitePool::connect(&db_url).await {
        Ok(pool) => {
            println!("✅ Connection to the database is successful!");
            pool
        }
        Err(err) => {
            eprintln!("fail to  create_sqlite_conn_pool. err = {}", err);
            std::process::exit(1);
        }
    }
}
