#pragma once

#include "BMap.hpp"
#include <algorithm>

/**
@note
# Design Note:

## Function Declaration

All exported interface functions will always return a bool to indicate whether current operations is successful.
The layout of interface functions' parameters is Essential Param -> Input Param -> Out Param.
A example is in there: `BMAP_EXPORT bool BMSomeFunc(BMPARAM_OBJECT_DECL, BMPARAM_IN(LibCmo::CK2::CK_ID, someobj), BMPARAM_OUT(LibCmo::CKSTRING, out_name));`
First param is `BMPARAM_OBJECT_DECL`. It is essential param for this function. In this exmaple, it is the combination of BMFile* and CK_ID. If your provide invalid value for them, the function will failed immediately.
Second param is `BMPARAM_IN(LibCmo::CK2::CK_ID, someobj)`. It declare this function accept a Object ID for underlying function calling.
Last param is `BMPARAM_OUT(LibCmo::CKSTRING, out_name)`. It is the return value of this function. Only will be filled when this function success.
Input Param and Out Param can be multiple. No count limit.

## CK_ID Interface.

We use CK_ID as CKObject visitor to ensure the visiting is safe.
We choose CK_ID because checking a valid CK_ID is more easy and cost lower performance than checking a valid CKObject*.
Another reason is that CKObject* relate to class inheritance and need cast. Use CK_ID can leyt them get safe cast in C++, not in other binding languages.
Each CK_ID also should be used with its corresponding BMFile*, because each BMfile* will create a unique CKContext*. CK_ID between different BMFile* is not shared.

+ We use raw pointer for BMFile and BMMeshTransition
	- These 2 class do no involve class inheritance so we can safely pass them to this DLL.
	- These 2 class is not a part of CK core. It is more easy to check them validation. It is enough checking them in a std::set.


*/

#pragma region Macros for Exporting Functions

// Reference: https://stackoverflow.com/questions/2164827/explicitly-exporting-shared-library-functions-in-linux
// Generate macro for import and export respectively.
#if defined(_MSC_VER)
// Microsoft
#define BMAP_RAW_EXPORT __declspec(dllexport)
#define BMAP_RAW_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
// GCC
#define BMAP_RAW_EXPORT __attribute__((visibility("default")))
#define BMAP_RAW_IMPORT
#elif defined(__clang__)
// Clang
#define BMAP_RAW_EXPORT __attribute__((visibility("default")))
#define BMAP_RAW_IMPORT
#else
//  Do nothing and hope for the best?
#define BMAP_RAW_EXPORT
#define BMAP_RAW_IMPORT
#error "Unknown dynamic link import/export semantics."
#endif

// Choosee import or export command according to special macro.
#if defined(BMAP_EXPORTING)
#define BMAP_NAKED_EXPORT BMAP_RAW_EXPORT
#else
#define BMAP_NAKED_EXPORT BMAP_RAW_IMPORT
#endif

// Create real export macro according to whether in C++ environment.
#if defined(__cplusplus)
#define BMAP_EXPORT extern "C" BMAP_NAKED_EXPORT
#else
#define BMAP_EXPORT BMAP_NAKED_EXPORT
#endif

#pragma endregion

#pragma region Interface Used Macros

/** The first param used by BMFile function family */
#define BMPARAM_FILE_DECL(_bmfile) BMap::BMFile* _bmfile
/** The first param used by BMMeshTrans function family */
#define BMPARAM_MESHTRANS_DECL(_trans) BMap::BMMeshTransition* _trans
/** The first param used by BMObject, BMGroup, BM3dObject, BMMesh, BMMaterial and BMTexture function family */
#define BMPARAM_OBJECT_DECL(_bmfile, _objid) BMap::BMFile* _bmfile, LibCmo::CK2::CK_ID _objid
/** Declare an input parameter */
#define BMPARAM_IN(_t, _name) _t _name
/**
@brief
Declare an output parameter.
@details
A pointer will be added automatically for caller receive it.
See BMPARAM_OUT_ASSIGN and BMPARAM_OUT_VAL to know how to use output param in function body.
@remark
There is an example:
```
bool some_interface_func(BMPARAM_OUT(Type_t, param_name)) {
	BMPARAM_OUT_ASSIGN(param_name, some_value);	// assign to out param.
	return BMPARAM_OUT_VAL(param_name) != other_value;	// use out param value.
}
```
@see BMPARAM_OUT_ASSIGN, BMPARAM_OUT_VAL
*/
#define BMPARAM_OUT(_t, _name) _t* _name
/** Assign value for out param in function body. */
#define BMPARAM_OUT_ASSIGN(_name, _val) (*(_name)) = (_val)
/** Visit value for out param in function body. */
#define BMPARAM_OUT_VAL(_name) (*(_name))

