import ctypes, os

#region Type Defines

bm_CKSTRING = ctypes.c_char_p
bm_CKSTRING_p = ctypes.POINTER(bm_CKSTRING)
bm_CKDWORD = ctypes.c_uint32
bm_CKDWORD_p = ctypes.POINTER(bm_CKDWORD)
bm_CKWORD = ctypes.c_uint16
bm_CKWORD_p = ctypes.POINTER(bm_CKWORD)
bm_CKID = ctypes.c_uint32
bm_CKID_p = ctypes.POINTER(bm_CKID)
bm_CKFLOAT = ctypes.c_float
bm_CKFLOAT_p = ctypes.POINTER(bm_CKFLOAT)
bm_CKINT = types.c_int32

bm_enum = bm_CKDWORD
bm_bool = ctypes.c_bool
bm_void_p = ctypes.c_void_p

class bm_VxVector2(ctypes.Structure):
    _fields_ = [
        ('x', bm_CKFLOAT),
        ('y', bm_CKFLOAT),
    ]
bm_VxVector2_p = ctypes.POINTER(bm_VxVector2)
class bm_VxVector3(ctypes.Structure):
    _fields_ = [
        ('x', bm_CKFLOAT),
        ('y', bm_CKFLOAT),
        ('z', bm_CKFLOAT),
    ]
bm_VxVector3_p = ctypes.POINTER(bm_VxVector3)

class bm_VxMatrix(ctypes.Structure):
    _fields_ = list(
        (f'i{idx}', bm_CKFLOAT) for idx in range(16)
    )

#endregion

#region BMap Loader

_g_BMapModule: ctypes.CDLL = None
try:
    _g_BMapModule = ctypes.cdll.LoadLibrary(
        os.path.join(os.path.dirname(__file__), "BMap.dll")
    )
except:
    _g_BMapModule = None

def is_bmap_available() -> bool:
    return _g_BMapModule is not None

def _create_bmap_func(fct_name: str, fct_params: list) -> None:
    if _g_BMapModule is None: return None
    
    cache = getattr(_g_BMapModule, fct_name)
    cache.argtypes = fct_params
    cache.restype = bm_bool
    return cache

#endregion
