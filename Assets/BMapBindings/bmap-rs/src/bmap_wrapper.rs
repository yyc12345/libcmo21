//! The module includes all senior wrappers for BMap FFI calling in Rust style.
//!
//! This module is what user of this library should use.
use crate::bmap::{self, BMBOOL, CKDWORD, CKID, CKSTRING, PBMVOID};
use crate::marshaler;
use std::cmp::Ordering;
use std::hash::{Hash, Hasher};
use std::iter::{ExactSizeIterator, FusedIterator, Iterator};
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

/// Specialized AbstractPointer which is only implemented by BMFileReader and BMFileWriter.
trait BMFileRW: AbstractPointer {}

/// Specialized AbstractPointer which is only implemented by BM Objects.
trait BMFileObject: AbstractObject {}

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
            None => return Err(Error::OutOfLength),
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

static BMAP_SINGLETON: LazyLock<Result<BMapGuard>> = LazyLock::new(|| BMapGuard::new());

fn get_bmap_guard_singleton() -> Result<&'static BMapGuard> {
    BMAP_SINGLETON.as_ref().map_err(|e| e.clone())
}

pub fn is_bmap_available() -> bool {
    BMAP_SINGLETON.is_ok()
}

// endregion

// region: BMFileReader

// type FnProtoGetCount = unsafe extern "C" fn(PBMVOID, param_out!(CKDWORD));
// type FnProtoGetObject = unsafe extern "C" fn(PBMVOID, param_in!(CKDWORD), param_out!(CKID));

// pub struct FileObjectIterator<'a, RW, FT, O>
// where
//     RW: BMFileRW,
//     FT: Fn(PBMVOID, CKID) -> O,
//     O: AbstractObject,
// {
//     fget: FnProtoGetObject,
//     fcret: FT,
//     cnt: usize,
//     i: usize,
//     rw: &'a RW,
// }

pub struct BMFileReader<'a> {
    handle: PBMVOID,
    phantom: PhantomData<&'a BMapGuard>,
}

impl<'a> BMFileReader<'a> {
    pub fn new<T>(
        file_name: &str,
        temp_folder: &str,
        texture_folder: &str,
        encodings: &[T],
    ) -> Result<Self>
    where
        T: Into<Vec<u8>> + Copy,
    {
        let file_name = unsafe { marshaler::to_native_string(file_name)? };
        let temp_folder = unsafe { marshaler::to_native_string(temp_folder)? };
        let texture_folder = unsafe { marshaler::to_native_string(texture_folder) }?;
        let encodings = unsafe { marshaler::to_native_string_array(encodings)? };
    }
}

// impl<'a> BMFileReader<'a> {

//     fn get_generic_object_count<O, FC>(&self, fc: FnProtoGetCount) -> Result<usize> {

//     }

//     fn get_generic_objects<'a, O, FC, FO>(&'a self, fc: FnProtoGetCount, fo: FnProtoGetObject) -> Result<FileObjectIterator<'a, Self, _, O>> {
//         todo!()
//     }

// }

impl<'a> AbstractPointer for BMFileReader<'a> {
    unsafe fn get_pointer(&self) -> PBMVOID {
        self.handle
    }
}

impl<'a> BMFileRW for BMFileReader<'a> {}

// endregion

// region: BMFileWriter

pub struct BMFileWriter<'a> {
    handle: PBMVOID,
    phantom: PhantomData<&'a BMapGuard>,
}

impl<'a> AbstractPointer for BMFileWriter<'a> {
    unsafe fn get_pointer(&self) -> PBMVOID {
        self.handle
    }
}

impl<'a> BMFileRW for BMFileWriter<'a> {}

// endregion

// region: BMObjects

// region: Utility Functions

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

pub trait BMObject: AbstractObject {
    fn get_name(&self) -> Result<Option<String>> {
        get_string_value(self, bmap::BMObject_GetName)
    }
    fn set_name(&mut self, name: Option<&str>) -> Result<()> {
        set_string_value(self, bmap::BMObject_SetName, name)
    }
}

pub trait BMTexture: BMObject {
    fn get_file_name(&self) -> Result<Option<String>> {
        get_string_value(self, bmap::BMTexture_GetFileName)
    }

    // YYC MARK:
    // Although load_image and save_image is not a "value setter",
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

pub trait BMMaterial: BMObject {
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

