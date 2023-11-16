import ctypes, os, sys

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

_g_BMapModule: ctypes.CDLL = None
try:
    _g_BMapModule = ctypes.cdll.LoadLibrary(
        os.path.join(os.path.dirname(__file__), _g_BMapLibName)
    )
except:
    _g_BMapModule = None

def is_bmap_available() -> bool:
    return _g_BMapModule is not None

def _bmap_error_check(result: bm_bool, func, args):
    if not result:
        raise BMapException("BMap operation failed.")
    return result

def _create_bmap_func(fct_name: str, fct_params: list[ctypes._SimpleCData]) -> ctypes._CFuncPtr:
    if _g_BMapModule is None: return None
    
    cache: ctypes._CFuncPtr = getattr(_g_BMapModule, fct_name)
    cache.argtypes = fct_params
    cache.restype = bm_bool
    cache.errcheck = _bmap_error_check
    return cache

#endregion
