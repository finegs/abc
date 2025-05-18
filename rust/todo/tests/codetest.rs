#[test]
fn t01() {
    fn parse(text: &str) -> Vec<&str> {
        return text.split(&[',', '.'][..]).collect();
    }

    let ss = "abc,bde.001,111:000.01";
    let tg = "abc bde 001 111:000 01";
    let rs = parse(ss);
    assert_eq!(tg, rs.join(" "));
    println!("### &ss = [{:p}]", &ss);
    println!("### &rs[0] = [{:p}]", &rs[0]);
    println!("### &rs = [{:p}]", &rs);
    println!("### parse({}) = [{}]", ss, rs.join(" "));
}

#[test]
fn t02() {
    assert_ne!(true, false, "assert_eq is failed with t02");
}
