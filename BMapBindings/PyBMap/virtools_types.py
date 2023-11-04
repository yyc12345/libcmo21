import typing, enum

ConstVxVector2 = tuple[float, float]
ConstVxVector3 = tuple[float, float, float]
ConstVxVector4 = tuple[float, float, float, float]

class VxVector2():
    x: float
    y: float

    def __init__(self, _x: float = 0.0, _y: float = 0.0):
        self.x = _x
        self.y = _y

    def from_const(self, cv: ConstVxVector2) -> None:
        (self.x, self.y, ) = cv

    def to_const(self) -> ConstVxVector2:
        return (self.x, self.y, )

class VxVector3():
    x: float
    y: float
    z: float

    def __init__(self, _x: float = 0.0, _y: float = 0.0, _z: float = 0.0):
        self.x = _x
        self.y = _y
        self.z = _z

    def from_const(self, cv: ConstVxVector3) -> None:
        (self.x, self.y, self.z) = cv

    def to_const(self) -> ConstVxVector3:
        return (self.x, self.y, self.z)

ConstVxColorRGBA = tuple[float, float, float, float]
ConstVxColorRGB = tuple[float, float, float]

class VxColor():
    """
    The Color struct support RGBA.
    """
    a: float
    r: float
    g: float
    b: float
    def __init__(self, _r: float, _g: float, _b: float, _a: float = 1.0):
        self.r = _r
        self.g = _g
        self.b = _b
        self.a = _a
        self.regulate()

    def to_const_rgba(self) -> ConstVxColorRGBA:
        return (self.r, self.g, self.b, self.a)
    
    def to_const_rgb(self) -> ConstVxColorRGB:
        return (self.r, self.g, self.b)

    def from_const_rgba(self, val: ConstVxColorRGBA) -> None:
        (self.r, self.g, self.b, self.a) = val
        self.regulate()

    def from_const_rgb(self, val: ConstVxColorRGB) -> None:
        (self.r, self.g, self.b) = val
        self.a = 1.0
        self.regulate()

    def clone(self):
        return VxColor(self.r, self.g, self.b, self.a)

    @staticmethod
    def _clamp_factor(val: float) -> float:
        if val > 1.0: return 1.0
        elif val < 0.0: return 0.0
        else: return val

    def regulate(self):
        self.a = VxColor._clamp_factor(self.a)
        self.r = VxColor._clamp_factor(self.r)
        self.g = VxColor._clamp_factor(self.g)
        self.b = VxColor._clamp_factor(self.b)

class VxMatrix():
    """
    The Matrix representation.
    The bracket statement exactly equal with Virtools.
    """
    __mData: list[list[float]]

    def __init__(self):
        # init array
        self.__mData = [[0] * 4 for i in range(4)]
        # set to identy
        self.reset()

    def _get_raw(self) -> list[list[float]]:
        return self.__mData

    def reset(self) -> None:
        # reset to identy
        for i in range(4):
            for j in range(4):
                self.__mData[i][j] = 0.0
        
        self.__mData[0][0] = 1.0
        self.__mData[1][1] = 1.0
        self.__mData[2][2] = 1.0
        self.__mData[3][3] = 1.0

class VXTEXTURE_BLENDMODE(enum.IntEnum):
    """!
    Blend Mode Flags   
    """
    VXTEXTUREBLEND_DECAL = 1  ##< Texture replace any material information
    VXTEXTUREBLEND_MODULATE = 2  ##< Texture and material are combine. Alpha information of the texture replace material alpha component.
    VXTEXTUREBLEND_DECALALPHA = 3  ##< Alpha information in the texture specify how material and texture are combined. Alpha information of the texture replace material alpha component.
    VXTEXTUREBLEND_MODULATEALPHA = 4  ##< Alpha information in the texture specify how material and texture are combined
    VXTEXTUREBLEND_DECALMASK = 5
    VXTEXTUREBLEND_MODULATEMASK = 6
    VXTEXTUREBLEND_COPY = 7  ##< Equivalent to DECAL
    VXTEXTUREBLEND_ADD = 8
    VXTEXTUREBLEND_DOTPRODUCT3 = 9  ##< Perform a Dot Product 3 between texture (normal map) and a referential vector given in VXRENDERSTATE_TEXTUREFACTOR.
    VXTEXTUREBLEND_MAX = 10

