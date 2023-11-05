import ctypes, typing, atexit
import bmap, virtools_types

#region Basic Class Defines

g_InvalidPtr: bmap.bm_void_p = bmap.bm_void_p(0)
g_InvalidCKID: int = 0
g_BMapEncoding: str = "utf-8"

class _AbstractPointer():
    __mRawPointer: int

    def __init__(self, raw_pointer: bmap.bm_void_p):
        self._set_pointer(raw_pointer)

    def _is_valid(self) -> bool:
        return self.__mRawPointer != 0

    def _get_pointer(self) -> bmap.bm_void_p:
        return bmap.bm_void_p(self.__mRawPointer)
    
    def _set_pointer(self, raw_pointer: bmap.bm_void_p):
        if raw_pointer.value is None:
            self.__mRawPointer = 0
        else:
            self.__mRawPointer = raw_pointer.value

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

TCKObj = typing.TypeVar('TCKObj', bound = _AbstractCKObject)

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

"""!
BMFileReader, BMFileWriter, and BMMeshTrans can be create by given constructor.
But they must be destroyed by calling dispose(). Otherwise it may cause memory leak.
You also can use python `with` statement to achieve this automatically.

BMObject, BMTexture, BMMaterial, BMMesh, and BM3dObject should NOT be constructed from given constructor.
They must be obtained from BMFileReader, BMFileWriter, and BMMeshTrans.
Thus BMObject, BMTexture, BMMaterial, BMMesh, and BM3dObject also do not need to free 
because these resources are sotred in BMFileReader, BMFileWriter, and BMMeshTrans.
We just provide them as a visitor.
"""

class BMObject(_AbstractCKObject):
    def get_name(self) -> str | None:
        name: bmap.bm_CKSTRING = bmap.bm_CKSTRING()
        bmap.BMObject_GetName(self._get_pointer(), self._get_ckid(), ctypes.byref(name))
        if name.value is None:
            return None
        else:
            return name.value.decode(g_BMapEncoding)

    def set_name(self, name: str | None) -> None:
        name: bmap.bm_CKSTRING
        if name is None:
            name = bmap.bm_CKSTRING(0)
        else:
            name = bmap.bm_CKSTRING(name.encode(g_BMapEncoding))
        bmap.BMObject_SetName(self._get_pointer(), self._get_ckid(), name)

class BMTexture(BMObject):
    pass

class BMMaterial(BMObject):
    pass

class BMMesh(BMObject):
    pass

class BM3dObject(BMObject):
    def get_world_matrix(self) -> virtools_types.ConstVxMatrix:
        mat: bmap.bm_VxMatrix = bmap.bm_VxMatrix()
        bmap.BM3dObject_GetWorldMatrix(self._get_pointer(), self._get_ckid(), ctypes.byref(mat))
        # use cast & pointer to get matrix data conveniently
        flat: bmap.bm_CKFLOAT_p = ctypes.cast(ctypes.byref(mat), bmap.bm_CKFLOAT_p)
        return tuple(flat[i] for i in range(16))

    def set_world_matrix(self, mat: virtools_types.ConstVxMatrix) -> None:
        # star syntax expand the tuple as the argument.
        mat: bmap.bm_VxMatrix = bmap.bm_VxMatrix(*mat)
        bmap.BM3dObject_SetWorldMatrix(self._get_pointer(), self._get_ckid(), mat)

    def get_current_mesh(self) -> BMMesh | None:
        ckid: bmap.bm_CKID = bmap.bm_CKID()
        bmap.BM3dObject_GetCurrentMesh(self._get_pointer(), self._get_ckid(), ctypes.byref(ckid))
        if ckid.value == g_InvalidCKID:
            return None
        else:
            return BMMesh(self._get_pointer(), ckid)

    def set_current_mesh(self, mesh: BMMesh | None) -> None:
        ckid: bmap.bm_CKID
        if mesh is None:
            ckid = bmap.bm_CKID(g_InvalidCKID)
        else:
            ckid = bmap.bm_CKID(mesh._get_ckid())
        bmap.BM3dObject_SetCurrentMesh(self._get_pointer(), self._get_ckid(), ckid)

    def get_visibility(self) -> bool:
        visb: bmap.bm_bool = bmap.bm_bool()
        bmap.BM3dObject_GetVisibility(self._get_pointer(), self._get_ckid(), ctypes.byref(visb))
        return visb.value

    def set_visibility(self, visb_: bool) -> None:
        visb: bmap.bm_bool = bmap.bm_bool(visb_)
        bmap.BM3dObject_SetVisibility(self._get_pointer(), self._get_ckid(), visb)

