use std::time::{SystemTime, UNIX_EPOCH};

pub mod log_macro;

pub fn now_u128() -> u128 {
    let now = SystemTime::now();
    let duration_since_epoch = now.duration_since(UNIX_EPOCH).unwrap();
    let seconds = duration_since_epoch.as_secs();
    let nanoseconds = duration_since_epoch.subsec_nanos();
    seconds as u128 * 1_000_000 + (nanoseconds as u128 / 1_000)
}

struct TimestampU128(u128);

trait TimeStampStr {
    fn to_string() -> String;
}

impl TimeStampStr for TimestampU128 {
    fn to_string() -> String {
        todo!()
    }
}

impl From<chrono::DateTime<chrono::Local>> for TimestampU128 {
    fn from(value: chrono::DateTime<chrono::Local>) -> Self {
        TimestampU128(value.timestamp() as u128 * 1000_000u128 + value.timestamp_micros() as u128)
    }
}

#[cfg(test)]
mod tests_log {

    use crate::{log, tslog};

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