#pragma endregion

#pragma region Init / Dispose

BMAP_EXPORT bool BMInit();
BMAP_EXPORT bool BMDispose();

#pragma endregion

#pragma region BMFile

BMAP_EXPORT bool BMFile_Load(
	BMPARAM_IN(LibCmo::CKSTRING, file_name),
	BMPARAM_IN(LibCmo::CKSTRING, temp_folder),
	BMPARAM_IN(LibCmo::CKSTRING, texture_folder),
	BMPARAM_IN(BMap::NakedOutputCallback, raw_callback),
	BMPARAM_IN(LibCmo::CKDWORD, encoding_count),
	BMPARAM_IN(LibCmo::CKSTRING*, encodings),
	BMPARAM_OUT(BMap::BMFile*, out_file)
);
BMAP_EXPORT bool BMFile_Create(
	BMPARAM_IN(LibCmo::CKSTRING, temp_folder),
	BMPARAM_IN(LibCmo::CKSTRING, texture_folder),
	BMPARAM_IN(BMap::NakedOutputCallback, raw_callback),
	BMPARAM_IN(LibCmo::CKDWORD, encoding_count),
	BMPARAM_IN(LibCmo::CKSTRING*, encodings),
	BMPARAM_OUT(BMap::BMFile*, out_file)
);
BMAP_EXPORT bool BMFile_Save(
	BMPARAM_IN(BMap::BMFile*, map_file),
	BMPARAM_IN(LibCmo::CKSTRING, file_name),
	BMPARAM_IN(LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS, texture_save_opt), 
	BMPARAM_IN(bool, use_compress),
	BMPARAM_IN(LibCmo::CKINT, compreess_level)
);
BMAP_EXPORT bool BMFile_Free(
	BMPARAM_IN(BMap::BMFile*, map_file)
);

BMAP_EXPORT bool BMFile_GetGroupCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
BMAP_EXPORT bool BMFile_GetGroup(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
BMAP_EXPORT bool BMFile_CreateGroup(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
BMAP_EXPORT bool BMFile_Get3dObjectCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
BMAP_EXPORT bool BMFile_Get3dObject(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
BMAP_EXPORT bool BMFile_Create3dObject(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
BMAP_EXPORT bool BMFile_GetMeshCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
BMAP_EXPORT bool BMFile_GetMesh(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
BMAP_EXPORT bool BMFile_CreateMesh(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
BMAP_EXPORT bool BMFile_GetMaterialCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
BMAP_EXPORT bool BMFile_GetMaterial(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
BMAP_EXPORT bool BMFile_CreateMaterial(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
BMAP_EXPORT bool BMFile_GetTextureCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
BMAP_EXPORT bool BMFile_GetTexture(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
BMAP_EXPORT bool BMFile_CreateTexture(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
BMAP_EXPORT bool BMFile_GetTargetLightCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
BMAP_EXPORT bool BMFile_GetTargetLight(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
BMAP_EXPORT bool BMFile_CreateTargetLight(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));

#pragma endregion

#pragma region BMMeshTransition

BMAP_EXPORT bool BMMeshTrans_New(BMPARAM_OUT(BMap::BMMeshTransition*, out_trans));
BMAP_EXPORT bool BMMeshTrans_Delete(BMPARAM_IN(BMap::BMMeshTransition*, trans));

BMAP_EXPORT bool BMMeshTrans_PrepareVertexCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count));
BMAP_EXPORT bool BMMeshTrans_PrepareVertex(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem));
BMAP_EXPORT bool BMMeshTrans_PrepareNormalCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count));
BMAP_EXPORT bool BMMeshTrans_PrepareNormal(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem));
BMAP_EXPORT bool BMMeshTrans_PrepareUVCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count));
BMAP_EXPORT bool BMMeshTrans_PrepareUV(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::VxMath::VxVector2*, out_mem));
BMAP_EXPORT bool BMMeshTrans_PrepareMtlSlotCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count));
BMAP_EXPORT bool BMMeshTrans_PrepareMtlSlot(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CK2::CK_ID*, out_mem));
BMAP_EXPORT bool BMMeshTrans_PrepareFaceCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count));
BMAP_EXPORT bool BMMeshTrans_PrepareFaceVertexIndices(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem));
BMAP_EXPORT bool BMMeshTrans_PrepareFaceNormalIndices(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem));
BMAP_EXPORT bool BMMeshTrans_PrepareFaceUVIndices(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem));
BMAP_EXPORT bool BMMeshTrans_PrepareFaceMtlSlot(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem));
BMAP_EXPORT bool BMMeshTrans_Parse(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(BMap::BMFile*, bmfile), BMPARAM_IN(LibCmo::CK2::CK_ID, objid));

