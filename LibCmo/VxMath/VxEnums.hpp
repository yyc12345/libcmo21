#pragma once

#include "../VTUtils.hpp"
#include <cinttypes>
#include <cstdint>

namespace LibCmo::VxMath {
	
	/**
	@brief Pixel format types. 
	@see VxImageDesc2PixelFormat, VxPixelFormat2ImageDesc
	 */
	enum class VX_PIXELFORMAT : CKDWORD {
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
	{filename:VXLIGHT_TYPE}
	Summary: Light type.

	Remarks:
	+ Used by CKLight::SetType to specify the type of a light.
	See also: CKLight::SetType,CKLight::GetType
	 */
	enum class VXLIGHT_TYPE : CKDWORD {
		VX_LIGHTPOINT = 1UL,	/**< The Light is a point of light  */
		VX_LIGHTSPOT = 2UL,	/**< The light is a spotlight  */
		VX_LIGHTDIREC = 3UL,	/**< The light is directional light : Lights comes from an infinite point so only direction of light can be given  */
		VX_LIGHTPARA = 4UL,	/**< Obsolete, do not use  */
	};

	/**
	  Summary: Blend Mode Flags
	Remarks:
		+ The VXTEXTURE_BLENDMODE is used by CKMaterial::SetTextureBlendMode() to specify how
		texture is applied on primitives.
		+ Also used as value for CKRST_TSS_TEXTUREMAPBLEND texture stage state. 
	See Also: Using Materials,CKMaterial,CKTexture,CKMaterial::SetTextureBlendMode,CKRST_TSS_TEXTUREMAPBLEND.
	 */
	enum class VXTEXTURE_BLENDMODE : CKDWORD {
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
	enum class VXTEXTURE_FILTERMODE : CKDWORD {
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
	enum class VXBLEND_MODE : CKDWORD {
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
	enum class VXTEXTURE_ADDRESSMODE : CKDWORD {
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
	enum class VXFILL_MODE : CKDWORD {
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
	enum class VXSHADE_MODE : CKDWORD {
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
	enum class VXCMPFUNC : CKDWORD {
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
		+ Effects provide additionnal functionnalities to take advantage of graphic features such as bump mapping,cube maps etc... 
		+ When an effect is enabled on a material (CKMaterial::SetEffect) it may override the default settings of mesh channels or material blend options
		+ New effects can be created by providing a callback function (see CKRenderManager::AddEffect)
		+ This enumeration provides the list of hardcoded existing effects.
		+ Most of this effect are heavily hardware and device (DX8,DX7,etc..) dependant 
	See also: CKMaterial::SetEffect,CKMaterial::GetEffect,CKRenderManager::AddEffect
	 */
	enum class VX_EFFECT : CKDWORD {
		VXEFFECT_NONE = 0UL,	/**< No Effect  */
		VXEFFECT_TEXGEN = 1UL,	/**< Texture coordinate generation using current viewpoint as referential  */
		VXEFFECT_TEXGENREF = 2UL,	/**< texture generation generation with an optionnal referential  */
		VXEFFECT_BUMPENV = 3UL,	/**< Environment Bump Mapping  */
		VXEFFECT_DP3 = 4UL,	/**< Dot Product 3 bump mapping  */
		VXEFFECT_2TEXTURES = 5UL,	/**< Blend 2 Textures  */
		VXEFFECT_3TEXTURES = 6UL,	/**< Blend 3 Textures  */
		VXEFFECT_MASK = 0xFUL,	/**< Mask for all possible values.  */
	};

