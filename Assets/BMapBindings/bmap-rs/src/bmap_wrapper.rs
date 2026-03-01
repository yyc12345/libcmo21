//! The module includes all senior wrappers for BMap FFI calling in Rust style.
//!
//! This module is what user of this library should use.

mod marshaler;

use crate::bmap::{self, BMBOOL, CKDWORD, CKID, CKINT, CKSTRING, CKWORD, PBMVOID};
use std::cmp::Ordering;
use std::hash::{Hash, Hasher};
use std::iter::{ExactSizeIterator, FusedIterator, Iterator};
use std::marker::PhantomData;
use std::mem::MaybeUninit;
use std::sync::{Mutex, OnceLock};
use thiserror::Error as TeError;

// region: Error and Result Types

/// Any possible error occurs in this module.
#[derive(Debug, TeError, Clone)]
pub enum Error {
    #[error("native BMap operation failed")]
    BadCall,
    #[error("{0}")]
    Marshaler(#[from] marshaler::Error),
    #[error("the length of given data iterator is too short when assigning data array.")]
    OutOfLength,
    #[error("bad cast to integral value")]
    BadIntCast(#[from] std::num::TryFromIntError),
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
pub trait AbstractPointer<'p> {
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
pub trait AbstractObject<'o, P>: AbstractPointer<'o>
where
    P: AbstractPointer<'o> + ?Sized,
{
    /// Internal used function for creating object.
    ///
    /// Do **NOT** use this function.
    /// It can not be hidden due to the limitation of Rust trait.
    unsafe fn with_parent(parent: &'o P, handle: PBMVOID, id: CKID) -> Self;
    /// Internal used function for creating object.
    ///
    /// Do **NOT** use this function.
    /// It can not be hidden due to the limitation of Rust trait.
    unsafe fn with_sibling<AO>(sibling: &'o AO, handle: PBMVOID, id: CKID) -> Self
    where
        AO: AbstractObject<'o, P> + ?Sized;
    /// Internal used function for fetching object underlying ID.
    ///
    /// Do **NOT** use this function.
    /// It can not be hidden due to the limitation of Rust trait.
    unsafe fn get_ckid(&self) -> CKID;
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

// region: Common Macros

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

// region: Library Pointer Macros

macro_rules! libptr_struct {
    ($name:ident) => {
        #[derive(Debug)]
        pub struct $name<'p> {
            handle: PBMVOID,
            parent: PhantomData<&'p BMap>,
        }
    };
}

macro_rules! libptr_impl_eq_ord_hash {
    ($name:ident) => {
        impl<'p> PartialEq for $name<'p> {
            fn eq(&self, other: &Self) -> bool {
                self.handle == other.handle
            }
        }

        impl<'p> Eq for $name<'p> {}

        impl<'p> PartialOrd for $name<'p> {
            fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
                self.handle.partial_cmp(&other.handle)
            }
        }

        impl<'p> Ord for $name<'p> {
            fn cmp(&self, other: &Self) -> Ordering {
                self.handle.cmp(&other.handle)
            }
        }

        impl<'p> Hash for $name<'p> {
            fn hash<H: Hasher>(&self, state: &mut H) {
                self.handle.hash(state);
            }
        }
    };
}

macro_rules! libptr_impl_abstract_pointer {
    ($name:ident) => {
        impl<'p> AbstractPointer<'p> for $name<'p> {
            unsafe fn get_pointer(&self) -> PBMVOID {
                self.handle
            }
        }
    };
}

// endregion

// region: Library Object Macros

macro_rules! libobj_struct {
    ($name:ident) => {
        #[derive(Debug)]
        pub struct $name<'o, P>
        where
            P: AbstractPointer<'o> + ?Sized,
        {
            handle: PBMVOID,
            id: CKID,
            parent: PhantomData<&'o P>,
        }
    };
}

macro_rules! libobj_impl_clone {
    ($name:ident) => {
        impl<'o, P> Clone for $name<'o, P> where P: AbstractPointer<'o> + ?Sized {
            fn clone(&self) -> Self {
                Self {
                    handle: self.handle,
                    id: self.id,
                    parent: PhantomData::<&'o P>,
                }
            }
        }
    };
}

macro_rules! libobj_impl_eq_ord_hash {
    ($name:ident) => {
        impl<'o, P> PartialEq for $name<'o, P>
        where
            P: AbstractPointer<'o> + ?Sized,
        {
            fn eq(&self, other: &Self) -> bool {
                self.handle == other.handle && self.id == other.id
            }
        }

        impl<'o, P> Eq for $name<'o, P> where P: AbstractPointer<'o> + ?Sized {}

        impl<'o, P> PartialOrd for $name<'o, P>
        where
            P: AbstractPointer<'o> + ?Sized,
        {
            fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
                match self.handle.partial_cmp(&other.handle) {
                    Some(Ordering::Equal) => self.id.partial_cmp(&other.id),
                    other => other,
                }
            }
        }

        impl<'o, P> Ord for $name<'o, P>
        where
            P: AbstractPointer<'o> + ?Sized,
        {
            fn cmp(&self, other: &Self) -> Ordering {
                match self.handle.cmp(&other.handle) {
                    Ordering::Equal => self.id.cmp(&other.id),
                    other => other,
                }
            }
        }

        impl<'o, P> Hash for $name<'o, P>
        where
            P: AbstractPointer<'o> + ?Sized,
        {
            fn hash<H: Hasher>(&self, state: &mut H) {
                self.handle.hash(state);
                self.id.hash(state);
            }
        }
    };
}

macro_rules! libobj_impl_abstract_object {
    ($name:ident) => {
        impl<'o, P> AbstractPointer<'o> for $name<'o, P>
        where
            P: AbstractPointer<'o> + ?Sized,
        {
            unsafe fn get_pointer(&self) -> PBMVOID {
                self.handle
            }
        }

        impl<'o, P> AbstractObject<'o, P> for $name<'o, P>
        where
            P: AbstractPointer<'o> + ?Sized,
        {
            unsafe fn with_parent(_: &'o P, handle: PBMVOID, id: CKID) -> Self {
                Self {
                    handle: handle,
                    id: id,
                    parent: PhantomData::<&'o P>,
                }
            }
            unsafe fn with_sibling<AO>(_: &'o AO, handle: PBMVOID, id: CKID) -> Self
            where
                AO: AbstractObject<'o, P> + ?Sized,
            {
                Self {
                    handle: handle,
                    id: id,
                    parent: PhantomData::<&'o P>,
                }
            }
            unsafe fn get_ckid(&self) -> CKID {
                self.id
            }
        }
    };
}

macro_rules! libobj_impl_obj_trait {
    ($name:ident, $trait:ident) => {
        impl<'o, P> $trait<'o, P> for $name<'o, P> where P: AbstractPointer<'o> + ?Sized {}
    };
}

// endregion

// region: Iterator Utility Macros

macro_rules! libiter_len_body {
    ($current_cnt:expr, $all_cnt:expr) => {
        if $current_cnt >= $all_cnt {
            0
        } else {
            $all_cnt - $current_cnt
        }
    };
}

macro_rules! libiter_size_hint_body {
    ($current_cnt:expr, $all_cnt:expr) => {
        if $current_cnt >= $all_cnt {
            (0, Some(0))
        } else {
            let remaining = $all_cnt - $current_cnt;
            (remaining, Some(remaining))
        }
    };
}

// endregion

// endregion

// region: Struct Iterator and Assigner

fn struct_assigner<T, I>(ptr: *mut T, cnt: usize, mut it: I) -> Result<()>
where
    T: Sized + Copy,
    I: Iterator<Item = T>,
{
    for i in 0..cnt {
        // Fetch item
        let item = match it.next() {
            Some(v) => v,
            None => return Err(Error::OutOfLength),
        };
        // Write item
        unsafe { *ptr.add(i) = item };
    }
    Ok(())
}

pub struct StructIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: Sized + Copy,
{
    ptr: *mut T,
    cnt: usize,
    i: usize,
    phantom: PhantomData<P>,
    parent: PhantomData<&'o O>,
}

impl<'o, P, O, T> StructIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: Sized + Copy,
{
    fn new(_: &'o O, ptr: *mut T, cnt: usize) -> Self {
        Self {
            ptr,
            cnt,
            i: 0,
            phantom: PhantomData,
            parent: PhantomData,
        }
    }
}

impl<'o, P, O, T> Iterator for StructIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: Sized + Copy,
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
        libiter_size_hint_body!(self.i, self.cnt)
    }
}