    fn get_texture(&self) -> Result<Option<Box<dyn BMTexture>>> {
        todo!();
    }
    fn set_texture(&mut self, texture: Option<&dyn BMTexture>) -> Result<()> {
        todo!();
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

pub trait BMMesh: BMObject {
    fn get_lit_mode(&self) -> Result<bmap::VXMESH_LITMODE> {
        get_copyable_value(self, bmap::BMMesh_GetLitMode)
    }
    fn set_lit_mode(&mut self, data: bmap::VXMESH_LITMODE) -> Result<()> {
        set_copyable_value(self, bmap::BMMesh_SetLitMode, data)
    }
}

pub trait BM3dEntity: BMObject {
    fn get_visibility(&self) -> Result<bool> {
        get_copyable_value(self, bmap::BM3dEntity_GetVisibility)
    }
    fn set_visibility(&mut self, data: bool) -> Result<()> {
        set_copyable_value(self, bmap::BM3dEntity_SetVisibility, data)
    }
}

pub trait BM3dObject: BM3dEntity {}

pub trait BMLight: BM3dEntity {
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

pub trait BMTargetLight: BMLight {}

pub trait BMCamera: BM3dEntity {
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

pub trait BMTargetCamera: BMCamera {}

pub trait BMGroup: BMObject {}

// endregion

// region: Utility Macros

macro_rules! libobj_struct {
    ($name:ident) => {
        #[derive(Debug)]
        struct $name<'a, RW>
        where
            RW: BMFileRW,
        {
            handle: PBMVOID,
            id: CKID,
            parent: PhantomData<&'a RW>,
        }
    };
}

macro_rules! libobj_impl_new {
    ($name:ident) => {
        impl<'a, RW> $name<'a, RW>
        where
            RW: BMFileRW,
        {
            fn new(_: &'a RW, handle: PBMVOID, id: CKID) -> Self {
                Self {
                    handle,
                    id,
                    parent: PhantomData::<&'a RW>,
                }
            }
        }
    };
}

macro_rules! libobj_impl_eq_ord_hash {
    ($name:ident) => {
        impl<'a, RW> PartialEq for $name<'a, RW>
        where
            RW: BMFileRW,
        {
            fn eq(&self, other: &Self) -> bool {
                self.handle == other.handle && self.id == other.id
            }
        }

        impl<'a, RW> Eq for $name<'a, RW> where RW: BMFileRW {}

        impl<'a, RW> PartialOrd for $name<'a, RW>
        where
            RW: BMFileRW,
        {
            fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
                match self.handle.partial_cmp(&other.handle) {
                    Some(Ordering::Equal) => self.id.partial_cmp(&other.id),
                    other => other,
                }
            }
        }

        impl<'a, RW> Ord for $name<'a, RW>
        where
            RW: BMFileRW,
        {
            fn cmp(&self, other: &Self) -> Ordering {
                match self.handle.cmp(&other.handle) {
                    Ordering::Equal => self.id.cmp(&other.id),
                    other => other,
                }
            }
        }

        impl<'a, RW> Hash for $name<'a, RW>
        where
            RW: BMFileRW,
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
        impl<'a, RW> AbstractPointer for $name<'a, RW>
        where
            RW: BMFileRW,
        {
            unsafe fn get_pointer(&self) -> PBMVOID {
                self.handle
            }
        }

        impl<'a, RW> AbstractObject for $name<'a, RW>
        where
            RW: BMFileRW,
        {
            unsafe fn get_ckid(&self) -> CKID {
                self.id
            }
        }
    };
}

macro_rules! libobj_impl_obj_trait {
    ($name:ident, $trait:ident) => {
        impl<'a, RW> $trait for $name<'a, RW> where RW: BMFileRW {}
    };
}

// endregion

// region: Structs

libobj_struct!(BMObjectImpl);
libobj_impl_new!(BMObjectImpl);
libobj_impl_eq_ord_hash!(BMObjectImpl);
libobj_impl_abstract_object!(BMObjectImpl);
libobj_impl_obj_trait!(BMObjectImpl, BMObject);

libobj_struct!(BMTextureImpl);
libobj_impl_new!(BMTextureImpl);
libobj_impl_eq_ord_hash!(BMTextureImpl);
libobj_impl_abstract_object!(BMTextureImpl);
libobj_impl_obj_trait!(BMTextureImpl, BMObject);
libobj_impl_obj_trait!(BMTextureImpl, BMTexture);

libobj_struct!(BMMaterialImpl);
libobj_impl_new!(BMMaterialImpl);
libobj_impl_eq_ord_hash!(BMMaterialImpl);
libobj_impl_abstract_object!(BMMaterialImpl);
libobj_impl_obj_trait!(BMMaterialImpl, BMObject);
libobj_impl_obj_trait!(BMMaterialImpl, BMMaterial);

libobj_struct!(BMMeshImpl);
libobj_impl_new!(BMMeshImpl);
libobj_impl_eq_ord_hash!(BMMeshImpl);
libobj_impl_abstract_object!(BMMeshImpl);
libobj_impl_obj_trait!(BMMeshImpl, BMObject);
libobj_impl_obj_trait!(BMMeshImpl, BMMesh);

libobj_struct!(BM3dEntityImpl);
libobj_impl_new!(BM3dEntityImpl);
libobj_impl_eq_ord_hash!(BM3dEntityImpl);
libobj_impl_abstract_object!(BM3dEntityImpl);
libobj_impl_obj_trait!(BM3dEntityImpl, BMObject);
libobj_impl_obj_trait!(BM3dEntityImpl, BM3dEntity);

libobj_struct!(BM3dObjectImpl);
libobj_impl_new!(BM3dObjectImpl);
libobj_impl_eq_ord_hash!(BM3dObjectImpl);
libobj_impl_abstract_object!(BM3dObjectImpl);
libobj_impl_obj_trait!(BM3dObjectImpl, BMObject);
libobj_impl_obj_trait!(BM3dObjectImpl, BM3dEntity);
libobj_impl_obj_trait!(BM3dObjectImpl, BM3dObject);

libobj_struct!(BMLightImpl);
libobj_impl_new!(BMLightImpl);
libobj_impl_eq_ord_hash!(BMLightImpl);
libobj_impl_abstract_object!(BMLightImpl);
libobj_impl_obj_trait!(BMLightImpl, BMObject);
libobj_impl_obj_trait!(BMLightImpl, BM3dEntity);
libobj_impl_obj_trait!(BMLightImpl, BMLight);

libobj_struct!(BMTargetLightImpl);
libobj_impl_new!(BMTargetLightImpl);
libobj_impl_eq_ord_hash!(BMTargetLightImpl);
libobj_impl_abstract_object!(BMTargetLightImpl);
libobj_impl_obj_trait!(BMTargetLightImpl, BMObject);
libobj_impl_obj_trait!(BMTargetLightImpl, BM3dEntity);
libobj_impl_obj_trait!(BMTargetLightImpl, BMLight);
libobj_impl_obj_trait!(BMTargetLightImpl, BMTargetLight);

libobj_struct!(BMCameraImpl);
libobj_impl_new!(BMCameraImpl);
libobj_impl_eq_ord_hash!(BMCameraImpl);
libobj_impl_abstract_object!(BMCameraImpl);
libobj_impl_obj_trait!(BMCameraImpl, BMObject);
libobj_impl_obj_trait!(BMCameraImpl, BM3dEntity);
libobj_impl_obj_trait!(BMCameraImpl, BMCamera);

libobj_struct!(BMTargetCameraImpl);
libobj_impl_new!(BMTargetCameraImpl);
libobj_impl_eq_ord_hash!(BMTargetCameraImpl);
libobj_impl_abstract_object!(BMTargetCameraImpl);
libobj_impl_obj_trait!(BMTargetCameraImpl, BMObject);
libobj_impl_obj_trait!(BMTargetCameraImpl, BM3dEntity);
libobj_impl_obj_trait!(BMTargetCameraImpl, BMCamera);
libobj_impl_obj_trait!(BMTargetCameraImpl, BMTargetCamera);

libobj_struct!(BMGroupImpl);
libobj_impl_new!(BMGroupImpl);
libobj_impl_eq_ord_hash!(BMGroupImpl);
libobj_impl_abstract_object!(BMGroupImpl);
libobj_impl_obj_trait!(BMGroupImpl, BMObject);
libobj_impl_obj_trait!(BMGroupImpl, BMGroup);

// endregion

// endregion

// region: BMMeshTrans

pub struct BMMeshTrans<'a> {
    handle: PBMVOID,
    phantom: PhantomData<&'a BMapGuard>,
}

impl<'a> AbstractPointer for BMMeshTrans<'a> {
    unsafe fn get_pointer(&self) -> PBMVOID {
        self.handle
    }
}

// endregion
