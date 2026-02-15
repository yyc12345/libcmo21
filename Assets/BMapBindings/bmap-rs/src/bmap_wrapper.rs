//! The module includes all senior wrappers for BMap FFI calling in Rust style.
//!
//! This module is what user of this library should use.

use crate::bmap::{self, BMBOOL, CKID, CKSTRING, PBMVOID};
use crate::marshaler;
use std::marker::PhantomData;
use thiserror::Error as TeError;

// region: Error and Result Types

/// Any possible error occurs in this module.
#[derive(Debug, TeError)]
pub enum Error {
    #[error("native BMap operation failed")]
    BadCall,
    #[error("{0}")]
    Marshaler(#[from] marshaler::Error),
}

/// The result type used in this module.
pub type Result<T> = std::result::Result<T, Error>;

// endregion

// region: Utilities

/// Internal used trait representing all instance created by BMap.
///
/// Do **NOT** use this trait.
/// It can not be hidden due to the limitation of Rust trait.
pub trait AbstractPointer: Sized {
    /// Internal used function for fetching instance underlying pointer.
    ///
    /// Do **NOT** use this function.
    /// It can not be hidden due to the limitation of Rust trait.
    unsafe fn get_pointer(&self) -> PBMVOID;
}

/// Internal used trait representing all objects create by BMap file reader and writer.
///
/// Do **NOT** use this trait.
/// It can not be hidden due to the limitation of Rust trait.
pub trait AbstractObject: AbstractPointer {
    /// Internal used function for fetching object underlying ID.
    ///
    /// Do **NOT** use this function.
    /// It can not be hidden due to the limitation of Rust trait.
    unsafe fn get_ckid(&self) -> CKID;
}

/// The representation of invalid raw pointer.
const INVALID_PTR: PBMVOID = std::ptr::null_mut();
/// The representation of invalid CK_ID.
const INVALID_CKID: CKID = 0;

/// The function used as callback for BMap.
/// It just writes the data in console.
unsafe extern "C" fn bmap_rs_callback(msg: CKSTRING) {
    println!("[bmap-rs] {}", "");
}

/// The convenient macro for wrapping all BMap calling in Rust error procession pattern.
macro_rules! bmap_exec {
    ($f:expr) => {
        if !unsafe { $f } {
            return Err(Error::BadCall);
        }
    };
}

macro_rules! arg_in {
    ($v:ident) => {
        $v
    };
}

macro_rules! arg_out {
    ($v:ident, $t:ty) => {
        &mut $v as *mut $t
    };
}

// endregion

// region: BMap

/// The BMap environment guard.
///
/// This struct make sure that all BMap calling is executed under initialized BMap environment,
/// and shutdown BMap environment automatically when there is no more calling.
pub struct BMap {}

impl BMap {
    pub fn new() -> Result<Self> {
        bmap_exec!(bmap::BMInit());
        Ok(Self {})
    }
}

impl Drop for BMap {
    fn drop(&mut self) {
        // Ignore return of this function by design.
        // Because using Result in Drop is inviable,
        // and using `panic!` is also not suggested.
        let _ = unsafe { bmap::BMDispose() };
    }
}

// endregion

// region: BMFileReader

// endregion

// region: BMFileWriter

// endregion

// region: CKObjects

// region: Helper

fn get_string_value<T>(
    o: &T,
    f: unsafe extern "C" fn(PBMVOID, CKID, *mut CKSTRING) -> BMBOOL,
) -> Result<Option<String>>
where
    T: AbstractObject,
{
    let mut data = CKSTRING::default();
    bmap_exec!(f(o.get_pointer(), o.get_ckid(), arg_out!(data, CKSTRING)));

    if data.is_null() {
        Ok(None)
    } else {
        Ok(Some(unsafe { marshaler::from_native_string(data)? }))
    }
}

fn set_string_value<T>(
    o: &T,
    f: unsafe extern "C" fn(PBMVOID, CKID, CKSTRING) -> BMBOOL,
    s: Option<&str>,
) -> Result<()>
where
    T: AbstractObject,
{
    let native = match s {
        Some(s) => Some(unsafe { marshaler::to_native_string(s)? }),
        None => None,
    };
    let data = match &native {
        Some(native) => unsafe { native.as_raw() },
        None => std::ptr::null_mut() as CKSTRING,
    };
    bmap_exec!(f(o.get_pointer(), o.get_ckid(), arg_in!(data)));

    Ok(())
}

// endregion

// region: Traits

pub trait CKObject: AbstractObject {
    fn get_name(&self) -> Result<Option<String>> {
        get_string_value(self, bmap::BMObject_GetName)
    }
    fn set_name(&mut self, name: Option<&str>) -> Result<()> {
        set_string_value(self, bmap::BMObject_SetName, name)
    }
}

pub trait CKTexture: CKObject {}

pub trait CKMaterial: CKObject {}

pub trait CKMesh: CKObject {}

pub trait CK3dEntity: CKObject {}

pub trait CK3dObject: CK3dEntity {}

pub trait CKLight: CK3dEntity {}

pub trait CKTargetLight: CKLight {}

pub trait CKCamera: CK3dEntity {}

pub trait CKTargetCamera: CKCamera {}

// endregion

// region: Structs

pub struct BMObject {}

pub struct BMTexture {}
pub struct BMMaterial {}
pub struct BMMesh {}
pub struct BM3dEntity {}
pub struct BM3dObject {}
pub struct BMLight {}
pub struct BMTargetLight {}
pub struct BMCamera {}
pub struct BMTargetCamera {}

// endregion

// endregion

// region: BMMeshTrans

// endregion
