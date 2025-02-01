#![allow(unused)]
#[cfg(test)]
use std::cell::{Cell, RefCell};
use std::{clone, ops::Deref, rc::Rc};

#[allow(unused_imports)]
use std::sync::RwLock;

use app_util::log::tlog;

#[test]
fn deref_test() 
{
    let a = 1;
    let b = &1;
    assert_eq!(1, a);
    assert_eq!(1, *b);
}

#[test]
fn block_borrow_test() 
{
    let mut s = "hello".to_owned();
    {
        let s1 = &mut s;
        s1.push_str(" Appended in block.");
        tlog(format!("{s1}"));
    }

    let mut s1 = s.clone();
    s1.push_str(" Appended.");
    tlog(format!("{s}, {s1}"));
}

#[test]
fn borrow_test() {
    let s = Rc::new(RefCell::new("hello".to_owned()));
    let s1 = s.clone();
    let s2 = s.clone();
    let s3_ = s.clone();
    let mut s3 = s3_.borrow_mut();
    s3.push_str(" .... Appended by push_str");

    tlog(format!("{:?}, {:?}, {:?}, {:?}", s, s1, s2, s3));
}

#[test]
fn refcell_test() {
    let a = RefCell::new(1u64);

    /*
     * borrow_mut make panic after borrow for same reference
     */
    // let b = a.borrow();
    // let c = a.borrow();

    *a.borrow_mut() += 1;

    tlog(format!("{}", a.borrow()));
}

#[test]
fn cell_test() {
    let a = Cell::new(1);

    tlog(format!("before : {}", a.get()));

    a.set(2);

    tlog(format!("after : {}", a.get()));
}

#[test]
fn refcell_1_test() {
    let a = RefCell::new(5);

    let b = &a;
    let c = &a;

    *b.borrow_mut() += 1;

    assert_eq!(*c.borrow(), 6);
}

#[test]
fn refcell_mb_b_mixed_test() {
    let a = Rc::new(RefCell::new(5));
    let b = a.clone();
    let d = a.clone();
    // let d_b = d.borrow();

    *d.borrow_mut() += 10;
    // let mut c = b.borrow_mut();
    // *c = 10;

    //  panic  already  borrow_mut  used

    assert_eq!(*b.borrow(), 15);
}

#[test]
fn n_mut_error_test() {
    let mut s = String::from("hello");

    {
        let r1 = &mut s;
        r1.push_str(" world");

        tlog(format!("first &mut : {}", r1));

        assert_eq!(r1, "hello world");
    }

    {
        let r2 = &mut s;
        r2.push_str(" second pushed_str");

        tlog(format!("second &mut : {}", r2));

        assert_eq!(r2, "hello world second pushed_str");
    }

    tlog(format!(" s : {}", s));

    s.push_str(" ###");

    assert_eq!(s, "hello world second pushed_str ###");
}

#[test]
fn arc_mutex_refcell_test() {
    use std::cell::RefCell;
    use std::sync::{Arc, Mutex};
    use std::thread;

    // RefCell은 동일한 스레드에서 가변 참조를 허용하지만, 여러 스레드에서 사용할 수 없습니다.
    // 따라서 Arc와 Mutex를 사용하여 스레드 간 안전성을 보장해야 합니다.
    // 아래 구조는 Arc를 사용하여 래퍼를 만든 다음, Mutex를 사용하여 가변 참조를 안전하게 관리합니다.
    let data = Arc::new(Mutex::new(RefCell::new(vec![1, 2, 3])));

    let mut handles = vec![];

    for idx in 0..3 {
        // data에 대한 Arc 클론을 생성하여 각 스레드가 독립적으로 참조하도록 합니다.
        let data_cloned = Arc::clone(&data);
        let handle = thread::spawn(move || {
            // Mutex를 잠근 다음, RefCell에 대한 가변 참조를 얻습니다.
            let data_locked = data_cloned.lock().unwrap();
            let mut data_ref_mut = data_locked.borrow_mut();

            // 벡터에 값을 추가합니다.
            data_ref_mut.push((idx as u32).pow(2));

            // 스레드가 종료되면 Mutex가 자동으로 해제됩니다.
        });
        handles.push(handle);
    }

    // 모든 스레드가 완료되기를 기다립니다.
    for handle in handles {
        handle.join().unwrap();
    }

    // 결과를 출력합니다.
    let data_locked = data.lock().unwrap();
    tlog(format!("data: {:?}", data_locked.borrow()));
}


