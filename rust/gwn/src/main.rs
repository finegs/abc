use std::fmt;

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

    fn set_name(&mut self, name:  &str) {
        self.name = String::from(name);
    }

    fn set_desc(&mut self, desc: &str) {
        self.desc = String::from(desc);
    }
}

impl fmt::Display for Item {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "({}, {}, {})", self.id.unwrap_or(0), self.name, self.desc)
    }
}

fn main() {
    let mut i1 = Item::new();
    i1.set_name("name of i1");
    i1.set_desc("desc of i1");
    println!("test i1 : {}", i1);
}
