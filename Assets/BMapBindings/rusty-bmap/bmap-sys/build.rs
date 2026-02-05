use std::env;
use std::path::PathBuf;

fn main() {
    // Fetch user specified install directory of built BMap.
    let install_dir = env::var("LibCmo_ROOT").expect("You must set LibCmo_ROOT to the install directory of LibCmo built by CMake before building this Rust crate.");
    let install_path = PathBuf::from(install_dir);

    // Tell Rust compiler where to find linkd dynamic library.
    println!("cargo:rustc-link-search=native={}", install_path.join("lib").display());
    // Tell Rust compiler the name of linked dynamic library.
    println!("cargo:rustc-link-lib=dylib=BMap");
}
