#[derive(Debug)]
struct Rectangle {
    width: u32,
    height: u32,
}

impl Rectangle {
    fn area(&self) ->u32 {
        self.width * self.height
    }
}

fn main() {
    // struct Rectangle rec(30, 30);
    // rec is Rectangle {width: 30, height: 30};
    let rec = Rectangle {width: 30, height: 30};
    let name = String::from("Deng");
    println!("Hello, {}", name);
    println!("This is a Rectangle: {:?}", rec);
    println!("rec's area is {}",rec.area());
}
