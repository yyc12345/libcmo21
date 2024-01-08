import ctypes, os, sys, typing

#region Type Defines

class BMapException(Exception):
    """
    The exception thrown by BMap bindings.
    """
    pass

bm_CKSTRING = ctypes.c_char_p
bm_CKSTRING_p = ctypes.POINTER(bm_CKSTRING)
bm_CKDWORD = ctypes.c_uint32
bm_CKDWORD_p = ctypes.POINTER(bm_CKDWORD)
bm_CKDWORD_pp = ctypes.POINTER(bm_CKDWORD_p)
bm_CKWORD = ctypes.c_uint16
bm_CKWORD_p = ctypes.POINTER(bm_CKWORD)
bm_CKWORD_pp = ctypes.POINTER(bm_CKWORD_p)
bm_CKID = ctypes.c_uint32
bm_CKID_p = ctypes.POINTER(bm_CKID)
bm_CKID_pp = ctypes.POINTER(bm_CKID_p)
bm_CKFLOAT = ctypes.c_float
bm_CKFLOAT_p = ctypes.POINTER(bm_CKFLOAT)
bm_CKINT = ctypes.c_int32
bm_CKBYTE = ctypes.c_uint8
bm_CKBYTE_p = ctypes.POINTER(bm_CKBYTE)

bm_enum = bm_CKDWORD
bm_enum_p = ctypes.POINTER(bm_enum)
bm_bool = ctypes.c_bool
bm_bool_p = ctypes.POINTER(bm_bool)
bm_void_p = ctypes.c_void_p
bm_void_pp = ctypes.POINTER(ctypes.c_void_p)

bm_callback = ctypes.CFUNCTYPE(None, bm_CKSTRING)

class bm_VxVector2(ctypes.Structure):
    _fields_ = [
        ('x', bm_CKFLOAT),
        ('y', bm_CKFLOAT),
    ]
bm_VxVector2_p = ctypes.POINTER(bm_VxVector2)
bm_VxVector2_pp = ctypes.POINTER(bm_VxVector2_p)
class bm_VxVector3(ctypes.Structure):
    _fields_ = [
        ('x', bm_CKFLOAT),
        ('y', bm_CKFLOAT),
        ('z', bm_CKFLOAT),
    ]
bm_VxVector3_p = ctypes.POINTER(bm_VxVector3)
bm_VxVector3_pp = ctypes.POINTER(bm_VxVector3_p)
class bm_VxColor(ctypes.Structure):
    _fields_ = [
        ('r', bm_CKFLOAT),
        ('g', bm_CKFLOAT),
        ('b', bm_CKFLOAT),
        ('a', bm_CKFLOAT),
    ]
bm_VxColor_p = ctypes.POINTER(bm_VxColor)
class bm_VxMatrix(ctypes.Structure):
    _fields_ = list(
        (f'i{idx}', bm_CKFLOAT) for idx in range(16)
    )
bm_VxMatrix_p = ctypes.POINTER(bm_VxMatrix)

#endregion

#region BMap Loader

_g_BMapLibName: str

if sys.platform.startswith('win32') or sys.platform.startswith('cygwin'):
    _g_BMapLibName = "BMap.dll"
elif sys.platform.startswith('linux') or sys.platform.startswith('freebsd'):
    _g_BMapLibName = "BMap.so"
elif sys.platform.startswith('darwin'):
    _g_BMapLibName = "BMap.dylib"
else:
    _g_BMapLibName = "BMap.bin"

_g_BMapModule: ctypes.CDLL | None = None
try:
    _g_BMapModule = ctypes.cdll.LoadLibrary(
        os.path.join(os.path.dirname(__file__), _g_BMapLibName)
    )
except:
    _g_BMapModule = None

def is_bmap_available() -> bool:
    return _g_BMapModule is not None

def _bmap_error_check(result: bool, func, args):
    if not result:
        raise BMapException("BMap operation failed.")
    return result

def _create_bmap_func(fct_name: str, fct_params: list[typing.Any]) -> typing.Callable[..., bm_bool]:
    if _g_BMapModule is None: return None
    
    cache: typing.Callable[..., bm_bool] = getattr(_g_BMapModule, fct_name)
    cache.argtypes = fct_params
    cache.restype = bm_bool
    cache.errcheck = _bmap_error_check
    return cache

#endregion

#region Function Defines