impl<'o, P, O, T> FusedIterator for StructIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: Sized + Copy,
{
}

impl<'o, P, O, T> ExactSizeIterator for StructIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: Sized + Copy,
{
    fn len(&self) -> usize {
        libiter_len_body!(self.i, self.cnt)
    }
}

fn struct_iterator<'o, P, O, T>(
    o: &'o O,
    ptr: *mut T,
    cnt: usize,
) -> Result<StructIter<'o, P, O, T>>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: Sized + Copy,
{
    Ok(StructIter::new(o, ptr, cnt))
}

// endregion

// endregion

// region: BMap Guard

/// The BMap environment guard.
///
/// This struct make sure that all BMap calling is executed under initialized BMap environment,
/// and shutdown BMap environment automatically when there is no more calling.
pub struct BMap {}

impl BMap {
    fn new() -> Result<Self> {
        bmap_exec!(bmap::BMInit());
        Ok(Self {})
    }
}

impl Drop for BMap {
    fn drop(&mut self) {
        // YYC MARK:
        // Ignore return of this function by design.
        // Because using Result in Drop is inviable,
        // and using `panic!` is also not suggested.
        let _ = unsafe { bmap::BMDispose() };
    }
}

impl BMap {
    pub fn create_file_reader<'b, I, T>(
        &'b mut self,
        file_name: &str,
        temp_folder: &str,
        texture_folder: &str,
        encodings: I,
    ) -> Result<BMFileReader<'b>>
    where
        I: Iterator<Item = T>,
        T: Into<Vec<u8>>,
    {
        BMFileReader::new(self, file_name, temp_folder, texture_folder, encodings)
    }

    pub fn create_file_writer<'b, I, T>(
        &'b mut self,
        temp_folder: &str,
        texture_folder: &str,
        encodings: I,
    ) -> Result<BMFileWriter<'b>>
    where
        I: Iterator<Item = T>,
        T: Into<Vec<u8>>,
    {
        BMFileWriter::new(self, temp_folder, texture_folder, encodings)
    }

    pub fn create_mesh_trans<'b>(&'b mut self) -> Result<BMMeshTrans<'b>> {
        BMMeshTrans::new(self)
    }
}

impl BMap {
    pub fn instance() -> &'static Result<Mutex<BMap>> {
        static INSTANCE: OnceLock<Result<Mutex<BMap>>> = OnceLock::new();
        INSTANCE.get_or_init(|| BMap::new().map(|v| Mutex::new(v)))
    }

    pub fn is_available() -> bool {
        Self::instance().is_ok()
    }

    pub fn with_bmap<F, R>(f: F) -> R
    where
        F: FnOnce(&mut Self) -> R,
    {
        let mut instance = Self::instance()
            .as_ref()
            .expect("bad BMap environment (not initialized)")
            .lock()
            .expect("failed mutex lock");
        f(&mut instance)
    }
}

// endregion

// region: BMObjects

// region: Utility Structs

type FnMeshMtlSlotGetter =
    unsafe extern "C" fn(PBMVOID, CKID, param_in!(CKDWORD), param_out!(CKID)) -> BMBOOL;

pub struct MeshMtlSlotIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: AbstractObject<'o, P>,
{
    f: FnMeshMtlSlotGetter,
    cnt: usize,
    i: usize,
    phantom_pointer: PhantomData<P>,
    parent: &'o O,
    phantom_target: PhantomData<T>,
}

impl<'o, P, O, T> MeshMtlSlotIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: AbstractObject<'o, P>,
{
    fn new(parent: &'o O, f: FnMeshMtlSlotGetter, cnt: usize) -> Self {
        Self {
            f: f,
            cnt: cnt,
            i: 0,
            phantom_pointer: PhantomData,
            parent: parent,
            phantom_target: PhantomData,
        }
    }
}

impl<'o, P, O, T> Iterator for MeshMtlSlotIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: AbstractObject<'o, P>,
{
    type Item = Result<Option<T>>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.i >= self.cnt {
            None
        } else {
            let mut ckid = MaybeUninit::<CKID>::uninit();
            let i = match self.i.try_into() {
                Ok(v) => v,
                Err(e) => return Some(Err(Error::from(e))),
            };

            let rv = unsafe {
                (self.f)(
                    self.parent.get_pointer(),
                    self.parent.get_ckid(),
                    arg_in!(i),
                    arg_out!(ckid.as_mut_ptr(), CKID),
                )
            };
            if !rv {
                return Some(Err(Error::BadCall));
            }

            let ckid = unsafe { ckid.assume_init() };
            self.i += 1;

            if ckid == INVALID_CKID {
                Some(Ok(None))
            } else {
                Some(Ok(Some(unsafe {
                    T::with_sibling(self.parent, self.parent.get_pointer(), ckid)
                })))
            }
        }
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        libiter_size_hint_body!(self.i, self.cnt)
    }
}

impl<'o, P, O, T> FusedIterator for MeshMtlSlotIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: AbstractObject<'o, P>,
{
}

impl<'o, P, O, T> ExactSizeIterator for MeshMtlSlotIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: AbstractObject<'o, P>,
{
    fn len(&self) -> usize {
        libiter_len_body!(self.i, self.cnt)
    }
}

type FnGroupObjectGetter =
    unsafe extern "C" fn(PBMVOID, CKID, param_in!(CKDWORD), param_out!(CKID)) -> BMBOOL;

pub struct GroupObjectIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: AbstractObject<'o, P>,
{
    f: FnGroupObjectGetter,
    cnt: usize,
    i: usize,
    phantom_pointer: PhantomData<P>,
    parent: &'o O,
    phantom_target: PhantomData<T>,
}

impl<'o, P, O, T> GroupObjectIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: AbstractObject<'o, P>,
{
    fn new(parent: &'o O, f: FnGroupObjectGetter, cnt: usize) -> Self {
        Self {
            f: f,
            cnt: cnt,
            i: 0,
            phantom_pointer: PhantomData,
            parent: parent,
            phantom_target: PhantomData,
        }
    }
}

impl<'o, P, O, T> Iterator for GroupObjectIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: AbstractObject<'o, P>,
{
    type Item = Result<T>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.i >= self.cnt {
            None
        } else {
            let mut ckid = MaybeUninit::<CKID>::uninit();
            let i = match self.i.try_into() {
                Ok(v) => v,
                Err(e) => return Some(Err(Error::from(e))),
            };

            let rv = unsafe {
                (self.f)(
                    self.parent.get_pointer(),
                    self.parent.get_ckid(),
                    arg_in!(i),
                    arg_out!(ckid.as_mut_ptr(), CKID),
                )
            };
            if !rv {
                return Some(Err(Error::BadCall));
            }

            let ckid = unsafe { ckid.assume_init() };
            self.i += 1;

            Some(Ok(unsafe {
                T::with_sibling(self.parent, self.parent.get_pointer(), ckid)
            }))
        }
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        libiter_size_hint_body!(self.i, self.cnt)
    }
}

