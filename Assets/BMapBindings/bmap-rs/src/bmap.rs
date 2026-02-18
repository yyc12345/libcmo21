//! The module contains raw FFI bindings to native BMap library.
//! 
//! For the user of this library, use `bmap_wrapper` instead of this module,
//! except you really need these raw calling and know what you are doing.

use std::ffi::{c_float, c_void};
//use std::ptr;

// region: Type Defines

// YYC MARK:
// Due to the limitation of our script,
// we can not distinguish mutable and constant of arguments.
// So we declare them all in mutable for convenience.

pub type CKSTRING = *mut i8;
pub type PCKSTRING = *mut CKSTRING;
pub type CKDWORD = u32;
pub type PCKDWORD = *mut CKDWORD;
pub type CKWORD = u16;
pub type PCKWORD = *mut CKWORD;
pub type CKID = u32;
pub type PCKID = *mut CKID;
pub type CKFLOAT = c_float;
pub type CKINT = i32;
pub type CKBYTE = u8;

pub type BMBOOL = bool;
pub type BMVOID = c_void;
pub type PBMVOID = *mut BMVOID;
pub type BMCALLBACK = unsafe extern "C" fn(msg: CKSTRING) -> ();

pub use crate::virtools_types::*;
pub type PVxVector2 = *mut VxVector2;
pub type PVxVector3 = *mut VxVector3;

// endregion

// region: Utilities

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

// endregion

// region: Function Defines