class BMGroup(BMObject):
    def add_object(self, member: BM3dObject) -> None:
        bmap.BMGroup_AddObject(self._get_pointer(), self._get_ckid(), member._get_ckid())

    def iterate_objects(self) -> typing.Iterator[BM3dObject]:
        # get count
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        bmap.BMGroup_GetObjectCount(self._get_pointer(), self._get_ckid(), ctypes.byref(csize))

        # iterate list
        cidx: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        retid: bmap.bm_CKID = bmap.bm_CKID()
        for i in range(csize.value):
            cidx.value = i
            bmap.BMGroup_GetObject(self._get_pointer(), self._get_ckid(), cidx, ctypes.byref(retid))
            # return visitor
            yield BM3dObject(self._get_pointer(), retid)
            
class BMFileReader(_AbstractPointer):
    def __init__(self, file_name_: str, temp_folder_: str, texture_folder_: str, encodings_: tuple[str]):
        # create param
        file_name: bmap.bm_CKSTRING = bmap.bm_CKSTRING(file_name_.encode(g_BMapEncoding))
        temp_folder: bmap.bm_CKSTRING = bmap.bm_CKSTRING(temp_folder_.encode(g_BMapEncoding))
        texture_folder: bmap.bm_CKSTRING = bmap.bm_CKSTRING(texture_folder_.encode(g_BMapEncoding))
        encoding_count: bmap.bm_CKDWORD = bmap.bm_CKDWORD(len(encodings_))
        encodings: ctypes.Array = (bmap.bm_CKSTRING * len(encodings_))(
            *(strl.encode(g_BMapEncoding) for strl in encodings_)
        )
        out_file: bmap.bm_void_p = bmap.bm_void_p()

        # exec
        bmap.BMFile_Load(
            file_name, temp_folder, texture_folder, encoding_count, encodings,
            ctypes.byref(out_file)
        )

        # init self
        _AbstractPointer.__init__(self, out_file)

    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_value, traceback):
        self.dispose()
    
    def dispose(self) -> None:
        if self._is_valid():
            bmap.BMFile_Free(self._get_pointer())
            self._set_pointer(g_InvalidPtr)

    def __iterate_ckobjects(self, 
        class_type: type[TCKObj],
        count_getter: typing.Callable[[bmap.bm_void_p, bmap.bm_CKDWORD_p], bool],
        obj_getter: typing.Callable[[bmap.bm_void_p, bmap.bm_CKDWORD, bmap.bm_CKID_p], bool]) -> typing.Iterator[TCKObj]:
        # get size first
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        count_getter(self._get_pointer(), ctypes.byref(csize))

        # iterate list
        cidx: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        retid: bmap.bm_CKID = bmap.bm_CKID()
        for i in range(csize.value):
            cidx.value = i
            obj_getter(self._get_pointer(), cidx, ctypes.byref(retid))
            # yield return constructed obj visitor
            yield class_type(self._get_pointer(), retid)

    def iterate_textures(self) -> typing.Iterator[BMTexture]:
        return self.__iterate_ckobjects(
            BMTexture,
            bmap.BMFile_GetTextureCount,
            bmap.BMFile_GetTexture
        )
    
    def iterate_materials(self) -> typing.Iterator[BMMaterial]:
        return self.__iterate_ckobjects(
            BMMaterial,
            bmap.BMFile_GetMaterialCount,
            bmap.BMFile_GetMaterial
        )
    
    def iterate_meshs(self) -> typing.Iterator[BMMesh]:
        return self.__iterate_ckobjects(
            BMMesh,
            bmap.BMFile_GetMeshCount,
            bmap.BMFile_GetMesh
        )
    
    def iterate_3dobjects(self) -> typing.Iterator[BM3dObject]:
        return self.__iterate_ckobjects(
            BM3dObject,
            bmap.BMFile_Get3dObjectCount,
            bmap.BMFile_Get3dObject
        )
    
    def iterate_groups(self) -> typing.Iterator[BMGroup]:
        return self.__iterate_ckobjects(
            BMGroup,
            bmap.BMFile_GetGroupCount,
            bmap.BMFile_GetGroup
        )
    
