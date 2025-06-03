#[macro_export]
macro_rules! log {
    ( $( $arg:expr ),* ) => {
        println!("{}", tslog!($($arg),*));
    };
}

#[macro_export]
macro_rules! tslog {
    ( $( $arg:expr ),* ) => {

        {
            use chrono;
            use std::thread;

            let now = chrono::Local::now();
            format!(
                "{} - [{:?}] : {}",
                now.format("%Y-%m-%d %H:%M:%S%.6f"),
                thread::current().id(),
                format!($($arg),*)
            )
        }
    };
}
