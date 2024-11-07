import ctypes, typing, atexit, enum
from . import bmap, virtools_types

#region Basic Class & Constant Defines

g_InvalidPtr: bmap.bm_void_p = bmap.bm_void_p(0)
g_InvalidCKID: int = 0
g_BMapEncoding: str = "utf-8"

def _python_callback(strl: bytes):
    """
    The Python type callback for BMFile.
    Simply add a prefix when output.
    Need a convertion before passing to BMFile.
    """
    # YYC Remarks:
    # The passing value to this function is bytes, not bmap.bm_CKSTRING.
    # I think Python do an auto convertion in there.
    if strl is not None:
        print(f'[PyBMap] {strl.decode(g_BMapEncoding)}')
_g_RawCallback: bmap.bm_callback = bmap.bm_callback(_python_callback)

#endregion

#region Help Functions

class _Utils:
    @staticmethod
    def raise_out_of_length_exception() -> None:
        raise bmap.BMapException("The length of given data is too short when assigning struct array.")

    @staticmethod
    def _vector_assigner(pdata: typing.Any, item_count: int, factor_count: int, itor: typing.Iterator[tuple[typing.Any, ...]]) -> None:
        idx: int = 0
        try:
            for _i in range(item_count):
                user_vector: tuple[typing.Any, ...] = next(itor)
                for _j in range(factor_count):
                    pdata[idx] = user_vector[_j]
                    idx += 1
        except StopIteration:
            _Utils.raise_out_of_length_exception()

    @staticmethod
    def _vector_iterator(pdata: typing.Any, item_count: int, factor_count: int) -> typing.Iterator[tuple[typing.Any, ...]]:
        idx: int = 0
        for _i in range(item_count):
            yield tuple(map(
                lambda _j: pdata[idx + _j],
                range(factor_count)
            ))
            idx += factor_count

    @staticmethod
    def vxvector3_assigner(pvector: bmap.bm_VxVector3_p, count: int, itor: typing.Iterator[virtools_types.VxVector3]) -> None:
        _Utils._vector_assigner(ctypes.cast(pvector, bmap.bm_CKFLOAT_p), count, 3, map(lambda v: (v.x, v.y, v.z), itor))
    @staticmethod
    def vxvector3_iterator(pvector: bmap.bm_VxVector3_p, count: int) -> typing.Iterator[virtools_types.VxVector3]:
        return map(
            lambda v: virtools_types.VxVector3(*v),
            _Utils._vector_iterator(ctypes.cast(pvector, bmap.bm_CKFLOAT_p), count, 3)
        )

    @staticmethod     
    def vxvector2_assigner(pvector: bmap.bm_VxVector2_p, count: int, itor: typing.Iterator[virtools_types.VxVector2]) -> None:
        _Utils._vector_assigner(ctypes.cast(pvector, bmap.bm_CKFLOAT_p), count, 2, map(lambda v: (v.x, v.y), itor))
    @staticmethod
    def vxvector2_iterator(pvector: bmap.bm_VxVector2_p, count: int) -> typing.Iterator[virtools_types.VxVector2]:
        return map(
            lambda v: virtools_types.VxVector2(*v),
            _Utils._vector_iterator(ctypes.cast(pvector, bmap.bm_CKFLOAT_p), count, 2)
        )
    
    """!
    @remarks
    bmap.bm_CKWORD_p | bmap.bm_CKDWORD_p is just a type hint.
    We actually do not need distinguish them in code.
    Because the stride when increasing them is decided by their runtime type.
    """
    
    @staticmethod
    def ckfaceindices_assigner(pindices: bmap.bm_CKWORD_p | bmap.bm_CKDWORD_p, count: int, itor: typing.Iterator[virtools_types.CKFaceIndices]) -> None:
        _Utils._vector_assigner(pindices, count, 3, map(lambda v: (v.i1, v.i2, v.i3), itor))
    @staticmethod
    def ckfaceindices_iterator(pindices: bmap.bm_CKWORD_p | bmap.bm_CKDWORD_p, count: int) -> typing.Iterator[virtools_types.CKFaceIndices]:
        return map(
            lambda v: virtools_types.CKFaceIndices(*v),
            _Utils._vector_iterator(pindices, count, 3)
        )

#endregion

#region Basic Classes

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

