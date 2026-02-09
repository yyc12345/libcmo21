use std::ffi::{CStr, CString};
use std::os::raw::{c_float, c_void};
//use std::ptr;

#[rustfmt::skip]
#[link(name = "BMap", kind = "dylib")]
unsafe extern "C" {
    pub unsafe fn BMInit() -> bool;
    pub unsafe fn BMDispose() -> bool;
}