	/**
	{filename:VX_MOVEABLE_FLAGS}
	Summary: 3dEntity additionnal flags Options

	Remarks:
		+ The VX_MOVEABLE_FLAGS is used by CK3dEntity::SetMoveableFlags to specify different hints to the render engine about the entity.
		+ The (Engine) flags are set by the render engine and should not be modified by user. They can be checked with the CK3dEntity::GetMoveableFlags method.
		+ The (User) flags are to be set by the user or can be set by a specific method of CK3dEntity.

	See Also: CK3dEntity::SetMoveableFlags 
	 */
	enum class VX_MOVEABLE_FLAGS : CKDWORD {
		VX_MOVEABLE_PICKABLE = 0x00000001,	/**< (User)If not set this entity cannot be returned by CKRenderContext::Pick() or CKRenderContext::RectPict() functions.  */
		VX_MOVEABLE_VISIBLE = 0x00000002,	/**< (Engine) See CKObject::Show,CK3dEntity::IsVisible  */
		VX_MOVEABLE_UPTODATE = 0x00000004,	/**< (Engine) Used to Notify change in the data of the entity.  */
		VX_MOVEABLE_RENDERCHANNELS = 0x00000008,	/**< (User) If not set, additional material channels on the mesh used by this entity won't be rendered (CK3dEntity::SetRenderChannels)  */
		VX_MOVEABLE_USERBOX = 0x00000010,	/**< (Engine) When CK3dEntity::SetBoundingBox is called with a user box, this flag is set.  */
		VX_MOVEABLE_EXTENTSUPTODATE = 0x00000020,	/**< (Engine) Indicate that object 2D extents are up to date  */
		VX_MOVEABLE_BOXVALID = 0x00004000,	/**< (Engine) If not set the moveable has no mesh associated so its bounding box is irrelevant (a point).  */
		VX_MOVEABLE_RENDERLAST = 0x00010000,	/**< (User) If set the moveable will be rendered with the transparent objects (i.e in last) (CK3dEntity::SetRenderAsTransparent)  */
		VX_MOVEABLE_HASMOVED = 0x00020000,	/**< (Engine) Set when its position or orientation has changed. (Reset every frame when rendering starts)  */
		VX_MOVEABLE_WORLDALIGNED = 0x00040000,	/**< (User) Hint for render engine : this object is aligned with world position and orientation.  */
		VX_MOVEABLE_NOZBUFFERWRITE = 0x00080000,	/**< (User) Set by the user to warn Render Engine that this object must not write information to Z buffer  */
		VX_MOVEABLE_RENDERFIRST = 0x00100000,	/**< (User) If set the moveable will be rendered within the firsts objects  */
		VX_MOVEABLE_NOZBUFFERTEST = 0x00200000,	/**< (User) Set by the user to warn Render Engine that this object must not test against Z buffer (This override settings of all materials used by this Entity)  */
		VX_MOVEABLE_INVERSEWORLDMATVALID = 0x00400000,	/**< (Engine) Inverse world matrix is not up to date and should be recomputed  */
		VX_MOVEABLE_DONTUPDATEFROMPARENT = 0x00800000,	/**< (User) This object will not be updated by parent (neither World nor Local matrix wil be updated) . This flags can be used by physic engine for example in which hierarchy is not relevant for physicalised objects  */
		VX_MOVEABLE_INDIRECTMATRIX = 0x01000000,	/**< (User/Engine) Set by the engine at load time  : The object matrix is in left hand referential, culling needs to be inverted  */
		VX_MOVEABLE_ZBUFONLY = 0x02000000,	/**< (User) The object will only be rendered in depth buffer  */
		VX_MOVEABLE_STENCILONLY = 0x04000000,	/**< (User) The object will only be rendered in stencil buffer  */
		VX_MOVEABLE_HIERARCHICALHIDE = 0x10000000,	/**< (Engine) If Object has this flags and is hidden its children won't be rendered  */
		VX_MOVEABLE_CHARACTERRENDERED = 0x20000000,	/**< (Engine) Set if a character was rendered last frame...  */
		VX_MOVEABLE_RESERVED2 = 0x40000000,	/**< (Engine)  */
	};
	/**
	{filename:VXMESH_FLAGS}
	Summary:Mesh Flags Options

	Remarks:
		+ The VXMESH_FLAGS is used by CKMesh::SetFlags to specify different hints to the render engine about the mesh.
		+ Most of this flags can be set or asked using the appropriate method of CKMesh (given between () in the members documentation).
	See Also: CKMesh,CKMesh::SetFlags 
	 */
	enum class VXMESH_FLAGS : CKDWORD {
		VXMESH_BOUNDINGUPTODATE = 0x00000001,	/**< If set the bounding box is up to date (internal).  */
		VXMESH_VISIBLE = 0x00000002,	/**< If not set the mesh will not be rendered (CKMesh::Show)  */
		VXMESH_OPTIMIZED = 0x00000004,	/**< Set by the render engine if the mesh is optimized for rendering. Unset it to force to recreate optimized structures (when changing materials or face organization ) (CKMesh::VertexMove)  */
		VXMESH_RENDERCHANNELS = 0x00000008,	/**< If not set  Additional material channels won't be rendered.  */
		VXMESH_HASTRANSPARENCY = 0x00000010,	/**< If set indicates that one or more of the faces of this mesh use a transparent material (internal)  */
		VXMESH_PRELITMODE = 0x00000080,	/**< If set, no lightning should occur for this mesh, vertex color should be used instead (CKMesh::SetLitMode)  */
		VXMESH_WRAPU = 0x00000100,	/**< Texture coordinates wrapping among u texture coordinates. (CKMesh::SetWrapMode)  */
		VXMESH_WRAPV = 0x00000200,	/**< Texture coordinates wrapping among v texture coordinates. (CKMesh::SetWrapMode)  */
		VXMESH_FORCETRANSPARENCY = 0x00001000,	/**< Forces this mesh to be considered as transparent even if no material is tranparent. (CKMesh::SetTransparent)  */
		VXMESH_TRANSPARENCYUPTODATE = 0x00002000,	/**< If set, the flags VXMESH_HASTRANSPARENCY is up to date. (internal)  */
		VXMESH_UV_CHANGED = 0x00004000,	/**< Must be set if texture coordinates changed to enable the render engine to reconstruct potential display lists or vertex buffers. (CKMesh::UVChanged)  */
		VXMESH_NORMAL_CHANGED = 0x00008000,	/**< Must be set if normal coordinates changed to enable the render engine to reconstruct potential display lists or vertex buffers. (CKMesh::NormalChanged)  */
		VXMESH_COLOR_CHANGED = 0x00010000,	/**< Must be set if colors changed to enable the render engine to reconstruct potential display lists or vertex buffers.	(CKMesh::ColorChanged)  */
		VXMESH_POS_CHANGED = 0x00020000,	/**< Must be set if vertex position changed to enable the render engine to reconstruct potential display lists or vertex buffers. (CKMesh::VertexMove)  */
		VXMESH_HINTDYNAMIC = 0x00040000,	/**< Hint for render engine : Mesh geometry is updated frequently  */
		VXMESH_GENNORMALS = 0x00080000,	/**< Hint : Normals were generated by BuildNormals : Do not save	(internal)  */
		VXMESH_PROCEDURALUV = 0x00100000,	/**< Hint : UVs are generated : Do not save (internal)  */
		VXMESH_PROCEDURALPOS = 0x00200000,	/**< Hint : Vertices postions are generated : Do not save (internal)  */
		VXMESH_STRIPIFY = 0x00400000,	/**< If set the mesh will be stripified.  */
		VXMESH_MONOMATERIAL = 0x00800000,	/**< Set by the render engine if the mesh use only one material.  */
		VXMESH_PM_BUILDNORM = 0x01000000,	/**< Build normals when performing progressive meshing : Do not save (internal)  */
		VXMESH_BWEIGHTS_CHANGED = 0x02000000,	/**< Must be set if vertex blend weights have changed to enable the render engine to reconstruct potential display lists or vertex buffers. (CKMesh::VertexMove)  */
		VXMESH_ALLFLAGS = 0x007FF39F,
	};
	/**
	{filename:VXMESH_LITMODE}
	Summary: Mesh lighting options

	Remarks:
	  + The VXMESH_LITMODE is used by CKMesh::SetLitMode to specify how lighting is done.
	See Also: CKMaterial,CKMesh 
	 */
	enum class VXMESH_LITMODE : CKDWORD {
		VX_PRELITMESH = 0,	/**< Lighting use color information store with vertices  */
		VX_LITMESH = 1,	/**< Lighting is done by renderer using normals and face material information.  */
	};
	/**
	{filename:VXCHANNEL_FLAGS}
	Summary:Mesh additionnal material channel options

	Remarks:
		+ The VXCHANNEL_FLAGS is used by CKMesh::SetChannelFlags to give the behavior of 
		an additional material channel.
	See Also: CKMesh,CKMesh::AddChannel,CKMesh::IsChannelLit,CKMesh::IsChannelActive 
	 */
	enum class VXCHANNEL_FLAGS : CKDWORD {
		VXCHANNEL_ACTIVE = 0x00000001,	/**< This channel is active  */
		VXCHANNEL_SAMEUV = 0x00800000,	/**< This channel should use the texture coordinates of the base mesh.  */
		VXCHANNEL_NOTLIT = 0x01000000,	/**< Additionnal Material Channel should not be lit (some channels may not be rendered in one pass with this option)  */
		VXCHANNEL_MONO = 0x02000000,	/**< Set at runtime by render engine to indicate whether this channel was rendered using multiple pass or not.(Dot Not Modify)  */
		VXCHANNEL_RESERVED1 = 0x04000000,	/**< Reserved for internal use  */
		VXCHANNEL_LAST = 0x08000000,	/**< Set at runtime by render engine to indicate this channel isthe last to be rendered. Dot Not Modify  */
	};
	/**
	{filename:VXTEXTURE_WRAPMODE}
	Summary: Wrapping Flags

	Remarks:
		+ The VXTEXTURE_WRAPMODE is used by CKMesh::SetWrapMode() to specify how
		texture coordinates are interpolated.
	See Also: Using Materials,CKMaterial,CKMesh::SetWrapMode. 
	 */
	enum class VXTEXTURE_WRAPMODE : CKDWORD {
		VXTEXTUREWRAP_NONE = 0x00000000,	/**< Flat texture addressing  */
		VXTEXTUREWRAP_U = 0x00000001,	/**< Vertical	cylinder mapping  */
		VXTEXTUREWRAP_V = 0x00000002,	/**< Horizontal cylinder mapping  */
		VXTEXTUREWRAP_UV = 0x00000003,	/**< Spherical mapping  */
	};

}
