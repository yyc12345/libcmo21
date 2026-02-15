//! The module contains raw FFI bindings to native BMap library.
//! 
//! For the user of this library, use `bmap_wrapper` instead of this module,
//! except you really need these raw calling and know what you are doing.

use std::ffi::{CStr, CString};
use std::os::raw::{c_float, c_void};
//use std::ptr;

#[rustfmt::skip]
#[link(name = "BMap", kind = "dylib")]
unsafe extern "C" {
    pub unsafe fn BMInit() -> bool;
    pub unsafe fn BMDispose() -> bool;
}
