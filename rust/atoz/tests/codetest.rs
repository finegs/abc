use std::fmt::Display;
use std::sync::atomic::Ordering;
use std::sync::{atomic, mpsc};
use std::thread;

use atoz::{elog, log_err, log_info, tslog};
use serde::{Deserialize, Serialize};

mod t_thead_01 {
    use super::*;

    #[test]
    fn test_snd_rcv_test_prd_cons() {
        use serde::{Deserialize, Serialize};
        use serde_json;

        use atoz::log;

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
                        format!("### Fail to serde_json::to_string(). err : [{err}]")
                    }
                };
                write!(f, "{ss}")
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
                log_info!("Received: {}", message);

                // handle for terminate cmd from sender.
                if message.is_terminate_cmd() {
                    log_info!("### Receiver receive terminate cmd. cmd : [{}]", message);
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
                    log_err!("### fail to send. idx : [{}], err : [{}]", idx, err);
                }
            };

            thread::sleep(std::time::Duration::from_millis(500));
        }

        log_info!("### send/recv for n-thread. msg_count : {}", msg_count);

        // Send a message
        tx.send(Msg::new("Last send string from main thread."))
            .unwrap();

        // Wait for the thread to finish
        thread.join().unwrap();

        log_info!("### done.");
    }
}

mod t_01 {
    #[test]
    fn let_from_inner_val() {
        let y = {
            let x = 3;
            x + 1
        };

        println!("y : {y}");
    }

    #[test]
    fn parse() {
        let guess: i128 = "42123".parse().expect("Not a number!");

        println!("guess : {guess}");

        let nn = 11_333;
        println!("nn : {nn}");
    }
}

mod t_disjoint_tree {
    struct DisjointSet {
        parent: Vec<usize>,
        size: Vec<usize>,
    }

    impl DisjointSet {
        /// 새로운 DisjointSet 구조체를 생성합니다.
        ///
        /// n개의 독립적인 원소를 가집니다.
        fn new(n: usize) -> Self {
            DisjointSet {
                parent: (0..n).collect(), // 각 원소는 자신을 부모로 가집니다.
                size: vec![1; n],         // 각 집합의 크기는 1로 초기화됩니다.
            }
        }

        /// 원소 i가 속한 집합의 대표(루트)를 찾습니다.
        ///
        /// 경로 압축(Path Compression)을 사용하여 효율성을 높입니다.
        fn find(&mut self, i: usize) -> usize {
            if self.parent[i] == i {
                return i;
            }
            self.parent[i] = self.find(self.parent[i]);
            self.parent[i]
        }

        /// 원소 i가 속한 집합과 원소 j가 속한 집합을 합칩니다.
        ///
        /// 크기 기반 합병(Union by Size)을 사용하여 트리의 높이를 낮춥니다.
        fn union(&mut self, i: usize, j: usize) -> bool {
            let root_i = self.find(i);
            let root_j = self.find(j);

            if root_i != root_j {
                // 더 작은 트리를 더 큰 트리에 연결합니다.
                if self.size[root_i] < self.size[root_j] {
                    self.parent[root_i] = root_j;
                    self.size[root_j] += self.size[root_i];
                } else {
                    self.parent[root_j] = root_i;
                    self.size[root_i] += self.size[root_j];
                }
                true // 성공적으로 합병됨
            } else {
                false // 이미 같은 집합에 속해 있음
            }
        }

        /// 두 원소 i와 j가 같은 집합에 속해 있는지 확인합니다.
        fn are_connected(&mut self, i: usize, j: usize) -> bool {
            self.find(i) == self.find(j)
        }
    }

    #[test]
    fn test_disjoint_tree() {
        let mut ds = DisjointSet::new(5); // 0, 1, 2, 3, 4 다섯 개의 원소로 초기화합니다.

        println!(
            "초기 상태: 0과 1은 연결되어 있나요? {}",
            ds.are_connected(0, 1)
        ); // false

        // 0과 1을 합칩니다.
        ds.union(0, 1);
        println!("0과 1을 합쳤습니다.");
        println!("0과 1은 이제 연결되어 있나요? {}", ds.are_connected(0, 1)); // true

        // 2와 3을 합칩니다.
        ds.union(2, 3);
        println!("2와 3을 합쳤습니다.");

        // 1과 3을 합칩니다.
        ds.union(1, 3);
        println!("1과 3을 합쳤습니다.");

        // 이 시점에서 0, 1, 2, 3은 모두 같은 집합에 속합니다.
        println!("2와 4는 연결되어 있나요? {}", ds.are_connected(2, 4)); // false
        println!("0과 3은 연결되어 있나요? {}", ds.are_connected(0, 3)); // true
    }
}
