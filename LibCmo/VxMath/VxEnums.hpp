#pragma once

#include "../VTUtils.hpp"
#include <cinttypes>
#include <cstdint>

namespace LibCmo::VxMath {
	
	/**
	@brief Pixel format types. 
	@see VxImageDesc2PixelFormat, VxPixelFormat2ImageDesc
	 */
	enum class VX_PIXELFORMAT : uint32_t {
		UNKNOWN_PF = 0,	/**< Unknown pixel format  */
		_32_ARGB8888 = 1,	/**< 32-bit ARGB pixel format with alpha  */
		_32_RGB888 = 2,	/**< 32-bit RGB pixel format without alpha  */
		_24_RGB888 = 3,	/**< 24-bit RGB pixel format  */
		_16_RGB565 = 4,	/**< 16-bit RGB pixel format  */
		_16_RGB555 = 5,	/**< 16-bit RGB pixel format (5 bits per color)  */
		_16_ARGB1555 = 6,	/**< 16-bit ARGB pixel format (5 bits per color + 1 bit for alpha)  */
		_16_ARGB4444 = 7,	/**< 16-bit ARGB pixel format (4 bits per color)  */
		_8_RGB332 = 8,	/**< 8-bit  RGB pixel format  */
		_8_ARGB2222 = 9,	/**< 8-bit  ARGB pixel format  */
		_32_ABGR8888 = 10,	/**< 32-bit ABGR pixel format  */
		_32_RGBA8888 = 11,	/**< 32-bit RGBA pixel format  */
		_32_BGRA8888 = 12,	/**< 32-bit BGRA pixel format  */
		_32_BGR888 = 13,	/**< 32-bit BGR pixel format  */
		_24_BGR888 = 14,	/**< 24-bit BGR pixel format  */
		_16_BGR565 = 15,	/**< 16-bit BGR pixel format  */
		_16_BGR555 = 16,	/**< 16-bit BGR pixel format (5 bits per color)  */
		_16_ABGR1555 = 17,	/**< 16-bit ABGR pixel format (5 bits per color + 1 bit for alpha)  */
		_16_ABGR4444 = 18,	/**< 16-bit ABGR pixel format (4 bits per color)  */
		_DXT1 = 19,	/**< S3/DirectX Texture Compression 1  */
		_DXT2 = 20,	/**< S3/DirectX Texture Compression 2  */
		_DXT3 = 21,	/**< S3/DirectX Texture Compression 3  */
		_DXT4 = 22,	/**< S3/DirectX Texture Compression 4  */
		_DXT5 = 23,	/**< S3/DirectX Texture Compression 5  */
		_16_V8U8 = 24,	/**< 16-bit Bump Map format format (8 bits per color)  */
		_32_V16U16 = 25,	/**< 32-bit Bump Map format format (16 bits per color)  */
		_16_L6V5U5 = 26,	/**< 16-bit Bump Map format format with luminance  */
		_32_X8L8V8U8 = 27,	/**< 32-bit Bump Map format format with luminance  */
		_8_ABGR8888_CLUT = 28,	/**< 8 bits indexed CLUT (ABGR)  */
		_8_ARGB8888_CLUT = 29,	/**< 8 bits indexed CLUT (ARGB)  */
		_4_ABGR8888_CLUT = 30,	/**< 4 bits indexed CLUT (ABGR)  */
		_4_ARGB8888_CLUT = 31,	/**< 4 bits indexed CLUT (ARGB)  */
	};

