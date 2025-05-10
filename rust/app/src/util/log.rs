#[allow(unused_imports)]
use chrono::{prelude, Local};
#[allow(unused_imports)]
use std::str::FromStr;
use std::thread;

pub fn tlog<T>(msg: T)
where
    T: FromStr + std::fmt::Display,
{
    println!(
        "{} - {:?} : {}",
        Local::now().format("%Y-%m-%d %H:%M:%S.%6f"),
        thread::current().id(),
        msg
    );
}

pub fn etlog<T>(msg: T)
where
    T: FromStr + std::fmt::Display,
{
    eprintln!(
        "{} - {:?} : {}",
        Local::now().format("%Y-%m-%d %H:%M:%S.%6f"),
        thread::current().id(),
        msg
    );
}
