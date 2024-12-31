#[cfg(test)]
mod my_tests {
    use super::*;
    use std::rc::Rc;
    use std::cell::RefCell;

    #[test]
    fn block_borrow_test() {
       let mut s = "hello".to_owned(); 
       {
           let s1 = &mut s;
           s1.push_str(" Appended in block.");
           println!("{s1}");
       }

       let mut s1 = s.clone();
       s1.push_str(" Appended.");
       println!("{s}, {s1}");
    }

    #[test]
    fn borrow_test() {
       let s = Rc::new(RefCell::new("hello".to_owned())); 
       let s1 = s.clone();
       let s2 = s.clone();
       let s3_ = s.clone();
       let mut s3 = s3_.borrow_mut();
       s3.push_str(" .... Appended by push_str");

       println!("{:?}, {:?}, {:?}, {:?}", s, s1, s2, s3);
    }
} /* MyTest */

