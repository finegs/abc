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

#[cfg(test)]
mod tests_log {

    #[test]
    fn test_dateformat() {
        let now = chrono::Local::now();
        println!(
            "[{}] test_dateformat is printed.",
            now.format("%Y-%m-%d %H:%M:%S%.6f")
        )
    }

    #[test]
    fn test_log() {
        log!("test_log : {}", 1 + 100);
    }

    #[test]
    fn test_tslog() {
        let msg = tslog!(" from test_tslog, calc : {}", 11 + 1000);
        log!("[{}]", msg);

        assert!(!msg.is_empty(), "tslog! must format with timestamp");
    }
}