TEnumType = typing.TypeVar('TEnumType', bound = enum.IntEnum)
TIntegralType = bmap.bm_CKDWORD | bmap.bm_CKWORD | bmap.bm_CKINT | bmap.bm_CKBYTE | bmap.bm_CKID
TFloatPointType = bmap.bm_CKFLOAT
TPointerType = typing.TypeVar('TPointerType')

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
        if not _AbstractPointer.__eq__(self, obj): return False

        if isinstance(obj, self.__class__):
            return obj.__mCKID == self.__mCKID
        else:
            return False
        
    def __hash__(self) -> int:
        return hash((_AbstractPointer.__hash__(self), self.__mCKID))

    # Convenient Value Getter Setter
    # Focusing on those which widely called types.

    def _get_primitive_value(self, primitive_type_: typing.Any, getter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, typing.Any], bmap.bm_bool]) -> typing.Any:
        data = primitive_type_()
        getter_(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return data.value
    def _set_primitive_value(self, primitive_type_: typing.Any, setter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, typing.Any], bmap.bm_bool], data_: typing.Any) -> None:
        data = primitive_type_(data_)
        setter_(self._get_pointer(), self._get_ckid(), data)

    def _get_integral_value(self, integral_type_: type[TIntegralType], getter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, typing.Any], bmap.bm_bool]) -> int:
        return self._get_primitive_value(integral_type_, getter_)
    def _set_integral_value(self, integral_type_: type[TIntegralType], setter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, typing.Any], bmap.bm_bool], data_: int) -> None:
        self._set_primitive_value(integral_type_, setter_, data_)
    
    def _get_float_point_value(self, float_point_type_: type[TFloatPointType], getter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, typing.Any], bmap.bm_bool]) -> float:
        return self._get_primitive_value(float_point_type_, getter_)
    def _set_float_point_value(self, float_point_type_: type[TFloatPointType], setter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, typing.Any], bmap.bm_bool], data_: float) -> None:
        self._set_primitive_value(float_point_type_, setter_, data_)
    
    def _get_bool_value(self, getter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, bmap.bm_bool_p], bmap.bm_bool]) -> bool:
        return self._get_primitive_value(bmap.bm_bool, getter_)
    def _set_bool_value(self, setter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, bmap.bm_bool], bmap.bm_bool], data_: bool) -> None:
        self._set_primitive_value(bmap.bm_bool, setter_, data_)
    
    def _get_enum_value(self, enum_type_: type[TEnumType], getter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, bmap.bm_enum_p], bmap.bm_bool]) -> TEnumType:
        return enum_type_(self._get_primitive_value(bmap.bm_enum, getter_))
    def _set_enum_value(self, setter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, bmap.bm_enum], bmap.bm_bool], data_: TEnumType) -> None:
        self._set_primitive_value(bmap.bm_enum, setter_, data_.value)

    def _get_str_value(self, getter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, bmap.bm_CKSTRING_p], bmap.bm_bool]) -> str | None:
        data: bmap.bm_CKSTRING = bmap.bm_CKSTRING()
        getter_(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        if data.value is None: return None
        else: return data.value.decode(g_BMapEncoding)
    def _set_str_value(self, setter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, bmap.bm_CKSTRING], bmap.bm_bool], data_: str | None) -> None:
        data: bmap.bm_CKSTRING
        if data_ is None: data = bmap.bm_CKSTRING(0)
        else: data = bmap.bm_CKSTRING(data_.encode(g_BMapEncoding))
        setter_(self._get_pointer(), self._get_ckid(), data)

    def _set_vxcolor_value(self,
        setter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, bmap.bm_VxColor], bmap.bm_bool],
        col_: virtools_types.VxColor) -> None:
        # set to raw color
        col: bmap.bm_VxColor = bmap.bm_VxColor()
        (col.r, col.g, col.b, col.a) = (col_.r, col_.g, col_.b, col_.a)
        # assign
        setter_(self._get_pointer(), self._get_ckid(), col)
    def _get_vxcolor_value(self,
        getter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, bmap.bm_VxColor_p], bmap.bm_bool]) -> virtools_types.VxColor:
        # get raw color
        col: bmap.bm_VxColor = bmap.bm_VxColor()
        getter_(self._get_pointer(), self._get_ckid(), ctypes.byref(col))
        # get from raw color
        ret: virtools_types.VxColor = virtools_types.VxColor()
        (ret.r, ret.g, ret.b, ret.a) = (col.r, col.g, col.b, col.a)
        return ret
    
    def _get_pointer_value(self, ptr_type_: type[TPointerType], getter_: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID, typing.Any], bmap.bm_bool]) -> TPointerType:
        data = ptr_type_()
        getter_(self._get_pointer(), self._get_ckid(), ctypes.byref(data))
        return data

