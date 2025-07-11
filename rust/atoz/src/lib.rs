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
}
