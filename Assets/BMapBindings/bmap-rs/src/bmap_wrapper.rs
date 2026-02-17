//! The module includes all senior wrappers for BMap FFI calling in Rust style.
//!
//! This module is what user of this library should use.
use crate::bmap::{self, BMBOOL, CKID, CKSTRING, PBMVOID};
use crate::marshaler;
use std::iter::{Iterator, FusedIterator, ExactSizeIterator};
use std::marker::PhantomData;
use std::mem::MaybeUninit;
use std::sync::LazyLock;
use thiserror::Error as TeError;

// region: Error and Result Types

/// Any possible error occurs in this module.
#[derive(Debug, TeError, Clone)]
pub enum Error {
    #[error("native BMap operation failed")]
    BadCall,
    #[error("{0}")]
    Marshaler(#[from] marshaler::Error),
    #[error("the length of given data iterator is too short when assigning struct array.")]
    OutOfLength,
}

/// The result type used in this module.
pub type Result<T> = std::result::Result<T, Error>;

// endregion

// region: Utilities

// region: Traits

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

// endregion

// region: Struct Iterator and Assigner

fn struct_assigner<O, T, I>(_: &O, ptr: *mut T, cnt: usize, it: &mut I) -> Result<()>
where
    O: AbstractObject,
    T: Sized + Copy,
    I: Iterator<Item = T>,
{
    for i in 0..cnt {
        // Fetch item
        let item = match it.next() {
            Some(v) => v,
            None => return Err(Error::OutOfLength)
        };
        // Write item
        unsafe { *ptr.add(i) = item };
    }
    Ok(())
}

pub struct StructIterator<'a, O, T>
where
    T: Sized + Copy,
    O: AbstractObject,
{
    ptr: *mut T,
    cnt: usize,
    i: usize,
    phantom: PhantomData<&'a O>,
}

impl<'a, O, T> StructIterator<'a, O, T>
where
    T: Sized + Copy,
    O: AbstractObject,
{
    fn new(_: &'a O, ptr: *mut T, cnt: usize) -> Self {
        Self {
            ptr,
            cnt,
            i: 0,
            phantom: PhantomData::<&'a O>,
        }
    }
}

impl<'a, O, T> Iterator for StructIterator<'a, O, T>
where
    T: Sized + Copy,
    O: AbstractObject,
{
    type Item = T;

    fn next(&mut self) -> Option<Self::Item> {
        if self.i >= self.cnt {
            None
        } else {
            let rv = unsafe { *self.ptr.add(self.i) };
            self.i += 1;
            Some(rv)
        }
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        let remaining = self.len();
        (remaining, Some(remaining))
    }
}

impl<'a, O, T> FusedIterator for StructIterator<'a, O, T>
where
    T: Sized + Copy,
    O: AbstractObject,
{
}

impl<'a, O, T> ExactSizeIterator for StructIterator<'a, O, T>
where
    T: Sized + Copy,
    O: AbstractObject,
{
    fn len(&self) -> usize {
        if self.i >= self.cnt {
            0
        } else {
            self.i - self.cnt
        }
    }
}

fn struct_iterator<'a, O, T>(o: &'a O, ptr: *mut T, cnt: usize) -> Result<StructIterator<'a, O, T>>
where
    O: AbstractObject,
    T: Sized + Copy,
{
    Ok(StructIterator::new(o, ptr, cnt))
}

// endregion

// region: Constants

/// The representation of invalid raw pointer.
const INVALID_PTR: PBMVOID = std::ptr::null_mut();
/// The representation of invalid CK_ID.
const INVALID_CKID: CKID = 0;

/// The function used as callback for BMap.
/// It just writes the data in console.
unsafe extern "C" fn bmap_rs_callback(msg: CKSTRING) {
    if let Ok(msg) = unsafe { marshaler::from_native_string(msg) } {
        println!("[bmap-rs] {}", msg);
    }
}

// endregion

// region: Macros

/// The convenient macro for wrapping all BMap calling in Rust error procession pattern.
macro_rules! bmap_exec {
    ($f:expr) => {
        if !unsafe { $f } {
            return Err(Error::BadCall);
        }
    };
}

macro_rules! param_in {
    ($t:ty) => {
        $t
    };
}

macro_rules! param_out {
    ($t:ty) => {
        *mut $t
    };
}