impl<'o, P, O, T> FusedIterator for GroupObjectIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: AbstractObject<'o, P>,
{
}

impl<'o, P, O, T> ExactSizeIterator for GroupObjectIter<'o, P, O, T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: AbstractObject<'o, P>,
{
    fn len(&self) -> usize {
        libiter_len_body!(self.i, self.cnt)
    }
}

// endregion

// region: Utility Functions

type FnCopyableValueGetter<T> = unsafe extern "C" fn(PBMVOID, CKID, param_out!(T)) -> BMBOOL;
type FnCopyableValueSetter<T> = unsafe extern "C" fn(PBMVOID, CKID, param_in!(T)) -> BMBOOL;

fn get_copyable_value<'o, P, O, T>(o: &O, f: FnCopyableValueGetter<T>) -> Result<T>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
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

fn set_copyable_value<'o, P, O, T>(o: &O, f: FnCopyableValueSetter<T>, data: T) -> Result<()>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
    T: Sized + Copy,
{
    bmap_exec!(f(o.get_pointer(), o.get_ckid(), arg_in!(data)));
    Ok(())
}

// YYC MARK:
// Due to the pointer to C-style stirng is also copyable,
// so we can reuse these code.

type FnStringValueGetter = FnCopyableValueGetter<CKSTRING>;
type FnStringValueSetter = FnCopyableValueSetter<CKSTRING>;

fn get_string_value<'o, P, O>(o: &O, f: FnStringValueGetter) -> Result<Option<String>>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
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

fn set_string_value<'o, P, O>(o: &O, f: FnStringValueSetter, s: Option<&str>) -> Result<()>
where
    P: AbstractPointer<'o> + ?Sized,
    O: AbstractObject<'o, P> + ?Sized,
{
    // Build raw string pointer.
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

pub trait BMObjectDecl<'o, P>: AbstractObject<'o, P>
where
    P: AbstractPointer<'o> + ?Sized,
{
    fn get_name(&self) -> Result<Option<String>> {
        get_string_value(self, bmap::BMObject_GetName)
    }
    fn set_name(&mut self, name: Option<&str>) -> Result<()> {
        set_string_value(self, bmap::BMObject_SetName, name)
    }
}

pub trait BMTextureDecl<'o, P>: BMObjectDecl<'o, P>
where
    P: AbstractPointer<'o> + ?Sized + 'o,
{
    fn get_file_name(&self) -> Result<Option<String>> {
        get_string_value(self, bmap::BMTexture_GetFileName)
    }

    // YYC MARK:
    // Although "load_image" and "save_image" is not a "value setter",
    // but they have same function signature so we can reuse those code.
    fn load_image(&mut self, filepath: &str) -> Result<()> {
        set_string_value(self, bmap::BMTexture_LoadImage, Some(filepath))
    }
    fn save_image(&self, filepath: &str) -> Result<()> {
        set_string_value(self, bmap::BMTexture_SaveImage, Some(filepath))
    }

    fn get_save_options(&self) -> Result<bmap::CK_TEXTURE_SAVEOPTIONS> {
        get_copyable_value(self, bmap::BMTexture_GetSaveOptions)
    }
    fn set_save_options(&mut self, opt: bmap::CK_TEXTURE_SAVEOPTIONS) -> Result<()> {
        set_copyable_value(self, bmap::BMTexture_SetSaveOptions, opt)
    }
    fn get_video_format(&self) -> Result<bmap::VX_PIXELFORMAT> {
        get_copyable_value(self, bmap::BMTexture_GetVideoFormat)
    }
    fn set_video_format(&mut self, fmt: bmap::VX_PIXELFORMAT) -> Result<()> {
        set_copyable_value(self, bmap::BMTexture_SetVideoFormat, fmt)
    }
}

pub trait BMMaterialDecl<'o, P>: BMObjectDecl<'o, P>
where
    P: AbstractPointer<'o> + ?Sized + 'o,
{
    fn get_diffuse(&self) -> Result<bmap::VxColor> {
        get_copyable_value(self, bmap::BMMaterial_GetDiffuse)
    }
    fn set_diffuse(&mut self, col: bmap::VxColor) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetDiffuse, col)
    }
    fn get_ambient(&self) -> Result<bmap::VxColor> {
        get_copyable_value(self, bmap::BMMaterial_GetAmbient)
    }
    fn set_ambient(&mut self, col: bmap::VxColor) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetAmbient, col)
    }
    fn get_specular(&self) -> Result<bmap::VxColor> {
        get_copyable_value(self, bmap::BMMaterial_GetSpecular)
    }
    fn set_specular(&mut self, col: bmap::VxColor) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetSpecular, col)
    }
    fn get_emissive(&self) -> Result<bmap::VxColor> {
        get_copyable_value(self, bmap::BMMaterial_GetEmissive)
    }
    fn set_emissive(&mut self, col: bmap::VxColor) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetEmissive, col)
    }

    fn get_specular_power(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMMaterial_GetSpecularPower)
    }
    fn set_specular_power(&mut self, power: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetSpecularPower, power)
    }

    // YYC MARK:
    // We use "value getter" to get CKID first then convert it to our instance.
    // Same pattern for using "value setter".
    fn get_texture(&'o self) -> Result<Option<BMTexture<'o, P>>> {
        let ckid: CKID = get_copyable_value(self, bmap::BMMaterial_GetTexture)?;
        Ok(if ckid == INVALID_CKID {
            None
        } else {
            Some(unsafe { BMTexture::with_sibling(self, self.get_pointer(), ckid) })
        })
    }
    fn set_texture(&mut self, texture: Option<&BMTexture<'o, P>>) -> Result<()> {
        let ckid: CKID = match texture {
            Some(texture) => unsafe { texture.get_ckid() },
            None => INVALID_CKID,
        };
        set_copyable_value(self, bmap::BMMaterial_SetTexture, ckid)
    }

    fn get_texture_border_color(&self) -> Result<bmap::VxColor> {
        let intermediary = get_copyable_value(self, bmap::BMMaterial_GetTextureBorderColor)?;
        Ok(bmap::VxColor::with_dword(intermediary))
    }
    fn set_texture_border_color(&mut self, col: bmap::VxColor) -> Result<()> {
        let intermediary = col.to_dword();
        set_copyable_value(self, bmap::BMMaterial_SetTextureBorderColor, intermediary)
    }

    fn get_texture_blend_mode(&self) -> Result<bmap::VXTEXTURE_BLENDMODE> {
        get_copyable_value(self, bmap::BMMaterial_GetTextureBlendMode)
    }
    fn set_texture_blend_mode(&mut self, data: bmap::VXTEXTURE_BLENDMODE) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetTextureBlendMode, data)
    }
    fn get_texture_min_mode(&self) -> Result<bmap::VXTEXTURE_FILTERMODE> {
        get_copyable_value(self, bmap::BMMaterial_GetTextureMinMode)
    }
    fn set_texture_min_mode(&mut self, data: bmap::VXTEXTURE_FILTERMODE) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetTextureMinMode, data)
    }
    fn get_texture_mag_mode(&self) -> Result<bmap::VXTEXTURE_FILTERMODE> {
        get_copyable_value(self, bmap::BMMaterial_GetTextureMagMode)
    }
    fn set_texture_mag_mode(&mut self, data: bmap::VXTEXTURE_FILTERMODE) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetTextureMagMode, data)
    }
    fn get_texture_address_mode(&self) -> Result<bmap::VXTEXTURE_ADDRESSMODE> {
        get_copyable_value(self, bmap::BMMaterial_GetTextureAddressMode)
    }
    fn set_texture_address_mode(&mut self, data: bmap::VXTEXTURE_ADDRESSMODE) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetTextureAddressMode, data)
    }
    fn get_source_blend(&self) -> Result<bmap::VXBLEND_MODE> {
        get_copyable_value(self, bmap::BMMaterial_GetSourceBlend)
    }
    fn set_source_blend(&mut self, data: bmap::VXBLEND_MODE) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetSourceBlend, data)
    }
    fn get_dest_blend(&self) -> Result<bmap::VXBLEND_MODE> {
        get_copyable_value(self, bmap::BMMaterial_GetDestBlend)
    }
    fn set_dest_blend(&mut self, data: bmap::VXBLEND_MODE) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetDestBlend, data)
    }
    fn get_fill_mode(&self) -> Result<bmap::VXFILL_MODE> {
        get_copyable_value(self, bmap::BMMaterial_GetFillMode)
    }
    fn set_fill_mode(&mut self, data: bmap::VXFILL_MODE) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetFillMode, data)
    }
    fn get_shade_mode(&self) -> Result<bmap::VXSHADE_MODE> {
        get_copyable_value(self, bmap::BMMaterial_GetShadeMode)
    }
    fn set_shade_mode(&mut self, data: bmap::VXSHADE_MODE) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetShadeMode, data)
    }

    fn get_alpha_test_enabled(&self) -> Result<bool> {
        get_copyable_value(self, bmap::BMMaterial_GetAlphaTestEnabled)
    }
    fn set_alpha_test_enabled(&mut self, data: bool) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetAlphaTestEnabled, data)
    }
    fn get_alpha_blend_enabled(&self) -> Result<bool> {
        get_copyable_value(self, bmap::BMMaterial_GetAlphaBlendEnabled)
    }
    fn set_alpha_blend_enabled(&mut self, data: bool) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetAlphaBlendEnabled, data)
    }
    fn get_perspective_correction_enabled(&self) -> Result<bool> {
        get_copyable_value(self, bmap::BMMaterial_GetPerspectiveCorrectionEnabled)
    }
    fn set_perspective_correction_enabled(&mut self, data: bool) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetPerspectiveCorrectionEnabled, data)
    }
    fn get_z_write_enabled(&self) -> Result<bool> {
        get_copyable_value(self, bmap::BMMaterial_GetZWriteEnabled)
    }
    fn set_z_write_enabled(&mut self, data: bool) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetZWriteEnabled, data)
    }
    fn get_two_sided_enabled(&self) -> Result<bool> {
        get_copyable_value(self, bmap::BMMaterial_GetTwoSidedEnabled)
    }
    fn set_two_sided_enabled(&mut self, data: bool) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetTwoSidedEnabled, data)
    }

    fn get_alpha_ref(&self) -> Result<u8> {
        get_copyable_value(self, bmap::BMMaterial_GetAlphaRef)
    }
    fn set_alpha_ref(&mut self, data: u8) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetAlphaRef, data)
    }

    fn get_alpha_func(&self) -> Result<bmap::VXCMPFUNC> {
        get_copyable_value(self, bmap::BMMaterial_GetAlphaFunc)
    }
    fn set_alpha_func(&mut self, data: bmap::VXCMPFUNC) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetAlphaFunc, data)
    }
    fn get_z_func(&self) -> Result<bmap::VXCMPFUNC> {
        get_copyable_value(self, bmap::BMMaterial_GetZFunc)
    }
    fn set_z_func(&mut self, data: bmap::VXCMPFUNC) -> Result<()> {
        set_copyable_value(self, bmap::BMMaterial_SetZFunc, data)
    }
}

