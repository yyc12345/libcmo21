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

ConstCKFaceIndices = tuple[int, int, int]

class CKFaceIndices():
    i1: int
    i2: int
    i3: int

    def __init__(self, i1_: int = 0, i2_: int = 0, i3_: int = 0):
        self.i1 = i1_
        self.i2 = i2_
        self.i3 = i3_

    def from_const(self, cv: ConstCKFaceIndices) -> None:
        (self.i1, self.i2, self.i3) = cv

    def to_const(self) -> ConstCKFaceIndices:
        return (self.i1, self.i2, self.i3)

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
    def __init__(self, _r: float = 0.0, _g: float = 0.0, _b: float = 0.0, _a: float = 1.0):
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

    def from_dword(self, val: int) -> None:
        self.b = float(val & 0xFF) / 255.0
        val >>= 8
        self.g = float(val & 0xFF) / 255.0
        val >>= 8
        self.r = float(val & 0xFF) / 255.0
        val >>= 8
        self.a = float(val & 0xFF) / 255.0
        val >>= 8
        
    def to_dword(self) -> int:
        # regulate self
        self.regulate()
        # construct value
        val: int = 0
        val |= int(self.a * 255)
        val <<= 8
        val |= int(self.r * 255)
        val <<= 8
        val |= int(self.g * 255)
        val <<= 8
        val |= int(self.b * 255)
        return val

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

ConstVxMatrix = tuple[
    float, float, float, float,
    float, float, float, float,
    float, float, float, float,
    float, float, float, float
]

class VxMatrix():
    """
    The Matrix representation.
    The bracket statement exactly equal with Virtools.
    """
    data: list[list[float]]

    def __init__(self):
        # init array
        self.data = [[0] * 4 for i in range(4)]
        # set to identy
        self.reset()

    def _get_raw(self) -> list[list[float]]:
        return self.data

    def reset(self) -> None:
        # reset to identy
        for i in range(4):
            for j in range(4):
                self.data[i][j] = 0.0
        
        self.data[0][0] = 1.0
        self.data[1][1] = 1.0
        self.data[2][2] = 1.0
        self.data[3][3] = 1.0

    def from_const(self, cm: ConstVxMatrix) -> None:
        (
            self.data[0][0], self.data[0][1], self.data[0][2], self.data[0][3],
            self.data[1][0], self.data[1][1], self.data[1][2], self.data[1][3],
            self.data[2][0], self.data[2][1], self.data[2][2], self.data[2][3],
            self.data[3][0], self.data[3][1], self.data[3][2], self.data[3][3]
        ) = cm

    def to_const(self) -> ConstVxMatrix:
        return (
            self.data[0][0], self.data[0][1], self.data[0][2], self.data[0][3],
            self.data[1][0], self.data[1][1], self.data[1][2], self.data[1][3],
            self.data[2][0], self.data[2][1], self.data[2][2], self.data[2][3],
            self.data[3][0], self.data[3][1], self.data[3][2], self.data[3][3]
        )
    
class CK_TEXTURE_SAVEOPTIONS(enum.IntEnum):
    """!
    Specify the way textures or sprites will be saved
    """
    CKTEXTURE_RAWDATA = 0    ##< Save raw data inside file. The bitmap is saved in a raw 32 bit per pixel format. 
    CKTEXTURE_EXTERNAL = 1    ##< Store only the file name for the texture. The bitmap file must be present in the bitmap paths when loading the composition. 
    CKTEXTURE_IMAGEFORMAT = 2    ##< Save using format specified. The bitmap data will be converted to the specified format by the correspondant bitmap plugin and saved inside file. 
    CKTEXTURE_USEGLOBAL = 3    ##< Use Global settings, that is the settings given with CKContext::SetGlobalImagesSaveOptions. (Not valid when using CKContext::SetImagesSaveOptions). 
    CKTEXTURE_INCLUDEORIGINALFILE = 4    ##< Insert original image file inside CMO file. The bitmap file that was used originally for the texture or sprite will be append to the composition file and extracted when the file is loaded. 

class VX_PIXELFORMAT(enum.IntEnum):
    """!
    Pixel format types.
    """
    #UNKNOWN_PF = 0    ##< Unknown pixel format 
    _32_ARGB8888 = 1    ##< 32-bit ARGB pixel format with alpha 
    _32_RGB888 = 2    ##< 32-bit RGB pixel format without alpha 
    _24_RGB888 = 3    ##< 24-bit RGB pixel format 
    _16_RGB565 = 4    ##< 16-bit RGB pixel format 
    _16_RGB555 = 5    ##< 16-bit RGB pixel format (5 bits per color) 
    _16_ARGB1555 = 6    ##< 16-bit ARGB pixel format (5 bits per color + 1 bit for alpha) 
    _16_ARGB4444 = 7    ##< 16-bit ARGB pixel format (4 bits per color) 
    _8_RGB332 = 8    ##< 8-bit  RGB pixel format 
    _8_ARGB2222 = 9    ##< 8-bit  ARGB pixel format 
    _32_ABGR8888 = 10    ##< 32-bit ABGR pixel format 
    _32_RGBA8888 = 11    ##< 32-bit RGBA pixel format 
    _32_BGRA8888 = 12    ##< 32-bit BGRA pixel format 
    _32_BGR888 = 13    ##< 32-bit BGR pixel format 
    _24_BGR888 = 14    ##< 24-bit BGR pixel format 
    _16_BGR565 = 15    ##< 16-bit BGR pixel format 
    _16_BGR555 = 16    ##< 16-bit BGR pixel format (5 bits per color) 
    _16_ABGR1555 = 17    ##< 16-bit ABGR pixel format (5 bits per color + 1 bit for alpha) 
    _16_ABGR4444 = 18    ##< 16-bit ABGR pixel format (4 bits per color) 
    _DXT1 = 19    ##< S3/DirectX Texture Compression 1 
    _DXT2 = 20    ##< S3/DirectX Texture Compression 2 
    _DXT3 = 21    ##< S3/DirectX Texture Compression 3 
    _DXT4 = 22    ##< S3/DirectX Texture Compression 4 
    _DXT5 = 23    ##< S3/DirectX Texture Compression 5 
    _16_V8U8 = 24    ##< 16-bit Bump Map format format (8 bits per color) 
    _32_V16U16 = 25    ##< 32-bit Bump Map format format (16 bits per color) 
    _16_L6V5U5 = 26    ##< 16-bit Bump Map format format with luminance 
    _32_X8L8V8U8 = 27    ##< 32-bit Bump Map format format with luminance 
    _8_ABGR8888_CLUT = 28    ##< 8 bits indexed CLUT (ABGR) 
    _8_ARGB8888_CLUT = 29    ##< 8 bits indexed CLUT (ARGB) 
    _4_ABGR8888_CLUT = 30    ##< 4 bits indexed CLUT (ABGR) 
    _4_ARGB8888_CLUT = 31    ##< 4 bits indexed CLUT (ARGB) 

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
    
class VXMESH_LITMODE(enum.IntEnum):
    """!
    {filename:VXMESH_LITMODE}
    Summary: Mesh lighting options
    
    Remarks:
    + The VXMESH_LITMODE is used by CKMesh::SetLitMode to specify how lighting is done.
    See Also: CKMaterial,CKMesh
    """
    VX_PRELITMESH = 0    ##< Lighting use color information store with vertices 
    VX_LITMESH = 1    ##< Lighting is done by renderer using normals and face material information. 