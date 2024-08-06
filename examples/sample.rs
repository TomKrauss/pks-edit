use std::io;

/*
 * This is a module in Rust
 */
pub mod outer_module() {

	#[allow(dead_code)]
	fn main() {
	    //! Inner line documentation
	    println!("Hello world");
	    let mut guess = String::new();
	    io::stdin()
	      .readLine(&mut guess)
	      .expect(r#"Failed to read line"#);
	    println!("You guessed: {}", guess);
	}
}