#[test]
fn refcell_2_test() {
    #[derive(Debug)]
    struct Wallet {
        balance: Rc<RefCell<i64>>,
    }

    impl Wallet {
        pub fn new(balance: i64) -> Self {
           Self { balance : Rc::new(RefCell::new(balance)) }
        }

        pub fn deposit(&self, amount: i64) {
           let mut ref_cell = self.balance.borrow_mut();
           *ref_cell += amount;
        }

        pub fn withrawal(&self, amount: i64) {
           let mut ref_cell = self.balance.borrow_mut();
           *ref_cell -= amount;
        }

        pub fn get_balance_value(&self) -> i64 {
            *self.balance.borrow()
        }

        pub fn get_balance(&self) -> Rc<RefCell<i64>> {
           self.balance.clone()
        }
    }

    let  w = Wallet::new(0);

    w.deposit(10);
    w.withrawal(5);

    let bc =  w.get_balance().clone();

    *bc.borrow_mut() += 10;

    assert_ne!(w.get_balance_value(), 10);
    assert_eq!(w.get_balance_value(), 15);
    assert_eq!(*bc.borrow(), 15);

}

#[test]
fn generic_impl_fn_test() {
    fn parse_csv_document<R>(src: R) -> std::io::Result<Vec<Vec<String>>> 
        where 
            R: std::io::BufRead 
    {
        src.lines()
            .map(|line| {
                // For each line in the source
                line.map(|line| {
                    // If the line was read successfully, process it, if not, return the error
                    line.split(',') // Split the line separated by commas
                        .map(|entry| String::from(entry.trim())) // Remove leading and trailing whitespace
                        .collect() // Collect all strings in a row into a Vec<String>
                })
            })
            .collect() // Collect all lines into a Vec<Vec<String>>
    }

    // parse_csv_document(std::io::Empty()); 
    // This is the exact same function, but its return type uses `impl Trait`.
    // Look how much simpler it is!
    fn combine_vecs<T>(
        v: Vec<T>,
        u: Vec<T>,
    ) -> impl Iterator<Item=T> 
        where
            T:  Clone
    {
        v.into_iter().chain(u.into_iter()).cycle()
    }

    let a = vec![1,2,3];
    let b = vec![4,5];

    let mut ss = String::new();
    a.into_iter().for_each(|e| {
        ss.push_str(e.to_string().to_string().as_str());
    });
    println!("ss is [{}]", ss);

}

#[test]
fn test_generic_with_trait() {


    pub trait Summary {
        fn summarize(&self) -> String;
    }

    #[derive(Debug, Clone)]
    struct Article {
        title: String,
        author: String,
        content: String
    }

    impl Summary for Article {
        fn summarize(&self) -> String {
            format!("@{}:{}", self.author, self.title)
        }
    }

    #[derive(Debug, Clone)]
    struct Tweet {
        username: String,
        content: String,
    }

    impl Summary for Tweet {
        fn summarize(&self) -> String{
            format!("@{}: {}", self.username, self.content)
        }
    }

    let article = Article {
        title: String::from("Rust Programming"),
        author: String::from("Jane Doe"),
        content: String::from("Rust is a systems programming language..."),
    };

    let tweet = Tweet {
        username: String::from("rustacean"),
        content: String::from("Loving Rust!"),
    };

    fn notify<T: Summary>(item: &T) {
        println!("Breaking news! {}", item.summarize());
    }

    let a = Article{
                                title: "title".into(),
                                author: "aa".into(),
                                content: "...".into()
                            };

    let b = Tweet{
        username: "user-01".into(),
        content: "....".into()
    };

    let ss: Vec<&dyn Summary>  = vec![&a, &b];

    notify(&article);
    notify(&tweet);

    for (i, s) in ss.iter().enumerate() {
        println!("[{}] : {}", i, s.summarize());
    }
        
}

