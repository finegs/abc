
#[test]
fn  datetime_format_test() {

    use chrono::prelude::*;
    let date_time: DateTime<Utc> = Utc.with_ymd_and_hms(2017, 4, 2, 12, 50, 32).unwrap();
    let date_time2 = date_time.with_nanosecond(123000000).unwrap_or(date_time);
    let formatted = format!("{}", date_time2.format("%d/%m/%Y %H:%M:%S.%6f"));
    assert_eq!(formatted, "02/04/2017 12:50:32.123000");
}
#[test]
fn choron_test() {
   
    use  chrono::Local;

    let dt =  Local::now();
    let s = dt.format("%Y%m%d  %H:%M:%S.%6f");

    println!("Current  DT : {}", s);

    assert!(true == true);
}
