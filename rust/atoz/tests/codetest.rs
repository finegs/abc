use std::fmt::Display;
use std::sync::atomic::Ordering;
use std::sync::{atomic, mpsc};
use std::thread;

use atoz::{elog, log, tslog};
use serde::{Deserialize, Serialize};

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
                msg: msg.to_owned(),
                ts: log::now_u128(),
            }
        }

        fn is_terminate_cmd(&self) -> bool {
            matches!(
                self.msg.as_str(),
                "EXIT" | "TERMINATE" | "QUIT" | "exit" | "terminate" | "quit"
            )
        }
    }

    impl Display for Msg {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            let ss = match serde_json::to_string(self) {
                Ok(s) => s,
                Err(err) => {
                    format!("### Fail to serde_json::to_string(). err : [{}]", err)
                }
            };
            write!(f, "{}", ss)
        }
    }

    let is_shutdown_reserved = atomic::AtomicBool::new(false);

    // Create a channel
    let (tx, rx) = mpsc::channel::<Msg>();

    // Create a thread
    let thread = thread::spawn(move || {
        loop {
            if is_shutdown_reserved.load(Ordering::SeqCst) {
                break;
            }
            // Receive the message
            let message = rx.recv().unwrap();
            log!("Received: {}", message);

            // handle for terminate cmd from sender.
            if message.is_terminate_cmd() {
                log!("### Receiver receive terminate cmd. cmd : [{}]", message);
                is_shutdown_reserved.fetch_xor(true, Ordering::SeqCst);
                break;
            }
        }
    });

    let msg_count = 30;

    for idx in 0..msg_count {
        match tx.send(Msg::new(idx.to_string().as_str())) {
            Ok(()) => {}
            Err(err) => {
                elog!("### fail to send. idx : [{}], err : [{}]", idx, err);
            }
        };

        thread::sleep(std::time::Duration::from_millis(500));
    }

    log!("### send/recv for n-thread. msg_count : {}", msg_count);

    // Send a message
    tx.send(Msg::new("Last send string from main thread."))
        .unwrap();

    // Wait for the thread to finish
    thread.join().unwrap();

    log!("### done.");
}
