import ctypes, typing, atexit
from . import bmap, virtools_types

#region Basic Class & Constant Defines

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

#endregion

#region Help Function & Type Define

TCKObj = typing.TypeVar('TCKObj', bound = _AbstractCKObject)

def _vxvector3_assigner(pvector: bmap.bm_VxVector3_p, count: int, itor: typing.Iterator[virtools_types.VxVector3]) -> None:
    pfloat: bmap.bm_CKFLOAT_p = ctypes.cast(pvector, bmap.bm_CKFLOAT_p)
    idx: int = 0
    for _ in range(count):
        uservector: virtools_types.VxVector3 = next(itor)
        pfloat[idx] = uservector.x
        pfloat[idx + 1] = uservector.y
        pfloat[idx + 2] = uservector.z
        idx += 3

def _vxvector3_iterator(pvector: bmap.bm_VxVector3_p, count: int) -> typing.Iterator[virtools_types.VxVector3]:
    ret: virtools_types.VxVector3 = virtools_types.VxVector3()
    pfloat: bmap.bm_CKFLOAT_p = ctypes.cast(pvector, bmap.bm_CKFLOAT_p)
    idx: int = 0
    for _ in range(count):
        ret.x = pfloat[idx]
        ret.y = pfloat[idx + 1]
        ret.z = pfloat[idx + 2]
        idx += 3
        yield ret
        
def _vxvector2_assigner(pvector: bmap.bm_VxVector2_p, count: int, itor: typing.Iterator[virtools_types.VxVector2]) -> None:
    pfloat: bmap.bm_CKFLOAT_p = ctypes.cast(pvector, bmap.bm_CKFLOAT_p)
    idx: int = 0
    for _ in range(count):
        uservector: virtools_types.VxVector2 = next(itor)
        pfloat[idx] = uservector.x
        pfloat[idx + 1] = uservector.y
        idx += 2

def _vxvector2_iterator(pvector: bmap.bm_VxVector2_p, count: int) -> typing.Iterator[virtools_types.VxVector2]:
    ret: virtools_types.VxVector2 = virtools_types.VxVector2()
    pfloat: bmap.bm_CKFLOAT_p = ctypes.cast(pvector, bmap.bm_CKFLOAT_p)
    idx: int = 0
    for _ in range(count):
        ret.x = pfloat[idx]
        ret.y = pfloat[idx + 1]
        idx += 2
        yield ret
      
def _ckfaceindices_assigner(pindices: bmap.bm_CKDWORD_p, count: int, itor: typing.Iterator[virtools_types.CKFaceIndices]) -> None:
    idx: int = 0
    for _ in range(count):
        userindices: virtools_types.CKFaceIndices = next(itor)
        pindices[idx] = userindices.i1
        pindices[idx + 1] = userindices.i2
        pindices[idx + 2] = userindices.i3
        idx += 3

