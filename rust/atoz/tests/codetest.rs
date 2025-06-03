use std::fmt::{write, Display};
use std::sync::mpsc;
use std::thread;

use atoz::log;
use chrono;
use serde::{Deserialize, Serialize};
use serde_json::Result;

#[test]
fn test_snd_rcv_test_prd_cons() {
    use serde::{Deserialize, Serialize};
    use serde_json::{self, Result};

    use crate::log;

    #[derive(Serialize, Deserialize)]
    struct Msg {
        msg: String,
        ts: u128,
    }

    impl Msg {
        fn new(msg: &str) -> Msg {
            Msg {
                msg: msg.to_string(),
                ts: log::now_u128(),
            }
        }
    }

    impl Display for Msg {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "{}", serde_json::to_string(self))
        }
    }

    // Create a channel
    let (tx, rx) = mpsc::channel();

    // Create a thread
    let thread = thread::spawn(move || {
        // Receive the message
        let message = rx.recv().unwrap();
        println!("Received: {}", message);
    });

    for idx in (1..10) {
        tx.send(Msg::new(format!("[{}]", idx)));
    }
    // Send a message
    tx.send("Hello from main thread!".to_string()).unwrap();

    // Wait for the thread to finish
    thread.join().unwrap();
}