	/**
	  Summary: Blend Mode Flags
	Remarks:
		+ The VXTEXTURE_BLENDMODE is used by CKMaterial::SetTextureBlendMode() to specify how
		texture is applied on primitives.
		+ Also used as value for CKRST_TSS_TEXTUREMAPBLEND texture stage state. 
	See Also: Using Materials,CKMaterial,CKTexture,CKMaterial::SetTextureBlendMode,CKRST_TSS_TEXTUREMAPBLEND.
	 */
	enum class VXTEXTURE_BLENDMODE : uint32_t {
		VXTEXTUREBLEND_DECAL = 1UL,	/**< Texture replace any material information  */
		VXTEXTUREBLEND_MODULATE = 2UL,	/**< Texture and material are combine. Alpha information of the texture replace material alpha component.  */
		VXTEXTUREBLEND_DECALALPHA = 3UL,	/**< Alpha information in the texture specify how material and texture are combined. Alpha information of the texture replace material alpha component.  */
		VXTEXTUREBLEND_MODULATEALPHA = 4UL,	/**< Alpha information in the texture specify how material and texture are combined  */
		VXTEXTUREBLEND_DECALMASK = 5UL,
		VXTEXTUREBLEND_MODULATEMASK = 6UL,
		VXTEXTUREBLEND_COPY = 7UL,	/**< Equivalent to DECAL  */
		VXTEXTUREBLEND_ADD = 8UL,
		VXTEXTUREBLEND_DOTPRODUCT3 = 9UL,	/**< Perform a Dot Product 3 between texture (normal map) and a referential vector given in VXRENDERSTATE_TEXTUREFACTOR.  */
		VXTEXTUREBLEND_MAX = 10UL,
		VXTEXTUREBLEND_MASK = 0xFUL,
	};

	/**
	  Summary: Filter Mode Options
	Remarks:
		+ The VXTEXTURE_FILTERMODE is used by CKMaterial::SetTextureMagMode and CKMaterial::SetTextureMinMode to specify how
		texture is filtered when magnified.
		+ Also used as value for CKRST_TSS_MAGFILTER and CKRST_TSS_MINFILTER texture stage state. 
	See Also: Using Materials,CKMaterial,CKTexture,CKMaterial::SetTextureMagMode,CKMaterial::SetTextureMinMode,,CKRenderContext::SetTextureStageState
	 */
	enum class VXTEXTURE_FILTERMODE : uint32_t {
		VXTEXTUREFILTER_NEAREST = 1UL,	/**< No Filter  */
		VXTEXTUREFILTER_LINEAR = 2UL,	/**< Bilinear Interpolation  */
		VXTEXTUREFILTER_MIPNEAREST = 3UL,	/**< Mip mapping  */
		VXTEXTUREFILTER_MIPLINEAR = 4UL,	/**< Mip Mapping with Bilinear interpolation  */
		VXTEXTUREFILTER_LINEARMIPNEAREST = 5UL,	/**< Mip Mapping with Bilinear interpolation between mipmap levels.  */
		VXTEXTUREFILTER_LINEARMIPLINEAR = 6UL,	/**< Trilinear Filtering  */
		VXTEXTUREFILTER_ANISOTROPIC = 7UL,	/**< Anisotropic filtering  */
		VXTEXTUREFILTER_MASK = 0xFUL,
	};

