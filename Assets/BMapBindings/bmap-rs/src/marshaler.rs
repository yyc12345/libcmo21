//! BMap wrapper used marshaler for string and string array.

use crate::bmap::{CKSTRING, PCKSTRING};
use std::ffi::{CStr, CString};
use thiserror::Error as TeError;

// region: Error and Result Types

/// Any possible error occurs in this module.
#[derive(Debug, TeError, Clone)]
pub enum Error {
    #[error("can not parse from native string")]
    FromNative(#[from] std::str::Utf8Error),
    #[error("can not format into native string")]
    ToNative(#[from] std::ffi::NulError),
}

/// The result type used in this module.
pub type Result<T> = std::result::Result<T, Error>;

// endregion

pub unsafe fn from_native_string(ptr: CKSTRING) -> Result<String> {
    let s = unsafe { CStr::from_ptr(ptr) };
    Ok(String::from(s.to_str()?))
}

pub unsafe fn to_native_string<T>(words: T) -> Result<BMString>
where
    T: Into<Vec<u8>>,
{
    BMString::new(words)
}

pub struct BMString {
    inner: CString,
}

impl BMString {
    fn new<T>(words: T) -> Result<Self>
    where
        T: Into<Vec<u8>>,
    {
        Ok(Self {
            inner: CString::new(words)?,
        })
    }

    pub unsafe fn as_raw(&self) -> CKSTRING {
        self.inner.as_ptr() as CKSTRING
    }
}

pub unsafe fn from_native_string_array(ptr: PCKSTRING) -> Result<Vec<String>> {
    let mut rv = Vec::new();

    loop {
        let item_ptr = unsafe { *ptr } as CKSTRING;
        if item_ptr.is_null() {
            break;
        }

        let item = unsafe { from_native_string(item_ptr)? };
        rv.push(item);
    }

    Ok(rv)
}

pub unsafe fn to_native_string_array<T>(words: &[T]) -> Result<BMStringArray>
where
    T: Into<Vec<u8>> + Copy,
{
    BMStringArray::new(words)
}

pub struct BMStringArray {
    #[allow(dead_code)]
    array_items: Vec<CString>,
    array_body: Vec<CKSTRING>,
}

impl BMStringArray {
    fn new<T>(words: &[T]) -> Result<Self>
    where
        T: Into<Vec<u8>> + Copy,
    {
        // Build array items
        let array_items = words
            .iter()
            .map(|word| CString::new(*word))
            .collect::<std::result::Result<Vec<CString>, std::ffi::NulError>>()?;
        // Build array body.
        // In theory, move operation will not affect data allocated on heap.
        // So we can simply fetch the address of this new generated array.
        let array_body = array_items
            .iter()
            .map(|i| i.as_ptr() as CKSTRING)
            .chain(std::iter::once(std::ptr::null_mut() as CKSTRING))
            .collect::<Vec<CKSTRING>>();

        // Return value
        Ok(Self { array_items, array_body })
    }

    pub unsafe fn as_raw(&mut self) -> PCKSTRING {
        self.array_body.as_ptr() as PCKSTRING
    }
}