TCKObject = typing.TypeVar('TCKObject', bound = _AbstractCKObject)

#endregion

#region Validation Check, Init and Dispose

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
@remarks
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
        return self._get_str_value(bmap.BMObject_GetName)
    def set_name(self, name_: str | None) -> None:
        self._set_str_value(bmap.BMObject_SetName, name_)

class BMTexture(BMObject):
    def get_file_name(self) -> str | None:
        return self._get_str_value(bmap.BMTexture_GetFileName)

    def load_image(self, filepath: str) -> None:
        filename: bmap.bm_CKSTRING = bmap.bm_CKSTRING(filepath.encode(g_BMapEncoding))
        bmap.BMTexture_LoadImage(self._get_pointer(), self._get_ckid(), filename)
    def save_image(self, filepath: str) -> None:
        filename: bmap.bm_CKSTRING = bmap.bm_CKSTRING(filepath.encode(g_BMapEncoding))
        bmap.BMTexture_SaveImage(self._get_pointer(), self._get_ckid(), filename)

    def get_save_options(self) -> virtools_types.CK_TEXTURE_SAVEOPTIONS:
        return self._get_enum_value(virtools_types.CK_TEXTURE_SAVEOPTIONS, bmap.BMTexture_GetSaveOptions)
    def set_save_options(self, opt_: virtools_types.CK_TEXTURE_SAVEOPTIONS) -> None:
        self._set_enum_value(bmap.BMTexture_SetSaveOptions, opt_)
    def get_video_format(self) -> virtools_types.VX_PIXELFORMAT:
        return self._get_enum_value(virtools_types.VX_PIXELFORMAT, bmap.BMTexture_GetVideoFormat)
    def set_video_format(self, fmt_: virtools_types.VX_PIXELFORMAT) -> None:
        self._set_enum_value(bmap.BMTexture_SetVideoFormat, fmt_)

