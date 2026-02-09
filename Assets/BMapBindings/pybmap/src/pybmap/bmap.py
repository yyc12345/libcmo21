import ctypes
import sys
import typing
import pathlib

#region Type Defines

class BMapException(Exception):
    """
    The exception thrown by BMap bindings.
    """
    pass

# YYC MARK:
# Due to the shitty type hints support for Python ctypes module,
# I have to use typing.TYPE_CHECKING to write these shit code for type hint.
# And the more disgusting thing is that these code is still not perfect.

bm_CKSTRING = ctypes.c_char_p
if typing.TYPE_CHECKING:
    bm_CKSTRING_p = ctypes._Pointer[bm_CKSTRING]
else:
    bm_CKSTRING_p = ctypes.POINTER(bm_CKSTRING)
bm_CKDWORD = ctypes.c_uint32
if typing.TYPE_CHECKING:
    bm_CKDWORD_p = ctypes._Pointer[bm_CKDWORD]
    bm_CKDWORD_pp = ctypes._Pointer[bm_CKDWORD_p]
else:
    bm_CKDWORD_p = ctypes.POINTER(bm_CKDWORD)
    bm_CKDWORD_pp = ctypes.POINTER(bm_CKDWORD_p)
bm_CKWORD = ctypes.c_uint16
if typing.TYPE_CHECKING:
    bm_CKWORD_p = ctypes._Pointer[bm_CKWORD]
    bm_CKWORD_pp = ctypes._Pointer[bm_CKWORD_p]
else:
    bm_CKWORD_p = ctypes.POINTER(bm_CKWORD)
    bm_CKWORD_pp = ctypes.POINTER(bm_CKWORD_p)
bm_CKID = ctypes.c_uint32
if typing.TYPE_CHECKING:
    bm_CKID_p = ctypes._Pointer[bm_CKID]
    bm_CKID_pp = ctypes._Pointer[bm_CKID_p]
else:
    bm_CKID_p = ctypes.POINTER(bm_CKID)
    bm_CKID_pp = ctypes.POINTER(bm_CKID_p)
bm_CKFLOAT = ctypes.c_float
if typing.TYPE_CHECKING:
    bm_CKFLOAT_p = ctypes._Pointer[bm_CKFLOAT]
else:
    bm_CKFLOAT_p = ctypes.POINTER(bm_CKFLOAT)
bm_CKINT = ctypes.c_int32
bm_CKBYTE = ctypes.c_uint8
if typing.TYPE_CHECKING:
    bm_CKBYTE_p = ctypes._Pointer[bm_CKBYTE]
else:
    bm_CKBYTE_p = ctypes.POINTER(bm_CKBYTE)

bm_enum = bm_CKDWORD
if typing.TYPE_CHECKING:
    bm_enum_p = ctypes._Pointer[bm_enum]
else:
    bm_enum_p = ctypes.POINTER(bm_enum)
bm_bool = ctypes.c_bool
if typing.TYPE_CHECKING:
    bm_bool_p = ctypes._Pointer[bm_bool]
else:
    bm_bool_p = ctypes.POINTER(bm_bool)
bm_void_p = ctypes.c_void_p
if typing.TYPE_CHECKING:
    bm_void_pp = ctypes._Pointer[bm_void_p]
else:
    bm_void_pp = ctypes.POINTER(ctypes.c_void_p)

# YYC MARK:
# I can't find any viable solution for type hint of C callbakc function type.
# So I give up. I leave original code here.
bm_callback = ctypes.CFUNCTYPE(None, bm_CKSTRING)

class bm_VxVector2(ctypes.Structure):
    _fields_ = [
        ('x', bm_CKFLOAT),
        ('y', bm_CKFLOAT),
    ]
if typing.TYPE_CHECKING:
    bm_VxVector2_p = ctypes._Pointer[bm_VxVector2]
    bm_VxVector2_pp = ctypes._Pointer[bm_VxVector2_p]
else:
    bm_VxVector2_p = ctypes.POINTER(bm_VxVector2)
    bm_VxVector2_pp = ctypes.POINTER(bm_VxVector2_p)
class bm_VxVector3(ctypes.Structure):
    _fields_ = [
        ('x', bm_CKFLOAT),
        ('y', bm_CKFLOAT),
        ('z', bm_CKFLOAT),
    ]
if typing.TYPE_CHECKING:
    bm_VxVector3_p = ctypes._Pointer[bm_VxVector3]
    bm_VxVector3_pp = ctypes._Pointer[bm_VxVector3_p]
else:
    bm_VxVector3_p = ctypes.POINTER(bm_VxVector3)
    bm_VxVector3_pp = ctypes.POINTER(bm_VxVector3_p)
class bm_VxColor(ctypes.Structure):
    _fields_ = [
        ('r', bm_CKFLOAT),
        ('g', bm_CKFLOAT),
        ('b', bm_CKFLOAT),
        ('a', bm_CKFLOAT),
    ]
if typing.TYPE_CHECKING:
    bm_VxColor_p = ctypes._Pointer[bm_VxColor]
else:
    bm_VxColor_p = ctypes.POINTER(bm_VxColor)
class bm_VxMatrix(ctypes.Structure):
    _fields_ = list(
        (f'i{idx}', bm_CKFLOAT) for idx in range(16)
    )
if typing.TYPE_CHECKING:
    bm_VxMatrix_p = ctypes._Pointer[bm_VxMatrix]
else:
    bm_VxMatrix_p = ctypes.POINTER(bm_VxMatrix)

#endregion

#region BMap Loader

# Decide the name of DLL
_BMAP_DLL_NAME: str