pub trait BMMeshDecl<'o, P>: BMObjectDecl<'o, P>
where
    P: AbstractPointer<'o> + ?Sized + 'o,
{
    fn get_lit_mode(&self) -> Result<bmap::VXMESH_LITMODE> {
        get_copyable_value(self, bmap::BMMesh_GetLitMode)
    }
    fn set_lit_mode(&mut self, data: bmap::VXMESH_LITMODE) -> Result<()> {
        set_copyable_value(self, bmap::BMMesh_SetLitMode, data)
    }

    fn get_vertex_count(&self) -> Result<u32> {
        get_copyable_value(self, bmap::BMMesh_GetVertexCount)
    }
    fn set_vertex_count(&mut self, count: u32) -> Result<()> {
        set_copyable_value(self, bmap::BMMesh_SetVertexCount, count)
    }
    fn get_vertex_positions(&'o self) -> Result<StructIter<'o, P, Self, bmap::VxVector3>> {
        let ptr = get_copyable_value(self, bmap::BMMesh_GetVertexPositions)?;
        struct_iterator(self, ptr, self.get_vertex_count()?.try_into()?)
    }
    fn set_vertex_positions<I>(&mut self, iem: I) -> Result<()>
    where
        I: Iterator<Item = bmap::VxVector3>,
    {
        let ptr = get_copyable_value(self, bmap::BMMesh_GetVertexPositions)?;
        struct_assigner(ptr, self.get_vertex_count()?.try_into()?, iem)
    }
    fn get_vertex_normals(&'o self) -> Result<StructIter<'o, P, Self, bmap::VxVector3>> {
        let ptr = get_copyable_value(self, bmap::BMMesh_GetVertexNormals)?;
        struct_iterator(self, ptr, self.get_vertex_count()?.try_into()?)
    }
    fn set_vertex_normals<I>(&mut self, iem: I) -> Result<()>
    where
        I: Iterator<Item = bmap::VxVector3>,
    {
        let ptr = get_copyable_value(self, bmap::BMMesh_GetVertexNormals)?;
        struct_assigner(ptr, self.get_vertex_count()?.try_into()?, iem)
    }
    fn get_vertex_uvs(&'o self) -> Result<StructIter<'o, P, Self, bmap::VxVector2>> {
        let ptr = get_copyable_value(self, bmap::BMMesh_GetVertexUVs)?;
        struct_iterator(self, ptr, self.get_vertex_count()?.try_into()?)
    }
    fn set_vertex_uvs<I>(&mut self, iem: I) -> Result<()>
    where
        I: Iterator<Item = bmap::VxVector2>,
    {
        let ptr = get_copyable_value(self, bmap::BMMesh_GetVertexUVs)?;
        struct_assigner(ptr, self.get_vertex_count()?.try_into()?, iem)
    }

    fn get_face_count(&self) -> Result<u32> {
        get_copyable_value(self, bmap::BMMesh_GetFaceCount)
    }
    fn set_face_count(&mut self, count: u32) -> Result<()> {
        set_copyable_value(self, bmap::BMMesh_SetFaceCount, count)
    }
    fn get_face_indices(&'o self) -> Result<StructIter<'o, P, Self, bmap::CKShortFaceIndices>> {
        let ptr =
            get_copyable_value(self, bmap::BMMesh_GetFaceIndices)? as *mut bmap::CKShortFaceIndices;
        struct_iterator(self, ptr, self.get_face_count()?.try_into()?)
    }
    fn set_face_indices<I>(&mut self, iem: I) -> Result<()>
    where
        I: Iterator<Item = bmap::CKShortFaceIndices>,
    {
        let ptr =
            get_copyable_value(self, bmap::BMMesh_GetFaceIndices)? as *mut bmap::CKShortFaceIndices;
        struct_assigner(ptr, self.get_face_count()?.try_into()?, iem)
    }
    fn get_face_material_slot_indexs(&'o self) -> Result<StructIter<'o, P, Self, CKWORD>> {
        let ptr = get_copyable_value(self, bmap::BMMesh_GetFaceMaterialSlotIndexs)?;
        struct_iterator(self, ptr, self.get_face_count()?.try_into()?)
    }
    fn set_face_material_slot_indexs<I>(&mut self, iem: I) -> Result<()>
    where
        I: Iterator<Item = CKWORD>,
    {
        let ptr = get_copyable_value(self, bmap::BMMesh_GetFaceMaterialSlotIndexs)?;
        struct_assigner(ptr, self.get_face_count()?.try_into()?, iem)
    }

    fn get_material_slot_count(&self) -> Result<u32> {
        get_copyable_value(self, bmap::BMMesh_GetMaterialSlotCount)
    }
    fn set_material_slot_count(&mut self, count: u32) -> Result<()> {
        set_copyable_value(self, bmap::BMMesh_SetMaterialSlotCount, count)
    }
    fn get_material_slots(&'o self) -> Result<MeshMtlSlotIter<'o, P, Self, BMMaterial<'o, P>>> {
        Ok(MeshMtlSlotIter::new(
            self,
            bmap::BMMesh_GetMaterialSlot,
            self.get_material_slot_count()?.try_into()?,
        ))
    }
    fn set_material_slots<I>(&mut self, mut iem: I) -> Result<()>
    where
        I: Iterator<Item = Option<BMMaterial<'o, P>>>,
    {
        let cnt = self.get_material_slot_count()?;
        for i in 0..cnt {
            // Fetch item
            let item = match iem.next() {
                Some(v) => v,
                None => return Err(Error::OutOfLength),
            };
            let ckid = match item {
                Some(v) => unsafe { v.get_ckid() },
                None => INVALID_CKID
            };
            // Write item
            bmap_exec!(bmap::BMMesh_SetMaterialSlot(self.get_pointer(), self.get_ckid(), i, ckid))
        }
        Ok(())
    }
}

pub trait BM3dEntityDecl<'o, P>: BMObjectDecl<'o, P>
where
    P: AbstractPointer<'o> + ?Sized + 'o,
{
    fn get_world_matrix(&self) -> Result<bmap::VxMatrix> {
        get_copyable_value(self, bmap::BM3dEntity_GetWorldMatrix)
    }
    fn set_world_matrix(&mut self, mat: bmap::VxMatrix) -> Result<()> {
        set_copyable_value(self, bmap::BM3dEntity_SetWorldMatrix, mat)
    }

    // YYC MARK:
    // Same reason for the reuse of "value setter" and "value getter".
    fn get_current_mesh(&'o self) -> Result<Option<BMMesh<'o, P>>> {
        let ckid: CKID = get_copyable_value(self, bmap::BM3dEntity_GetCurrentMesh)?;
        Ok(if ckid == INVALID_CKID {
            None
        } else {
            Some(unsafe { BMMesh::with_sibling(self, self.get_pointer(), ckid) })
        })
    }
    fn set_current_mesh(&mut self, mesh: Option<BMMesh<'o, P>>) -> Result<()> {
        let ckid: CKID = match mesh {
            Some(mesh) => unsafe { mesh.get_ckid() },
            None => INVALID_CKID,
        };
        set_copyable_value(self, bmap::BM3dEntity_SetCurrentMesh, ckid)
    }

    fn get_visibility(&self) -> Result<bool> {
        get_copyable_value(self, bmap::BM3dEntity_GetVisibility)
    }
    fn set_visibility(&mut self, data: bool) -> Result<()> {
        set_copyable_value(self, bmap::BM3dEntity_SetVisibility, data)
    }
}

pub trait BM3dObjectDecl<'o, P>: BM3dEntityDecl<'o, P>
where
    P: AbstractPointer<'o> + ?Sized + 'o,
{
}

pub trait BMLightDecl<'o, P>: BM3dEntityDecl<'o, P>
where
    P: AbstractPointer<'o> + ?Sized + 'o,
{
    fn get_type(&self) -> Result<bmap::VXLIGHT_TYPE> {
        get_copyable_value(self, bmap::BMLight_GetType)
    }
    fn set_type(&mut self, data: bmap::VXLIGHT_TYPE) -> Result<()> {
        set_copyable_value(self, bmap::BMLight_SetType, data)
    }

    fn get_color(&self) -> Result<bmap::VxColor> {
        get_copyable_value(self, bmap::BMLight_GetColor)
    }
    fn set_color(&mut self, col: bmap::VxColor) -> Result<()> {
        set_copyable_value(self, bmap::BMLight_SetColor, col)
    }

    fn get_constant_attenuation(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMLight_GetConstantAttenuation)
    }
    fn set_constant_attenuation(&mut self, val: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMLight_SetConstantAttenuation, val)
    }
    fn get_linear_attenuation(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMLight_GetLinearAttenuation)
    }
    fn set_linear_attenuation(&mut self, val: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMLight_SetLinearAttenuation, val)
    }
    fn get_quadratic_attenuation(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMLight_GetQuadraticAttenuation)
    }
    fn set_quadratic_attenuation(&mut self, val: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMLight_SetQuadraticAttenuation, val)
    }

    fn get_range(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMLight_GetRange)
    }
    fn set_range(&mut self, val: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMLight_SetRange, val)
    }

    fn get_hot_spot(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMLight_GetHotSpot)
    }
    fn set_hot_spot(&mut self, val: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMLight_SetHotSpot, val)
    }
    fn get_falloff(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMLight_GetFalloff)
    }
    fn set_falloff(&mut self, val: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMLight_SetFalloff, val)
    }
    fn get_falloff_shape(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMLight_GetFalloffShape)
    }
    fn set_falloff_shape(&mut self, val: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMLight_SetFalloffShape, val)
    }
}