#[test]
fn dijkstra_test() {
    use std::cmp::Ordering;
    use std::collections::BinaryHeap;

    #[derive(Copy, Clone, Eq, PartialEq)]
    struct State {
        cost: usize,
        position: usize,
    }

    // The priority queue depends on `Ord`.
    // Explicitly implement the trait so the queue becomes a min-heap
    // instead of a max-heap.
    impl Ord for State {
        fn cmp(&self, other: &Self) -> Ordering {
            // Notice that we flip the ordering on costs.
            // In case of a tie we compare positions - this step is necessary
            // to make implementations of `PartialEq` and `Ord` consistent.
            other.cost.cmp(&self.cost)
                .then_with(|| self.position.cmp(&other.position))
        }
    }

    // `PartialOrd` needs to be implemented as well.
    impl PartialOrd for State {
        fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
            Some(self.cmp(other))
        }
    }

    // Each node is represented as a `usize`, for a shorter implementation.
    struct Edge {
        node: usize,
        cost: usize,
    }

    // Dijkstra's shortest path algorithm.

    // Start at `start` and use `dist` to track the current shortest distance
    // to each node. This implementation isn't memory-efficient as it may leave duplicate
    // nodes in the queue. It also uses `usize::MAX` as a sentinel value,
    // for a simpler implementation.
    fn shortest_path(adj_list: &Vec<Vec<Edge>>, start: usize, goal: usize) -> Option<usize> {
        // dist[node] = current shortest distance from `start` to `node`
        let mut dist: Vec<_> = (0..adj_list.len()).map(|_| usize::MAX).collect();

        let mut heap = BinaryHeap::new();

        // We're at `start`, with a zero cost
        dist[start] = 0;
        heap.push(State { cost: 0, position: start });

        // Examine the frontier with lower cost nodes first (min-heap)
        while let Some(State { cost, position }) = heap.pop() {
            // Alternatively we could have continued to find all shortest paths
            if position == goal { return Some(cost); }

            // Important as we may have already found a better way
            if cost > dist[position] { continue; }

            // For each node we can reach, see if we can find a way with
            // a lower cost going through this node
            for edge in &adj_list[position] {
                let next = State { cost: cost + edge.cost, position: edge.node };

                // If so, add it to the frontier and continue
                if next.cost < dist[next.position] {
                    heap.push(next);
                    // Relaxation, we have now found a better way
                    dist[next.position] = next.cost;
                }
            }
        }

        // Goal not reachable
        None
    }

    // This is the directed graph we're going to use.
    // The node numbers correspond to the different states,
    // and the edge weights symbolize the cost of moving
    // from one node to another.
    // Note that the edges are one-way.
    //
    //                  7
    //          +-----------------+
    //          |                 |
    //          v   1        2    |  2
    //          0 -----> 1 -----> 3 ---> 4
    //          |        ^        ^      ^
    //          |        | 1      |      |
    //          |        |        | 3    | 1
    //          +------> 2 -------+      |
    //           10      |               |
    //                   +---------------+
    //
    // The graph is represented as an adjacency list where each index,
    // corresponding to a node value, has a list of outgoing edges.
    // Chosen for its efficiency.
    let graph = vec![
        // Node 0
        vec![Edge { node: 2, cost: 10 },
             Edge { node: 1, cost: 1 }],
        // Node 1
        vec![Edge { node: 3, cost: 2 }],
        // Node 2
        vec![Edge { node: 1, cost: 1 },
             Edge { node: 3, cost: 3 },
             Edge { node: 4, cost: 1 }],
        // Node 3
        vec![Edge { node: 0, cost: 7 },
             Edge { node: 4, cost: 2 }],
        // Node 4
        vec![]
    ];

    assert_eq!(shortest_path(&graph, 0, 1), Some(1));
    assert_eq!(shortest_path(&graph, 0, 3), Some(3));
    assert_eq!(shortest_path(&graph, 3, 0), Some(7));
    assert_eq!(shortest_path(&graph, 0, 4), Some(5));
    assert_eq!(shortest_path(&graph, 4, 0), None);
}