def _ckfaceindices_iterator(pindices: bmap.bm_CKDWORD_p, count: int) -> typing.Iterator[virtools_types.CKFaceIndices]:
    ret: virtools_types.CKFaceIndices = virtools_types.CKFaceIndices()
    idx: int = 0
    for _ in range(count):
        ret.i1 = pindices[idx]
        ret.i2 = pindices[idx + 1]
        ret.i3 = pindices[idx + 2]
        idx += 3

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
@remark
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
    def get_file_name(self) -> str | None:
        filename: bmap.bm_CKSTRING = bmap.bm_CKSTRING()
        bmap.BMTexture_GetFileName(self._get_pointer(), self._get_ckid(), ctypes.byref(filename))
        if filename.value is None:
            return None
        else:
            return filename.value.decode(g_BMapEncoding)

    def load_image(self, filepath: str) -> None:
        filename: bmap.bm_CKSTRING = bmap.bm_CKSTRING(filepath.encode(g_BMapEncoding))
        bmap.BMTexture_LoadImage(self._get_pointer(), self._get_ckid(), filename)

    def save_image(self, filepath: str) -> None:
        filename: bmap.bm_CKSTRING = bmap.bm_CKSTRING(filepath.encode(g_BMapEncoding))
        bmap.BMTexture_SaveImage(self._get_pointer(), self._get_ckid(), filename)

    def get_save_options(self) -> virtools_types.CK_TEXTURE_SAVEOPTIONS:
        opt: bmap.bm_enum = bmap.bm_enum()
        bmap.BMTexture_GetSaveOptions(self._get_pointer(), self._get_ckid(), ctypes.byref(opt))
        return virtools_types.CK_TEXTURE_SAVEOPTIONS(opt.value)

    def set_save_options(self, opt_: virtools_types.CK_TEXTURE_SAVEOPTIONS) -> None:
        opt: bmap.bm_enum = bmap.bm_enum(opt_.value)
        bmap.BMTexture_SetSaveOptions(self._get_pointer(), self._get_ckid(), opt)

    def get_video_format(self) -> virtools_types.VX_PIXELFORMAT:
        fmt: bmap.bm_enum = bmap.bm_enum()
        bmap.BMTexture_GetVideoFormat(self._get_pointer(), self._get_ckid(), ctypes.byref(fmt))
        return virtools_types.VX_PIXELFORMAT(fmt.value)

    def set_video_format(self, fmt_: virtools_types.VX_PIXELFORMAT) -> None:
        fmt: bmap.bm_enum = bmap.bm_enum(fmt_.value)
        bmap.BMTexture_SetVideoFormat(self._get_pointer(), self._get_ckid(), fmt)