	/**
	Summary: Blending Mode options
	Remarks:
		+ The VXBLEND_MODE is used by CKMaterial::SetSourceBlend() and SetDestBlend() to specify the blend 
	factors that are used when blending is enabled. (Rs,Gs,Bs,As) are color components of the source pixel (being drawn) and
	(Rd,Gd,Bd,Ad) are color components of the destination pixel (current pixel on screen).
	When blending is enabled the final pixel will be equal to : 
		
			  SrcBlendFactor * SrcPixel + DstBlendFactor * CurrentPixelOnScreen

		+ Also used as value for VXRENDERSTATE_SRCBLEND and VXRENDERSTATE_DESTBLEND render state. 

	See Also: CKMaterial,CKTexture,CKMaterial::SetSourceBlend,CKMaterial::SetDestBlend,CKRenderContext::SetState,CKSprite::SetBlending,VXRENDERSTATE_SRCBLEND,VXRENDERSTATE_DESTBLEND
	 */
	enum class VXBLEND_MODE : uint32_t {
		VXBLEND_ZERO = 1UL,	/**< Blend factor is (0, 0, 0, 0).  */
		VXBLEND_ONE = 2UL,	/**< Blend factor is (1, 1, 1, 1).  */
		VXBLEND_SRCCOLOR = 3UL,	/**< Blend factor is (Rs, Gs, Bs, As).  */
		VXBLEND_INVSRCCOLOR = 4UL,	/**< Blend factor is (1-Rs, 1-Gs, 1-Bs, 1-As).  */
		VXBLEND_SRCALPHA = 5UL,	/**< Blend factor is (As, As, As, As).  */
		VXBLEND_INVSRCALPHA = 6UL,	/**< Blend factor is (1-As, 1-As, 1-As, 1-As).  */
		VXBLEND_DESTALPHA = 7UL,	/**< Blend factor is (Ad, Ad, Ad, Ad).  */
		VXBLEND_INVDESTALPHA = 8UL,	/**< Blend factor is (1-Ad, 1-Ad, 1-Ad, 1-Ad).  */
		VXBLEND_DESTCOLOR = 9UL,	/**< Blend factor is (Rd, Gd, Bd, Ad).  */
		VXBLEND_INVDESTCOLOR = 10UL,	/**< Blend factor is (1-Rd, 1-Gd, 1-Bd, 1-Ad).  */
		VXBLEND_SRCALPHASAT = 11UL,	/**< Blend factor is (f, f, f, 1); f = min(As, 1-Ad).  */
		VXBLEND_BOTHSRCALPHA = 12UL,	/**< Source blend factor is (As, As, As, As) and destination blend factor is (1-As, 1-As, 1-As, 1-As)  */
		VXBLEND_BOTHINVSRCALPHA = 13UL,	/**< Source blend factor is (1-As, 1-As, 1-As, 1-As) and destination blend factor is (As, As, As, As)  */
		VXBLEND_MASK = 0xFUL,	/**< Source blend factor is (1-As, 1-As, 1-As, 1-As) and destination blend factor is (As, As, As, As)  */
	};