if sys.platform.startswith('win32') or sys.platform.startswith('cygwin'):
    _BMAP_DLL_NAME = "BMap.dll"
elif sys.platform.startswith('linux') or sys.platform.startswith('freebsd'):
    _BMAP_DLL_NAME = "BMap.so"
elif sys.platform.startswith('darwin'):
    _BMAP_DLL_NAME = "BMap.dylib"
else:
    _BMAP_DLL_NAME = "BMap.bin"

# Build the path to DLL
_BMAP_DLL_PATH: pathlib.Path = pathlib.Path(__file__).resolve().parent / _BMAP_DLL_NAME

# Load DLL into Python
_BMAP_MODULE: ctypes.CDLL | None = None

try:
    _BMAP_MODULE = ctypes.cdll.LoadLibrary(str(_BMAP_DLL_PATH))
except:
    print(f'Fail to load native BMap dynamic library file "{_BMAP_DLL_PATH}".')
    _BMAP_MODULE = None

def is_bmap_available() -> bool:
    return _BMAP_MODULE is not None

def _bmap_error_checker(result: bool, func, args) -> bool:
    # YYC MARK:
    # It seems that Python do reveal the underlying type of bm_bool
    # and pass Python bool as the first argument for this function at runtime.
    if not result:
        raise BMapException("BMap operation failed.")
    return result

Ts = typing.TypeVarTuple('Ts')

def _create_bmap_func(fct_name: str, fct_params: tuple[*Ts]) -> typing.Callable[[typing.Unpack[Ts]], bm_bool]:
    if _BMAP_MODULE is None:
        raise BMapException(f'Fail to load native BMap dynamic library file "{_BMAP_DLL_PATH}".')
    
    # Reference: https://docs.python.org/3.11/library/ctypes.html#foreign-functions
    fct = _BMAP_MODULE.__getattr__(fct_name)
    # YYC MARK:
    # Idk why these type hints for the members of non-public ctypes._NamedFuncPointer 
    # are so weired that can not accept my correct parameters.
    # I've had enough. I use typing.cast and typing.Any to bury my head in the sand.
    fct.argtypes = typing.cast(typing.Any, fct_params)
    fct.restype = bm_bool
    fct.errcheck = typing.cast(typing.Any, _bmap_error_checker)
    return fct

#endregion

#region Function Defines

##### GENERATED FUNCTIONS BEGIN #####

BMInit = _create_bmap_func('BMInit', ())
"""
BMInit

:return: True if no error, otherwise False.
:rtype: bool
"""
BMDispose = _create_bmap_func('BMDispose', ())
"""
BMDispose

:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_Load = _create_bmap_func('BMFile_Load', (bm_CKSTRING, bm_CKSTRING, bm_CKSTRING, bm_callback, bm_CKDWORD, bm_CKSTRING_p, bm_void_pp, ))
"""
BMFile_Load