class BMMaterial(BMObject):
    def _set_vxcolor(self,
        setter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, bmap.bm_VxColor], bool],
        col_: virtools_types.VxColor) -> None:
        # set to raw color
        col: bmap.bm_VxColor = bmap.bm_VxColor()
        col.r = col_.r
        col.g = col_.g
        col.b = col_.b
        col.a = col_.a
        # assign
        setter_(self._get_pointer(), self._get_ckid(), col)
    def _get_vxcolor(self,
        getter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, bmap.bm_VxColor_p], bool]) -> virtools_types.VxColor:
        # get raw color
        col: bmap.bm_VxColor = bmap.bm_VxColor()
        getter_(self._get_pointer(), self._get_ckid(), ctypes.byref(col))
        # get from raw color
        ret: virtools_types.VxColor = virtools_types.VxColor()
        ret.r = col.r
        ret.g = col.g
        ret.b = col.b
        ret.a = col.a
        return ret
    
    def get_diffuse(self) -> virtools_types.VxColor:
        return self._get_vxcolor(bmap.BMMaterial_GetDiffuse)
    def set_diffuse(self, col: virtools_types.VxColor) -> None:
        self._set_vxcolor(bmap.BMMaterial_SetDiffuse, col)
    def get_ambient(self) -> virtools_types.VxColor:
        return self._get_vxcolor(bmap.BMMaterial_GetAmbient)
    def set_ambient(self, col: virtools_types.VxColor) -> None:
        self._set_vxcolor(bmap.BMMaterial_SetAmbient, col)
    def get_specular(self) -> virtools_types.VxColor:
        return self._get_vxcolor(bmap.BMMaterial_GetSpecular)
    def set_specular(self, col: virtools_types.VxColor) -> None:
        self._set_vxcolor(bmap.BMMaterial_SetSpecular, col)
    def get_emissive(self) -> virtools_types.VxColor:
        return self._get_vxcolor(bmap.BMMaterial_GetEmissive)
    def set_emissive(self, col: virtools_types.VxColor) -> None:
        self._set_vxcolor(bmap.BMMaterial_SetEmissive, col)

    def get_specular_power(self) -> float:
        power: bmap.bm_CKFLOAT = bmap.bm_CKFLOAT()
        bmap.BMMaterial_GetSpecularPower(self._get_pointer(), self._get_ckid(), ctypes.byref(power))
        return power.value
    def set_specular_power(self, power_: float) -> None:
        power: bmap.bm_CKFLOAT = bmap.bm_CKFLOAT(power_)
        bmap.BMMaterial_SetSpecularPower(self._get_pointer(), self._get_ckid(), power)
        
    def get_texture(self) -> BMTexture | None:
        objid: bmap.bm_CKID = bmap.bm_CKID()
        bmap.BMMaterial_GetTexture(self._get_pointer(), self._get_ckid(), ctypes.byref(objid))
        if objid.value == g_InvalidCKID:
            return None
        else:
            return BMTexture(self._get_pointer(), objid)
        
    def set_texture(self, tex_: BMTexture | None) -> None:
        objid: bmap.bm_CKID = bmap.bm_CKID(g_InvalidCKID)
        if tex_ is not None:
            objid.value = tex_._get_ckid()
        bmap.BMMaterial_SetTexture(self._get_pointer(), self._get_ckid(), objid)

    def get_texture_border_color(self) -> virtools_types.VxColor:
        col: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        bmap.BMMaterial_GetTextureBorderColor(self._get_pointer(), self._get_ckid(), ctypes.byref(col))
        ret: virtools_types.VxColor = virtools_types.VxColor()
        ret.from_dword(col.value)
        return ret
    def set_texture_border_color(self, col_: virtools_types.VxColor) -> None:
        col: bmap.bm_CKDWORD = bmap.bm_CKDWORD(col_.to_dword())
        bmap.BMMaterial_SetTextureBorderColor(self._get_pointer(), self._get_ckid(), col)

    def get_texture_blend_mode(self) -> virtools_types.VXTEXTURE_BLENDMODE:
        data: bmap.bm_enum = bmap.bm_enum()
        bmap.BMMaterial_GetTextureBlendMode(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return virtools_types.VXTEXTURE_BLENDMODE(data.value)
    def set_texture_blend_mode(self, data_: virtools_types.VXTEXTURE_BLENDMODE) -> None:
        data: bmap.bm_enum = bmap.bm_enum(data_.value)
        bmap.BMMaterial_SetTextureBlendMode(self._get_pointer(), self._get_ckid(), data)
    def get_texture_min_mode(self) -> virtools_types.VXTEXTURE_FILTERMODE:
        data: bmap.bm_enum = bmap.bm_enum()
        bmap.BMMaterial_GetTextureMinMode(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return virtools_types.VXTEXTURE_FILTERMODE(data.value)
    def set_texture_min_mode(self, data_: virtools_types.VXTEXTURE_FILTERMODE) -> None:
        data: bmap.bm_enum = bmap.bm_enum(data_.value)
        bmap.BMMaterial_SetTextureMinMode(self._get_pointer(), self._get_ckid(), data)
    def get_texture_mag_mode(self) -> virtools_types.VXTEXTURE_FILTERMODE:
        data: bmap.bm_enum = bmap.bm_enum()
        bmap.BMMaterial_GetTextureMagMode(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return virtools_types.VXTEXTURE_FILTERMODE(data.value)
    def set_texture_mag_mode(self, data_: virtools_types.VXTEXTURE_FILTERMODE) -> None:
        data: bmap.bm_enum = bmap.bm_enum(data_.value)
        bmap.BMMaterial_SetTextureMagMode(self._get_pointer(), self._get_ckid(), data)
    def get_texture_address_mode(self) -> virtools_types.VXTEXTURE_ADDRESSMODE:
        data: bmap.bm_enum = bmap.bm_enum()
        bmap.BMMaterial_GetTextureAddressMode(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return virtools_types.VXTEXTURE_ADDRESSMODE(data.value)
    def set_texture_address_mode(self, data_: virtools_types.VXTEXTURE_ADDRESSMODE) -> None:
        data: bmap.bm_enum = bmap.bm_enum(data_.value)
        bmap.BMMaterial_SetTextureAddressMode(self._get_pointer(), self._get_ckid(), data)
    def get_source_blend(self) -> virtools_types.VXBLEND_MODE:
        data: bmap.bm_enum = bmap.bm_enum()
        bmap.BMMaterial_GetSourceBlend(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return virtools_types.VXBLEND_MODE(data.value)
    def set_source_blend(self, data_: virtools_types.VXBLEND_MODE) -> None:
        data: bmap.bm_enum = bmap.bm_enum(data_.value)
        bmap.BMMaterial_SetSourceBlend(self._get_pointer(), self._get_ckid(), data)
    def get_dest_blend(self) -> virtools_types.VXBLEND_MODE:
        data: bmap.bm_enum = bmap.bm_enum()
        bmap.BMMaterial_GetDestBlend(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return virtools_types.VXBLEND_MODE(data.value)
    def set_dest_blend(self, data_: virtools_types.VXBLEND_MODE) -> None:
        data: bmap.bm_enum = bmap.bm_enum(data_.value)
        bmap.BMMaterial_SetDestBlend(self._get_pointer(), self._get_ckid(), data)
    def get_fill_mode(self) -> virtools_types.VXFILL_MODE:
        data: bmap.bm_enum = bmap.bm_enum()
        bmap.BMMaterial_GetFillMode(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return virtools_types.VXFILL_MODE(data.value)
    def set_fill_mode(self, data_: virtools_types.VXFILL_MODE) -> None:
        data: bmap.bm_enum = bmap.bm_enum(data_.value)
        bmap.BMMaterial_SetFillMode(self._get_pointer(), self._get_ckid(), data)
    def get_shade_mode(self) -> virtools_types.VXSHADE_MODE:
        data: bmap.bm_enum = bmap.bm_enum()
        bmap.BMMaterial_GetShadeMode(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return virtools_types.VXSHADE_MODE(data.value)
    def set_shade_mode(self, data_: virtools_types.VXSHADE_MODE) -> None:
        data: bmap.bm_enum = bmap.bm_enum(data_.value)
        bmap.BMMaterial_SetShadeMode(self._get_pointer(), self._get_ckid(), data)

    def get_alpha_test_enabled(self) -> bool:
        data: bmap.bm_bool = bmap.bm_bool()
        bmap.BMMaterial_GetAlphaTestEnabled(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return data.value
    def set_alpha_test_enabled(self, data_: bool) -> None:
        data: bmap.bm_bool = bmap.bm_bool(data_)
        bmap.BMMaterial_SetAlphaTestEnabled(self._get_pointer(), self._get_ckid(), data)
    def get_alpha_blend_enabled(self) -> bool:
        data: bmap.bm_bool = bmap.bm_bool()
        bmap.BMMaterial_GetAlphaBlendEnabled(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return data.value
    def set_alpha_blend_enabled(self, data_: bool) -> None:
        data: bmap.bm_bool = bmap.bm_bool(data_)
        bmap.BMMaterial_SetAlphaBlendEnabled(self._get_pointer(), self._get_ckid(), data)
    def get_perspective_correction_enabled(self) -> bool:
        data: bmap.bm_bool = bmap.bm_bool()
        bmap.BMMaterial_GetPerspectiveCorrectionEnabled(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return data.value
    def set_perspective_correction_enabled(self, data_: bool) -> None:
        data: bmap.bm_bool = bmap.bm_bool(data_)
        bmap.BMMaterial_SetPerspectiveCorrectionEnabled(self._get_pointer(), self._get_ckid(), data)
    def get_z_write_enabled(self) -> bool:
        data: bmap.bm_bool = bmap.bm_bool()
        bmap.BMMaterial_GetZWriteEnabled(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return data.value
    def set_z_write_enabled(self, data_: bool) -> None:
        data: bmap.bm_bool = bmap.bm_bool(data_)
        bmap.BMMaterial_SetZWriteEnabled(self._get_pointer(), self._get_ckid(), data)
    def get_two_sided_enabled(self) -> bool:
        data: bmap.bm_bool = bmap.bm_bool()
        bmap.BMMaterial_GetTwoSidedEnabled(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return data.value
    def set_two_sided_enabled(self, data_: bool) -> None:
        data: bmap.bm_bool = bmap.bm_bool(data_)
        bmap.BMMaterial_SetTwoSidedEnabled(self._get_pointer(), self._get_ckid(), data)

    def get_alpha_ref(self) -> int:
        data: bmap.bm_CKBYTE = bmap.bm_CKBYTE()
        bmap.BMMaterial_GetAlphaRef(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return data.value
    def set_alpha_ref(self, data_: int):
        data: bmap.bm_CKBYTE = bmap.bm_CKBYTE(data_)
        bmap.BMMaterial_SetAlphaRef(self._get_pointer(), self._get_ckid(), data)
        
    def get_alpha_func(self) -> virtools_types.VXCMPFUNC:
        data: bmap.bm_enum = bmap.bm_enum()
        bmap.BMMaterial_GetAlphaFunc(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return virtools_types.VXCMPFUNC(data.value)
    def set_alpha_func(self, data_: virtools_types.VXCMPFUNC) -> None:
        data: bmap.bm_enum = bmap.bm_enum(data_.value)
        bmap.BMMaterial_SetAlphaFunc(self._get_pointer(), self._get_ckid(), data)
    def get_z_func(self) -> virtools_types.VXCMPFUNC:
        data: bmap.bm_enum = bmap.bm_enum()
        bmap.BMMaterial_GetZFunc(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return virtools_types.VXCMPFUNC(data.value)
    def set_z_func(self, data_: virtools_types.VXCMPFUNC) -> None:
        data: bmap.bm_enum = bmap.bm_enum(data_.value)
        bmap.BMMaterial_SetZFunc(self._get_pointer(), self._get_ckid(), data)

class BMMesh(BMObject):
    def get_vertex_count(self) -> int:
        count: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        bmap.BMMesh_GetVertexCount(self._get_pointer(), self._get_ckid(), ctypes.byref(count))
        return count.value
    
    def set_vertex_count(self, count_: int) -> None:
        count: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count_)
        bmap.BMMesh_SetVertexCount(self._get_pointer(), self._get_ckid(), count)

    def get_vertex_positions(self) -> typing.Iterator[virtools_types.VxVector3]:
        # get raw pointer and return
        raw_vector: bmap.bm_VxVector3_p = bmap.bm_VxVector3_p()
        bmap.BMMesh_GetVertexPositions(self._get_pointer(), self._get_ckid(), ctypes.byref(raw_vector))
        return _vxvector3_iterator(raw_vector, self.get_vertex_count())
            
    def set_vertex_positions(self, itor: typing.Iterator[virtools_types.VxVector3]) -> None:
        # get raw float pointer and assign
        raw_vector: bmap.bm_VxVector3_p = bmap.bm_VxVector3_p()
        bmap.BMMesh_GetVertexPositions(self._get_pointer(), self._get_ckid(), ctypes.byref(raw_vector))
        _vxvector3_assigner(raw_vector, self.get_vertex_count(), itor)

    def get_vertex_normals(self) -> typing.Iterator[virtools_types.VxVector3]:
        raw_vector: bmap.bm_VxVector3_p = bmap.bm_VxVector3_p()
        bmap.BMMesh_GetVertexNormals(self._get_pointer(), self._get_ckid(), ctypes.byref(raw_vector))
        return _vxvector3_iterator(raw_vector, self.get_vertex_count())
            
    def set_vertex_normals(self, itor: typing.Iterator[virtools_types.VxVector3]) -> None:
        raw_vector: bmap.bm_VxVector3_p = bmap.bm_VxVector3_p()
        bmap.BMMesh_GetVertexNormals(self._get_pointer(), self._get_ckid(), ctypes.byref(raw_vector))
        _vxvector3_assigner(raw_vector, self.get_vertex_count(), itor)

    def get_vertex_uvs(self) -> typing.Iterator[virtools_types.VxVector2]:
        raw_vector: bmap.bm_VxVector2_p = bmap.bm_VxVector2_p()
        bmap.BMMesh_GetVertexUVs(self._get_pointer(), self._get_ckid(), ctypes.byref(raw_vector))
        _vxvector2_iterator(raw_vector, self.get_vertex_count())

    def set_vertex_uvs(self, itor: typing.Iterator[virtools_types.VxVector2]) -> None:
        raw_vector: bmap.bm_VxVector2_p = bmap.bm_VxVector2_p()
        bmap.BMMesh_GetVertexUVs(self._get_pointer(), self._get_ckid(), ctypes.byref(raw_vector))
        _vxvector2_assigner(raw_vector, self.get_vertex_count(), itor)

    def get_face_count(self) -> int:
        count: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        bmap.BMMesh_GetFaceCount(self._get_pointer(), self._get_ckid(), ctypes.byref(count))
        return count.value
    
    def set_face_count(self, count_: int) -> None:
        count: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count_)
        bmap.BMMesh_SetFaceCount(self._get_pointer(), self._get_ckid(), count)

    def get_face_indices(self) -> typing.Iterator[virtools_types.CKFaceIndices]:
        raw_idx: bmap.bm_CKWORD_p = bmap.bm_CKWORD_p()
        bmap.BMMesh_GetFaceIndices(self._get_pointer(), self._get_ckid(), ctypes.byref(raw_idx))
        return _ckfaceindices_iterator(raw_idx, self.get_face_count())

    def set_face_indices(self, itor: typing.Iterator[virtools_types.CKFaceIndices]) -> None:
        raw_idx: bmap.bm_CKWORD_p = bmap.bm_CKWORD_p()
        bmap.BMMesh_GetFaceIndices(self._get_pointer(), self._get_ckid(), ctypes.byref(raw_idx))
        _ckfaceindices_assigner(raw_idx, self.get_face_count(), itor)

    def get_face_material_slot_indexs(self) -> typing.Iterator[int]:
        raw_idx: bmap.bm_CKWORD_p = bmap.bm_CKWORD_p()
        bmap.BMMesh_GetFaceIndices(self._get_pointer(), self._get_ckid(), ctypes.byref(raw_idx))
        for i in range(self.get_face_count()):
            yield raw_idx[i]

    def set_face_material_slot_indexs(self, itor: typing.Iterator[int]) -> None:
        raw_idx: bmap.bm_CKWORD_p = bmap.bm_CKWORD_p()
        bmap.BMMesh_GetFaceIndices(self._get_pointer(), self._get_ckid(), ctypes.byref(raw_idx))
        for i in range(self.get_face_count()):
            raw_idx[i] = next(itor)

    def get_material_slot_count(self) -> int:
        count: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        bmap.BMMesh_GetMaterialSlotCount(self._get_pointer(), self._get_ckid(), ctypes.byref(count))
        return count.value
    
    def set_material_slot_count(self, count_: int) -> None:
        count: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count_)
        bmap.BMMesh_SetMaterialSlotCount(self._get_pointer(), self._get_ckid(), count)

    def get_material_slots(self) -> typing.Iterator[BMMaterial | None]:
        idx: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        mtlid: bmap.bm_CKID = bmap.bm_CKID()
        for i in range(self.get_material_slot_count()):
            idx.value = i
            bmap.BMMesh_GetMaterialSlot(self._get_pointer(), self._get_ckid(), idx, ctypes.byref(mtlid))
            if mtlid.value == g_InvalidCKID:
                yield None
            else:
                yield BMMaterial(self._get_pointer(), mtlid)

    def set_material_slots(self, itor: typing.Iterator[BMMaterial | None]) -> None:
        idx: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        mtlid: bmap.bm_CKID = bmap.bm_CKID()
        for i in range(self.get_material_slot_count()):
            idx.value = i
            # analyze mtl item
            mtlobj: BMMaterial | None = next(itor)
            if mtlobj is None:
                mtlid.value = g_InvalidCKID
            else:
                mtlid.value = mtlobj._get_ckid()
            # set
            bmap.BMMesh_SetMaterialSlot(self._get_pointer(), self._get_ckid(), idx, mtlid)

class BM3dObject(BMObject):
    def get_world_matrix(self) -> virtools_types.VxMatrix:
        mat: bmap.bm_VxMatrix = bmap.bm_VxMatrix()
        bmap.BM3dObject_GetWorldMatrix(self._get_pointer(), self._get_ckid(), ctypes.byref(mat))
        # use cast & pointer to get matrix data conveniently
        flat: bmap.bm_CKFLOAT_p = ctypes.cast(ctypes.byref(mat), bmap.bm_CKFLOAT_p)
        ret: virtools_types.VxMatrix = virtools_types.VxMatrix()
        ret.from_const(tuple(flat[i] for i in range(16)))
        return ret

    def set_world_matrix(self, mat: virtools_types.VxMatrix) -> None:
        # star syntax expand the tuple as the argument.
        mat: bmap.bm_VxMatrix = bmap.bm_VxMatrix(*(mat.to_const()))
        bmap.BM3dObject_SetWorldMatrix(self._get_pointer(), self._get_ckid(), mat)

    def get_current_mesh(self) -> BMMesh | None:
        ckid: bmap.bm_CKID = bmap.bm_CKID()
        bmap.BM3dObject_GetCurrentMesh(self._get_pointer(), self._get_ckid(), ctypes.byref(ckid))
        if ckid.value == g_InvalidCKID:
            return None
        else:
            return BMMesh(self._get_pointer(), ckid)

    def set_current_mesh(self, mesh: BMMesh | None) -> None:
        ckid: bmap.bm_CKID = bmap.bm_CKID(g_InvalidCKID)
        if mesh is not None:
            ckid.value = mesh._get_ckid()
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

    def get_object_count(self) -> int:
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        bmap.BMGroup_GetObjectCount(self._get_pointer(), self._get_ckid(), ctypes.byref(csize))
        return csize.value

    def get_objects(self) -> typing.Iterator[BM3dObject]:
        csize: int = self.get_object_count()

        # iterate list
        cidx: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        retid: bmap.bm_CKID = bmap.bm_CKID()
        for i in range(csize):
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

    def __get_ckobject_count(self,
        count_getter: typing.Callable[[bmap.bm_void_p, bmap.bm_CKDWORD_p], bool]) -> int:
        # get size
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        count_getter(self._get_pointer(), ctypes.byref(csize))
        return csize.value

    def __get_ckobjects(self, 
        class_type: type[TCKObj],
        count_getter: typing.Callable[[bmap.bm_void_p, bmap.bm_CKDWORD_p], bool],
        obj_getter: typing.Callable[[bmap.bm_void_p, bmap.bm_CKDWORD, bmap.bm_CKID_p], bool]) -> typing.Iterator[TCKObj]:
        # get size first
        csize: int = self.__get_ckobject_count(count_getter)

        # iterate list
        cidx: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        retid: bmap.bm_CKID = bmap.bm_CKID()
        for i in range(csize):
            cidx.value = i
            obj_getter(self._get_pointer(), cidx, ctypes.byref(retid))
            # yield return constructed obj visitor
            yield class_type(self._get_pointer(), retid)

    def get_texture_count(self) -> int:
        return self.__get_ckobject_count(bmap.BMFile_GetTextureCount)
    def get_textures(self) -> typing.Iterator[BMTexture]:
        return self.__get_ckobjects(
            BMTexture,
            bmap.BMFile_GetTextureCount,
            bmap.BMFile_GetTexture
        )
    
    def get_material_count(self) -> int:
        return self.__get_ckobject_count(bmap.BMFile_GetMaterialCount)
    def get_materials(self) -> typing.Iterator[BMMaterial]:
        return self.__get_ckobjects(
            BMMaterial,
            bmap.BMFile_GetMaterialCount,
            bmap.BMFile_GetMaterial
        )
    
    def get_mesh_count(self) -> int:
        return self.__get_ckobject_count(bmap.BMFile_GetMeshCount)
    def get_meshs(self) -> typing.Iterator[BMMesh]:
        return self.__get_ckobjects(
            BMMesh,
            bmap.BMFile_GetMeshCount,
            bmap.BMFile_GetMesh
        )
    
    def get_3dobject_count(self) -> int:
        return self.__get_ckobject_count(bmap.BMFile_Get3dObjectCount)
    def get_3dobjects(self) -> typing.Iterator[BM3dObject]:
        return self.__get_ckobjects(
            BM3dObject,
            bmap.BMFile_Get3dObjectCount,
            bmap.BMFile_Get3dObject
        )
    
    def get_group_count(self) -> int:
        return self.__get_ckobject_count(bmap.BMFile_GetGroupCount)
    def get_groups(self) -> typing.Iterator[BMGroup]:
        return self.__get_ckobjects(
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

    def parse(self, bmfile: BMFileWriter, objmesh: BMMesh) -> None:
        bmap.BMMeshTrans_Parse(self._get_pointer(), bmfile._get_pointer(), objmesh._get_ckid())

    def prepare_vertex(self, count: int, itor: typing.Iterator[virtools_types.VxVector3]) -> None:
        # prepare count first
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count)
        bmap.BMMeshTrans_PrepareVertexCount(self._get_pointer(), csize)
        # get raw pointer and conv to float ptr for convenient visit
        raw_vector: bmap.bm_VxVector3_p = bmap.bm_VxVector3_p()
        bmap.BMMeshTrans_PrepareVertex(self._get_pointer(), ctypes.byref(raw_vector))
        # set by pointer
        _vxvector3_assigner(raw_vector, count, itor)
    
    def prepare_normal(self, count: int, itor: typing.Iterator[virtools_types.VxVector3]) -> None:
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count)
        bmap.BMMeshTrans_PrepareNormalCount(self._get_pointer(), csize)
        
        raw_vector: bmap.bm_VxVector3_p = bmap.bm_VxVector3_p()
        bmap.BMMeshTrans_PrepareNormal(self._get_pointer(), ctypes.byref(raw_vector))
        
        _vxvector3_assigner(raw_vector, count, itor)
    
    def prepare_uv(self, count: int, itor: typing.Iterator[virtools_types.VxVector2]) -> None:
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count)
        bmap.BMMeshTrans_PrepareUVCount(self._get_pointer(), csize)
        
        raw_vector: bmap.bm_VxVector2_p = bmap.bm_VxVector2_p()
        bmap.BMMeshTrans_PrepareUV(self._get_pointer(), ctypes.byref(raw_vector))
        
        _vxvector2_assigner(raw_vector, count, itor)
    
    def prepare_mtl_slot(self, count: int, itor: typing.Iterator[BMMaterial | None]) -> None:
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count)
        bmap.BMMeshTrans_PrepareMtlSlotCount(self._get_pointer(), csize)
        
        raw_ckid: bmap.bm_CKID_p = bmap.bm_CKID_p()
        bmap.BMMeshTrans_PrepareMtlSlot(self._get_pointer(), ctypes.byref(raw_ckid))
        
        idx: int = 0
        for _ in range(count):
            usermtl: BMMaterial = next(itor)
            if usermtl is None:
                raw_ckid[idx] = g_InvalidCKID
            else:
                raw_ckid[idx] = usermtl._get_ckid().value
            idx += 1
    
    def prepare_face(self,
        count: int,
        vec_idx: typing.Iterator[virtools_types.CKFaceIndices],
        nml_idx: typing.Iterator[virtools_types.CKFaceIndices],
        uv_idx: typing.Iterator[virtools_types.CKFaceIndices],
        mtl_idx: typing.Iterator[int]) -> None:
        # prepare face size
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count)
        bmap.BMMeshTrans_PrepareFaceCount(self._get_pointer(), csize)

        # get 4 raw pointer for following assign
        raw_vec_idx: bmap.bm_CKDWORD_p = bmap.bm_CKDWORD_p()
        raw_nml_idx: bmap.bm_CKDWORD_p = bmap.bm_CKDWORD_p()
        raw_uv_idx: bmap.bm_CKDWORD_p = bmap.bm_CKDWORD_p()
        raw_mtl_idx: bmap.bm_CKDWORD_p = bmap.bm_CKDWORD_p()
        bmap.BMMeshTrans_PrepareFaceVertexIndices(self._get_pointer(), ctypes.byref(raw_vec_idx))
        bmap.BMMeshTrans_PrepareFaceNormalIndices(self._get_pointer(), ctypes.byref(raw_nml_idx))
        bmap.BMMeshTrans_PrepareFaceUVIndices(self._get_pointer(), ctypes.byref(raw_uv_idx))
        bmap.BMMeshTrans_PrepareFaceMtlSlot(self._get_pointer(), ctypes.byref(raw_mtl_idx))

        # iterate and assign
        # assigne triple indices
        _ckfaceindices_assigner(raw_vec_idx, count, vec_idx)
        _ckfaceindices_assigner(raw_nml_idx, count, nml_idx)
        _ckfaceindices_assigner(raw_uv_idx, count, uv_idx)
        # assign mtl index
        idx: int = 0
        for _ in range(count):
            raw_mtl_idx[idx] = next(mtl_idx)
            idx += 1

#endregion
