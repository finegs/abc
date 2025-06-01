use chrono;

#[macro_export]
macro_rules! log {
    ($( $arg:expr ),* ) => {
        println!($($arg),*);
    };
}

#[macro_export]
macro_rules! tslog {
    ($($arg:expr),*) => {
        use chrono;
        use std::thread;

        let now = chrono::Local::now();
        format!(
            "{} - [{:?}] : {}",
            now.format("%Y-%m-%D %H:%M:%s.%.06f"),
            thread::current().id(),
            msg
        );
    };
}

#[cfg(test)]
mod tests_log {

    #[test]
    fn test_log() {
        log!("test_log : {}", 1 + 100);
    }

    #[test]
    fn test_tslog() {
        let mm = tslog!(" from test_tslog, calc : {}", 11 + 1000);
        log!(" tslog : {}", mm);
    }
}
