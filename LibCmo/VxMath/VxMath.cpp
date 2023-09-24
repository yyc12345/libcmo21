#include "VxMath.hpp"
#include "stb_image_resize.h"


namespace LibCmo::VxMath {

#pragma region Structure copying

	void VxFillStructure(CKDWORD Count, void* Dst, CKDWORD Stride, CKDWORD SizeSrc, const void* Src) {
		VxCopyStructure(Count, Dst, Stride, SizeSrc, Src, SizeSrc);
	}

	void VxCopyStructure(CKDWORD Count, void* Dst, CKDWORD OutStride, CKDWORD SizeSrc, const void* Src, CKDWORD InStride) {
		if (Dst == nullptr || Src == nullptr) return;

		CKBYTE* cdst = static_cast<CKBYTE*>(Dst);
		const CKBYTE* csrc = static_cast<const CKBYTE*>(Src);
		for (CKDWORD i = 0; i < Count; ++i) {
			std::memcpy(cdst, csrc, SizeSrc);
			cdst += OutStride;
			csrc += InStride;
		}
	}

#pragma endregion

#pragma region Graphic Utilities

	void VxDoBlit(const VxImageDescEx* origin, VxImageDescEx* dst) {
		if (dst == nullptr || origin == nullptr) return;
		if (!dst->IsValid() || !origin->IsValid()) return;

		// if have same size, directly copy it
		if (dst->IsHWEqual(*origin)) {
			std::memcpy(dst->GetMutableImage(), origin->GetImage(), dst->GetImageSize());
			return;
		}

		// perform resize by stb
		stbir_resize(
			origin->GetImage(), static_cast<int>(origin->GetWidth()), static_cast<int>(origin->GetHeight()), 0,
			dst->GetMutableImage(), static_cast<int>(dst->GetWidth()), static_cast<int>(dst->GetHeight()), 0,
			STBIR_TYPE_UINT8, 4, STBIR_ALPHA_CHANNEL_NONE, 0,	// no alpha channel, mean we treat alpha channel as a normal color factor.
			STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
			STBIR_FILTER_BOX, STBIR_FILTER_BOX,
			STBIR_COLORSPACE_SRGB, nullptr
		);
	}

	void VxDoBlitUpsideDown(const VxImageDescEx* origin, VxImageDescEx* dst) {
		if (dst == nullptr || origin == nullptr) return;
		if (!dst->IsValid() || !origin->IsValid()) return;

		// if size is not matched, return
		if (!dst->IsHWEqual(*origin)) {
			return;
		}

		// copy and swap data by line
		CKDWORD height = dst->GetHeight(),
			rowsize = VxImageDescEx::PixelSize * dst->GetWidth();
		for (CKDWORD row = 0; row < height; ++row) {
			std::memcpy(
				dst->GetMutableImage() + (row * rowsize),
				origin->GetImage() + ((height - row - 1) * rowsize),
				rowsize
			);
		}
	}

	CKDWORD VxGetBitCount(CKDWORD dwMask) {
		if (dwMask == 0u) return 0;
		dwMask >>= VxGetBitShift(dwMask);
		CKDWORD count = 0;
		while ((dwMask & 1u) == 0u) {
			dwMask >>= 1u;
			++count;
		}
		return count;
	}

	CKDWORD VxGetBitShift(CKDWORD dwMask) {
		if (dwMask == 0u) return 0;
		CKDWORD count = 0;
		while ((dwMask & 1u) != 0u) {
			dwMask >>= 1u;
			++count;
		}
		return count;
	}

	//CKDWORD VxScaleFactor(CKDWORD val, CKDWORD srcBitCount, CKDWORD dstBitCount) {
	//	if (srcBitCount == dstBitCount) return val;
	//	if (srcBitCount > dstBitCount) {
	//		return val >> (srcBitCount - dstBitCount);
	//	} else {
	//		return val << (dstBitCount - srcBitCount);
	//	}
	//}

	void VxDoAlphaBlit(VxImageDescEx* dst_desc, CKBYTE AlphaValue) {
		if (dst_desc == nullptr) return;

		CKDWORD* pixels = dst_desc->GetMutablePixels();
		CKDWORD pixelcount = dst_desc->GetPixelCount();

		for (CKDWORD i = 0; i < pixelcount; ++i) {
			*pixels = (*pixels) & 0x00FFFFFF | (static_cast<CKDWORD>(AlphaValue) << 24);
			++pixels;
		}
	}

	void VxDoAlphaBlit(VxImageDescEx* dst_desc, const CKBYTE* AlphaValues) {
		if (dst_desc == nullptr) return;
		
		CKDWORD* pixels = dst_desc->GetMutablePixels();
		CKDWORD pixelcount = dst_desc->GetPixelCount();

		for (CKDWORD i = 0; i < pixelcount; ++i) {
			*pixels =  (*pixels) & 0x00FFFFFF | (static_cast<CKDWORD>(*AlphaValues) << 24);
			++pixels;
			++AlphaValues;
		}
	}

#pragma endregion

#pragma region Patched

	namespace NSVxVector {

		float DotProduct(const VxVector2& lhs, const VxVector2& rhs) {
			return lhs * rhs;
		}
		
		float DotProduct(const VxVector3& lhs, const VxVector3& rhs) {
			return lhs * rhs;
		}

		float DotProduct(const VxVector4& lhs, const VxVector4& rhs) {
			return lhs * rhs;
		}

		VxVector3 CrossProduct(const VxVector3& lhs, const VxVector3& rhs) {
			return VxVector3(
				lhs.y * rhs.z - lhs.z * rhs.y,
				lhs.z * rhs.x - lhs.x * rhs.z,
				lhs.x * rhs.y - lhs.y * rhs.x
			);
		}

	}

#pragma endregion


}