#[rustfmt::skip]
#[link(name = "BMap", kind = "dylib")]
unsafe extern "C" {
    // ##### GENERATED FUNCTIONS BEGIN #####

    /// BMInit
    /// 
    /// # Parameters
    ///
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMInit() -> BMBOOL;
    /// BMDispose
    /// 
    /// # Parameters
    ///
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMDispose() -> BMBOOL;
    /// BMFile_Load
    /// 
    /// # Parameters
    ///
    /// - `file_name`: Direction: input. C++ type: `LibCmo::CKSTRING`. 
    /// - `temp_folder`: Direction: input. C++ type: `LibCmo::CKSTRING`. 
    /// - `texture_folder`: Direction: input. C++ type: `LibCmo::CKSTRING`. 
    /// - `raw_callback`: Direction: input. C++ type: `BMap::NakedOutputCallback`. 
    /// - `encoding_count`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `encodings`: Direction: input. C++ type: `LibCmo::CKSTRING*`. 
    /// - `out_file`: Direction: output. C++ type: `BMap::BMFile*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_Load(file_name: param_in!(CKSTRING), temp_folder: param_in!(CKSTRING), texture_folder: param_in!(CKSTRING), raw_callback: param_in!(BMCALLBACK), encoding_count: param_in!(CKDWORD), encodings: param_in!(PCKSTRING), out_file: param_out!(PBMVOID)) -> BMBOOL;
    /// BMFile_Create
    /// 
    /// # Parameters
    ///
    /// - `temp_folder`: Direction: input. C++ type: `LibCmo::CKSTRING`. 
    /// - `texture_folder`: Direction: input. C++ type: `LibCmo::CKSTRING`. 
    /// - `raw_callback`: Direction: input. C++ type: `BMap::NakedOutputCallback`. 
    /// - `encoding_count`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `encodings`: Direction: input. C++ type: `LibCmo::CKSTRING*`. 
    /// - `out_file`: Direction: output. C++ type: `BMap::BMFile*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_Create(temp_folder: param_in!(CKSTRING), texture_folder: param_in!(CKSTRING), raw_callback: param_in!(BMCALLBACK), encoding_count: param_in!(CKDWORD), encodings: param_in!(PCKSTRING), out_file: param_out!(PBMVOID)) -> BMBOOL;
    /// BMFile_Save
    /// 
    /// # Parameters
    ///
    /// - `map_file`: Direction: input. C++ type: `BMap::BMFile*`. 
    /// - `file_name`: Direction: input. C++ type: `LibCmo::CKSTRING`. 
    /// - `texture_save_opt`: Direction: input. C++ type: `LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS`. 
    /// - `use_compress`: Direction: input. C++ type: `bool`. 
    /// - `compreess_level`: Direction: input. C++ type: `LibCmo::CKINT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_Save(map_file: param_in!(PBMVOID), file_name: param_in!(CKSTRING), texture_save_opt: param_in!(CK_TEXTURE_SAVEOPTIONS), use_compress: param_in!(BMBOOL), compreess_level: param_in!(CKINT)) -> BMBOOL;
    /// BMFile_Free
    /// 
    /// # Parameters
    ///
    /// - `map_file`: Direction: input. C++ type: `BMap::BMFile*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_Free(map_file: param_in!(PBMVOID)) -> BMBOOL;
    /// BMFile_GetGroupCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_count`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetGroupCount(bmfile: param_in!(PBMVOID), out_count: param_out!(CKDWORD)) -> BMBOOL;
    /// BMFile_GetGroup
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `idx`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetGroup(bmfile: param_in!(PBMVOID), idx: param_in!(CKDWORD), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_CreateGroup
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_CreateGroup(bmfile: param_in!(PBMVOID), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_Get3dObjectCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_count`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_Get3dObjectCount(bmfile: param_in!(PBMVOID), out_count: param_out!(CKDWORD)) -> BMBOOL;
    /// BMFile_Get3dObject
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `idx`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_Get3dObject(bmfile: param_in!(PBMVOID), idx: param_in!(CKDWORD), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_Create3dObject
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_Create3dObject(bmfile: param_in!(PBMVOID), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_GetMeshCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_count`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetMeshCount(bmfile: param_in!(PBMVOID), out_count: param_out!(CKDWORD)) -> BMBOOL;
    /// BMFile_GetMesh
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `idx`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetMesh(bmfile: param_in!(PBMVOID), idx: param_in!(CKDWORD), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_CreateMesh
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_CreateMesh(bmfile: param_in!(PBMVOID), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_GetMaterialCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_count`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetMaterialCount(bmfile: param_in!(PBMVOID), out_count: param_out!(CKDWORD)) -> BMBOOL;
    /// BMFile_GetMaterial
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `idx`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetMaterial(bmfile: param_in!(PBMVOID), idx: param_in!(CKDWORD), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_CreateMaterial
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_CreateMaterial(bmfile: param_in!(PBMVOID), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_GetTextureCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_count`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetTextureCount(bmfile: param_in!(PBMVOID), out_count: param_out!(CKDWORD)) -> BMBOOL;
    /// BMFile_GetTexture
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `idx`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetTexture(bmfile: param_in!(PBMVOID), idx: param_in!(CKDWORD), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_CreateTexture
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_CreateTexture(bmfile: param_in!(PBMVOID), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_GetTargetLightCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_count`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetTargetLightCount(bmfile: param_in!(PBMVOID), out_count: param_out!(CKDWORD)) -> BMBOOL;
    /// BMFile_GetTargetLight
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `idx`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetTargetLight(bmfile: param_in!(PBMVOID), idx: param_in!(CKDWORD), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_CreateTargetLight
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_CreateTargetLight(bmfile: param_in!(PBMVOID), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_GetTargetCameraCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_count`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetTargetCameraCount(bmfile: param_in!(PBMVOID), out_count: param_out!(CKDWORD)) -> BMBOOL;
    /// BMFile_GetTargetCamera
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `idx`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_GetTargetCamera(bmfile: param_in!(PBMVOID), idx: param_in!(CKDWORD), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMFile_CreateTargetCamera
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `out_id`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMFile_CreateTargetCamera(bmfile: param_in!(PBMVOID), out_id: param_out!(CKID)) -> BMBOOL;
    /// BMMeshTrans_New
    /// 
    /// # Parameters
    ///
    /// - `out_trans`: Direction: output. C++ type: `BMap::BMMeshTransition*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_New(out_trans: param_out!(PBMVOID)) -> BMBOOL;
    /// BMMeshTrans_Delete
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_Delete(trans: param_in!(PBMVOID)) -> BMBOOL;
    /// BMMeshTrans_PrepareVertexCount
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `count`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareVertexCount(trans: param_in!(PBMVOID), count: param_in!(CKDWORD)) -> BMBOOL;
    /// BMMeshTrans_PrepareVertex
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::VxMath::VxVector3*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareVertex(trans: param_in!(PBMVOID), out_mem: param_out!(PVxVector3)) -> BMBOOL;
    /// BMMeshTrans_PrepareNormalCount
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `count`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareNormalCount(trans: param_in!(PBMVOID), count: param_in!(CKDWORD)) -> BMBOOL;
    /// BMMeshTrans_PrepareNormal
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::VxMath::VxVector3*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareNormal(trans: param_in!(PBMVOID), out_mem: param_out!(PVxVector3)) -> BMBOOL;
    /// BMMeshTrans_PrepareUVCount
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `count`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareUVCount(trans: param_in!(PBMVOID), count: param_in!(CKDWORD)) -> BMBOOL;
    /// BMMeshTrans_PrepareUV
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::VxMath::VxVector2*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareUV(trans: param_in!(PBMVOID), out_mem: param_out!(PVxVector2)) -> BMBOOL;
    /// BMMeshTrans_PrepareMtlSlotCount
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `count`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareMtlSlotCount(trans: param_in!(PBMVOID), count: param_in!(CKDWORD)) -> BMBOOL;
    /// BMMeshTrans_PrepareMtlSlot
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::CK2::CK_ID*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareMtlSlot(trans: param_in!(PBMVOID), out_mem: param_out!(PCKID)) -> BMBOOL;
    /// BMMeshTrans_PrepareFaceCount
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `count`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareFaceCount(trans: param_in!(PBMVOID), count: param_in!(CKDWORD)) -> BMBOOL;
    /// BMMeshTrans_PrepareFaceVertexIndices
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::CKDWORD*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareFaceVertexIndices(trans: param_in!(PBMVOID), out_mem: param_out!(PCKDWORD)) -> BMBOOL;
    /// BMMeshTrans_PrepareFaceNormalIndices
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::CKDWORD*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareFaceNormalIndices(trans: param_in!(PBMVOID), out_mem: param_out!(PCKDWORD)) -> BMBOOL;
    /// BMMeshTrans_PrepareFaceUVIndices
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::CKDWORD*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareFaceUVIndices(trans: param_in!(PBMVOID), out_mem: param_out!(PCKDWORD)) -> BMBOOL;
    /// BMMeshTrans_PrepareFaceMtlSlot
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::CKDWORD*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_PrepareFaceMtlSlot(trans: param_in!(PBMVOID), out_mem: param_out!(PCKDWORD)) -> BMBOOL;
    /// BMMeshTrans_Parse
    /// 
    /// # Parameters
    ///
    /// - `trans`: Direction: input. C++ type: `BMap::BMMeshTransition*`. The pointer to corresponding BMMeshTransition.
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. 
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMeshTrans_Parse(trans: param_in!(PBMVOID), bmfile: param_in!(PBMVOID), objid: param_in!(CKID)) -> BMBOOL;
    /// BMObject_GetName
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_name`: Direction: output. C++ type: `LibCmo::CKSTRING`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMObject_GetName(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_name: param_out!(CKSTRING)) -> BMBOOL;
    /// BMObject_SetName
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `name`: Direction: input. C++ type: `LibCmo::CKSTRING`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMObject_SetName(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), name: param_in!(CKSTRING)) -> BMBOOL;
    /// BMGroup_AddObject
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `memberid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMGroup_AddObject(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), memberid: param_in!(CKID)) -> BMBOOL;
    /// BMGroup_GetObjectCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_count`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMGroup_GetObjectCount(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_count: param_out!(CKDWORD)) -> BMBOOL;
    /// BMGroup_GetObject
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `pos`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `out_objid`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMGroup_GetObject(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), pos: param_in!(CKDWORD), out_objid: param_out!(CKID)) -> BMBOOL;
    /// BMTexture_GetFileName
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_filename`: Direction: output. C++ type: `LibCmo::CKSTRING`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMTexture_GetFileName(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_filename: param_out!(CKSTRING)) -> BMBOOL;
    /// BMTexture_LoadImage
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `filename`: Direction: input. C++ type: `LibCmo::CKSTRING`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMTexture_LoadImage(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), filename: param_in!(CKSTRING)) -> BMBOOL;
    /// BMTexture_SaveImage
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `filename`: Direction: input. C++ type: `LibCmo::CKSTRING`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMTexture_SaveImage(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), filename: param_in!(CKSTRING)) -> BMBOOL;
    /// BMTexture_GetSaveOptions
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_saveopt`: Direction: output. C++ type: `LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMTexture_GetSaveOptions(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_saveopt: param_out!(CK_TEXTURE_SAVEOPTIONS)) -> BMBOOL;
    /// BMTexture_SetSaveOptions
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `saveopt`: Direction: input. C++ type: `LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMTexture_SetSaveOptions(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), saveopt: param_in!(CK_TEXTURE_SAVEOPTIONS)) -> BMBOOL;
    /// BMTexture_GetVideoFormat
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_vfmt`: Direction: output. C++ type: `LibCmo::VxMath::VX_PIXELFORMAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMTexture_GetVideoFormat(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_vfmt: param_out!(VX_PIXELFORMAT)) -> BMBOOL;
    /// BMTexture_SetVideoFormat
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `vfmt`: Direction: input. C++ type: `LibCmo::VxMath::VX_PIXELFORMAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMTexture_SetVideoFormat(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), vfmt: param_in!(VX_PIXELFORMAT)) -> BMBOOL;
    /// BMMaterial_GetDiffuse
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VxColor`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetDiffuse(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VxColor)) -> BMBOOL;
    /// BMMaterial_SetDiffuse
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `col`: Direction: input. C++ type: `LibCmo::VxMath::VxColor`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetDiffuse(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), col: param_in!(VxColor)) -> BMBOOL;
    /// BMMaterial_GetAmbient
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VxColor`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetAmbient(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VxColor)) -> BMBOOL;
    /// BMMaterial_SetAmbient
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `col`: Direction: input. C++ type: `LibCmo::VxMath::VxColor`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetAmbient(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), col: param_in!(VxColor)) -> BMBOOL;
    /// BMMaterial_GetSpecular
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VxColor`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetSpecular(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VxColor)) -> BMBOOL;
    /// BMMaterial_SetSpecular
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `col`: Direction: input. C++ type: `LibCmo::VxMath::VxColor`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetSpecular(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), col: param_in!(VxColor)) -> BMBOOL;
    /// BMMaterial_GetEmissive
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VxColor`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetEmissive(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VxColor)) -> BMBOOL;
    /// BMMaterial_SetEmissive
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `col`: Direction: input. C++ type: `LibCmo::VxMath::VxColor`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetEmissive(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), col: param_in!(VxColor)) -> BMBOOL;
    /// BMMaterial_GetSpecularPower
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetSpecularPower(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMMaterial_SetSpecularPower
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetSpecularPower(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMMaterial_GetTexture
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_texid`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetTexture(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_texid: param_out!(CKID)) -> BMBOOL;
    /// BMMaterial_SetTexture
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `texid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetTexture(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), texid: param_in!(CKID)) -> BMBOOL;
    /// BMMaterial_GetTextureBorderColor
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetTextureBorderColor(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKDWORD)) -> BMBOOL;
    /// BMMaterial_SetTextureBorderColor
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetTextureBorderColor(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKDWORD)) -> BMBOOL;
    /// BMMaterial_GetTextureBlendMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VXTEXTURE_BLENDMODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetTextureBlendMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VXTEXTURE_BLENDMODE)) -> BMBOOL;
    /// BMMaterial_SetTextureBlendMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::VxMath::VXTEXTURE_BLENDMODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetTextureBlendMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(VXTEXTURE_BLENDMODE)) -> BMBOOL;
    /// BMMaterial_GetTextureMinMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VXTEXTURE_FILTERMODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetTextureMinMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VXTEXTURE_FILTERMODE)) -> BMBOOL;
    /// BMMaterial_SetTextureMinMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::VxMath::VXTEXTURE_FILTERMODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetTextureMinMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(VXTEXTURE_FILTERMODE)) -> BMBOOL;
    /// BMMaterial_GetTextureMagMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VXTEXTURE_FILTERMODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetTextureMagMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VXTEXTURE_FILTERMODE)) -> BMBOOL;
    /// BMMaterial_SetTextureMagMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::VxMath::VXTEXTURE_FILTERMODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetTextureMagMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(VXTEXTURE_FILTERMODE)) -> BMBOOL;
    /// BMMaterial_GetTextureAddressMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VXTEXTURE_ADDRESSMODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetTextureAddressMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VXTEXTURE_ADDRESSMODE)) -> BMBOOL;
    /// BMMaterial_SetTextureAddressMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::VxMath::VXTEXTURE_ADDRESSMODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetTextureAddressMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(VXTEXTURE_ADDRESSMODE)) -> BMBOOL;
    /// BMMaterial_GetSourceBlend
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VXBLEND_MODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetSourceBlend(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VXBLEND_MODE)) -> BMBOOL;
    /// BMMaterial_SetSourceBlend
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::VxMath::VXBLEND_MODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetSourceBlend(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(VXBLEND_MODE)) -> BMBOOL;
    /// BMMaterial_GetDestBlend
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VXBLEND_MODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetDestBlend(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VXBLEND_MODE)) -> BMBOOL;
    /// BMMaterial_SetDestBlend
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::VxMath::VXBLEND_MODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetDestBlend(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(VXBLEND_MODE)) -> BMBOOL;
    /// BMMaterial_GetFillMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VXFILL_MODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetFillMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VXFILL_MODE)) -> BMBOOL;
    /// BMMaterial_SetFillMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::VxMath::VXFILL_MODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetFillMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(VXFILL_MODE)) -> BMBOOL;
    /// BMMaterial_GetShadeMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VXSHADE_MODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetShadeMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VXSHADE_MODE)) -> BMBOOL;
    /// BMMaterial_SetShadeMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::VxMath::VXSHADE_MODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetShadeMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(VXSHADE_MODE)) -> BMBOOL;
    /// BMMaterial_GetAlphaTestEnabled
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetAlphaTestEnabled(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(BMBOOL)) -> BMBOOL;
    /// BMMaterial_SetAlphaTestEnabled
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `enabled`: Direction: input. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetAlphaTestEnabled(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), enabled: param_in!(BMBOOL)) -> BMBOOL;
    /// BMMaterial_GetAlphaBlendEnabled
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetAlphaBlendEnabled(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(BMBOOL)) -> BMBOOL;
    /// BMMaterial_SetAlphaBlendEnabled
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `enabled`: Direction: input. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetAlphaBlendEnabled(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), enabled: param_in!(BMBOOL)) -> BMBOOL;
    /// BMMaterial_GetPerspectiveCorrectionEnabled
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetPerspectiveCorrectionEnabled(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(BMBOOL)) -> BMBOOL;
    /// BMMaterial_SetPerspectiveCorrectionEnabled
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `enabled`: Direction: input. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetPerspectiveCorrectionEnabled(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), enabled: param_in!(BMBOOL)) -> BMBOOL;
    /// BMMaterial_GetZWriteEnabled
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetZWriteEnabled(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(BMBOOL)) -> BMBOOL;
    /// BMMaterial_SetZWriteEnabled
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `enabled`: Direction: input. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetZWriteEnabled(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), enabled: param_in!(BMBOOL)) -> BMBOOL;
    /// BMMaterial_GetTwoSidedEnabled
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetTwoSidedEnabled(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(BMBOOL)) -> BMBOOL;
    /// BMMaterial_SetTwoSidedEnabled
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `enabled`: Direction: input. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetTwoSidedEnabled(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), enabled: param_in!(BMBOOL)) -> BMBOOL;
    /// BMMaterial_GetAlphaRef
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKBYTE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetAlphaRef(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKBYTE)) -> BMBOOL;
    /// BMMaterial_SetAlphaRef
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKBYTE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetAlphaRef(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKBYTE)) -> BMBOOL;
    /// BMMaterial_GetAlphaFunc
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VXCMPFUNC`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetAlphaFunc(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VXCMPFUNC)) -> BMBOOL;
    /// BMMaterial_SetAlphaFunc
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::VxMath::VXCMPFUNC`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetAlphaFunc(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(VXCMPFUNC)) -> BMBOOL;
    /// BMMaterial_GetZFunc
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VXCMPFUNC`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_GetZFunc(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VXCMPFUNC)) -> BMBOOL;
    /// BMMaterial_SetZFunc
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::VxMath::VXCMPFUNC`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMaterial_SetZFunc(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(VXCMPFUNC)) -> BMBOOL;
    /// BMMesh_GetLitMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_mode`: Direction: output. C++ type: `LibCmo::VxMath::VXMESH_LITMODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_GetLitMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_mode: param_out!(VXMESH_LITMODE)) -> BMBOOL;
    /// BMMesh_SetLitMode
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `mode`: Direction: input. C++ type: `LibCmo::VxMath::VXMESH_LITMODE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_SetLitMode(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), mode: param_in!(VXMESH_LITMODE)) -> BMBOOL;
    /// BMMesh_GetVertexCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_count`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_GetVertexCount(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_count: param_out!(CKDWORD)) -> BMBOOL;
    /// BMMesh_SetVertexCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `count`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_SetVertexCount(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), count: param_in!(CKDWORD)) -> BMBOOL;
    /// BMMesh_GetVertexPositions
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::VxMath::VxVector3*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_GetVertexPositions(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_mem: param_out!(PVxVector3)) -> BMBOOL;
    /// BMMesh_GetVertexNormals
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::VxMath::VxVector3*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_GetVertexNormals(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_mem: param_out!(PVxVector3)) -> BMBOOL;
    /// BMMesh_GetVertexUVs
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::VxMath::VxVector2*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_GetVertexUVs(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_mem: param_out!(PVxVector2)) -> BMBOOL;
    /// BMMesh_GetFaceCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_count`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_GetFaceCount(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_count: param_out!(CKDWORD)) -> BMBOOL;
    /// BMMesh_SetFaceCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `count`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_SetFaceCount(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), count: param_in!(CKDWORD)) -> BMBOOL;
    /// BMMesh_GetFaceIndices
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::CKWORD*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_GetFaceIndices(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_mem: param_out!(PCKWORD)) -> BMBOOL;
    /// BMMesh_GetFaceMaterialSlotIndexs
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_mem`: Direction: output. C++ type: `LibCmo::CKWORD*`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_GetFaceMaterialSlotIndexs(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_mem: param_out!(PCKWORD)) -> BMBOOL;
    /// BMMesh_GetMaterialSlotCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_count`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_GetMaterialSlotCount(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_count: param_out!(CKDWORD)) -> BMBOOL;
    /// BMMesh_SetMaterialSlotCount
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `count`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_SetMaterialSlotCount(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), count: param_in!(CKDWORD)) -> BMBOOL;
    /// BMMesh_GetMaterialSlot
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `index`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `out_mtlid`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_GetMaterialSlot(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), index: param_in!(CKDWORD), out_mtlid: param_out!(CKID)) -> BMBOOL;
    /// BMMesh_SetMaterialSlot
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `index`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `mtlid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMMesh_SetMaterialSlot(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), index: param_in!(CKDWORD), mtlid: param_in!(CKID)) -> BMBOOL;
    /// BM3dEntity_GetWorldMatrix
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_mat`: Direction: output. C++ type: `LibCmo::VxMath::VxMatrix`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BM3dEntity_GetWorldMatrix(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_mat: param_out!(VxMatrix)) -> BMBOOL;
    /// BM3dEntity_SetWorldMatrix
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `mat`: Direction: input. C++ type: `LibCmo::VxMath::VxMatrix`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BM3dEntity_SetWorldMatrix(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), mat: param_in!(VxMatrix)) -> BMBOOL;
    /// BM3dEntity_GetCurrentMesh
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_meshid`: Direction: output. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BM3dEntity_GetCurrentMesh(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_meshid: param_out!(CKID)) -> BMBOOL;
    /// BM3dEntity_SetCurrentMesh
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `meshid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BM3dEntity_SetCurrentMesh(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), meshid: param_in!(CKID)) -> BMBOOL;
    /// BM3dEntity_GetVisibility
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_isVisible`: Direction: output. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BM3dEntity_GetVisibility(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_isVisible: param_out!(BMBOOL)) -> BMBOOL;
    /// BM3dEntity_SetVisibility
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `is_visible`: Direction: input. C++ type: `bool`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BM3dEntity_SetVisibility(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), is_visible: param_in!(BMBOOL)) -> BMBOOL;
    /// BMLight_GetType
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VXLIGHT_TYPE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_GetType(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VXLIGHT_TYPE)) -> BMBOOL;
    /// BMLight_SetType
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::VxMath::VXLIGHT_TYPE`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_SetType(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(VXLIGHT_TYPE)) -> BMBOOL;
    /// BMLight_GetColor
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::VxMath::VxColor`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_GetColor(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(VxColor)) -> BMBOOL;
    /// BMLight_SetColor
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `col`: Direction: input. C++ type: `LibCmo::VxMath::VxColor`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_SetColor(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), col: param_in!(VxColor)) -> BMBOOL;
    /// BMLight_GetConstantAttenuation
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_GetConstantAttenuation(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMLight_SetConstantAttenuation
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_SetConstantAttenuation(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMLight_GetLinearAttenuation
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_GetLinearAttenuation(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMLight_SetLinearAttenuation
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_SetLinearAttenuation(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMLight_GetQuadraticAttenuation
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_GetQuadraticAttenuation(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMLight_SetQuadraticAttenuation
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_SetQuadraticAttenuation(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMLight_GetRange
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_GetRange(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMLight_SetRange
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_SetRange(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMLight_GetHotSpot
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_GetHotSpot(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMLight_SetHotSpot
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_SetHotSpot(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMLight_GetFalloff
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_GetFalloff(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMLight_SetFalloff
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_SetFalloff(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMLight_GetFalloffShape
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_GetFalloffShape(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMLight_SetFalloffShape
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMLight_SetFalloffShape(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMCamera_GetProjectionType
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CK2::CK_CAMERA_PROJECTION`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_GetProjectionType(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CK_CAMERA_PROJECTION)) -> BMBOOL;
    /// BMCamera_SetProjectionType
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CK2::CK_CAMERA_PROJECTION`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_SetProjectionType(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CK_CAMERA_PROJECTION)) -> BMBOOL;
    /// BMCamera_GetOrthographicZoom
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_GetOrthographicZoom(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMCamera_SetOrthographicZoom
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_SetOrthographicZoom(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMCamera_GetFrontPlane
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_GetFrontPlane(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMCamera_SetFrontPlane
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_SetFrontPlane(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMCamera_GetBackPlane
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_GetBackPlane(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMCamera_SetBackPlane
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_SetBackPlane(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMCamera_GetFov
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_val`: Direction: output. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_GetFov(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_val: param_out!(CKFLOAT)) -> BMBOOL;
    /// BMCamera_SetFov
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `val`: Direction: input. C++ type: `LibCmo::CKFLOAT`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_SetFov(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), val: param_in!(CKFLOAT)) -> BMBOOL;
    /// BMCamera_GetAspectRatio
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `out_width`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// - `out_height`: Direction: output. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_GetAspectRatio(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), out_width: param_out!(CKDWORD), out_height: param_out!(CKDWORD)) -> BMBOOL;
    /// BMCamera_SetAspectRatio
    /// 
    /// # Parameters
    ///
    /// - `bmfile`: Direction: input. C++ type: `BMap::BMFile*`. The pointer to corresponding BMFile.
    /// - `objid`: Direction: input. C++ type: `LibCmo::CK2::CK_ID`. The CKID of object you accessing.
    /// - `width`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// - `height`: Direction: input. C++ type: `LibCmo::CKDWORD`. 
    /// 
    /// # Return
    /// 
    /// True if no error, otherwise False.
    pub unsafe fn BMCamera_SetAspectRatio(bmfile: param_in!(PBMVOID), objid: param_in!(CKID), width: param_in!(CKDWORD), height: param_in!(CKDWORD)) -> BMBOOL;

    // ##### GENERATED FUNCTIONS END #####
}

// endregion