macro_rules! arg_in {
    ($v:expr) => {
        $v
    };
}

macro_rules! arg_out {
    ($v:expr, $t:ty) => {
        $v as param_out!($t)
    };
}

// endregion

// endregion

// region: BMap Guard

/// The BMap environment guard.
///
/// This struct make sure that all BMap calling is executed under initialized BMap environment,
/// and shutdown BMap environment automatically when there is no more calling.
struct BMapGuard {}

impl BMapGuard {
    fn new() -> Result<Self> {
        bmap_exec!(bmap::BMInit());
        Ok(Self {})
    }
}

impl Drop for BMapGuard {
    fn drop(&mut self) {
        // Ignore return of this function by design.
        // Because using Result in Drop is inviable,
        // and using `panic!` is also not suggested.
        let _ = unsafe { bmap::BMDispose() };
    }
}

static BMAP_SINGLETON: LazyLock<Result<BMapGuard>> = LazyLock::new(|| {
    BMapGuard::new()
});

fn get_bmap_guard_singleton() -> Result<&'static BMapGuard> {
    BMAP_SINGLETON.as_ref().map_err(|e| e.clone())
}

pub fn is_bmap_available() -> bool {
    BMAP_SINGLETON.is_ok()
}

// endregion

// region: BMFileReader

pub struct BMFileReader<'a> {
    handle: PBMVOID,
    phantom: PhantomData<&'a BMapGuard>
}

impl<'a> AbstractPointer for BMFileReader<'a> {
    unsafe fn get_pointer(&self) -> PBMVOID {
        self.handle
    }
}

// endregion

// region: BMFileWriter

pub struct BMFileWriter<'a> {
    handle: PBMVOID,
    phantom: PhantomData<&'a BMapGuard>
}

impl<'a> AbstractPointer for BMFileWriter<'a> {
    unsafe fn get_pointer(&self) -> PBMVOID {
        self.handle
    }
}

// endregion

// region: CKObjects

// region: Utilities Functions

fn get_copyable_value<O, T>(
    o: &O,
    f: unsafe extern "C" fn(PBMVOID, CKID, param_out!(T)) -> BMBOOL,
) -> Result<T>
where
    O: AbstractObject,
    T: Sized + Copy,
{
    let mut data = MaybeUninit::<T>::uninit();
    bmap_exec!(f(
        o.get_pointer(),
        o.get_ckid(),
        arg_out!(data.as_mut_ptr(), T)
    ));
    Ok(unsafe { data.assume_init() })
}

fn set_copyable_value<O, T>(
    o: &O,
    f: unsafe extern "C" fn(PBMVOID, CKID, param_in!(T)) -> BMBOOL,
    data: T,
) -> Result<()>
where
    O: AbstractObject,
    T: Sized + Copy,
{
    bmap_exec!(f(o.get_pointer(), o.get_ckid(), arg_in!(data)));
    Ok(())
}

fn get_string_value<O>(
    o: &O,
    f: unsafe extern "C" fn(PBMVOID, CKID, param_out!(CKSTRING)) -> BMBOOL,
) -> Result<Option<String>>
where
    O: AbstractObject,
{
    // Get raw string pointer
    let data: CKSTRING = get_copyable_value(o, f)?;
    // Check raw string pointer.
    if data.is_null() {
        Ok(None)
    } else {
        Ok(Some(unsafe { marshaler::from_native_string(data)? }))
    }
}

fn set_string_value<O>(
    o: &O,
    f: unsafe extern "C" fn(PBMVOID, CKID, param_in!(CKSTRING)) -> BMBOOL,
    s: Option<&str>,
) -> Result<()>
where
    O: AbstractObject,
{
    // Buold raw string pointer.
    let native = match s {
        Some(s) => Some(unsafe { marshaler::to_native_string(s)? }),
        None => None,
    };
    let data: CKSTRING = match &native {
        Some(native) => unsafe { native.as_raw() },
        None => std::ptr::null_mut() as CKSTRING,
    };
    // Set it
    set_copyable_value(o, f, data)
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

pub struct BMMeshTrans<'a> {
    handle: PBMVOID,
    phantom: PhantomData<&'a BMapGuard>
}

impl<'a> AbstractPointer for BMMeshTrans<'a> {
    unsafe fn get_pointer(&self) -> PBMVOID {
        self.handle
    }
}

// endregion