class VXTEXTURE_FILTERMODE(enum.IntEnum):
    """!
    Filter Mode Options
    """
    VXTEXTUREFILTER_NEAREST = 1  ##< No Filter
    VXTEXTUREFILTER_LINEAR = 2  ##< Bilinear Interpolation
    VXTEXTUREFILTER_MIPNEAREST = 3  ##< Mip mapping
    VXTEXTUREFILTER_MIPLINEAR = 4  ##< Mip Mapping with Bilinear interpolation
    VXTEXTUREFILTER_LINEARMIPNEAREST = 5  ##< Mip Mapping with Bilinear interpolation between mipmap levels.
    VXTEXTUREFILTER_LINEARMIPLINEAR = 6  ##< Trilinear Filtering
    VXTEXTUREFILTER_ANISOTROPIC = 7  ##< Anisotropic filtering

class VXTEXTURE_ADDRESSMODE(enum.IntEnum):
    """!
    Texture addressing modes.
    """
    VXTEXTURE_ADDRESSWRAP = 1  ##< Default mesh wrap mode is used (see CKMesh::SetWrapMode)
    VXTEXTURE_ADDRESSMIRROR = 2  ##< Texture coordinates outside the range [0..1] are flipped evenly.
    VXTEXTURE_ADDRESSCLAMP = 3  ##< Texture coordinates greater than 1.0 are set to 1.0, and values less than 0.0 are set to 0.0.
    VXTEXTURE_ADDRESSBORDER = 4  ##< When texture coordinates are greater than 1.0 or less than 0.0  texture is set to a color defined in CKMaterial::SetTextureBorderColor.
    VXTEXTURE_ADDRESSMIRRORONCE = 5  ##<

class VXBLEND_MODE(enum.IntEnum):
    """!
    Blending Mode options
    """
    VXBLEND_ZERO = 1  ##< Blend factor is (0, 0, 0, 0).
    VXBLEND_ONE = 2  ##< Blend factor is (1, 1, 1, 1).
    VXBLEND_SRCCOLOR = 3  ##< Blend factor is (Rs, Gs, Bs, As).
    VXBLEND_INVSRCCOLOR = 4  ##< Blend factor is (1-Rs, 1-Gs, 1-Bs, 1-As).
    VXBLEND_SRCALPHA = 5  ##< Blend factor is (As, As, As, As).
    VXBLEND_INVSRCALPHA = 6  ##< Blend factor is (1-As, 1-As, 1-As, 1-As).
    VXBLEND_DESTALPHA = 7  ##< Blend factor is (Ad, Ad, Ad, Ad).
    VXBLEND_INVDESTALPHA = 8  ##< Blend factor is (1-Ad, 1-Ad, 1-Ad, 1-Ad).
    VXBLEND_DESTCOLOR = 9  ##< Blend factor is (Rd, Gd, Bd, Ad).
    VXBLEND_INVDESTCOLOR = 10  ##< Blend factor is (1-Rd, 1-Gd, 1-Bd, 1-Ad).
    VXBLEND_SRCALPHASAT = 11  ##< Blend factor is (f, f, f, 1); f = min(As, 1-Ad).
    #VXBLEND_BOTHSRCALPHA = 12  ##< Source blend factor is (As, As, As, As) and destination blend factor is (1-As, 1-As, 1-As, 1-As)
    #VXBLEND_BOTHINVSRCALPHA = 13  ##< Source blend factor is (1-As, 1-As, 1-As, 1-As) and destination blend factor is (As, As, As, As)

class VXFILL_MODE(enum.IntEnum):
    """!
    Fill Mode Options 
    """
    VXFILL_POINT = 1  ##< Vertices rendering
    VXFILL_WIREFRAME = 2  ##< Edges rendering
    VXFILL_SOLID = 3  ##< Face rendering

class VXSHADE_MODE(enum.IntEnum):
    """!
    Shade Mode Options
    """
    VXSHADE_FLAT = 1  ##< Flat Shading
    VXSHADE_GOURAUD = 2  ##< Gouraud Shading
    VXSHADE_PHONG = 3  ##< Phong Shading (Not yet supported by most implementation)

class VXCMPFUNC(enum.IntEnum):
    """!
    Comparison Function
    """
    VXCMP_NEVER = 1  ##< Always fail the test.
    VXCMP_LESS = 2  ##< Accept if value if less than current value.
    VXCMP_EQUAL = 3  ##< Accept if value if equal than current value.
    VXCMP_LESSEQUAL = 4  ##< Accept if value if less or equal than current value.
    VXCMP_GREATER = 5  ##< Accept if value if greater than current value.
    VXCMP_NOTEQUAL = 6  ##< Accept if value if different than current value.
    VXCMP_GREATEREQUAL = 7  ##< Accept if value if greater or equal current value.
    VXCMP_ALWAYS = 8  ##< Always accept the test.
