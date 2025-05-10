pub mod util;

use crate::util::log;

fn main() {
    let a = String::from("a");
    log::tlog(format!("hello world. a : ${a}"));
}