pub trait BMTargetLightDecl<'o, P>: BMLightDecl<'o, P>
where
    P: AbstractPointer<'o> + ?Sized + 'o,
{
}

pub trait BMCameraDecl<'o, P>: BM3dEntityDecl<'o, P>
where
    P: AbstractPointer<'o> + ?Sized + 'o,
{
    fn get_projection_type(&self) -> Result<bmap::CK_CAMERA_PROJECTION> {
        get_copyable_value(self, bmap::BMCamera_GetProjectionType)
    }
    fn set_projection_type(&mut self, data: bmap::CK_CAMERA_PROJECTION) -> Result<()> {
        set_copyable_value(self, bmap::BMCamera_SetProjectionType, data)
    }

    fn get_orthographic_zoom(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMCamera_GetOrthographicZoom)
    }
    fn set_orthographic_zoom(&mut self, val: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMCamera_SetOrthographicZoom, val)
    }

    fn get_front_plane(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMCamera_GetFrontPlane)
    }
    fn set_front_plane(&mut self, val: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMCamera_SetFrontPlane, val)
    }
    fn get_back_plane(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMCamera_GetBackPlane)
    }
    fn set_back_plane(&mut self, val: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMCamera_SetBackPlane, val)
    }
    fn get_fov(&self) -> Result<f32> {
        get_copyable_value(self, bmap::BMCamera_GetFov)
    }
    fn set_fov(&mut self, val: f32) -> Result<()> {
        set_copyable_value(self, bmap::BMCamera_SetFov, val)
    }

    fn get_aspect_ratio(&self) -> Result<(u32, u32)> {
        let mut width = MaybeUninit::<u32>::uninit();
        let mut height = MaybeUninit::<u32>::uninit();
        bmap_exec!(bmap::BMCamera_GetAspectRatio(
            self.get_pointer(),
            self.get_ckid(),
            arg_out!(width.as_mut_ptr(), CKDWORD),
            arg_out!(height.as_mut_ptr(), CKDWORD)
        ));
        Ok(unsafe { (width.assume_init(), height.assume_init()) })
    }
    fn set_aspect_ratio(&self, width: u32, height: u32) -> Result<()> {
        bmap_exec!(bmap::BMCamera_SetAspectRatio(
            self.get_pointer(),
            self.get_ckid(),
            arg_in!(width),
            arg_in!(height)
        ));
        Ok(())
    }
}