## BMInit
#  @return True if no error, otherwise False.
BMInit = _create_bmap_func('BMInit', [])
## BMDispose
#  @return True if no error, otherwise False.
BMDispose = _create_bmap_func('BMDispose', [])
## BMFile_Load
#  @param file_name[in] Type: LibCmo::CKSTRING. 
#  @param temp_folder[in] Type: LibCmo::CKSTRING. 
#  @param texture_folder[in] Type: LibCmo::CKSTRING. 
#  @param raw_callback[in] Type: BMap::NakedOutputCallback. 
#  @param encoding_count[in] Type: LibCmo::CKDWORD. 
#  @param encodings[in] Type: LibCmo::CKSTRING*. 
#  @param out_file[out] Type: BMap::BMFile*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_Load = _create_bmap_func('BMFile_Load', [bm_CKSTRING, bm_CKSTRING, bm_CKSTRING, bm_callback, bm_CKDWORD, bm_CKSTRING_p, bm_void_pp])
## BMFile_Create
#  @param temp_folder[in] Type: LibCmo::CKSTRING. 
#  @param texture_folder[in] Type: LibCmo::CKSTRING. 
#  @param raw_callback[in] Type: BMap::NakedOutputCallback. 
#  @param encoding_count[in] Type: LibCmo::CKDWORD. 
#  @param encodings[in] Type: LibCmo::CKSTRING*. 
#  @param out_file[out] Type: BMap::BMFile*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_Create = _create_bmap_func('BMFile_Create', [bm_CKSTRING, bm_CKSTRING, bm_callback, bm_CKDWORD, bm_CKSTRING_p, bm_void_pp])
## BMFile_Save
#  @param map_file[in] Type: BMap::BMFile*. 
#  @param file_name[in] Type: LibCmo::CKSTRING. 
#  @param texture_save_opt[in] Type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. 
#  @param use_compress[in] Type: bool. 
#  @param compreess_level[in] Type: LibCmo::CKINT. 
#  @return True if no error, otherwise False.
BMFile_Save = _create_bmap_func('BMFile_Save', [bm_void_p, bm_CKSTRING, bm_enum, bm_bool, bm_CKINT])
## BMFile_Free
#  @param map_file[in] Type: BMap::BMFile*. 
#  @return True if no error, otherwise False.
BMFile_Free = _create_bmap_func('BMFile_Free', [bm_void_p])
## BMFile_GetGroupCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param out_count[out] Type: LibCmo::CKDWORD. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_GetGroupCount = _create_bmap_func('BMFile_GetGroupCount', [bm_void_p, bm_CKDWORD_p])
## BMFile_GetGroup
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param idx[in] Type: LibCmo::CKDWORD. 
#  @param out_id[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_GetGroup = _create_bmap_func('BMFile_GetGroup', [bm_void_p, bm_CKDWORD, bm_CKID_p])
## BMFile_CreateGroup
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param out_id[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_CreateGroup = _create_bmap_func('BMFile_CreateGroup', [bm_void_p, bm_CKID_p])
## BMFile_Get3dObjectCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param out_count[out] Type: LibCmo::CKDWORD. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_Get3dObjectCount = _create_bmap_func('BMFile_Get3dObjectCount', [bm_void_p, bm_CKDWORD_p])
## BMFile_Get3dObject
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param idx[in] Type: LibCmo::CKDWORD. 
#  @param out_id[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_Get3dObject = _create_bmap_func('BMFile_Get3dObject', [bm_void_p, bm_CKDWORD, bm_CKID_p])
## BMFile_Create3dObject
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param out_id[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_Create3dObject = _create_bmap_func('BMFile_Create3dObject', [bm_void_p, bm_CKID_p])
## BMFile_GetMeshCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param out_count[out] Type: LibCmo::CKDWORD. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_GetMeshCount = _create_bmap_func('BMFile_GetMeshCount', [bm_void_p, bm_CKDWORD_p])
## BMFile_GetMesh
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param idx[in] Type: LibCmo::CKDWORD. 
#  @param out_id[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_GetMesh = _create_bmap_func('BMFile_GetMesh', [bm_void_p, bm_CKDWORD, bm_CKID_p])
## BMFile_CreateMesh
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param out_id[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_CreateMesh = _create_bmap_func('BMFile_CreateMesh', [bm_void_p, bm_CKID_p])
## BMFile_GetMaterialCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param out_count[out] Type: LibCmo::CKDWORD. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_GetMaterialCount = _create_bmap_func('BMFile_GetMaterialCount', [bm_void_p, bm_CKDWORD_p])
## BMFile_GetMaterial
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param idx[in] Type: LibCmo::CKDWORD. 
#  @param out_id[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_GetMaterial = _create_bmap_func('BMFile_GetMaterial', [bm_void_p, bm_CKDWORD, bm_CKID_p])
## BMFile_CreateMaterial
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param out_id[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_CreateMaterial = _create_bmap_func('BMFile_CreateMaterial', [bm_void_p, bm_CKID_p])
## BMFile_GetTextureCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param out_count[out] Type: LibCmo::CKDWORD. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_GetTextureCount = _create_bmap_func('BMFile_GetTextureCount', [bm_void_p, bm_CKDWORD_p])
## BMFile_GetTexture
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param idx[in] Type: LibCmo::CKDWORD. 
#  @param out_id[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_GetTexture = _create_bmap_func('BMFile_GetTexture', [bm_void_p, bm_CKDWORD, bm_CKID_p])
## BMFile_CreateTexture
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param out_id[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMFile_CreateTexture = _create_bmap_func('BMFile_CreateTexture', [bm_void_p, bm_CKID_p])
## BMMeshTrans_New
#  @param out_trans[out] Type: BMap::BMMeshTransition*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMeshTrans_New = _create_bmap_func('BMMeshTrans_New', [bm_void_pp])
## BMMeshTrans_Delete
#  @param trans[in] Type: BMap::BMMeshTransition*. 
#  @return True if no error, otherwise False.
BMMeshTrans_Delete = _create_bmap_func('BMMeshTrans_Delete', [bm_void_p])
## BMMeshTrans_PrepareVertexCount
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param count[in] Type: LibCmo::CKDWORD. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareVertexCount = _create_bmap_func('BMMeshTrans_PrepareVertexCount', [bm_void_p, bm_CKDWORD])
## BMMeshTrans_PrepareVertex
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param out_mem[out] Type: LibCmo::VxMath::VxVector3*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareVertex = _create_bmap_func('BMMeshTrans_PrepareVertex', [bm_void_p, bm_VxVector3_pp])
## BMMeshTrans_PrepareNormalCount
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param count[in] Type: LibCmo::CKDWORD. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareNormalCount = _create_bmap_func('BMMeshTrans_PrepareNormalCount', [bm_void_p, bm_CKDWORD])
## BMMeshTrans_PrepareNormal
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param out_mem[out] Type: LibCmo::VxMath::VxVector3*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareNormal = _create_bmap_func('BMMeshTrans_PrepareNormal', [bm_void_p, bm_VxVector3_pp])
## BMMeshTrans_PrepareUVCount
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param count[in] Type: LibCmo::CKDWORD. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareUVCount = _create_bmap_func('BMMeshTrans_PrepareUVCount', [bm_void_p, bm_CKDWORD])
## BMMeshTrans_PrepareUV
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param out_mem[out] Type: LibCmo::VxMath::VxVector2*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareUV = _create_bmap_func('BMMeshTrans_PrepareUV', [bm_void_p, bm_VxVector2_pp])
## BMMeshTrans_PrepareMtlSlotCount
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param count[in] Type: LibCmo::CKDWORD. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareMtlSlotCount = _create_bmap_func('BMMeshTrans_PrepareMtlSlotCount', [bm_void_p, bm_CKDWORD])
## BMMeshTrans_PrepareMtlSlot
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param out_mem[out] Type: LibCmo::CK2::CK_ID*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareMtlSlot = _create_bmap_func('BMMeshTrans_PrepareMtlSlot', [bm_void_p, bm_CKID_pp])
## BMMeshTrans_PrepareFaceCount
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param count[in] Type: LibCmo::CKDWORD. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareFaceCount = _create_bmap_func('BMMeshTrans_PrepareFaceCount', [bm_void_p, bm_CKDWORD])
## BMMeshTrans_PrepareFaceVertexIndices
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param out_mem[out] Type: LibCmo::CKDWORD*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareFaceVertexIndices = _create_bmap_func('BMMeshTrans_PrepareFaceVertexIndices', [bm_void_p, bm_CKDWORD_pp])
## BMMeshTrans_PrepareFaceNormalIndices
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param out_mem[out] Type: LibCmo::CKDWORD*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareFaceNormalIndices = _create_bmap_func('BMMeshTrans_PrepareFaceNormalIndices', [bm_void_p, bm_CKDWORD_pp])
## BMMeshTrans_PrepareFaceUVIndices
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param out_mem[out] Type: LibCmo::CKDWORD*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareFaceUVIndices = _create_bmap_func('BMMeshTrans_PrepareFaceUVIndices', [bm_void_p, bm_CKDWORD_pp])
## BMMeshTrans_PrepareFaceMtlSlot
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param out_mem[out] Type: LibCmo::CKDWORD*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMeshTrans_PrepareFaceMtlSlot = _create_bmap_func('BMMeshTrans_PrepareFaceMtlSlot', [bm_void_p, bm_CKDWORD_pp])
## BMMeshTrans_Parse
#  @param trans[in] Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.
#  @param bmfile[in] Type: BMap::BMFile*. 
#  @param objid[in] Type: LibCmo::CK2::CK_ID. 
#  @return True if no error, otherwise False.
BMMeshTrans_Parse = _create_bmap_func('BMMeshTrans_Parse', [bm_void_p, bm_void_p, bm_CKID])
## BMObject_GetName
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_name[out] Type: LibCmo::CKSTRING. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMObject_GetName = _create_bmap_func('BMObject_GetName', [bm_void_p, bm_CKID, bm_CKSTRING_p])
## BMObject_SetName
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param name[in] Type: LibCmo::CKSTRING. 
#  @return True if no error, otherwise False.
BMObject_SetName = _create_bmap_func('BMObject_SetName', [bm_void_p, bm_CKID, bm_CKSTRING])
## BMGroup_AddObject
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param memberid[in] Type: LibCmo::CK2::CK_ID. 
#  @return True if no error, otherwise False.
BMGroup_AddObject = _create_bmap_func('BMGroup_AddObject', [bm_void_p, bm_CKID, bm_CKID])
## BMGroup_GetObjectCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_count[out] Type: LibCmo::CKDWORD. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMGroup_GetObjectCount = _create_bmap_func('BMGroup_GetObjectCount', [bm_void_p, bm_CKID, bm_CKDWORD_p])
## BMGroup_GetObject
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param pos[in] Type: LibCmo::CKDWORD. 
#  @param out_objid[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMGroup_GetObject = _create_bmap_func('BMGroup_GetObject', [bm_void_p, bm_CKID, bm_CKDWORD, bm_CKID_p])
## BMTexture_GetFileName
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_filename[out] Type: LibCmo::CKSTRING. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMTexture_GetFileName = _create_bmap_func('BMTexture_GetFileName', [bm_void_p, bm_CKID, bm_CKSTRING_p])
## BMTexture_LoadImage
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param filename[in] Type: LibCmo::CKSTRING. 
#  @return True if no error, otherwise False.
BMTexture_LoadImage = _create_bmap_func('BMTexture_LoadImage', [bm_void_p, bm_CKID, bm_CKSTRING])
## BMTexture_SaveImage
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param filename[in] Type: LibCmo::CKSTRING. 
#  @return True if no error, otherwise False.
BMTexture_SaveImage = _create_bmap_func('BMTexture_SaveImage', [bm_void_p, bm_CKID, bm_CKSTRING])
## BMTexture_GetSaveOptions
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_saveopt[out] Type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMTexture_GetSaveOptions = _create_bmap_func('BMTexture_GetSaveOptions', [bm_void_p, bm_CKID, bm_enum_p])
## BMTexture_SetSaveOptions
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param saveopt[in] Type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. 
#  @return True if no error, otherwise False.
BMTexture_SetSaveOptions = _create_bmap_func('BMTexture_SetSaveOptions', [bm_void_p, bm_CKID, bm_enum])
## BMTexture_GetVideoFormat
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_vfmt[out] Type: LibCmo::VxMath::VX_PIXELFORMAT. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMTexture_GetVideoFormat = _create_bmap_func('BMTexture_GetVideoFormat', [bm_void_p, bm_CKID, bm_enum_p])
## BMTexture_SetVideoFormat
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param vfmt[in] Type: LibCmo::VxMath::VX_PIXELFORMAT. 
#  @return True if no error, otherwise False.
BMTexture_SetVideoFormat = _create_bmap_func('BMTexture_SetVideoFormat', [bm_void_p, bm_CKID, bm_enum])
## BMMaterial_GetDiffuse
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VxColor. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetDiffuse = _create_bmap_func('BMMaterial_GetDiffuse', [bm_void_p, bm_CKID, bm_VxColor_p])
## BMMaterial_SetDiffuse
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param col[in] Type: LibCmo::VxMath::VxColor. 
#  @return True if no error, otherwise False.
BMMaterial_SetDiffuse = _create_bmap_func('BMMaterial_SetDiffuse', [bm_void_p, bm_CKID, bm_VxColor])
## BMMaterial_GetAmbient
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VxColor. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetAmbient = _create_bmap_func('BMMaterial_GetAmbient', [bm_void_p, bm_CKID, bm_VxColor_p])
## BMMaterial_SetAmbient
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param col[in] Type: LibCmo::VxMath::VxColor. 
#  @return True if no error, otherwise False.
BMMaterial_SetAmbient = _create_bmap_func('BMMaterial_SetAmbient', [bm_void_p, bm_CKID, bm_VxColor])
## BMMaterial_GetSpecular
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VxColor. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetSpecular = _create_bmap_func('BMMaterial_GetSpecular', [bm_void_p, bm_CKID, bm_VxColor_p])
## BMMaterial_SetSpecular
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param col[in] Type: LibCmo::VxMath::VxColor. 
#  @return True if no error, otherwise False.
BMMaterial_SetSpecular = _create_bmap_func('BMMaterial_SetSpecular', [bm_void_p, bm_CKID, bm_VxColor])
## BMMaterial_GetEmissive
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VxColor. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetEmissive = _create_bmap_func('BMMaterial_GetEmissive', [bm_void_p, bm_CKID, bm_VxColor_p])
## BMMaterial_SetEmissive
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param col[in] Type: LibCmo::VxMath::VxColor. 
#  @return True if no error, otherwise False.
BMMaterial_SetEmissive = _create_bmap_func('BMMaterial_SetEmissive', [bm_void_p, bm_CKID, bm_VxColor])
## BMMaterial_GetSpecularPower
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::CKFLOAT. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetSpecularPower = _create_bmap_func('BMMaterial_GetSpecularPower', [bm_void_p, bm_CKID, bm_CKFLOAT_p])
## BMMaterial_SetSpecularPower
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::CKFLOAT. 
#  @return True if no error, otherwise False.
BMMaterial_SetSpecularPower = _create_bmap_func('BMMaterial_SetSpecularPower', [bm_void_p, bm_CKID, bm_CKFLOAT])
## BMMaterial_GetTexture
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_texid[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetTexture = _create_bmap_func('BMMaterial_GetTexture', [bm_void_p, bm_CKID, bm_CKID_p])
## BMMaterial_SetTexture
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param texid[in] Type: LibCmo::CK2::CK_ID. 
#  @return True if no error, otherwise False.
BMMaterial_SetTexture = _create_bmap_func('BMMaterial_SetTexture', [bm_void_p, bm_CKID, bm_CKID])
## BMMaterial_GetTextureBorderColor
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::CKDWORD. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetTextureBorderColor = _create_bmap_func('BMMaterial_GetTextureBorderColor', [bm_void_p, bm_CKID, bm_CKDWORD_p])
## BMMaterial_SetTextureBorderColor
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::CKDWORD. 
#  @return True if no error, otherwise False.
BMMaterial_SetTextureBorderColor = _create_bmap_func('BMMaterial_SetTextureBorderColor', [bm_void_p, bm_CKID, bm_CKDWORD])
## BMMaterial_GetTextureBlendMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VXTEXTURE_BLENDMODE. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetTextureBlendMode = _create_bmap_func('BMMaterial_GetTextureBlendMode', [bm_void_p, bm_CKID, bm_enum_p])
## BMMaterial_SetTextureBlendMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::VxMath::VXTEXTURE_BLENDMODE. 
#  @return True if no error, otherwise False.
BMMaterial_SetTextureBlendMode = _create_bmap_func('BMMaterial_SetTextureBlendMode', [bm_void_p, bm_CKID, bm_enum])
## BMMaterial_GetTextureMinMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetTextureMinMode = _create_bmap_func('BMMaterial_GetTextureMinMode', [bm_void_p, bm_CKID, bm_enum_p])
## BMMaterial_SetTextureMinMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. 
#  @return True if no error, otherwise False.
BMMaterial_SetTextureMinMode = _create_bmap_func('BMMaterial_SetTextureMinMode', [bm_void_p, bm_CKID, bm_enum])
## BMMaterial_GetTextureMagMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetTextureMagMode = _create_bmap_func('BMMaterial_GetTextureMagMode', [bm_void_p, bm_CKID, bm_enum_p])
## BMMaterial_SetTextureMagMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. 
#  @return True if no error, otherwise False.
BMMaterial_SetTextureMagMode = _create_bmap_func('BMMaterial_SetTextureMagMode', [bm_void_p, bm_CKID, bm_enum])
## BMMaterial_GetTextureAddressMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VXTEXTURE_ADDRESSMODE. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetTextureAddressMode = _create_bmap_func('BMMaterial_GetTextureAddressMode', [bm_void_p, bm_CKID, bm_enum_p])
## BMMaterial_SetTextureAddressMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::VxMath::VXTEXTURE_ADDRESSMODE. 
#  @return True if no error, otherwise False.
BMMaterial_SetTextureAddressMode = _create_bmap_func('BMMaterial_SetTextureAddressMode', [bm_void_p, bm_CKID, bm_enum])
## BMMaterial_GetSourceBlend
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VXBLEND_MODE. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetSourceBlend = _create_bmap_func('BMMaterial_GetSourceBlend', [bm_void_p, bm_CKID, bm_enum_p])
## BMMaterial_SetSourceBlend
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::VxMath::VXBLEND_MODE. 
#  @return True if no error, otherwise False.
BMMaterial_SetSourceBlend = _create_bmap_func('BMMaterial_SetSourceBlend', [bm_void_p, bm_CKID, bm_enum])
## BMMaterial_GetDestBlend
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VXBLEND_MODE. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetDestBlend = _create_bmap_func('BMMaterial_GetDestBlend', [bm_void_p, bm_CKID, bm_enum_p])
## BMMaterial_SetDestBlend
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::VxMath::VXBLEND_MODE. 
#  @return True if no error, otherwise False.
BMMaterial_SetDestBlend = _create_bmap_func('BMMaterial_SetDestBlend', [bm_void_p, bm_CKID, bm_enum])
## BMMaterial_GetFillMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VXFILL_MODE. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetFillMode = _create_bmap_func('BMMaterial_GetFillMode', [bm_void_p, bm_CKID, bm_enum_p])
## BMMaterial_SetFillMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::VxMath::VXFILL_MODE. 
#  @return True if no error, otherwise False.
BMMaterial_SetFillMode = _create_bmap_func('BMMaterial_SetFillMode', [bm_void_p, bm_CKID, bm_enum])
## BMMaterial_GetShadeMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VXSHADE_MODE. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetShadeMode = _create_bmap_func('BMMaterial_GetShadeMode', [bm_void_p, bm_CKID, bm_enum_p])
## BMMaterial_SetShadeMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::VxMath::VXSHADE_MODE. 
#  @return True if no error, otherwise False.
BMMaterial_SetShadeMode = _create_bmap_func('BMMaterial_SetShadeMode', [bm_void_p, bm_CKID, bm_enum])
## BMMaterial_GetAlphaTestEnabled
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: bool. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetAlphaTestEnabled = _create_bmap_func('BMMaterial_GetAlphaTestEnabled', [bm_void_p, bm_CKID, bm_bool_p])
## BMMaterial_SetAlphaTestEnabled
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param enabled[in] Type: bool. 
#  @return True if no error, otherwise False.
BMMaterial_SetAlphaTestEnabled = _create_bmap_func('BMMaterial_SetAlphaTestEnabled', [bm_void_p, bm_CKID, bm_bool])
## BMMaterial_GetAlphaBlendEnabled
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: bool. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetAlphaBlendEnabled = _create_bmap_func('BMMaterial_GetAlphaBlendEnabled', [bm_void_p, bm_CKID, bm_bool_p])
## BMMaterial_SetAlphaBlendEnabled
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param enabled[in] Type: bool. 
#  @return True if no error, otherwise False.
BMMaterial_SetAlphaBlendEnabled = _create_bmap_func('BMMaterial_SetAlphaBlendEnabled', [bm_void_p, bm_CKID, bm_bool])
## BMMaterial_GetPerspectiveCorrectionEnabled
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: bool. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetPerspectiveCorrectionEnabled = _create_bmap_func('BMMaterial_GetPerspectiveCorrectionEnabled', [bm_void_p, bm_CKID, bm_bool_p])
## BMMaterial_SetPerspectiveCorrectionEnabled
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param enabled[in] Type: bool. 
#  @return True if no error, otherwise False.
BMMaterial_SetPerspectiveCorrectionEnabled = _create_bmap_func('BMMaterial_SetPerspectiveCorrectionEnabled', [bm_void_p, bm_CKID, bm_bool])
## BMMaterial_GetZWriteEnabled
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: bool. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetZWriteEnabled = _create_bmap_func('BMMaterial_GetZWriteEnabled', [bm_void_p, bm_CKID, bm_bool_p])
## BMMaterial_SetZWriteEnabled
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param enabled[in] Type: bool. 
#  @return True if no error, otherwise False.
BMMaterial_SetZWriteEnabled = _create_bmap_func('BMMaterial_SetZWriteEnabled', [bm_void_p, bm_CKID, bm_bool])
## BMMaterial_GetTwoSidedEnabled
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: bool. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetTwoSidedEnabled = _create_bmap_func('BMMaterial_GetTwoSidedEnabled', [bm_void_p, bm_CKID, bm_bool_p])
## BMMaterial_SetTwoSidedEnabled
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param enabled[in] Type: bool. 
#  @return True if no error, otherwise False.
BMMaterial_SetTwoSidedEnabled = _create_bmap_func('BMMaterial_SetTwoSidedEnabled', [bm_void_p, bm_CKID, bm_bool])
## BMMaterial_GetAlphaRef
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::CKBYTE. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetAlphaRef = _create_bmap_func('BMMaterial_GetAlphaRef', [bm_void_p, bm_CKID, bm_CKBYTE_p])
## BMMaterial_SetAlphaRef
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::CKBYTE. 
#  @return True if no error, otherwise False.
BMMaterial_SetAlphaRef = _create_bmap_func('BMMaterial_SetAlphaRef', [bm_void_p, bm_CKID, bm_CKBYTE])
## BMMaterial_GetAlphaFunc
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VXCMPFUNC. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetAlphaFunc = _create_bmap_func('BMMaterial_GetAlphaFunc', [bm_void_p, bm_CKID, bm_enum_p])
## BMMaterial_SetAlphaFunc
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::VxMath::VXCMPFUNC. 
#  @return True if no error, otherwise False.
BMMaterial_SetAlphaFunc = _create_bmap_func('BMMaterial_SetAlphaFunc', [bm_void_p, bm_CKID, bm_enum])
## BMMaterial_GetZFunc
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_val[out] Type: LibCmo::VxMath::VXCMPFUNC. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMaterial_GetZFunc = _create_bmap_func('BMMaterial_GetZFunc', [bm_void_p, bm_CKID, bm_enum_p])
## BMMaterial_SetZFunc
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param val[in] Type: LibCmo::VxMath::VXCMPFUNC. 
#  @return True if no error, otherwise False.
BMMaterial_SetZFunc = _create_bmap_func('BMMaterial_SetZFunc', [bm_void_p, bm_CKID, bm_enum])
## BMMesh_GetLitMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_mode[out] Type: LibCmo::VxMath::VXMESH_LITMODE. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMesh_GetLitMode = _create_bmap_func('BMMesh_GetLitMode', [bm_void_p, bm_CKID, bm_enum_p])
## BMMesh_SetLitMode
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param mode[in] Type: LibCmo::VxMath::VXMESH_LITMODE. 
#  @return True if no error, otherwise False.
BMMesh_SetLitMode = _create_bmap_func('BMMesh_SetLitMode', [bm_void_p, bm_CKID, bm_enum])
## BMMesh_GetVertexCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_count[out] Type: LibCmo::CKDWORD. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMesh_GetVertexCount = _create_bmap_func('BMMesh_GetVertexCount', [bm_void_p, bm_CKID, bm_CKDWORD_p])
## BMMesh_SetVertexCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param count[in] Type: LibCmo::CKDWORD. 
#  @return True if no error, otherwise False.
BMMesh_SetVertexCount = _create_bmap_func('BMMesh_SetVertexCount', [bm_void_p, bm_CKID, bm_CKDWORD])
## BMMesh_GetVertexPositions
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_mem[out] Type: LibCmo::VxMath::VxVector3*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMesh_GetVertexPositions = _create_bmap_func('BMMesh_GetVertexPositions', [bm_void_p, bm_CKID, bm_VxVector3_pp])
## BMMesh_GetVertexNormals
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_mem[out] Type: LibCmo::VxMath::VxVector3*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMesh_GetVertexNormals = _create_bmap_func('BMMesh_GetVertexNormals', [bm_void_p, bm_CKID, bm_VxVector3_pp])
## BMMesh_GetVertexUVs
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_mem[out] Type: LibCmo::VxMath::VxVector2*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMesh_GetVertexUVs = _create_bmap_func('BMMesh_GetVertexUVs', [bm_void_p, bm_CKID, bm_VxVector2_pp])
## BMMesh_GetFaceCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_count[out] Type: LibCmo::CKDWORD. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMesh_GetFaceCount = _create_bmap_func('BMMesh_GetFaceCount', [bm_void_p, bm_CKID, bm_CKDWORD_p])
## BMMesh_SetFaceCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param count[in] Type: LibCmo::CKDWORD. 
#  @return True if no error, otherwise False.
BMMesh_SetFaceCount = _create_bmap_func('BMMesh_SetFaceCount', [bm_void_p, bm_CKID, bm_CKDWORD])
## BMMesh_GetFaceIndices
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_mem[out] Type: LibCmo::CKWORD*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMesh_GetFaceIndices = _create_bmap_func('BMMesh_GetFaceIndices', [bm_void_p, bm_CKID, bm_CKWORD_pp])
## BMMesh_GetFaceMaterialSlotIndexs
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_mem[out] Type: LibCmo::CKWORD*. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMesh_GetFaceMaterialSlotIndexs = _create_bmap_func('BMMesh_GetFaceMaterialSlotIndexs', [bm_void_p, bm_CKID, bm_CKWORD_pp])
## BMMesh_GetMaterialSlotCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_count[out] Type: LibCmo::CKDWORD. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMesh_GetMaterialSlotCount = _create_bmap_func('BMMesh_GetMaterialSlotCount', [bm_void_p, bm_CKID, bm_CKDWORD_p])
## BMMesh_SetMaterialSlotCount
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param count[in] Type: LibCmo::CKDWORD. 
#  @return True if no error, otherwise False.
BMMesh_SetMaterialSlotCount = _create_bmap_func('BMMesh_SetMaterialSlotCount', [bm_void_p, bm_CKID, bm_CKDWORD])
## BMMesh_GetMaterialSlot
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param index[in] Type: LibCmo::CKDWORD. 
#  @param out_mtlid[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BMMesh_GetMaterialSlot = _create_bmap_func('BMMesh_GetMaterialSlot', [bm_void_p, bm_CKID, bm_CKDWORD, bm_CKID_p])
## BMMesh_SetMaterialSlot
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param index[in] Type: LibCmo::CKDWORD. 
#  @param mtlid[in] Type: LibCmo::CK2::CK_ID. 
#  @return True if no error, otherwise False.
BMMesh_SetMaterialSlot = _create_bmap_func('BMMesh_SetMaterialSlot', [bm_void_p, bm_CKID, bm_CKDWORD, bm_CKID])
## BM3dObject_GetWorldMatrix
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_mat[out] Type: LibCmo::VxMath::VxMatrix. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BM3dObject_GetWorldMatrix = _create_bmap_func('BM3dObject_GetWorldMatrix', [bm_void_p, bm_CKID, bm_VxMatrix_p])
## BM3dObject_SetWorldMatrix
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param mat[in] Type: LibCmo::VxMath::VxMatrix. 
#  @return True if no error, otherwise False.
BM3dObject_SetWorldMatrix = _create_bmap_func('BM3dObject_SetWorldMatrix', [bm_void_p, bm_CKID, bm_VxMatrix])
## BM3dObject_GetCurrentMesh
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_meshid[out] Type: LibCmo::CK2::CK_ID. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BM3dObject_GetCurrentMesh = _create_bmap_func('BM3dObject_GetCurrentMesh', [bm_void_p, bm_CKID, bm_CKID_p])
## BM3dObject_SetCurrentMesh
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param meshid[in] Type: LibCmo::CK2::CK_ID. 
#  @return True if no error, otherwise False.
BM3dObject_SetCurrentMesh = _create_bmap_func('BM3dObject_SetCurrentMesh', [bm_void_p, bm_CKID, bm_CKID])
## BM3dObject_GetVisibility
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param out_isVisible[out] Type: bool. Use ctypes.byref(data) pass it. 
#  @return True if no error, otherwise False.
BM3dObject_GetVisibility = _create_bmap_func('BM3dObject_GetVisibility', [bm_void_p, bm_CKID, bm_bool_p])
## BM3dObject_SetVisibility
#  @param bmfile[in] Type: BMap::BMFile*. The pointer to corresponding BMFile.
#  @param objid[in] Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.
#  @param is_visible[in] Type: bool. 
#  @return True if no error, otherwise False.
BM3dObject_SetVisibility = _create_bmap_func('BM3dObject_SetVisibility', [bm_void_p, bm_CKID, bm_bool])

#endregion