#pragma endregion

#pragma region CKObject

BMAP_EXPORT bool BMObject_GetName(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKSTRING, out_name));
BMAP_EXPORT bool BMObject_SetName(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKSTRING, name));

#pragma endregion

#pragma region CKGroup

BMAP_EXPORT bool BMGroup_AddObject(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_ID, memberid));
BMAP_EXPORT bool BMGroup_GetObjectCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
BMAP_EXPORT bool BMGroup_GetObject(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, pos), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_objid));

#pragma endregion

#pragma region CKTexture

BMAP_EXPORT bool BMTexture_GetFileName(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKSTRING, out_filename));
BMAP_EXPORT bool BMTexture_LoadImage(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKSTRING, filename));
BMAP_EXPORT bool BMTexture_SaveImage(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKSTRING, filename));
BMAP_EXPORT bool BMTexture_GetSaveOptions(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS, out_saveopt));
BMAP_EXPORT bool BMTexture_SetSaveOptions(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS, saveopt));
BMAP_EXPORT bool BMTexture_GetVideoFormat(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VX_PIXELFORMAT, out_vfmt));
BMAP_EXPORT bool BMTexture_SetVideoFormat(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VX_PIXELFORMAT, vfmt));

#pragma endregion

#pragma region CKMaterial

BMAP_EXPORT bool BMMaterial_GetDiffuse(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val));
BMAP_EXPORT bool BMMaterial_SetDiffuse(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col));
BMAP_EXPORT bool BMMaterial_GetAmbient(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val));
BMAP_EXPORT bool BMMaterial_SetAmbient(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col));
BMAP_EXPORT bool BMMaterial_GetSpecular(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val));
BMAP_EXPORT bool BMMaterial_SetSpecular(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col));
BMAP_EXPORT bool BMMaterial_GetEmissive(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val));
BMAP_EXPORT bool BMMaterial_SetEmissive(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col));
BMAP_EXPORT bool BMMaterial_GetSpecularPower(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKFLOAT, out_val));
BMAP_EXPORT bool BMMaterial_SetSpecularPower(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKFLOAT, val));

BMAP_EXPORT bool BMMaterial_GetTexture(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_texid));
BMAP_EXPORT bool BMMaterial_SetTexture(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_ID, texid));
BMAP_EXPORT bool BMMaterial_GetTextureBorderColor(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_val));
BMAP_EXPORT bool BMMaterial_SetTextureBorderColor(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, val));

BMAP_EXPORT bool BMMaterial_GetTextureBlendMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_BLENDMODE, out_val));
BMAP_EXPORT bool BMMaterial_SetTextureBlendMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_BLENDMODE, val));
BMAP_EXPORT bool BMMaterial_GetTextureMinMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_FILTERMODE, out_val));
BMAP_EXPORT bool BMMaterial_SetTextureMinMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_FILTERMODE, val));
BMAP_EXPORT bool BMMaterial_GetTextureMagMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_FILTERMODE, out_val));
BMAP_EXPORT bool BMMaterial_SetTextureMagMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_FILTERMODE, val));
BMAP_EXPORT bool BMMaterial_GetTextureAddressMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_ADDRESSMODE, out_val));
BMAP_EXPORT bool BMMaterial_SetTextureAddressMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_ADDRESSMODE, val));

BMAP_EXPORT bool BMMaterial_GetSourceBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXBLEND_MODE, out_val));
BMAP_EXPORT bool BMMaterial_SetSourceBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXBLEND_MODE, val));
BMAP_EXPORT bool BMMaterial_GetDestBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXBLEND_MODE, out_val));
BMAP_EXPORT bool BMMaterial_SetDestBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXBLEND_MODE, val));
BMAP_EXPORT bool BMMaterial_GetFillMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXFILL_MODE, out_val));
BMAP_EXPORT bool BMMaterial_SetFillMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXFILL_MODE, val));
BMAP_EXPORT bool BMMaterial_GetShadeMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXSHADE_MODE, out_val));
BMAP_EXPORT bool BMMaterial_SetShadeMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXSHADE_MODE, val));

BMAP_EXPORT bool BMMaterial_GetAlphaTestEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val));
BMAP_EXPORT bool BMMaterial_SetAlphaTestEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled));
BMAP_EXPORT bool BMMaterial_GetAlphaBlendEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val));
BMAP_EXPORT bool BMMaterial_SetAlphaBlendEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled));
BMAP_EXPORT bool BMMaterial_GetPerspectiveCorrectionEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val));
BMAP_EXPORT bool BMMaterial_SetPerspectiveCorrectionEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled));
BMAP_EXPORT bool BMMaterial_GetZWriteEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val));
BMAP_EXPORT bool BMMaterial_SetZWriteEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled));
BMAP_EXPORT bool BMMaterial_GetTwoSidedEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val));
BMAP_EXPORT bool BMMaterial_SetTwoSidedEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled));

