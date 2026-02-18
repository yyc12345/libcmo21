//! The module conatins all basic Virtools types used by BMap native FFI calling.

// region: Structures

#[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
#[repr(C)]
#[repr(packed(4))]
pub struct VxVector2 {
    pub x: f32,
    pub y: f32,
}

impl VxVector2 {
    pub fn new() -> Self {
        Self::with_xy(0.0, 0.0)
    }

    pub fn with_xy(x: f32, y: f32) -> Self {
        Self { x, y }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
#[repr(C)]
#[repr(packed(4))]
pub struct VxVector3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

impl VxVector3 {
    pub fn new() -> Self {
        Self::with_xyz(0.0, 0.0, 0.0)
    }

    pub fn with_xyz(x: f32, y: f32, z: f32) -> Self {
        Self { x, y, z }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
#[repr(C)]
#[repr(packed(4))]
pub struct VxColor {
    pub r: f32,
    pub g: f32,
    pub b: f32,
    pub a: f32,
}

impl VxColor {
    pub fn new() -> Self {
        Self::with_rgba(0.0, 0.0, 0.0, 0.0)
    }

    pub fn with_rgba(r: f32, g: f32, b: f32, a: f32) -> Self {
        let mut rv = Self { r, g, b, a };
        rv.regulate();
        return rv;
    }

    pub fn with_rgb(r: f32, g: f32, b: f32) -> Self {
        Self::with_rgba(r, g, b, 1.0)
    }

    pub fn with_dword(val: u32) -> Self {
        let mut rv = Self::new();
        rv.from_dword(val);
        return rv;
    }

    pub fn from_dword(&mut self, mut val: u32) {
        self.b = (val & 0xFF) as f32 / 255.0;
        val >>= 8;
        self.g = (val & 0xFF) as f32 / 255.0;
        val >>= 8;
        self.r = (val & 0xFF) as f32 / 255.0;
        val >>= 8;
        self.a = (val & 0xFF) as f32 / 255.0;
    }

    pub fn to_dword(&self) -> u32 {
        // Make a copy and regulate self first
        let mut copied = self.clone();
        copied.regulate();
        // Build result
        let mut rv = 0;
        rv |= (copied.b * 255.0) as u32;
        rv <<= 8;
        rv |= (copied.g * 255.0) as u32;
        rv <<= 8;
        rv |= (copied.r * 255.0) as u32;
        rv <<= 8;
        rv |= (copied.a * 255.0) as u32;
        return rv;
    }

    fn clamp_factor(factor: f32) -> f32 {
        factor.min(1.0).max(0.0)
    }

    pub fn regulate(&mut self) {
        self.r = Self::clamp_factor(self.r);
        self.g = Self::clamp_factor(self.g);
        self.b = Self::clamp_factor(self.b);
        self.a = Self::clamp_factor(self.a);
    }
}

#[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
#[repr(C)]
#[repr(packed(4))]
pub struct VxMatrix {
    pub data: [[f32; 4]; 4],
}

impl VxMatrix {
    pub fn new() -> Self {
        Self {
            data: [
                [1.0, 0.0, 0.0, 0.0],
                [0.0, 1.0, 0.0, 0.0],
                [0.0, 0.0, 1.0, 0.0],
                [0.0, 0.0, 0.0, 1.0],
            ],
        }
    }

    #[rustfmt::skip]
    pub fn reset(&mut self) {
        self.data[0][0] = 1.0; self.data[0][1] = 0.0; self.data[0][2] = 0.0; self.data[0][3] = 0.0;
        self.data[1][0] = 0.0; self.data[1][1] = 1.0; self.data[1][2] = 0.0; self.data[1][3] = 0.0;
        self.data[2][0] = 0.0; self.data[2][1] = 0.0; self.data[2][2] = 1.0; self.data[2][3] = 0.0;
        self.data[3][0] = 0.0; self.data[3][1] = 0.0; self.data[3][2] = 0.0; self.data[3][3] = 1.0;
    }
}

#[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
#[repr(C)]
#[repr(packed(4))]
pub struct CKFaceIndices {
    pub i1: u32,
    pub i2: u32,
    pub i3: u32,
}

impl CKFaceIndices {
    pub fn new() -> Self {
        Self::with_indices(0, 0, 0)
    }

    pub fn with_indices(i1: u32, i2: u32, i3: u32) -> Self {
        Self { i1, i2, i3 }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
#[repr(C)]
#[repr(packed(4))]
pub struct CKShortFaceIndices {
    pub i1: u16,
    pub i2: u16,
    pub i3: u16,
}

impl CKShortFaceIndices {
    pub fn new() -> Self {
        Self::with_indices(0, 0, 0)
    }

    pub fn with_indices(i1: u16, i2: u16, i3: u16) -> Self {
        Self { i1, i2, i3 }
    }
}

// endregion

// region: Enums

/// Specify the way textures or sprites will be saved
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum CK_TEXTURE_SAVEOPTIONS {
    /// Save raw data inside file. The bitmap is saved in a raw 32 bit per pixel format.
    CKTEXTURE_RAWDATA = 0,
    /// Store only the file name for the texture. The bitmap file must be present in the bitmap pathswhen loading the composition.
    CKTEXTURE_EXTERNAL = 1,
    /// Save using format specified. The bitmap data will be converted to thespecified format by the correspondant bitmap plugin and saved inside file.
    CKTEXTURE_IMAGEFORMAT = 2,
    /// Use Global settings, that is the settings given with CKContext::SetGlobalImagesSaveOptions. (Not valid when using CKContext::SetImagesSaveOptions).
    CKTEXTURE_USEGLOBAL = 3,
    /// Insert original image file inside CMO file. The bitmap file thatwas used originally for the texture or sprite will be append tothe composition file and extracted when the file is loaded.
    CKTEXTURE_INCLUDEORIGINALFILE = 4,
}

/// Pixel format types.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum VX_PIXELFORMAT {
    /// Unknown pixel format
    UNKNOWN_PF = 0,
    /// 32-bit ARGB pixel format with alpha
    _32_ARGB8888 = 1,
    /// 32-bit RGB pixel format without alpha
    _32_RGB888 = 2,
    /// 24-bit RGB pixel format
    _24_RGB888 = 3,
    /// 16-bit RGB pixel format
    _16_RGB565 = 4,
    /// 16-bit RGB pixel format (5 bits per color)
    _16_RGB555 = 5,
    /// 16-bit ARGB pixel format (5 bits per color + 1 bit for alpha)
    _16_ARGB1555 = 6,
    /// 16-bit ARGB pixel format (4 bits per color)
    _16_ARGB4444 = 7,
    /// 8-bit  RGB pixel format
    _8_RGB332 = 8,
    /// 8-bit  ARGB pixel format
    _8_ARGB2222 = 9,
    /// 32-bit ABGR pixel format
    _32_ABGR8888 = 10,
    /// 32-bit RGBA pixel format
    _32_RGBA8888 = 11,
    /// 32-bit BGRA pixel format
    _32_BGRA8888 = 12,
    /// 32-bit BGR pixel format
    _32_BGR888 = 13,
    /// 24-bit BGR pixel format
    _24_BGR888 = 14,
    /// 16-bit BGR pixel format
    _16_BGR565 = 15,
    /// 16-bit BGR pixel format (5 bits per color)
    _16_BGR555 = 16,
    /// 16-bit ABGR pixel format (5 bits per color + 1 bit for alpha)
    _16_ABGR1555 = 17,
    /// 16-bit ABGR pixel format (4 bits per color)
    _16_ABGR4444 = 18,
    /// S3/DirectX Texture Compression 1
    _DXT1 = 19,
    /// S3/DirectX Texture Compression 2
    _DXT2 = 20,
    /// S3/DirectX Texture Compression 3
    _DXT3 = 21,
    /// S3/DirectX Texture Compression 4
    _DXT4 = 22,
    /// S3/DirectX Texture Compression 5
    _DXT5 = 23,
    /// 16-bit Bump Map format format (8 bits per color)
    _16_V8U8 = 24,
    /// 32-bit Bump Map format format (16 bits per color)
    _32_V16U16 = 25,
    /// 16-bit Bump Map format format with luminance
    _16_L6V5U5 = 26,
    /// 32-bit Bump Map format format with luminance
    _32_X8L8V8U8 = 27,
    /// 8 bits indexed CLUT (ABGR)
    _8_ABGR8888_CLUT = 28,
    /// 8 bits indexed CLUT (ARGB)
    _8_ARGB8888_CLUT = 29,
    /// 4 bits indexed CLUT (ABGR)
    _4_ABGR8888_CLUT = 30,
    /// 4 bits indexed CLUT (ARGB)
    _4_ARGB8888_CLUT = 31,
}

/// Light type.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum VXLIGHT_TYPE {
    /// The Light is a point of light
    VX_LIGHTPOINT = 1,
    /// The light is a spotlight
    VX_LIGHTSPOT = 2,
    /// The light is directional light : Lights comes from an infinite point so only direction of light can be given
    VX_LIGHTDIREC = 3,
    // /// Obsolete, do not use
    // VX_LIGHTPARA = 4,
}

/// Blend Mode Flags
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum VXTEXTURE_BLENDMODE {
    /// Texture replace any material information
    VXTEXTUREBLEND_DECAL = 1,
    /// Texture and material are combine. Alpha information of the texture replace material alpha component.
    VXTEXTUREBLEND_MODULATE = 2,
    /// Alpha information in the texture specify how material and texture are combined. Alpha information of the texture replace material alpha component.
    VXTEXTUREBLEND_DECALALPHA = 3,
    /// Alpha information in the texture specify how material and texture are combined
    VXTEXTUREBLEND_MODULATEALPHA = 4,
    VXTEXTUREBLEND_DECALMASK = 5,
    VXTEXTUREBLEND_MODULATEMASK = 6,
    /// Equivalent to DECAL
    VXTEXTUREBLEND_COPY = 7,
    VXTEXTUREBLEND_ADD = 8,
    /// Perform a Dot Product 3 between texture (normal map)and a referential vector given in VXRENDERSTATE_TEXTUREFACTOR.
    VXTEXTUREBLEND_DOTPRODUCT3 = 9,
    VXTEXTUREBLEND_MAX = 10,
    // VXTEXTUREBLEND_MASK = 0xF,
}

/// Filter Mode Options
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum VXTEXTURE_FILTERMODE {
    /// No Filter
    VXTEXTUREFILTER_NEAREST = 1,
    /// Bilinear Interpolation
    VXTEXTUREFILTER_LINEAR = 2,
    /// Mip mapping
    VXTEXTUREFILTER_MIPNEAREST = 3,
    /// Mip Mapping with Bilinear interpolation
    VXTEXTUREFILTER_MIPLINEAR = 4,
    /// Mip Mapping with Bilinear interpolation between mipmap levels.
    VXTEXTUREFILTER_LINEARMIPNEAREST = 5,
    /// Trilinear Filtering
    VXTEXTUREFILTER_LINEARMIPLINEAR = 6,
    /// Anisotropic filtering
    VXTEXTUREFILTER_ANISOTROPIC = 7,
    // VXTEXTUREFILTER_MASK = 0xF,
}

/// Texture addressing modes.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum VXTEXTURE_ADDRESSMODE {
    /// Default mesh wrap mode is used (see CKMesh::SetWrapMode)
    VXTEXTURE_ADDRESSWRAP = 1,
    /// Texture coordinates outside the range [0..1] are flipped evenly.
    VXTEXTURE_ADDRESSMIRROR = 2,
    /// Texture coordinates greater than 1.0 are set to 1.0, and values less than 0.0 are set to 0.0.
    VXTEXTURE_ADDRESSCLAMP = 3,
    /// When texture coordinates are greater than 1.0 or less than 0.0  texture is set to a color defined in CKMaterial::SetTextureBorderColor.
    VXTEXTURE_ADDRESSBORDER = 4,
    /// 
    VXTEXTURE_ADDRESSMIRRORONCE = 5,
    // /// mask for all values
    // VXTEXTURE_ADDRESSMASK = 0x7,
}

/// Blending Mode options
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum VXBLEND_MODE {
    /// Blend factor is (0, 0, 0, 0).
    VXBLEND_ZERO = 1,
    /// Blend factor is (1, 1, 1, 1).
    VXBLEND_ONE = 2,
    /// Blend factor is (Rs, Gs, Bs, As).
    VXBLEND_SRCCOLOR = 3,
    /// Blend factor is (1-Rs, 1-Gs, 1-Bs, 1-As).
    VXBLEND_INVSRCCOLOR = 4,
    /// Blend factor is (As, As, As, As).
    VXBLEND_SRCALPHA = 5,
    /// Blend factor is (1-As, 1-As, 1-As, 1-As).
    VXBLEND_INVSRCALPHA = 6,
    /// Blend factor is (Ad, Ad, Ad, Ad).
    VXBLEND_DESTALPHA = 7,
    /// Blend factor is (1-Ad, 1-Ad, 1-Ad, 1-Ad).
    VXBLEND_INVDESTALPHA = 8,
    /// Blend factor is (Rd, Gd, Bd, Ad).
    VXBLEND_DESTCOLOR = 9,
    /// Blend factor is (1-Rd, 1-Gd, 1-Bd, 1-Ad).
    VXBLEND_INVDESTCOLOR = 10,
    /// Blend factor is (f, f, f, 1); f = min(As, 1-Ad).
    VXBLEND_SRCALPHASAT = 11,
    // /// Source blend factor is (As, As, As, As) and destination blend factor is (1-As, 1-As, 1-As, 1-As)
    // VXBLEND_BOTHSRCALPHA = 12,
    // /// Source blend factor is (1-As, 1-As, 1-As, 1-As) and destination blend factor is (As, As, As, As)
    // VXBLEND_BOTHINVSRCALPHA = 13,
    // /// Source blend factor is (1-As, 1-As, 1-As, 1-As) and destination blend factor is (As, As, As, As)
    // VXBLEND_MASK = 0xF,
}

/// Fill Mode Options
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum VXFILL_MODE {
    /// Vertices rendering
    VXFILL_POINT = 1,
    /// Edges rendering
    VXFILL_WIREFRAME = 2,
    /// Face rendering
    VXFILL_SOLID = 3,
    // VXFILL_MASK = 3,
}

/// Shade Mode Options
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum VXSHADE_MODE {
    /// Flat Shading
    VXSHADE_FLAT = 1,
    /// Gouraud Shading
    VXSHADE_GOURAUD = 2,
    /// Phong Shading (Not yet supported by most implementation)
    VXSHADE_PHONG = 3,
    // VXSHADE_MASK = 3,
}

/// Comparison Function
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum VXCMPFUNC {
    /// Always fail the test.
    VXCMP_NEVER = 1,
    /// Accept if value if less than current value.
    VXCMP_LESS = 2,
    /// Accept if value if equal than current value.
    VXCMP_EQUAL = 3,
    /// Accept if value if less or equal than current value.
    VXCMP_LESSEQUAL = 4,
    /// Accept if value if greater than current value.
    VXCMP_GREATER = 5,
    /// Accept if value if different than current value.
    VXCMP_NOTEQUAL = 6,
    /// Accept if value if greater or equal current value.
    VXCMP_GREATEREQUAL = 7,
    /// Always accept the test.
    VXCMP_ALWAYS = 8,
    // /// Mask for all possible values.
    // VXCMP_MASK = 0xF,
}

/// Mesh lighting options
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum VXMESH_LITMODE {
    /// Lighting use color information store with vertices
    VX_PRELITMESH = 0,
    /// Lighting is done by renderer using normals and face material information.
    VX_LITMESH = 1,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(u32)]
#[non_exhaustive]
#[rustfmt::skip]
#[allow(non_camel_case_types)]
pub enum CK_CAMERA_PROJECTION {
    CK_PERSPECTIVEPROJECTION = 1,
    CK_ORTHOGRAPHICPROJECTION = 2,
}

// endregion
