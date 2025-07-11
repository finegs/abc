// #[macro_export]
// macro_rules! log {
//     ( $( $arg:expr ),* ) => {
//         println!("{}", tslog!($($arg),*));
//     };
// }

// #[macro_export]
// macro_rules! elog {
//     ( $( $arg:expr ),* ) => {
//         eprintln!("{}", tslog!($($arg),*));
//     };
// }

#[macro_export]
macro_rules! tslog {
    ($level:tt, $( $arg:expr ),* ) => {

        {
            use chrono;
            use std::thread;

            let now = chrono::Local::now();
            format!(
                "{} - [{:?}] [{}] : {}",
                now.format("%Y-%m-%d %H:%M:%S%.6f"),
                thread::current().id(),
                $level,
                format!($($arg),*)
            )
        }
    };
}

#[macro_export]
macro_rules! log {
    ($level:tt, $( $arg:expr ),* ) => {
        println!("{}", tslog!($level, $($arg),*));
    };
}

#[macro_export]
macro_rules! elog {
    ($level:tt, $( $arg:expr ),* ) => {
        eprintln!("{}", tslog!($level, $($arg),*));
    };
}

#[macro_export]
macro_rules! log_debug {
    ($( $arg:expr ),* ) => {
        log!("D", $($arg),*)
    };
}

#[macro_export]
macro_rules! log_trace {
    ($( $arg:expr ),* ) => {
        log!("T", $($arg),*)
    };
}

#[macro_export]
macro_rules! log_info {
    ($( $arg:expr ),* ) => {
        log!("I", $($arg),*)
    };
}

#[macro_export]
macro_rules! log_err {
    ($( $arg:expr ),* ) => {
        elog!("E", $($arg),*)
    };
}

#[macro_export]
macro_rules! log_fatal {
    ($( $arg:expr ),* ) => {
        elog!("F", $($arg),*)
    };
}