class BMMaterial(BMObject):
    def get_diffuse(self) -> virtools_types.VxColor:
        return self._get_vxcolor_value(bmap.BMMaterial_GetDiffuse)
    def set_diffuse(self, col: virtools_types.VxColor) -> None:
        self._set_vxcolor_value(bmap.BMMaterial_SetDiffuse, col)
    def get_ambient(self) -> virtools_types.VxColor:
        return self._get_vxcolor_value(bmap.BMMaterial_GetAmbient)
    def set_ambient(self, col: virtools_types.VxColor) -> None:
        self._set_vxcolor_value(bmap.BMMaterial_SetAmbient, col)
    def get_specular(self) -> virtools_types.VxColor:
        return self._get_vxcolor_value(bmap.BMMaterial_GetSpecular)
    def set_specular(self, col: virtools_types.VxColor) -> None:
        self._set_vxcolor_value(bmap.BMMaterial_SetSpecular, col)
    def get_emissive(self) -> virtools_types.VxColor:
        return self._get_vxcolor_value(bmap.BMMaterial_GetEmissive)
    def set_emissive(self, col: virtools_types.VxColor) -> None:
        self._set_vxcolor_value(bmap.BMMaterial_SetEmissive, col)

    def get_specular_power(self) -> float:
        return self._get_float_point_value(bmap.bm_CKFLOAT, bmap.BMMaterial_GetSpecularPower)
    def set_specular_power(self, power_: float) -> None:
        self._set_float_point_value(bmap.bm_CKFLOAT, bmap.BMMaterial_SetSpecularPower, power_)
        
    def get_texture(self) -> BMTexture | None:
        objid: bmap.bm_CKID = bmap.bm_CKID()
        bmap.BMMaterial_GetTexture(self._get_pointer(), self._get_ckid(), ctypes.byref(objid))
        if objid.value == g_InvalidCKID: return None
        else: return BMTexture(self._get_pointer(), objid)
        
    def set_texture(self, tex_: BMTexture | None) -> None:
        objid: bmap.bm_CKID = bmap.bm_CKID(g_InvalidCKID)
        if tex_ is not None: objid = tex_._get_ckid()
        bmap.BMMaterial_SetTexture(self._get_pointer(), self._get_ckid(), objid)

    def get_texture_border_color(self) -> virtools_types.VxColor:
        dword_color: int = self._get_integral_value(bmap.bm_CKDWORD, bmap.BMMaterial_GetTextureBorderColor)
        ret: virtools_types.VxColor = virtools_types.VxColor()
        ret.from_dword(dword_color)
        return ret
    def set_texture_border_color(self, col_: virtools_types.VxColor) -> None:
        self._set_integral_value(bmap.bm_CKDWORD, bmap.BMMaterial_SetTextureBorderColor, col_.to_dword())

    def get_texture_blend_mode(self) -> virtools_types.VXTEXTURE_BLENDMODE:
        return self._get_enum_value(virtools_types.VXTEXTURE_BLENDMODE, bmap.BMMaterial_GetTextureBlendMode)
    def set_texture_blend_mode(self, data_: virtools_types.VXTEXTURE_BLENDMODE) -> None:
        self._set_enum_value(bmap.BMMaterial_SetTextureBlendMode, data_)
    def get_texture_min_mode(self) -> virtools_types.VXTEXTURE_FILTERMODE:
        return self._get_enum_value(virtools_types.VXTEXTURE_FILTERMODE, bmap.BMMaterial_GetTextureMinMode)
    def set_texture_min_mode(self, data_: virtools_types.VXTEXTURE_FILTERMODE) -> None:
        self._set_enum_value(bmap.BMMaterial_SetTextureMinMode, data_)
    def get_texture_mag_mode(self) -> virtools_types.VXTEXTURE_FILTERMODE:
        return self._get_enum_value(virtools_types.VXTEXTURE_FILTERMODE, bmap.BMMaterial_GetTextureMagMode)
    def set_texture_mag_mode(self, data_: virtools_types.VXTEXTURE_FILTERMODE) -> None:
        self._set_enum_value(bmap.BMMaterial_SetTextureMagMode, data_)
    def get_texture_address_mode(self) -> virtools_types.VXTEXTURE_ADDRESSMODE:
        return self._get_enum_value(virtools_types.VXTEXTURE_ADDRESSMODE, bmap.BMMaterial_GetTextureAddressMode)
    def set_texture_address_mode(self, data_: virtools_types.VXTEXTURE_ADDRESSMODE) -> None:
        self._set_enum_value(bmap.BMMaterial_SetTextureAddressMode, data_)
    def get_source_blend(self) -> virtools_types.VXBLEND_MODE:
        return self._get_enum_value(virtools_types.VXBLEND_MODE, bmap.BMMaterial_GetSourceBlend)
    def set_source_blend(self, data_: virtools_types.VXBLEND_MODE) -> None:
        self._set_enum_value(bmap.BMMaterial_SetSourceBlend, data_)
    def get_dest_blend(self) -> virtools_types.VXBLEND_MODE:
        return self._get_enum_value(virtools_types.VXBLEND_MODE, bmap.BMMaterial_GetDestBlend)
    def set_dest_blend(self, data_: virtools_types.VXBLEND_MODE) -> None:
        self._set_enum_value(bmap.BMMaterial_SetDestBlend, data_)
    def get_fill_mode(self) -> virtools_types.VXFILL_MODE:
        return self._get_enum_value(virtools_types.VXFILL_MODE, bmap.BMMaterial_GetFillMode)
    def set_fill_mode(self, data_: virtools_types.VXFILL_MODE) -> None:
        self._set_enum_value(bmap.BMMaterial_SetFillMode, data_)
    def get_shade_mode(self) -> virtools_types.VXSHADE_MODE:
        return self._get_enum_value(virtools_types.VXSHADE_MODE, bmap.BMMaterial_GetShadeMode)
    def set_shade_mode(self, data_: virtools_types.VXSHADE_MODE) -> None:
        self._set_enum_value(bmap.BMMaterial_SetShadeMode, data_)

    def get_alpha_test_enabled(self) -> bool:
        return self._get_bool_value(bmap.BMMaterial_GetAlphaTestEnabled)
    def set_alpha_test_enabled(self, data_: bool) -> None:
        self._set_bool_value(bmap.BMMaterial_SetAlphaTestEnabled, data_)
    def get_alpha_blend_enabled(self) -> bool:
        return self._get_bool_value(bmap.BMMaterial_GetAlphaBlendEnabled)
    def set_alpha_blend_enabled(self, data_: bool) -> None:
        self._set_bool_value(bmap.BMMaterial_SetAlphaBlendEnabled, data_)
    def get_perspective_correction_enabled(self) -> bool:
        return self._get_bool_value(bmap.BMMaterial_GetPerspectiveCorrectionEnabled)
    def set_perspective_correction_enabled(self, data_: bool) -> None:
        self._set_bool_value(bmap.BMMaterial_SetPerspectiveCorrectionEnabled, data_)
    def get_z_write_enabled(self) -> bool:
        return self._get_bool_value(bmap.BMMaterial_GetZWriteEnabled)
    def set_z_write_enabled(self, data_: bool) -> None:
        self._set_bool_value(bmap.BMMaterial_SetZWriteEnabled, data_)
    def get_two_sided_enabled(self) -> bool:
        return self._get_bool_value(bmap.BMMaterial_GetTwoSidedEnabled)
    def set_two_sided_enabled(self, data_: bool) -> None:
        self._set_bool_value(bmap.BMMaterial_SetTwoSidedEnabled, data_)

    def get_alpha_ref(self) -> int:
        return self._get_integral_value(bmap.bm_CKBYTE, bmap.BMMaterial_GetAlphaRef)
    def set_alpha_ref(self, data_: int):
        self._set_integral_value(bmap.bm_CKBYTE, bmap.BMMaterial_SetAlphaRef, data_)
        
    def get_alpha_func(self) -> virtools_types.VXCMPFUNC:
        return self._get_enum_value(virtools_types.VXCMPFUNC, bmap.BMMaterial_GetAlphaFunc)
    def set_alpha_func(self, data_: virtools_types.VXCMPFUNC) -> None:
        self._set_enum_value(bmap.BMMaterial_SetAlphaFunc, data_)
    def get_z_func(self) -> virtools_types.VXCMPFUNC:
        return self._get_enum_value(virtools_types.VXCMPFUNC, bmap.BMMaterial_GetZFunc)
    def set_z_func(self, data_: virtools_types.VXCMPFUNC) -> None:
        self._set_enum_value(bmap.BMMaterial_SetZFunc, data_)

