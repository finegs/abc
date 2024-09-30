use std::fmt::{Display, self};

struct Item {
    id : Option<u32>,
    name: String,
    desc: String
}

impl Item {
    fn new() -> Item {
        Item {
            id : None,
            name : String::new(),
            desc : String::new()
        }
    }
}

impl fmt::Display for Item {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "({}, {}, {})", self.id.unwrap_or(0), self.name, self.desc)
    }
}

fn main() {
    let i1 = Item::new();
    println!("test i1 : {}", i1);
}
