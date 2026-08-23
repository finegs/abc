#[cfg_attr(feature = "sqlite", path = "db_imp/db_sqlite.rs")]
#[cfg_attr(all(feature = "mysql", not(feature = "sqlite")), path = "db_imp/db_mysql.rs")]
mod db;

pub use db::create_conn_pool;