class BMMesh(BMObject):

    def get_lit_mode(self) -> virtools_types.VXMESH_LITMODE:
        return self._get_enum_value(virtools_types.VXMESH_LITMODE, bmap.BMMesh_GetLitMode)
    def set_lit_mode(self, mode_: virtools_types.VXMESH_LITMODE) -> None:
        self._set_enum_value(bmap.BMMesh_SetLitMode, mode_)

    def get_vertex_count(self) -> int:
        return self._get_integral_value(bmap.bm_CKDWORD, bmap.BMMesh_GetVertexCount)
    def set_vertex_count(self, count_: int) -> None:
        self._set_integral_value(bmap.bm_CKDWORD, bmap.BMMesh_SetVertexCount, count_)

    def get_vertex_positions(self) -> typing.Iterator[virtools_types.VxVector3]:
        # get raw pointer and return
        raw_vector = self._get_pointer_value(bmap.bm_VxVector3_p, bmap.BMMesh_GetVertexPositions)
        return _Utils.vxvector3_iterator(raw_vector, self.get_vertex_count())          
    def set_vertex_positions(self, itor: typing.Iterator[virtools_types.VxVector3]) -> None:
        # get raw float pointer and assign
        raw_vector = self._get_pointer_value(bmap.bm_VxVector3_p, bmap.BMMesh_GetVertexPositions)
        _Utils.vxvector3_assigner(raw_vector, self.get_vertex_count(), itor)

    def get_vertex_normals(self) -> typing.Iterator[virtools_types.VxVector3]:
        raw_vector = self._get_pointer_value(bmap.bm_VxVector3_p, bmap.BMMesh_GetVertexNormals)
        return _Utils.vxvector3_iterator(raw_vector, self.get_vertex_count())
    def set_vertex_normals(self, itor: typing.Iterator[virtools_types.VxVector3]) -> None:
        raw_vector = self._get_pointer_value(bmap.bm_VxVector3_p, bmap.BMMesh_GetVertexNormals)
        _Utils.vxvector3_assigner(raw_vector, self.get_vertex_count(), itor)

    def get_vertex_uvs(self) -> typing.Iterator[virtools_types.VxVector2]:
        raw_vector = self._get_pointer_value(bmap.bm_VxVector2_p, bmap.BMMesh_GetVertexUVs)
        return _Utils.vxvector2_iterator(raw_vector, self.get_vertex_count())
    def set_vertex_uvs(self, itor: typing.Iterator[virtools_types.VxVector2]) -> None:
        raw_vector = self._get_pointer_value(bmap.bm_VxVector2_p, bmap.BMMesh_GetVertexUVs)
        _Utils.vxvector2_assigner(raw_vector, self.get_vertex_count(), itor)

    def get_face_count(self) -> int:
        return self._get_integral_value(bmap.bm_CKDWORD, bmap.BMMesh_GetFaceCount)
    def set_face_count(self, count_: int) -> None:
        self._set_integral_value(bmap.bm_CKDWORD, bmap.BMMesh_SetFaceCount, count_)

    def get_face_indices(self) -> typing.Iterator[virtools_types.CKFaceIndices]:
        raw_idx = self._get_pointer_value(bmap.bm_CKWORD_p, bmap.BMMesh_GetFaceIndices)
        return _Utils.ckfaceindices_iterator(raw_idx, self.get_face_count())
    def set_face_indices(self, itor: typing.Iterator[virtools_types.CKFaceIndices]) -> None:
        raw_idx = self._get_pointer_value(bmap.bm_CKWORD_p, bmap.BMMesh_GetFaceIndices)
        _Utils.ckfaceindices_assigner(raw_idx, self.get_face_count(), itor)

    def get_face_material_slot_indexs(self) -> typing.Iterator[int]:
        raw_idx = self._get_pointer_value(bmap.bm_CKWORD_p, bmap.BMMesh_GetFaceMaterialSlotIndexs)
        for i in range(self.get_face_count()):
            yield raw_idx[i]

    def set_face_material_slot_indexs(self, itor: typing.Iterator[int]) -> None:
        raw_idx = self._get_pointer_value(bmap.bm_CKWORD_p, bmap.BMMesh_GetFaceMaterialSlotIndexs)
        try:
            for i in range(self.get_face_count()):
                raw_idx[i] = next(itor)
        except StopIteration:
            _Utils.raise_out_of_length_exception()

    def get_material_slot_count(self) -> int:
        return self._get_integral_value(bmap.bm_CKDWORD, bmap.BMMesh_GetMaterialSlotCount)
    def set_material_slot_count(self, count_: int) -> None:
        self._set_integral_value(bmap.bm_CKDWORD, bmap.BMMesh_SetMaterialSlotCount, count_)

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
        try:
            for i in range(self.get_material_slot_count()):
                idx.value = i
                # analyze mtl item
                mtlobj: BMMaterial | None = next(itor)
                if mtlobj is None:
                    mtlid.value = g_InvalidCKID
                else:
                    mtlid = mtlobj._get_ckid()
                # set
                bmap.BMMesh_SetMaterialSlot(self._get_pointer(), self._get_ckid(), idx, mtlid)
        except StopIteration:
            _Utils.raise_out_of_length_exception()