pub trait BMTargetCameraDecl<'o, P>: BMCameraDecl<'o, P>
where
    P: AbstractPointer<'o> + ?Sized + 'o,
{
}

pub trait BMGroupDecl<'o, P>: BMObjectDecl<'o, P>
where
    P: AbstractPointer<'o> + ?Sized + 'o,
{
    fn add_object(&mut self, member: Option<BM3dObject<'o, P>>) -> Result<()> {
        let ckid: CKID = match member {
            Some(member) => unsafe { member.get_ckid() },
            None => INVALID_CKID,
        };
        set_copyable_value(self, bmap::BMGroup_AddObject, ckid)
    }

    fn get_object_count(&self) -> Result<u32> {
        get_copyable_value(self, bmap::BMGroup_GetObjectCount)
    }

    fn get_objects(&'o self) -> Result<GroupObjectIter<'o, P, Self, BM3dObject<'o, P>>> {
        Ok(GroupObjectIter::new(
            self,
            bmap::BMGroup_GetObject,
            self.get_object_count()?.try_into()?,
        ))
    }
}

// endregion

// region: Structs

libobj_struct!(BMObject);
libobj_impl_clone!(BMObject);
libobj_impl_eq_ord_hash!(BMObject);
libobj_impl_abstract_object!(BMObject);
libobj_impl_obj_trait!(BMObject, BMObjectDecl);

libobj_struct!(BMTexture);
libobj_impl_clone!(BMTexture);
libobj_impl_eq_ord_hash!(BMTexture);
libobj_impl_abstract_object!(BMTexture);
libobj_impl_obj_trait!(BMTexture, BMObjectDecl);
libobj_impl_obj_trait!(BMTexture, BMTextureDecl);

libobj_struct!(BMMaterial);
libobj_impl_clone!(BMMaterial);
libobj_impl_eq_ord_hash!(BMMaterial);
libobj_impl_abstract_object!(BMMaterial);
libobj_impl_obj_trait!(BMMaterial, BMObjectDecl);
libobj_impl_obj_trait!(BMMaterial, BMMaterialDecl);

libobj_struct!(BMMesh);
libobj_impl_clone!(BMMesh);
libobj_impl_eq_ord_hash!(BMMesh);
libobj_impl_abstract_object!(BMMesh);
libobj_impl_obj_trait!(BMMesh, BMObjectDecl);
libobj_impl_obj_trait!(BMMesh, BMMeshDecl);

libobj_struct!(BM3dEntity);
libobj_impl_clone!(BM3dEntity);
libobj_impl_eq_ord_hash!(BM3dEntity);
libobj_impl_abstract_object!(BM3dEntity);
libobj_impl_obj_trait!(BM3dEntity, BMObjectDecl);
libobj_impl_obj_trait!(BM3dEntity, BM3dEntityDecl);

libobj_struct!(BM3dObject);
libobj_impl_clone!(BM3dObject);
libobj_impl_eq_ord_hash!(BM3dObject);
libobj_impl_abstract_object!(BM3dObject);
libobj_impl_obj_trait!(BM3dObject, BMObjectDecl);
libobj_impl_obj_trait!(BM3dObject, BM3dEntityDecl);
libobj_impl_obj_trait!(BM3dObject, BM3dObjectDecl);

libobj_struct!(BMLight);
libobj_impl_clone!(BMLight);
libobj_impl_eq_ord_hash!(BMLight);
libobj_impl_abstract_object!(BMLight);
libobj_impl_obj_trait!(BMLight, BMObjectDecl);
libobj_impl_obj_trait!(BMLight, BM3dEntityDecl);
libobj_impl_obj_trait!(BMLight, BMLightDecl);

libobj_struct!(BMTargetLight);
libobj_impl_clone!(BMTargetLight);
libobj_impl_eq_ord_hash!(BMTargetLight);
libobj_impl_abstract_object!(BMTargetLight);
libobj_impl_obj_trait!(BMTargetLight, BMObjectDecl);
libobj_impl_obj_trait!(BMTargetLight, BM3dEntityDecl);
libobj_impl_obj_trait!(BMTargetLight, BMLightDecl);
libobj_impl_obj_trait!(BMTargetLight, BMTargetLightDecl);

libobj_struct!(BMCamera);
libobj_impl_clone!(BMCamera);
libobj_impl_eq_ord_hash!(BMCamera);
libobj_impl_abstract_object!(BMCamera);
libobj_impl_obj_trait!(BMCamera, BMObjectDecl);
libobj_impl_obj_trait!(BMCamera, BM3dEntityDecl);
libobj_impl_obj_trait!(BMCamera, BMCameraDecl);

libobj_struct!(BMTargetCamera);
libobj_impl_clone!(BMTargetCamera);
libobj_impl_eq_ord_hash!(BMTargetCamera);
libobj_impl_abstract_object!(BMTargetCamera);
libobj_impl_obj_trait!(BMTargetCamera, BMObjectDecl);
libobj_impl_obj_trait!(BMTargetCamera, BM3dEntityDecl);
libobj_impl_obj_trait!(BMTargetCamera, BMCameraDecl);
libobj_impl_obj_trait!(BMTargetCamera, BMTargetCameraDecl);

libobj_struct!(BMGroup);
libobj_impl_clone!(BMGroup);
libobj_impl_eq_ord_hash!(BMGroup);
libobj_impl_abstract_object!(BMGroup);
libobj_impl_obj_trait!(BMGroup, BMObjectDecl);
libobj_impl_obj_trait!(BMGroup, BMGroupDecl);

// endregion

// endregion

// region: BMFileReader

libptr_struct!(BMFileReader);
libptr_impl_eq_ord_hash!(BMFileReader);
libptr_impl_abstract_pointer!(BMFileReader);

impl<'p> BMFileReader<'p> {
    fn new<I, T>(
        _: &'p BMap,
        file_name: &str,
        temp_folder: &str,
        texture_folder: &str,
        encodings: I,
    ) -> Result<Self>
    where
        I: Iterator<Item = T>,
        T: Into<Vec<u8>>,
    {
        let file_name = unsafe { marshaler::to_native_string(file_name)? };
        let temp_folder = unsafe { marshaler::to_native_string(temp_folder)? };
        let texture_folder = unsafe { marshaler::to_native_string(texture_folder)? };
        let encodings = unsafe { marshaler::to_native_string_array(encodings)? };

        let mut file = MaybeUninit::<PBMVOID>::uninit();
        bmap_exec!(bmap::BMFile_Load(
            file_name.as_raw(),
            temp_folder.as_raw(),
            texture_folder.as_raw(),
            bmap_rs_callback,
            encodings.len().try_into()?,
            encodings.as_raw(),
            arg_out!(file.as_mut_ptr(), PBMVOID)
        ));

        Ok(Self {
            handle: unsafe { file.assume_init() },
            parent: PhantomData::<&'p BMap>,
        })
    }
}

impl<'p> Drop for BMFileReader<'p> {
    fn drop(&mut self) {
        let _ = unsafe { bmap::BMFile_Free(self.handle) };
    }
}

type FnFileGetCount = unsafe extern "C" fn(PBMVOID, param_out!(CKDWORD)) -> BMBOOL;
type FnFileGetObject =
    unsafe extern "C" fn(PBMVOID, param_in!(CKDWORD), param_out!(CKID)) -> BMBOOL;

