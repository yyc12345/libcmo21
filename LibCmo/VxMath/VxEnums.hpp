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

}