class BM3dObject(BMObject):
    def get_world_matrix(self) -> virtools_types.VxMatrix:
        mat: bmap.bm_VxMatrix = bmap.bm_VxMatrix()
        bmap.BM3dObject_GetWorldMatrix(self._get_pointer(), self._get_ckid(), ctypes.byref(mat))
        # use cast & pointer to get matrix data conveniently
        flat: bmap.bm_CKFLOAT_p = ctypes.cast(ctypes.byref(mat), bmap.bm_CKFLOAT_p)
        ret: virtools_types.VxMatrix = virtools_types.VxMatrix()
        ret.from_const(tuple(flat[i] for i in range(16)))
        return ret

    def set_world_matrix(self, mat_: virtools_types.VxMatrix) -> None:
        # star syntax expand the tuple as the argument.
        mat: bmap.bm_VxMatrix = bmap.bm_VxMatrix(*(mat_.to_const()))
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
            ckid = mesh._get_ckid()
        bmap.BM3dObject_SetCurrentMesh(self._get_pointer(), self._get_ckid(), ckid)

    def get_visibility(self) -> bool:
        return self._get_bool_value(bmap.BM3dObject_GetVisibility)
    def set_visibility(self, visb_: bool) -> None:
        self._set_bool_value(bmap.BM3dObject_SetVisibility, visb_)