	/**
	  Summary: Texture addressing modes.
	Remarks:
		+ The VXTEXTURE_ADDRESSMODE is used by CKMaterial::SetTextureAddresMode to specify texture coordinate are 
		taken into account when they are outside the range [0.0 , 1.0].
		+ Also used as value for CKRST_TSS_ADDRESS texture stage state. 
	See Also: CKMaterial,CKTexture,CKRST_TSS_ADDRESS,CKRenderContext::SetTextureStageState
	 */
	enum class VXTEXTURE_ADDRESSMODE : uint32_t {
		VXTEXTURE_ADDRESSWRAP = 1UL,	/**< Default mesh wrap mode is used (see CKMesh::SetWrapMode)  */
		VXTEXTURE_ADDRESSMIRROR = 2UL,	/**< Texture coordinates outside the range [0..1] are flipped evenly.  */
		VXTEXTURE_ADDRESSCLAMP = 3UL,	/**< Texture coordinates greater than 1.0 are set to 1.0, and values less than 0.0 are set to 0.0.  */
		VXTEXTURE_ADDRESSBORDER = 4UL,	/**< When texture coordinates are greater than 1.0 or less than 0.0  texture is set to a color defined in CKMaterial::SetTextureBorderColor.  */
		VXTEXTURE_ADDRESSMIRRORONCE = 5UL,	/**<   */
		VXTEXTURE_ADDRESSMASK = 0x7UL,	/**< mask for all values  */
	};
	/**
	  Summary:  Fill Mode Options
	Remarks:
		+ The VXFILL_MODE is used by CKMaterial::SetFillMode to specify how faces are drawn.
		+ Also used as value for VXRENDERSTATE_FILLMODE render state. 
	See Also: CKMaterial::SetFillMode,VXRENDERSTATE_FILLMODE
	 */
	enum class VXFILL_MODE : uint32_t {
		VXFILL_POINT = 1UL,	/**< Vertices rendering  */
		VXFILL_WIREFRAME = 2UL,	/**< Edges rendering  */
		VXFILL_SOLID = 3UL,	/**< Face rendering  */
		VXFILL_MASK = 3UL,
	};
	/**
	Summary: Shade Mode Options
	Remarks:
		+ The VXSHADE_MODE is used by CKMaterial::SetShadeMode to specify how color 
		interpolation is perform on faces when they are drawn.
		+ Also used as value for VXRENDERSTATE_SHADEMODE render state. 
	See Also: CKMaterial::SetShadeMode,VXRENDERSTATE_SHADEMODE
	 */
	enum class VXSHADE_MODE : uint32_t {
		VXSHADE_FLAT = 1UL,	/**< Flat Shading  */
		VXSHADE_GOURAUD = 2UL,	/**< Gouraud Shading  */
		VXSHADE_PHONG = 3UL,	/**< Phong Shading (Not yet supported by most implementation)  */
		VXSHADE_MASK = 3UL,
	};
	/**
	Summary:  Comparison Function
	Remarks:
		+ Used by CKRenderContext::SetState with VXRENDERSTATE_ZFUNC, VXRENDERSTATE_ALPHAFUNC or VXRENDERSTATE_STENCILFUNC
		to specify the type of Z or Alpha comparison function. 
		+ The comparison function is used to compare the stencil,alpha or z reference value to a stencil,z or alpha entry.
	See also: CKRenderContext::SetState,VXRENDERSTATETYPE,VXRENDERSTATE_ZFUNC,VXRENDERSTATE_ALPHAFUNC
	 */
	enum class VXCMPFUNC : uint32_t {
		VXCMP_NEVER = 1UL,	/**< Always fail the test.  */
		VXCMP_LESS = 2UL,	/**< Accept if value if less than current value.  */
		VXCMP_EQUAL = 3UL,	/**< Accept if value if equal than current value.  */
		VXCMP_LESSEQUAL = 4UL,	/**< Accept if value if less or equal than current value.  */
		VXCMP_GREATER = 5UL,	/**< Accept if value if greater than current value.  */
		VXCMP_NOTEQUAL = 6UL,	/**< Accept if value if different than current value.  */
		VXCMP_GREATEREQUAL = 7UL,	/**< Accept if value if greater or equal current value.  */
		VXCMP_ALWAYS = 8UL,	/**< Always accept the test.  */
		VXCMP_MASK = 0xFUL,	/**< Mask for all possible values.  */
	};
	
	/**
	Summary:  Material special effects
	Remarks:
		o Effects provide additionnal functionnalities to take advantage of graphic features such as bump mapping,cube maps etc... 
		o When an effect is enabled on a material (CKMaterial::SetEffect) it may override the default settings of mesh channels or material blend options
		o New effects can be created by providing a callback function (see CKRenderManager::AddEffect)
		o This enumeration provides the list of hardcoded existing effects.
		o Most of this effect are heavily hardware and device (DX8,DX7,etc..) dependant 
	See also: CKMaterial::SetEffect,CKMaterial::GetEffect,CKRenderManager::AddEffect
	 */
	enum class VX_EFFECT : uint32_t {
		VXEFFECT_NONE = 0UL,	/**< No Effect  */
		VXEFFECT_TEXGEN = 1UL,	/**< Texture coordinate generation using current viewpoint as referential  */
		VXEFFECT_TEXGENREF = 2UL,	/**< texture generation generation with an optionnal referential  */
		VXEFFECT_BUMPENV = 3UL,	/**< Environment Bump Mapping  */
		VXEFFECT_DP3 = 4UL,	/**< Dot Product 3 bump mapping  */
		VXEFFECT_2TEXTURES = 5UL,	/**< Blend 2 Textures  */
		VXEFFECT_3TEXTURES = 6UL,	/**< Blend 3 Textures  */
		VXEFFECT_MASK = 0xFUL,	/**< Mask for all possible values.  */
	};

}