class BMFileWriter(_AbstractPointer):
    def __init__(self, temp_folder_: str, texture_folder_: str, encodings_: tuple[str]):
        # create param
        temp_folder: bmap.bm_CKSTRING = bmap.bm_CKSTRING(temp_folder_.encode(g_BMapEncoding))
        texture_folder: bmap.bm_CKSTRING = bmap.bm_CKSTRING(texture_folder_.encode(g_BMapEncoding))
        encoding_count: bmap.bm_CKDWORD = bmap.bm_CKDWORD(len(encodings_))
        encodings: ctypes.Array = (bmap.bm_CKSTRING * len(encodings_))(
            *(strl.encode(g_BMapEncoding) for strl in encodings_)
        )
        out_file: bmap.bm_void_p = bmap.bm_void_p()

        # exec
        bmap.BMFile_Create(
            temp_folder, texture_folder, encoding_count, encodings,
            ctypes.byref(out_file)
        )

        # init self
        _AbstractPointer.__init__(self, out_file)

    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_value, traceback):
        self.dispose()
    
    def save(self, file_name_: str, compress_level_: int) -> None:
        # create param
        file_name: bmap.bm_CKSTRING = bmap.bm_CKSTRING(file_name_.encode(g_BMapEncoding))
        compress_level: bmap.bm_CKINT = bmap.bm_CKINT(compress_level_)
        # exec
        bmap.BMFile_Save(self._get_pointer(), file_name, compress_level)

    def dispose(self) -> None:
        if self._is_valid():
            bmap.BMFile_Free(self._get_pointer())
            self._set_pointer(g_InvalidPtr)

    def __create_ckobject(self, 
        class_type: type[TCKObj],
        creator: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID_p], bool]) -> TCKObj:
        # prepare id container
        retid: bmap.bm_CKID = bmap.bm_CKID()
        # create new one
        creator(self._get_pointer(), ctypes.byref(retid))
        # return visitor
        return class_type(self._get_pointer(), retid)

    def create_texture(self) -> BMTexture:
        self.__create_ckobject(
            BMTexture,
            bmap.BMFile_CreateTexture
        )
    
    def create_material(self) -> BMMaterial:
        self.__create_ckobject(
            BMMaterial,
            bmap.BMFile_CreateMaterial
        )
    
    def create_mesh(self) -> BMMesh:
        self.__create_ckobject(
            BMMesh,
            bmap.BMFile_CreateMesh
        )
    
    def create_3dobject(self) -> BM3dObject:
        self.__create_ckobject(
            BM3dObject,
            bmap.BMFile_Create3dObject
        )
    
    def create_group(self) -> BMGroup:
        self.__create_ckobject(
            BMGroup,
            bmap.BMFile_CreateGroup
        )
    
class BMMeshTrans(_AbstractPointer):
    def __init__(self):
        ptr: bmap.bm_void_p = bmap.bm_void_p()
        bmap.BMMeshTrans_New(ctypes.byref(ptr))
        _AbstractPointer.__init__(self, ptr)
        
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_value, traceback):
        self.dispose()
    
    def dispose(self) -> None:
        if self._is_valid():
            bmap.BMMeshTrans_Delete(self._get_pointer())
            self._set_pointer(g_InvalidPtr)
    
#endregion