BMAP_EXPORT bool BMMaterial_GetAlphaRef(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKBYTE, out_val));
BMAP_EXPORT bool BMMaterial_SetAlphaRef(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKBYTE, val));
BMAP_EXPORT bool BMMaterial_GetAlphaFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXCMPFUNC, out_val));
BMAP_EXPORT bool BMMaterial_SetAlphaFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXCMPFUNC, val));
BMAP_EXPORT bool BMMaterial_GetZFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXCMPFUNC, out_val));
BMAP_EXPORT bool BMMaterial_SetZFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXCMPFUNC, val));

#pragma endregion

#pragma region CKMesh

BMAP_EXPORT bool BMMesh_GetLitMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXMESH_LITMODE, out_mode));
BMAP_EXPORT bool BMMesh_SetLitMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXMESH_LITMODE, mode));

BMAP_EXPORT bool BMMesh_GetVertexCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
BMAP_EXPORT bool BMMesh_SetVertexCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, count));
BMAP_EXPORT bool BMMesh_GetVertexPositions(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem));
BMAP_EXPORT bool BMMesh_GetVertexNormals(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem));
BMAP_EXPORT bool BMMesh_GetVertexUVs(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxVector2*, out_mem));

BMAP_EXPORT bool BMMesh_GetFaceCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
BMAP_EXPORT bool BMMesh_SetFaceCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, count));
BMAP_EXPORT bool BMMesh_GetFaceIndices(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKWORD*, out_mem));
BMAP_EXPORT bool BMMesh_GetFaceMaterialSlotIndexs(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKWORD*, out_mem));

BMAP_EXPORT bool BMMesh_GetMaterialSlotCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
BMAP_EXPORT bool BMMesh_SetMaterialSlotCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, count));
BMAP_EXPORT bool BMMesh_GetMaterialSlot(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, index), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_mtlid));
BMAP_EXPORT bool BMMesh_SetMaterialSlot(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, index), BMPARAM_IN(LibCmo::CK2::CK_ID, mtlid));

#pragma endregion

#pragma region CK3dEntity

BMAP_EXPORT bool BM3dEntity_GetWorldMatrix(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxMatrix, out_mat));
BMAP_EXPORT bool BM3dEntity_SetWorldMatrix(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxMatrix, mat));
BMAP_EXPORT bool BM3dEntity_GetCurrentMesh(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_meshid));
BMAP_EXPORT bool BM3dEntity_SetCurrentMesh(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_ID, meshid));
BMAP_EXPORT bool BM3dEntity_GetVisibility(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_isVisible));
BMAP_EXPORT bool BM3dEntity_SetVisibility(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, is_visible));

#pragma endregion

#pragma region CK3dObject

// nothing

#pragma endregion

#pragma region CKLight

BMAP_EXPORT bool BMLight_GetType(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXLIGHT_TYPE, out_val));
BMAP_EXPORT bool BMLight_SetType(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXLIGHT_TYPE, val));

BMAP_EXPORT bool BMLight_GetColor(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val));
BMAP_EXPORT bool BMLight_SetColor(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col));

BMAP_EXPORT bool BMLight_GetConstantAttenuation(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKFLOAT, out_val));
BMAP_EXPORT bool BMLight_SetConstantAttenuation(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKFLOAT, val));
BMAP_EXPORT bool BMLight_GetLinearAttenuation(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKFLOAT, out_val));
BMAP_EXPORT bool BMLight_SetLinearAttenuation(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKFLOAT, val));
BMAP_EXPORT bool BMLight_GetQuadraticAttenuation(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKFLOAT, out_val));
BMAP_EXPORT bool BMLight_SetQuadraticAttenuation(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKFLOAT, val));

BMAP_EXPORT bool BMLight_GetRange(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKFLOAT, out_val));
BMAP_EXPORT bool BMLight_SetRange(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKFLOAT, val));

BMAP_EXPORT bool BMLight_GetHotSpot(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKFLOAT, out_val));
BMAP_EXPORT bool BMLight_SetHotSpot(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKFLOAT, val));
BMAP_EXPORT bool BMLight_GetFalloff(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKFLOAT, out_val));
BMAP_EXPORT bool BMLight_SetFalloff(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKFLOAT, val));
BMAP_EXPORT bool BMLight_GetFalloffShape(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKFLOAT, out_val));
BMAP_EXPORT bool BMLight_SetFalloffShape(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKFLOAT, val));

#pragma endregion

#pragma region CKTargetLight

// nothing

#pragma endregion
