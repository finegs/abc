pub mod calc;
pub mod log;

#[cfg(test)]
mod tests {

    use crate::calc;
    use crate::{log, log_info, tslog};

    #[test]
    fn it_works() {
        let left = 2;
        let right = 100;
        let result = calc::add(2, 2);
        log_info!("### add({},{})={}", left, right, calc::add(left, right));
        assert_eq!(result, 4);
    }

    fn append(s: &mut String) {
        s.push_str(" ...");
    }

    #[test]
    fn test01() {
        let mut s = String::from("hello");
        let r1 = &s;
        let r2 = &s;

        log_info!("r1:{r1}, r2:{r2}");

        let r3 = &mut s;
        append(r3);
        log_info!("r3:{r3}");
    }
}