pub struct FileObjectIter<'p, O>
where
    O: AbstractObject<'p, BMFileReader<'p>>,
{
    fget: FnFileGetObject,
    cnt: usize,
    i: usize,
    parent: &'p BMFileReader<'p>,
    obj: PhantomData<O>,
}

impl<'p, O> FileObjectIter<'p, O>
where
    O: AbstractObject<'p, BMFileReader<'p>>,
{
    fn new(parent: &'p BMFileReader<'p>, fget: FnFileGetObject, cnt: usize) -> Self {
        Self {
            fget: fget,
            cnt: cnt,
            i: 0,
            parent: parent,
            obj: PhantomData,
        }
    }
}

impl<'p, O> Iterator for FileObjectIter<'p, O>
where
    O: AbstractObject<'p, BMFileReader<'p>>,
{
    type Item = Result<O>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.i >= self.cnt {
            None
        } else {
            let mut ckid = MaybeUninit::<CKID>::uninit();
            let i = match self.i.try_into() {
                Ok(v) => v,
                Err(e) => return Some(Err(Error::from(e))),
            };

            let rv = unsafe {
                (self.fget)(
                    self.parent.get_pointer(),
                    i,
                    arg_out!(ckid.as_mut_ptr(), CKID),
                )
            };
            if !rv {
                return Some(Err(Error::BadCall));
            }

            let ckid = unsafe { ckid.assume_init() };
            self.i += 1;

            Some(Ok(unsafe {
                O::with_parent(self.parent, self.parent.get_pointer(), ckid)
            }))
        }
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        libiter_size_hint_body!(self.i, self.cnt)
    }
}

impl<'p, O> FusedIterator for FileObjectIter<'p, O> where O: AbstractObject<'p, BMFileReader<'p>> {}

impl<'p, O> ExactSizeIterator for FileObjectIter<'p, O>
where
    O: AbstractObject<'p, BMFileReader<'p>>,
{
    fn len(&self) -> usize {
        libiter_len_body!(self.i, self.cnt)
    }
}

impl<'p> BMFileReader<'p> {
    fn get_generic_object_count(&self, fc: FnFileGetCount) -> Result<usize> {
        let mut cnt = MaybeUninit::<CKDWORD>::uninit();
        bmap_exec!(fc(self.get_pointer(), arg_out!(cnt.as_mut_ptr(), CKDWORD)));

        let cnt = unsafe { cnt.assume_init() };
        Ok(cnt.try_into()?)
    }
    fn get_generic_objects<O>(
        &'p self,
        fc: FnFileGetCount,
        fo: FnFileGetObject,
    ) -> Result<FileObjectIter<'p, O>>
    where
        O: AbstractObject<'p, BMFileReader<'p>>,
    {
        let cnt = self.get_generic_object_count(fc)?;
        Ok(FileObjectIter::new(self, fo, cnt))
    }

    pub fn get_texture_count(&'p self) -> Result<usize> {
        self.get_generic_object_count(bmap::BMFile_GetTextureCount)
    }
    pub fn get_textures(&'p self) -> Result<FileObjectIter<'p, BMTexture<'p, Self>>> {
        self.get_generic_objects(bmap::BMFile_GetTextureCount, bmap::BMFile_GetTexture)
    }
    pub fn get_material_count(&'p self) -> Result<usize> {
        self.get_generic_object_count(bmap::BMFile_GetMaterialCount)
    }
    pub fn get_materials(&'p self) -> Result<FileObjectIter<'p, BMMaterial<'p, Self>>> {
        self.get_generic_objects(bmap::BMFile_GetMaterialCount, bmap::BMFile_GetMaterial)
    }
    pub fn get_mesh_count(&'p self) -> Result<usize> {
        self.get_generic_object_count(bmap::BMFile_GetMeshCount)
    }
    pub fn get_meshes(&'p self) -> Result<FileObjectIter<'p, BMMesh<'p, Self>>> {
        self.get_generic_objects(bmap::BMFile_GetMeshCount, bmap::BMFile_GetMesh)
    }
    pub fn get_3dobject_count(&'p self) -> Result<usize> {
        self.get_generic_object_count(bmap::BMFile_Get3dObjectCount)
    }
    pub fn get_3dobjects(&'p self) -> Result<FileObjectIter<'p, BM3dObject<'p, Self>>> {
        self.get_generic_objects(bmap::BMFile_Get3dObjectCount, bmap::BMFile_Get3dObject)
    }
    pub fn get_group_count(&'p self) -> Result<usize> {
        self.get_generic_object_count(bmap::BMFile_GetGroupCount)
    }
    pub fn get_groups(&'p self) -> Result<FileObjectIter<'p, BMGroup<'p, Self>>> {
        self.get_generic_objects(bmap::BMFile_GetGroupCount, bmap::BMFile_GetGroup)
    }
    pub fn get_target_light_count(&'p self) -> Result<usize> {
        self.get_generic_object_count(bmap::BMFile_GetTargetLightCount)
    }
    pub fn get_target_lights(&'p self) -> Result<FileObjectIter<'p, BMTargetLight<'p, Self>>> {
        self.get_generic_objects(
            bmap::BMFile_GetTargetLightCount,
            bmap::BMFile_GetTargetLight,
        )
    }
    pub fn get_target_camera_count(&'p self) -> Result<usize> {
        self.get_generic_object_count(bmap::BMFile_GetTargetCameraCount)
    }
    pub fn get_target_cameras(&'p self) -> Result<FileObjectIter<'p, BMTargetCamera<'p, Self>>> {
        self.get_generic_objects(
            bmap::BMFile_GetTargetCameraCount,
            bmap::BMFile_GetTargetCamera,
        )
    }
}

// endregion

// region: BMFileWriter

libptr_struct!(BMFileWriter);
libptr_impl_eq_ord_hash!(BMFileWriter);
libptr_impl_abstract_pointer!(BMFileWriter);

