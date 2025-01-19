#[cfg(test)]
use std::cell::{Cell, RefCell};
use std::rc::Rc;

#[allow(unused_imports)]
use std::sync::RwLock;

use app_util::log::tlog;

#[test]
fn deref_test() {
    let a = 1;
    let b = &1;
    assert_eq!(1, a);
    assert_eq!(1, *b);
}

#[test]
fn block_borrow_test() {
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
