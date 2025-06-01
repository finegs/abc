pub mod calc;
pub mod log;

#[cfg(test)]
mod tests {
    use super::*;
    use crate::log;

    #[test]
    fn it_works() {
        let left = 2;
        let right = 100;
        let result = calc::add(2, 2);
        log!("### add({},{})={}", left, right, calc::add(left, right));
        assert_eq!(result, 4);
    }
}