class BMGroup(BMObject):
    def add_object(self, member: BM3dObject) -> None:
        bmap.BMGroup_AddObject(self._get_pointer(), self._get_ckid(), member._get_ckid())

    def get_object_count(self) -> int:
        return self._get_integral_value(bmap.bm_CKDWORD, bmap.BMGroup_GetObjectCount)

    def get_objects(self) -> typing.Iterator[BM3dObject]:
        # get list size
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
            file_name, temp_folder, texture_folder, _g_RawCallback,
            encoding_count, encodings,
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
        count_getter: typing.Callable[[bmap.bm_void_p, bmap.bm_CKDWORD_p], bmap.bm_bool]) -> int:
        # get size
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD()
        count_getter(self._get_pointer(), ctypes.byref(csize))
        return csize.value

    def __get_ckobjects(self, 
        class_type: type[TCKObject],
        count_getter: typing.Callable[[bmap.bm_void_p, bmap.bm_CKDWORD_p], bmap.bm_bool],
        obj_getter: typing.Callable[[bmap.bm_void_p, bmap.bm_CKDWORD, bmap.bm_CKID_p], bmap.bm_bool]) -> typing.Iterator[TCKObject]:
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
        return self.__get_ckobjects(BMTexture, bmap.BMFile_GetTextureCount, bmap.BMFile_GetTexture)
    def get_material_count(self) -> int:
        return self.__get_ckobject_count(bmap.BMFile_GetMaterialCount)
    def get_materials(self) -> typing.Iterator[BMMaterial]:
        return self.__get_ckobjects(BMMaterial, bmap.BMFile_GetMaterialCount, bmap.BMFile_GetMaterial)
    def get_mesh_count(self) -> int:
        return self.__get_ckobject_count(bmap.BMFile_GetMeshCount)
    def get_meshs(self) -> typing.Iterator[BMMesh]:
        return self.__get_ckobjects(BMMesh, bmap.BMFile_GetMeshCount, bmap.BMFile_GetMesh)
    def get_3dobject_count(self) -> int:
        return self.__get_ckobject_count(bmap.BMFile_Get3dObjectCount)
    def get_3dobjects(self) -> typing.Iterator[BM3dObject]:
        return self.__get_ckobjects(BM3dObject, bmap.BMFile_Get3dObjectCount, bmap.BMFile_Get3dObject)
    def get_group_count(self) -> int:
        return self.__get_ckobject_count(bmap.BMFile_GetGroupCount)
    def get_groups(self) -> typing.Iterator[BMGroup]:
        return self.__get_ckobjects(BMGroup, bmap.BMFile_GetGroupCount, bmap.BMFile_GetGroup)
    
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
            temp_folder, texture_folder, _g_RawCallback,
            encoding_count, encodings,
            ctypes.byref(out_file)
        )
        # init self
        _AbstractPointer.__init__(self, out_file)

    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_value, traceback):
        self.dispose()
    
    def save(self, file_name_: str, texture_save_opt_: virtools_types.CK_TEXTURE_SAVEOPTIONS, use_compress_: bool, compress_level_: int) -> None:
        # create param
        file_name: bmap.bm_CKSTRING = bmap.bm_CKSTRING(file_name_.encode(g_BMapEncoding))
        texture_save_opt: bmap.bm_enum = bmap.bm_enum(texture_save_opt_.value)
        use_compress: bmap.bm_bool = bmap.bm_bool(use_compress_)
        compress_level: bmap.bm_CKINT = bmap.bm_CKINT(compress_level_)
        # exec
        bmap.BMFile_Save(self._get_pointer(), file_name, texture_save_opt, use_compress, compress_level)

    def dispose(self) -> None:
        if self._is_valid():
            bmap.BMFile_Free(self._get_pointer())
            self._set_pointer(g_InvalidPtr)

    def __create_ckobject(self, 
        class_type: type[TCKObject],
        creator: typing.Callable[[bmap.bm_void_p, bmap.bm_CKID_p], bmap.bm_bool]) -> TCKObject:
        # prepare id container
        retid: bmap.bm_CKID = bmap.bm_CKID()
        # create new one
        creator(self._get_pointer(), ctypes.byref(retid))
        # return visitor
        return class_type(self._get_pointer(), retid)

    def create_texture(self) -> BMTexture:
        return self.__create_ckobject(BMTexture, bmap.BMFile_CreateTexture)
    def create_material(self) -> BMMaterial:
        return self.__create_ckobject(BMMaterial, bmap.BMFile_CreateMaterial)
    def create_mesh(self) -> BMMesh:
        return self.__create_ckobject(BMMesh, bmap.BMFile_CreateMesh)
    def create_3dobject(self) -> BM3dObject:
        return self.__create_ckobject(BM3dObject, bmap.BMFile_Create3dObject)
    def create_group(self) -> BMGroup:
        return self.__create_ckobject(BMGroup, bmap.BMFile_CreateGroup)
    
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

    def parse(self, objmesh: BMMesh) -> None:
        bmap.BMMeshTrans_Parse(self._get_pointer(), objmesh._get_pointer(), objmesh._get_ckid())

    def prepare_vertex(self, count: int, itor: typing.Iterator[virtools_types.VxVector3]) -> None:
        # prepare count first
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count)
        bmap.BMMeshTrans_PrepareVertexCount(self._get_pointer(), csize)
        # get raw pointer and conv to float ptr for convenient visit
        raw_vector: bmap.bm_VxVector3_p = bmap.bm_VxVector3_p()
        bmap.BMMeshTrans_PrepareVertex(self._get_pointer(), ctypes.byref(raw_vector))
        # set by pointer
        _Utils.vxvector3_assigner(raw_vector, count, itor)
    
    def prepare_normal(self, count: int, itor: typing.Iterator[virtools_types.VxVector3]) -> None:
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count)
        bmap.BMMeshTrans_PrepareNormalCount(self._get_pointer(), csize)
        
        raw_vector: bmap.bm_VxVector3_p = bmap.bm_VxVector3_p()
        bmap.BMMeshTrans_PrepareNormal(self._get_pointer(), ctypes.byref(raw_vector))
        
        _Utils.vxvector3_assigner(raw_vector, count, itor)
    
    def prepare_uv(self, count: int, itor: typing.Iterator[virtools_types.VxVector2]) -> None:
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count)
        bmap.BMMeshTrans_PrepareUVCount(self._get_pointer(), csize)
        
        raw_vector: bmap.bm_VxVector2_p = bmap.bm_VxVector2_p()
        bmap.BMMeshTrans_PrepareUV(self._get_pointer(), ctypes.byref(raw_vector))
        
        _Utils.vxvector2_assigner(raw_vector, count, itor)
    
    def prepare_mtl_slot(self, count: int, itor: typing.Iterator[BMMaterial | None]) -> None:
        csize: bmap.bm_CKDWORD = bmap.bm_CKDWORD(count)
        bmap.BMMeshTrans_PrepareMtlSlotCount(self._get_pointer(), csize)
        
        raw_ckid: bmap.bm_CKID_p = bmap.bm_CKID_p()
        bmap.BMMeshTrans_PrepareMtlSlot(self._get_pointer(), ctypes.byref(raw_ckid))
        try:
            idx: int = 0
            for _ in range(count):
                usermtl: BMMaterial | None = next(itor)
                if usermtl is None:
                    raw_ckid[idx] = g_InvalidCKID
                else:
                    raw_ckid[idx] = usermtl._get_ckid().value
                idx += 1
        except StopIteration:
            _Utils.raise_out_of_length_exception()
    
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
        _Utils.ckfaceindices_assigner(raw_vec_idx, count, vec_idx)
        _Utils.ckfaceindices_assigner(raw_nml_idx, count, nml_idx)
        _Utils.ckfaceindices_assigner(raw_uv_idx, count, uv_idx)
        # assign mtl index
        try:
            idx: int = 0
            for _ in range(count):
                raw_mtl_idx[idx] = next(mtl_idx)
                idx += 1
        except StopIteration:
            _Utils.raise_out_of_length_exception()

#endregion
