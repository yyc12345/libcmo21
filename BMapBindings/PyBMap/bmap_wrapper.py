import ctypes, typing, atexit
from . import bmap, virtools_types

#region Basic Class Defines

g_BMapEncoding: str = "utf-8"

class _AbstractPointer():
    __mRawPointer: int

    def __init__(self, raw_pointer: bmap.bm_void_p):
        if raw_pointer.value is None:
            self.__mRawPointer = 0
        else:
            self.__mRawPointer = raw_pointer.value

    def _is_valid(self) -> bool:
        return self.__mRawPointer != 0

    def _get_pointer(self) -> bmap.bm_void_p:
        return bmap.bm_void_p(self.__mRawPointer)

    def __eq__(self, obj: object) -> bool:
        if isinstance(obj, self.__class__):
            return obj.__mRawPointer == self.__mRawPointer
        else:
            return False

    def __hash__(self) -> int:
        return hash(self.__mRawPointer)

class _AbstractCKObject(_AbstractPointer):
    __mCKID: int

    def __init__(self, raw_pointer: bmap.bm_void_p, ckid: bmap.bm_CKID):
        _AbstractPointer.__init__(self, raw_pointer)
        self.__mCKID = ckid.value

    def _is_valid(self) -> bool:
        return _AbstractPointer._is_valid(self) and self.__mCKID != 0

    def _get_ckid(self) -> bmap.bm_CKID:
        return bmap.bm_CKID(self.__mCKID)

    def __eq__(self, obj: object) -> bool:
        if isinstance(obj, self.__class__):
            return obj.__mRawPointer == self.__mRawPointer and obj.__mCKID == self.__mCKID
        else:
            return False
        
    def __hash__(self) -> int:
        return hash((self.__mRawPointer, self.__mCKID))
    
#endregion

#region Valid Check, Init and Dispose

def is_bmap_available() -> bool:
    return bmap.is_bmap_available()

# init module self and register exit function
if is_bmap_available():
    bmap.BMInit()

    def _auto_exit():
        bmap.BMDispose()
    atexit.register(_auto_exit)

#endregion

#region Real Type Defines

class BMFileReader(_AbstractPointer):
    pass
    # write with `with` syntax
    
class BMFileWriter(_AbstractPointer):
    pass
    # write with `with` syntax
    
class BMMeshTrans(_AbstractPointer):
    pass
    # write with `with` syntax

class BMObject(_AbstractCKObject):
    def get_name(self) -> str:
        name: bmap.bm_CKSTRING = bmap.bm_CKSTRING()
        bmap.BMObject_GetName(self._get_pointer(), self._get_ckid(), ctypes.byref(name))
        return name.value.decode(g_BMapEncoding)

    def set_name(self, name: str) -> None:
        name: bmap.bm_CKSTRING = bmap.bm_CKSTRING(name.encode(g_BMapEncoding))
        bmap.BMObject_SetName(self._get_pointer(), self._get_ckid(), name)

class BMGroup(BMObject):
    pass

class BMTexture(BMObject):
    pass

class BMMaterial(BMObject):
    pass

class BMMesh(BMObject):
    pass

class BM3dObject(BMObject):
    pass

#endregion