:param file_name: Direction: input. 
:type file_name: bm_CKSTRING (LibCmo::CKSTRING in C++). 
:param temp_folder: Direction: input. 
:type temp_folder: bm_CKSTRING (LibCmo::CKSTRING in C++). 
:param texture_folder: Direction: input. 
:type texture_folder: bm_CKSTRING (LibCmo::CKSTRING in C++). 
:param raw_callback: Direction: input. 
:type raw_callback: bm_callback (BMap::NakedOutputCallback in C++). 
:param encoding_count: Direction: input. 
:type encoding_count: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param encodings: Direction: input. 
:type encodings: bm_CKSTRING_p (LibCmo::CKSTRING* in C++). 
:param out_file: Direction: output. 
:type out_file: bm_void_pp (BMap::BMFile* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_Create = _create_bmap_func('BMFile_Create', (bm_CKSTRING, bm_CKSTRING, bm_callback, bm_CKDWORD, bm_CKSTRING_p, bm_void_pp, ))
"""
BMFile_Create

:param temp_folder: Direction: input. 
:type temp_folder: bm_CKSTRING (LibCmo::CKSTRING in C++). 
:param texture_folder: Direction: input. 
:type texture_folder: bm_CKSTRING (LibCmo::CKSTRING in C++). 
:param raw_callback: Direction: input. 
:type raw_callback: bm_callback (BMap::NakedOutputCallback in C++). 
:param encoding_count: Direction: input. 
:type encoding_count: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param encodings: Direction: input. 
:type encodings: bm_CKSTRING_p (LibCmo::CKSTRING* in C++). 
:param out_file: Direction: output. 
:type out_file: bm_void_pp (BMap::BMFile* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_Save = _create_bmap_func('BMFile_Save', (bm_void_p, bm_CKSTRING, bm_enum, bm_bool, bm_CKINT, ))
"""
BMFile_Save

:param map_file: Direction: input. 
:type map_file: bm_void_p (BMap::BMFile* in C++). 
:param file_name: Direction: input. 
:type file_name: bm_CKSTRING (LibCmo::CKSTRING in C++). 
:param texture_save_opt: Direction: input. 
:type texture_save_opt: bm_enum (LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS in C++). 
:param use_compress: Direction: input. 
:type use_compress: bm_bool (bool in C++). 
:param compreess_level: Direction: input. 
:type compreess_level: bm_CKINT (LibCmo::CKINT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_Free = _create_bmap_func('BMFile_Free', (bm_void_p, ))
"""
BMFile_Free

:param map_file: Direction: input. 
:type map_file: bm_void_p (BMap::BMFile* in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetGroupCount = _create_bmap_func('BMFile_GetGroupCount', (bm_void_p, bm_CKDWORD_p, ))
"""
BMFile_GetGroupCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_count: Direction: output. 
:type out_count: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetGroup = _create_bmap_func('BMFile_GetGroup', (bm_void_p, bm_CKDWORD, bm_CKID_p, ))
"""
BMFile_GetGroup

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param idx: Direction: input. 
:type idx: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_CreateGroup = _create_bmap_func('BMFile_CreateGroup', (bm_void_p, bm_CKID_p, ))
"""
BMFile_CreateGroup

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_Get3dObjectCount = _create_bmap_func('BMFile_Get3dObjectCount', (bm_void_p, bm_CKDWORD_p, ))
"""
BMFile_Get3dObjectCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_count: Direction: output. 
:type out_count: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_Get3dObject = _create_bmap_func('BMFile_Get3dObject', (bm_void_p, bm_CKDWORD, bm_CKID_p, ))
"""
BMFile_Get3dObject

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param idx: Direction: input. 
:type idx: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_Create3dObject = _create_bmap_func('BMFile_Create3dObject', (bm_void_p, bm_CKID_p, ))
"""
BMFile_Create3dObject

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetMeshCount = _create_bmap_func('BMFile_GetMeshCount', (bm_void_p, bm_CKDWORD_p, ))
"""
BMFile_GetMeshCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_count: Direction: output. 
:type out_count: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetMesh = _create_bmap_func('BMFile_GetMesh', (bm_void_p, bm_CKDWORD, bm_CKID_p, ))
"""
BMFile_GetMesh

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param idx: Direction: input. 
:type idx: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_CreateMesh = _create_bmap_func('BMFile_CreateMesh', (bm_void_p, bm_CKID_p, ))
"""
BMFile_CreateMesh

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetMaterialCount = _create_bmap_func('BMFile_GetMaterialCount', (bm_void_p, bm_CKDWORD_p, ))
"""
BMFile_GetMaterialCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_count: Direction: output. 
:type out_count: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetMaterial = _create_bmap_func('BMFile_GetMaterial', (bm_void_p, bm_CKDWORD, bm_CKID_p, ))
"""
BMFile_GetMaterial

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param idx: Direction: input. 
:type idx: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_CreateMaterial = _create_bmap_func('BMFile_CreateMaterial', (bm_void_p, bm_CKID_p, ))
"""
BMFile_CreateMaterial

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetTextureCount = _create_bmap_func('BMFile_GetTextureCount', (bm_void_p, bm_CKDWORD_p, ))
"""
BMFile_GetTextureCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_count: Direction: output. 
:type out_count: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetTexture = _create_bmap_func('BMFile_GetTexture', (bm_void_p, bm_CKDWORD, bm_CKID_p, ))
"""
BMFile_GetTexture

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param idx: Direction: input. 
:type idx: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_CreateTexture = _create_bmap_func('BMFile_CreateTexture', (bm_void_p, bm_CKID_p, ))
"""
BMFile_CreateTexture

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetTargetLightCount = _create_bmap_func('BMFile_GetTargetLightCount', (bm_void_p, bm_CKDWORD_p, ))
"""
BMFile_GetTargetLightCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_count: Direction: output. 
:type out_count: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetTargetLight = _create_bmap_func('BMFile_GetTargetLight', (bm_void_p, bm_CKDWORD, bm_CKID_p, ))
"""
BMFile_GetTargetLight

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param idx: Direction: input. 
:type idx: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_CreateTargetLight = _create_bmap_func('BMFile_CreateTargetLight', (bm_void_p, bm_CKID_p, ))
"""
BMFile_CreateTargetLight

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetTargetCameraCount = _create_bmap_func('BMFile_GetTargetCameraCount', (bm_void_p, bm_CKDWORD_p, ))
"""
BMFile_GetTargetCameraCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_count: Direction: output. 
:type out_count: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_GetTargetCamera = _create_bmap_func('BMFile_GetTargetCamera', (bm_void_p, bm_CKDWORD, bm_CKID_p, ))
"""
BMFile_GetTargetCamera

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param idx: Direction: input. 
:type idx: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMFile_CreateTargetCamera = _create_bmap_func('BMFile_CreateTargetCamera', (bm_void_p, bm_CKID_p, ))
"""
BMFile_CreateTargetCamera

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param out_id: Direction: output. 
:type out_id: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_New = _create_bmap_func('BMMeshTrans_New', (bm_void_pp, ))
"""
BMMeshTrans_New

:param out_trans: Direction: output. 
:type out_trans: bm_void_pp (BMap::BMMeshTransition* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_Delete = _create_bmap_func('BMMeshTrans_Delete', (bm_void_p, ))
"""
BMMeshTrans_Delete

:param trans: Direction: input. 
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareVertexCount = _create_bmap_func('BMMeshTrans_PrepareVertexCount', (bm_void_p, bm_CKDWORD, ))
"""
BMMeshTrans_PrepareVertexCount

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param count: Direction: input. 
:type count: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareVertex = _create_bmap_func('BMMeshTrans_PrepareVertex', (bm_void_p, bm_VxVector3_pp, ))
"""
BMMeshTrans_PrepareVertex

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_VxVector3_pp (LibCmo::VxMath::VxVector3* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareNormalCount = _create_bmap_func('BMMeshTrans_PrepareNormalCount', (bm_void_p, bm_CKDWORD, ))
"""
BMMeshTrans_PrepareNormalCount

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param count: Direction: input. 
:type count: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareNormal = _create_bmap_func('BMMeshTrans_PrepareNormal', (bm_void_p, bm_VxVector3_pp, ))
"""
BMMeshTrans_PrepareNormal

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_VxVector3_pp (LibCmo::VxMath::VxVector3* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareUVCount = _create_bmap_func('BMMeshTrans_PrepareUVCount', (bm_void_p, bm_CKDWORD, ))
"""
BMMeshTrans_PrepareUVCount

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param count: Direction: input. 
:type count: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareUV = _create_bmap_func('BMMeshTrans_PrepareUV', (bm_void_p, bm_VxVector2_pp, ))
"""
BMMeshTrans_PrepareUV

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_VxVector2_pp (LibCmo::VxMath::VxVector2* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareMtlSlotCount = _create_bmap_func('BMMeshTrans_PrepareMtlSlotCount', (bm_void_p, bm_CKDWORD, ))
"""
BMMeshTrans_PrepareMtlSlotCount

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param count: Direction: input. 
:type count: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareMtlSlot = _create_bmap_func('BMMeshTrans_PrepareMtlSlot', (bm_void_p, bm_CKID_pp, ))
"""
BMMeshTrans_PrepareMtlSlot

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_CKID_pp (LibCmo::CK2::CK_ID* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareFaceCount = _create_bmap_func('BMMeshTrans_PrepareFaceCount', (bm_void_p, bm_CKDWORD, ))
"""
BMMeshTrans_PrepareFaceCount

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param count: Direction: input. 
:type count: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareFaceVertexIndices = _create_bmap_func('BMMeshTrans_PrepareFaceVertexIndices', (bm_void_p, bm_CKDWORD_pp, ))
"""
BMMeshTrans_PrepareFaceVertexIndices

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_CKDWORD_pp (LibCmo::CKDWORD* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareFaceNormalIndices = _create_bmap_func('BMMeshTrans_PrepareFaceNormalIndices', (bm_void_p, bm_CKDWORD_pp, ))
"""
BMMeshTrans_PrepareFaceNormalIndices

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_CKDWORD_pp (LibCmo::CKDWORD* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareFaceUVIndices = _create_bmap_func('BMMeshTrans_PrepareFaceUVIndices', (bm_void_p, bm_CKDWORD_pp, ))
"""
BMMeshTrans_PrepareFaceUVIndices

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_CKDWORD_pp (LibCmo::CKDWORD* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_PrepareFaceMtlSlot = _create_bmap_func('BMMeshTrans_PrepareFaceMtlSlot', (bm_void_p, bm_CKDWORD_pp, ))
"""
BMMeshTrans_PrepareFaceMtlSlot

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_CKDWORD_pp (LibCmo::CKDWORD* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMeshTrans_Parse = _create_bmap_func('BMMeshTrans_Parse', (bm_void_p, bm_void_p, bm_CKID, ))
"""
BMMeshTrans_Parse

:param trans: Direction: input. The pointer to corresponding BMMeshTransition.
:type trans: bm_void_p (BMap::BMMeshTransition* in C++). 
:param bmfile: Direction: input. 
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. 
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMObject_GetName = _create_bmap_func('BMObject_GetName', (bm_void_p, bm_CKID, bm_CKSTRING_p, ))
"""
BMObject_GetName

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_name: Direction: output. 
:type out_name: bm_CKSTRING_p (LibCmo::CKSTRING in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMObject_SetName = _create_bmap_func('BMObject_SetName', (bm_void_p, bm_CKID, bm_CKSTRING, ))
"""
BMObject_SetName

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param name: Direction: input. 
:type name: bm_CKSTRING (LibCmo::CKSTRING in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMGroup_AddObject = _create_bmap_func('BMGroup_AddObject', (bm_void_p, bm_CKID, bm_CKID, ))
"""
BMGroup_AddObject

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param memberid: Direction: input. 
:type memberid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMGroup_GetObjectCount = _create_bmap_func('BMGroup_GetObjectCount', (bm_void_p, bm_CKID, bm_CKDWORD_p, ))
"""
BMGroup_GetObjectCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_count: Direction: output. 
:type out_count: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMGroup_GetObject = _create_bmap_func('BMGroup_GetObject', (bm_void_p, bm_CKID, bm_CKDWORD, bm_CKID_p, ))
"""
BMGroup_GetObject

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param pos: Direction: input. 
:type pos: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param out_objid: Direction: output. 
:type out_objid: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMTexture_GetFileName = _create_bmap_func('BMTexture_GetFileName', (bm_void_p, bm_CKID, bm_CKSTRING_p, ))
"""
BMTexture_GetFileName

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_filename: Direction: output. 
:type out_filename: bm_CKSTRING_p (LibCmo::CKSTRING in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMTexture_LoadImage = _create_bmap_func('BMTexture_LoadImage', (bm_void_p, bm_CKID, bm_CKSTRING, ))
"""
BMTexture_LoadImage

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param filename: Direction: input. 
:type filename: bm_CKSTRING (LibCmo::CKSTRING in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMTexture_SaveImage = _create_bmap_func('BMTexture_SaveImage', (bm_void_p, bm_CKID, bm_CKSTRING, ))
"""
BMTexture_SaveImage

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param filename: Direction: input. 
:type filename: bm_CKSTRING (LibCmo::CKSTRING in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMTexture_GetSaveOptions = _create_bmap_func('BMTexture_GetSaveOptions', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMTexture_GetSaveOptions

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_saveopt: Direction: output. 
:type out_saveopt: bm_enum_p (LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMTexture_SetSaveOptions = _create_bmap_func('BMTexture_SetSaveOptions', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMTexture_SetSaveOptions

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param saveopt: Direction: input. 
:type saveopt: bm_enum (LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMTexture_GetVideoFormat = _create_bmap_func('BMTexture_GetVideoFormat', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMTexture_GetVideoFormat

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_vfmt: Direction: output. 
:type out_vfmt: bm_enum_p (LibCmo::VxMath::VX_PIXELFORMAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMTexture_SetVideoFormat = _create_bmap_func('BMTexture_SetVideoFormat', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMTexture_SetVideoFormat

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param vfmt: Direction: input. 
:type vfmt: bm_enum (LibCmo::VxMath::VX_PIXELFORMAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetDiffuse = _create_bmap_func('BMMaterial_GetDiffuse', (bm_void_p, bm_CKID, bm_VxColor_p, ))
"""
BMMaterial_GetDiffuse

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_VxColor_p (LibCmo::VxMath::VxColor in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetDiffuse = _create_bmap_func('BMMaterial_SetDiffuse', (bm_void_p, bm_CKID, bm_VxColor, ))
"""
BMMaterial_SetDiffuse

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param col: Direction: input. 
:type col: bm_VxColor (LibCmo::VxMath::VxColor in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetAmbient = _create_bmap_func('BMMaterial_GetAmbient', (bm_void_p, bm_CKID, bm_VxColor_p, ))
"""
BMMaterial_GetAmbient

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_VxColor_p (LibCmo::VxMath::VxColor in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetAmbient = _create_bmap_func('BMMaterial_SetAmbient', (bm_void_p, bm_CKID, bm_VxColor, ))
"""
BMMaterial_SetAmbient

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param col: Direction: input. 
:type col: bm_VxColor (LibCmo::VxMath::VxColor in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetSpecular = _create_bmap_func('BMMaterial_GetSpecular', (bm_void_p, bm_CKID, bm_VxColor_p, ))
"""
BMMaterial_GetSpecular

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_VxColor_p (LibCmo::VxMath::VxColor in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetSpecular = _create_bmap_func('BMMaterial_SetSpecular', (bm_void_p, bm_CKID, bm_VxColor, ))
"""
BMMaterial_SetSpecular

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param col: Direction: input. 
:type col: bm_VxColor (LibCmo::VxMath::VxColor in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetEmissive = _create_bmap_func('BMMaterial_GetEmissive', (bm_void_p, bm_CKID, bm_VxColor_p, ))
"""
BMMaterial_GetEmissive

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_VxColor_p (LibCmo::VxMath::VxColor in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetEmissive = _create_bmap_func('BMMaterial_SetEmissive', (bm_void_p, bm_CKID, bm_VxColor, ))
"""
BMMaterial_SetEmissive

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param col: Direction: input. 
:type col: bm_VxColor (LibCmo::VxMath::VxColor in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetSpecularPower = _create_bmap_func('BMMaterial_GetSpecularPower', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMMaterial_GetSpecularPower

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetSpecularPower = _create_bmap_func('BMMaterial_SetSpecularPower', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMMaterial_SetSpecularPower

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetTexture = _create_bmap_func('BMMaterial_GetTexture', (bm_void_p, bm_CKID, bm_CKID_p, ))
"""
BMMaterial_GetTexture

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_texid: Direction: output. 
:type out_texid: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetTexture = _create_bmap_func('BMMaterial_SetTexture', (bm_void_p, bm_CKID, bm_CKID, ))
"""
BMMaterial_SetTexture

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param texid: Direction: input. 
:type texid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetTextureBorderColor = _create_bmap_func('BMMaterial_GetTextureBorderColor', (bm_void_p, bm_CKID, bm_CKDWORD_p, ))
"""
BMMaterial_GetTextureBorderColor

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetTextureBorderColor = _create_bmap_func('BMMaterial_SetTextureBorderColor', (bm_void_p, bm_CKID, bm_CKDWORD, ))
"""
BMMaterial_SetTextureBorderColor

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetTextureBlendMode = _create_bmap_func('BMMaterial_GetTextureBlendMode', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMMaterial_GetTextureBlendMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::VxMath::VXTEXTURE_BLENDMODE in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetTextureBlendMode = _create_bmap_func('BMMaterial_SetTextureBlendMode', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMMaterial_SetTextureBlendMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::VxMath::VXTEXTURE_BLENDMODE in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetTextureMinMode = _create_bmap_func('BMMaterial_GetTextureMinMode', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMMaterial_GetTextureMinMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::VxMath::VXTEXTURE_FILTERMODE in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetTextureMinMode = _create_bmap_func('BMMaterial_SetTextureMinMode', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMMaterial_SetTextureMinMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::VxMath::VXTEXTURE_FILTERMODE in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetTextureMagMode = _create_bmap_func('BMMaterial_GetTextureMagMode', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMMaterial_GetTextureMagMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::VxMath::VXTEXTURE_FILTERMODE in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetTextureMagMode = _create_bmap_func('BMMaterial_SetTextureMagMode', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMMaterial_SetTextureMagMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::VxMath::VXTEXTURE_FILTERMODE in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetTextureAddressMode = _create_bmap_func('BMMaterial_GetTextureAddressMode', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMMaterial_GetTextureAddressMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::VxMath::VXTEXTURE_ADDRESSMODE in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetTextureAddressMode = _create_bmap_func('BMMaterial_SetTextureAddressMode', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMMaterial_SetTextureAddressMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::VxMath::VXTEXTURE_ADDRESSMODE in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetSourceBlend = _create_bmap_func('BMMaterial_GetSourceBlend', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMMaterial_GetSourceBlend

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::VxMath::VXBLEND_MODE in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetSourceBlend = _create_bmap_func('BMMaterial_SetSourceBlend', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMMaterial_SetSourceBlend

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::VxMath::VXBLEND_MODE in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetDestBlend = _create_bmap_func('BMMaterial_GetDestBlend', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMMaterial_GetDestBlend

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::VxMath::VXBLEND_MODE in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetDestBlend = _create_bmap_func('BMMaterial_SetDestBlend', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMMaterial_SetDestBlend

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::VxMath::VXBLEND_MODE in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetFillMode = _create_bmap_func('BMMaterial_GetFillMode', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMMaterial_GetFillMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::VxMath::VXFILL_MODE in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetFillMode = _create_bmap_func('BMMaterial_SetFillMode', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMMaterial_SetFillMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::VxMath::VXFILL_MODE in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetShadeMode = _create_bmap_func('BMMaterial_GetShadeMode', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMMaterial_GetShadeMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::VxMath::VXSHADE_MODE in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetShadeMode = _create_bmap_func('BMMaterial_SetShadeMode', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMMaterial_SetShadeMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::VxMath::VXSHADE_MODE in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetAlphaTestEnabled = _create_bmap_func('BMMaterial_GetAlphaTestEnabled', (bm_void_p, bm_CKID, bm_bool_p, ))
"""
BMMaterial_GetAlphaTestEnabled

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_bool_p (bool in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetAlphaTestEnabled = _create_bmap_func('BMMaterial_SetAlphaTestEnabled', (bm_void_p, bm_CKID, bm_bool, ))
"""
BMMaterial_SetAlphaTestEnabled

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param enabled: Direction: input. 
:type enabled: bm_bool (bool in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetAlphaBlendEnabled = _create_bmap_func('BMMaterial_GetAlphaBlendEnabled', (bm_void_p, bm_CKID, bm_bool_p, ))
"""
BMMaterial_GetAlphaBlendEnabled

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_bool_p (bool in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetAlphaBlendEnabled = _create_bmap_func('BMMaterial_SetAlphaBlendEnabled', (bm_void_p, bm_CKID, bm_bool, ))
"""
BMMaterial_SetAlphaBlendEnabled

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param enabled: Direction: input. 
:type enabled: bm_bool (bool in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetPerspectiveCorrectionEnabled = _create_bmap_func('BMMaterial_GetPerspectiveCorrectionEnabled', (bm_void_p, bm_CKID, bm_bool_p, ))
"""
BMMaterial_GetPerspectiveCorrectionEnabled

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_bool_p (bool in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetPerspectiveCorrectionEnabled = _create_bmap_func('BMMaterial_SetPerspectiveCorrectionEnabled', (bm_void_p, bm_CKID, bm_bool, ))
"""
BMMaterial_SetPerspectiveCorrectionEnabled

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param enabled: Direction: input. 
:type enabled: bm_bool (bool in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetZWriteEnabled = _create_bmap_func('BMMaterial_GetZWriteEnabled', (bm_void_p, bm_CKID, bm_bool_p, ))
"""
BMMaterial_GetZWriteEnabled

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_bool_p (bool in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetZWriteEnabled = _create_bmap_func('BMMaterial_SetZWriteEnabled', (bm_void_p, bm_CKID, bm_bool, ))
"""
BMMaterial_SetZWriteEnabled

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param enabled: Direction: input. 
:type enabled: bm_bool (bool in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetTwoSidedEnabled = _create_bmap_func('BMMaterial_GetTwoSidedEnabled', (bm_void_p, bm_CKID, bm_bool_p, ))
"""
BMMaterial_GetTwoSidedEnabled

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_bool_p (bool in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetTwoSidedEnabled = _create_bmap_func('BMMaterial_SetTwoSidedEnabled', (bm_void_p, bm_CKID, bm_bool, ))
"""
BMMaterial_SetTwoSidedEnabled

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param enabled: Direction: input. 
:type enabled: bm_bool (bool in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetAlphaRef = _create_bmap_func('BMMaterial_GetAlphaRef', (bm_void_p, bm_CKID, bm_CKBYTE_p, ))
"""
BMMaterial_GetAlphaRef

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKBYTE_p (LibCmo::CKBYTE in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetAlphaRef = _create_bmap_func('BMMaterial_SetAlphaRef', (bm_void_p, bm_CKID, bm_CKBYTE, ))
"""
BMMaterial_SetAlphaRef

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKBYTE (LibCmo::CKBYTE in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetAlphaFunc = _create_bmap_func('BMMaterial_GetAlphaFunc', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMMaterial_GetAlphaFunc

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::VxMath::VXCMPFUNC in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetAlphaFunc = _create_bmap_func('BMMaterial_SetAlphaFunc', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMMaterial_SetAlphaFunc

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::VxMath::VXCMPFUNC in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_GetZFunc = _create_bmap_func('BMMaterial_GetZFunc', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMMaterial_GetZFunc

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::VxMath::VXCMPFUNC in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMaterial_SetZFunc = _create_bmap_func('BMMaterial_SetZFunc', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMMaterial_SetZFunc

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::VxMath::VXCMPFUNC in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_GetLitMode = _create_bmap_func('BMMesh_GetLitMode', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMMesh_GetLitMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_mode: Direction: output. 
:type out_mode: bm_enum_p (LibCmo::VxMath::VXMESH_LITMODE in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_SetLitMode = _create_bmap_func('BMMesh_SetLitMode', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMMesh_SetLitMode

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param mode: Direction: input. 
:type mode: bm_enum (LibCmo::VxMath::VXMESH_LITMODE in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_GetVertexCount = _create_bmap_func('BMMesh_GetVertexCount', (bm_void_p, bm_CKID, bm_CKDWORD_p, ))
"""
BMMesh_GetVertexCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_count: Direction: output. 
:type out_count: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_SetVertexCount = _create_bmap_func('BMMesh_SetVertexCount', (bm_void_p, bm_CKID, bm_CKDWORD, ))
"""
BMMesh_SetVertexCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param count: Direction: input. 
:type count: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_GetVertexPositions = _create_bmap_func('BMMesh_GetVertexPositions', (bm_void_p, bm_CKID, bm_VxVector3_pp, ))
"""
BMMesh_GetVertexPositions

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_VxVector3_pp (LibCmo::VxMath::VxVector3* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_GetVertexNormals = _create_bmap_func('BMMesh_GetVertexNormals', (bm_void_p, bm_CKID, bm_VxVector3_pp, ))
"""
BMMesh_GetVertexNormals

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_VxVector3_pp (LibCmo::VxMath::VxVector3* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_GetVertexUVs = _create_bmap_func('BMMesh_GetVertexUVs', (bm_void_p, bm_CKID, bm_VxVector2_pp, ))
"""
BMMesh_GetVertexUVs

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_VxVector2_pp (LibCmo::VxMath::VxVector2* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_GetFaceCount = _create_bmap_func('BMMesh_GetFaceCount', (bm_void_p, bm_CKID, bm_CKDWORD_p, ))
"""
BMMesh_GetFaceCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_count: Direction: output. 
:type out_count: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_SetFaceCount = _create_bmap_func('BMMesh_SetFaceCount', (bm_void_p, bm_CKID, bm_CKDWORD, ))
"""
BMMesh_SetFaceCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param count: Direction: input. 
:type count: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_GetFaceIndices = _create_bmap_func('BMMesh_GetFaceIndices', (bm_void_p, bm_CKID, bm_CKWORD_pp, ))
"""
BMMesh_GetFaceIndices

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_CKWORD_pp (LibCmo::CKWORD* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_GetFaceMaterialSlotIndexs = _create_bmap_func('BMMesh_GetFaceMaterialSlotIndexs', (bm_void_p, bm_CKID, bm_CKWORD_pp, ))
"""
BMMesh_GetFaceMaterialSlotIndexs

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_mem: Direction: output. 
:type out_mem: bm_CKWORD_pp (LibCmo::CKWORD* in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_GetMaterialSlotCount = _create_bmap_func('BMMesh_GetMaterialSlotCount', (bm_void_p, bm_CKID, bm_CKDWORD_p, ))
"""
BMMesh_GetMaterialSlotCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_count: Direction: output. 
:type out_count: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_SetMaterialSlotCount = _create_bmap_func('BMMesh_SetMaterialSlotCount', (bm_void_p, bm_CKID, bm_CKDWORD, ))
"""
BMMesh_SetMaterialSlotCount

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param count: Direction: input. 
:type count: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_GetMaterialSlot = _create_bmap_func('BMMesh_GetMaterialSlot', (bm_void_p, bm_CKID, bm_CKDWORD, bm_CKID_p, ))
"""
BMMesh_GetMaterialSlot

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param index: Direction: input. 
:type index: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param out_mtlid: Direction: output. 
:type out_mtlid: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMMesh_SetMaterialSlot = _create_bmap_func('BMMesh_SetMaterialSlot', (bm_void_p, bm_CKID, bm_CKDWORD, bm_CKID, ))
"""
BMMesh_SetMaterialSlot

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param index: Direction: input. 
:type index: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param mtlid: Direction: input. 
:type mtlid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BM3dEntity_GetWorldMatrix = _create_bmap_func('BM3dEntity_GetWorldMatrix', (bm_void_p, bm_CKID, bm_VxMatrix_p, ))
"""
BM3dEntity_GetWorldMatrix

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_mat: Direction: output. 
:type out_mat: bm_VxMatrix_p (LibCmo::VxMath::VxMatrix in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BM3dEntity_SetWorldMatrix = _create_bmap_func('BM3dEntity_SetWorldMatrix', (bm_void_p, bm_CKID, bm_VxMatrix, ))
"""
BM3dEntity_SetWorldMatrix

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param mat: Direction: input. 
:type mat: bm_VxMatrix (LibCmo::VxMath::VxMatrix in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BM3dEntity_GetCurrentMesh = _create_bmap_func('BM3dEntity_GetCurrentMesh', (bm_void_p, bm_CKID, bm_CKID_p, ))
"""
BM3dEntity_GetCurrentMesh

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_meshid: Direction: output. 
:type out_meshid: bm_CKID_p (LibCmo::CK2::CK_ID in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BM3dEntity_SetCurrentMesh = _create_bmap_func('BM3dEntity_SetCurrentMesh', (bm_void_p, bm_CKID, bm_CKID, ))
"""
BM3dEntity_SetCurrentMesh

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param meshid: Direction: input. 
:type meshid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BM3dEntity_GetVisibility = _create_bmap_func('BM3dEntity_GetVisibility', (bm_void_p, bm_CKID, bm_bool_p, ))
"""
BM3dEntity_GetVisibility

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_isVisible: Direction: output. 
:type out_isVisible: bm_bool_p (bool in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BM3dEntity_SetVisibility = _create_bmap_func('BM3dEntity_SetVisibility', (bm_void_p, bm_CKID, bm_bool, ))
"""
BM3dEntity_SetVisibility

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param is_visible: Direction: input. 
:type is_visible: bm_bool (bool in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_GetType = _create_bmap_func('BMLight_GetType', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMLight_GetType

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::VxMath::VXLIGHT_TYPE in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_SetType = _create_bmap_func('BMLight_SetType', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMLight_SetType

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::VxMath::VXLIGHT_TYPE in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_GetColor = _create_bmap_func('BMLight_GetColor', (bm_void_p, bm_CKID, bm_VxColor_p, ))
"""
BMLight_GetColor

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_VxColor_p (LibCmo::VxMath::VxColor in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_SetColor = _create_bmap_func('BMLight_SetColor', (bm_void_p, bm_CKID, bm_VxColor, ))
"""
BMLight_SetColor

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param col: Direction: input. 
:type col: bm_VxColor (LibCmo::VxMath::VxColor in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_GetConstantAttenuation = _create_bmap_func('BMLight_GetConstantAttenuation', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMLight_GetConstantAttenuation

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_SetConstantAttenuation = _create_bmap_func('BMLight_SetConstantAttenuation', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMLight_SetConstantAttenuation

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_GetLinearAttenuation = _create_bmap_func('BMLight_GetLinearAttenuation', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMLight_GetLinearAttenuation

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_SetLinearAttenuation = _create_bmap_func('BMLight_SetLinearAttenuation', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMLight_SetLinearAttenuation

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_GetQuadraticAttenuation = _create_bmap_func('BMLight_GetQuadraticAttenuation', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMLight_GetQuadraticAttenuation

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_SetQuadraticAttenuation = _create_bmap_func('BMLight_SetQuadraticAttenuation', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMLight_SetQuadraticAttenuation

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_GetRange = _create_bmap_func('BMLight_GetRange', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMLight_GetRange

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_SetRange = _create_bmap_func('BMLight_SetRange', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMLight_SetRange

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_GetHotSpot = _create_bmap_func('BMLight_GetHotSpot', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMLight_GetHotSpot

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_SetHotSpot = _create_bmap_func('BMLight_SetHotSpot', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMLight_SetHotSpot

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_GetFalloff = _create_bmap_func('BMLight_GetFalloff', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMLight_GetFalloff

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_SetFalloff = _create_bmap_func('BMLight_SetFalloff', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMLight_SetFalloff

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_GetFalloffShape = _create_bmap_func('BMLight_GetFalloffShape', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMLight_GetFalloffShape

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMLight_SetFalloffShape = _create_bmap_func('BMLight_SetFalloffShape', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMLight_SetFalloffShape

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_GetProjectionType = _create_bmap_func('BMCamera_GetProjectionType', (bm_void_p, bm_CKID, bm_enum_p, ))
"""
BMCamera_GetProjectionType

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_enum_p (LibCmo::CK2::CK_CAMERA_PROJECTION in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_SetProjectionType = _create_bmap_func('BMCamera_SetProjectionType', (bm_void_p, bm_CKID, bm_enum, ))
"""
BMCamera_SetProjectionType

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_enum (LibCmo::CK2::CK_CAMERA_PROJECTION in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_GetOrthographicZoom = _create_bmap_func('BMCamera_GetOrthographicZoom', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMCamera_GetOrthographicZoom

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_SetOrthographicZoom = _create_bmap_func('BMCamera_SetOrthographicZoom', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMCamera_SetOrthographicZoom

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_GetFrontPlane = _create_bmap_func('BMCamera_GetFrontPlane', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMCamera_GetFrontPlane

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_SetFrontPlane = _create_bmap_func('BMCamera_SetFrontPlane', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMCamera_SetFrontPlane

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_GetBackPlane = _create_bmap_func('BMCamera_GetBackPlane', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMCamera_GetBackPlane

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_SetBackPlane = _create_bmap_func('BMCamera_SetBackPlane', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMCamera_SetBackPlane

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_GetFov = _create_bmap_func('BMCamera_GetFov', (bm_void_p, bm_CKID, bm_CKFLOAT_p, ))
"""
BMCamera_GetFov

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_val: Direction: output. 
:type out_val: bm_CKFLOAT_p (LibCmo::CKFLOAT in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_SetFov = _create_bmap_func('BMCamera_SetFov', (bm_void_p, bm_CKID, bm_CKFLOAT, ))
"""
BMCamera_SetFov

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param val: Direction: input. 
:type val: bm_CKFLOAT (LibCmo::CKFLOAT in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_GetAspectRatio = _create_bmap_func('BMCamera_GetAspectRatio', (bm_void_p, bm_CKID, bm_CKDWORD_p, bm_CKDWORD_p, ))
"""
BMCamera_GetAspectRatio

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param out_width: Direction: output. 
:type out_width: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:param out_height: Direction: output. 
:type out_height: bm_CKDWORD_p (LibCmo::CKDWORD in C++). Use ctypes.byref(data) pass it.
:return: True if no error, otherwise False.
:rtype: bool
"""
BMCamera_SetAspectRatio = _create_bmap_func('BMCamera_SetAspectRatio', (bm_void_p, bm_CKID, bm_CKDWORD, bm_CKDWORD, ))
"""
BMCamera_SetAspectRatio

:param bmfile: Direction: input. The pointer to corresponding BMFile.
:type bmfile: bm_void_p (BMap::BMFile* in C++). 
:param objid: Direction: input. The CKID of object you accessing.
:type objid: bm_CKID (LibCmo::CK2::CK_ID in C++). 
:param width: Direction: input. 
:type width: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:param height: Direction: input. 
:type height: bm_CKDWORD (LibCmo::CKDWORD in C++). 
:return: True if no error, otherwise False.
:rtype: bool
"""

##### GENERATED FUNCTIONS END #####

#endregion