impl<'p> BMFileWriter<'p> {
    fn new<I, T>(_: &'p BMap, temp_folder: &str, texture_folder: &str, encodings: I) -> Result<Self>
    where
        I: Iterator<Item = T>,
        T: Into<Vec<u8>>,
    {
        let temp_folder = unsafe { marshaler::to_native_string(temp_folder)? };
        let texture_folder = unsafe { marshaler::to_native_string(texture_folder)? };
        let encodings = unsafe { marshaler::to_native_string_array(encodings)? };

        let mut file = MaybeUninit::<PBMVOID>::uninit();
        bmap_exec!(bmap::BMFile_Create(
            temp_folder.as_raw(),
            texture_folder.as_raw(),
            bmap_rs_callback,
            encodings.len().try_into()?,
            encodings.as_raw(),
            arg_out!(file.as_mut_ptr(), PBMVOID)
        ));

        Ok(Self {
            handle: unsafe { file.assume_init() },
            parent: PhantomData::<&'p BMap>,
        })
    }
}

impl<'p> Drop for BMFileWriter<'p> {
    fn drop(&mut self) {
        let _ = unsafe { bmap::BMFile_Free(self.handle) };
    }
}

impl<'p> BMFileWriter<'p> {
    pub fn save(
        &self,
        filename: &str,
        texture_save_opt: bmap::CK_TEXTURE_SAVEOPTIONS,
        compress_level: Option<CKINT>,
    ) -> Result<()> {
        let filename = unsafe { marshaler::to_native_string(filename)? };
        let use_compress = compress_level.is_some();
        let compress_level = compress_level.unwrap_or(5);

        bmap_exec!(bmap::BMFile_Save(
            self.get_pointer(),
            filename.as_raw(),
            texture_save_opt,
            use_compress,
            compress_level
        ));

        Ok(())
    }
}

type FnFileCreateObject = unsafe extern "C" fn(PBMVOID, param_out!(CKID)) -> BMBOOL;

impl<'p> BMFileWriter<'p> {
    fn create_generic_objects<O>(&'p mut self, fc: FnFileCreateObject) -> Result<O>
    where
        O: AbstractObject<'p, BMFileWriter<'p>>,
    {
        let mut ckid = MaybeUninit::<CKID>::uninit();
        bmap_exec!(fc(self.get_pointer(), arg_out!(ckid.as_mut_ptr(), CKID)));

        let ckid = unsafe { ckid.assume_init() };
        Ok(unsafe { O::with_parent(self, self.get_pointer(), ckid) })
    }

    pub fn create_texture(&'p mut self) -> Result<BMTexture<'p, Self>> {
        self.create_generic_objects(bmap::BMFile_CreateTexture)
    }
    pub fn create_material(&'p mut self) -> Result<BMMaterial<'p, Self>> {
        self.create_generic_objects(bmap::BMFile_CreateMaterial)
    }
    pub fn create_mesh(&'p mut self) -> Result<BMMesh<'p, Self>> {
        self.create_generic_objects(bmap::BMFile_CreateMesh)
    }
    pub fn create_3dobject(&'p mut self) -> Result<BM3dObject<'p, Self>> {
        self.create_generic_objects(bmap::BMFile_Create3dObject)
    }
    pub fn create_group(&'p mut self) -> Result<BMGroup<'p, Self>> {
        self.create_generic_objects(bmap::BMFile_CreateGroup)
    }
    pub fn create_target_light(&'p mut self) -> Result<BMTargetLight<'p, Self>> {
        self.create_generic_objects(bmap::BMFile_CreateTargetLight)
    }
    pub fn create_target_camera(&'p mut self) -> Result<BMTargetCamera<'p, Self>> {
        self.create_generic_objects(bmap::BMFile_CreateTargetCamera)
    }
}

// endregion

// region: BMMeshTrans

libptr_struct!(BMMeshTrans);
libptr_impl_eq_ord_hash!(BMMeshTrans);
libptr_impl_abstract_pointer!(BMMeshTrans);

impl<'p> BMMeshTrans<'p> {
    fn new(_: &'p BMap) -> Result<Self> {
        let mut trans = MaybeUninit::<PBMVOID>::uninit();
        bmap_exec!(bmap::BMMeshTrans_New(arg_out!(trans.as_mut_ptr(), PBMVOID)));

        Ok(Self {
            handle: unsafe { trans.assume_init() },
            parent: PhantomData::<&'p BMap>,
        })
    }
}

impl<'b> Drop for BMMeshTrans<'b> {
    fn drop(&mut self) {
        let _ = unsafe { bmap::BMMeshTrans_Delete(self.handle) };
    }
}

impl<'b> BMMeshTrans<'b> {
    pub fn parse<P>(&self, objmesh: BMMesh<'b, P>) -> Result<()>
    where
        P: AbstractPointer<'b> + ?Sized,
    {
        bmap_exec!(bmap::BMMeshTrans_Parse(
            self.get_pointer(),
            objmesh.get_pointer(),
            objmesh.get_ckid()
        ));
        Ok(())
    }
}

type FnProtoMeshTransSetCount = unsafe extern "C" fn(PBMVOID, param_in!(CKDWORD)) -> BMBOOL;
type FnProtoMeshTransGetMem<T> = unsafe extern "C" fn(PBMVOID, param_out!(T)) -> BMBOOL;

impl<'b> BMMeshTrans<'b> {
    fn prepare_step_count(&mut self, f: FnProtoMeshTransSetCount, count: u32) -> Result<()> {
        bmap_exec!(f(self.get_pointer(), arg_in!(count)));
        Ok(())
    }
    fn prepare_step_mem<T>(&mut self, f: FnProtoMeshTransGetMem<T>) -> Result<T>
    where
        T: Copy + Sized,
    {
        let mut mem = MaybeUninit::<T>::uninit();
        bmap_exec!(f(self.get_pointer(), arg_out!(mem.as_mut_ptr(), T)));
        Ok(unsafe { mem.assume_init() })
    }

    pub fn prepare_vertex<I>(&mut self, count: u32, iem: I) -> Result<()>
    where
        I: Iterator<Item = bmap::VxVector3>,
    {
        self.prepare_step_count(bmap::BMMeshTrans_PrepareVertexCount, count)?;
        let mem = self.prepare_step_mem(bmap::BMMeshTrans_PrepareVertex)?;
        struct_assigner(mem, count.try_into()?, iem)
    }
    pub fn prepare_normal<I>(&mut self, count: u32, iem: I) -> Result<()>
    where
        I: Iterator<Item = bmap::VxVector3>,
    {
        self.prepare_step_count(bmap::BMMeshTrans_PrepareNormalCount, count)?;
        let mem = self.prepare_step_mem(bmap::BMMeshTrans_PrepareNormal)?;
        struct_assigner(mem, count.try_into()?, iem)
    }
    pub fn prepare_uv<I>(&mut self, count: u32, iem: I) -> Result<()>
    where
        I: Iterator<Item = bmap::VxVector2>,
    {
        self.prepare_step_count(bmap::BMMeshTrans_PrepareUVCount, count)?;
        let mem = self.prepare_step_mem(bmap::BMMeshTrans_PrepareUV)?;
        struct_assigner(mem, count.try_into()?, iem)
    }
    pub fn prepare_mtl_slot<I, P>(&'b mut self, count: u32, iem: I) -> Result<()>
    where
        I: Iterator<Item = Option<BMMaterial<'b, P>>>,
        P: AbstractPointer<'b> + ?Sized + 'b,
    {
        self.prepare_step_count(bmap::BMMeshTrans_PrepareMtlSlotCount, count)?;
        let mem = self.prepare_step_mem(bmap::BMMeshTrans_PrepareMtlSlot)?;
        struct_assigner(
            mem,
            count.try_into()?,
            iem.map(|m| match m {
                Some(m) => unsafe { m.get_ckid() },
                None => INVALID_CKID,
            }),
        )
    }
    pub fn prepare_face<IVec, INml, IUv, IMtl>(
        &mut self,
        count: u32,
        vec_idx: IVec,
        nml_idx: INml,
        uv_idx: IUv,
        mtl_idx: IMtl,
    ) -> Result<()>
    where
        IVec: Iterator<Item = bmap::CKFaceIndices>,
        INml: Iterator<Item = bmap::CKFaceIndices>,
        IUv: Iterator<Item = bmap::CKFaceIndices>,
        IMtl: Iterator<Item = bmap::CKDWORD>,
    {
        self.prepare_step_count(bmap::BMMeshTrans_PrepareFaceCount, count)?;

        let mem = self.prepare_step_mem(bmap::BMMeshTrans_PrepareFaceVertexIndices)?
            as *mut bmap::CKFaceIndices;
        struct_assigner(mem, count.try_into()?, vec_idx)?;
        let mem = self.prepare_step_mem(bmap::BMMeshTrans_PrepareFaceNormalIndices)?
            as *mut bmap::CKFaceIndices;
        struct_assigner(mem, count.try_into()?, nml_idx)?;
        let mem = self.prepare_step_mem(bmap::BMMeshTrans_PrepareFaceUVIndices)?
            as *mut bmap::CKFaceIndices;
        struct_assigner(mem, count.try_into()?, uv_idx)?;
        let mem = self.prepare_step_mem(bmap::BMMeshTrans_PrepareFaceMtlSlot)?;
        struct_assigner(mem, count.try_into()?, mtl_idx)?;

        Ok(())
    }
}

// endregion
