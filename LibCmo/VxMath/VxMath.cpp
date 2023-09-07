#include "VxMath.hpp"

namespace LibCmo::VxMath {

#pragma region Structure copying

	void VxFillStructure(CK2::CKDWORD Count, void* Dst, CK2::CKDWORD Stride, CK2::CKDWORD SizeSrc, const void* Src) {
		VxCopyStructure(Count, Dst, Stride, SizeSrc, Src, SizeSrc);
	}

	void VxCopyStructure(CK2::CKDWORD Count, void* Dst, CK2::CKDWORD OutStride, CK2::CKDWORD SizeSrc, const void* Src, CK2::CKDWORD InStride) {
		if (Dst == nullptr || Src == nullptr) return;

		char* cdst = reinterpret_cast<char*>(Dst);
		const char* csrc = reinterpret_cast<const char*>(Src);
		for (CK2::CKDWORD i = 0; i < Count; ++i) {
			std::memcpy(cdst, csrc, SizeSrc);
			cdst += OutStride;
			csrc += InStride;
		}
	}

#pragma endregion

#pragma region Graphic Utilities

	//CK2::CKDWORD VxGetBitCount(CK2::CKDWORD dwMask) {
	//	if (dwMask == 0u) return 0;
	//	dwMask >>= VxGetBitShift(dwMask);
	//	CK2::CKDWORD count = 0;
	//	while ((dwMask & 1u) == 0u) {
	//		dwMask >>= 1u;
	//		++count;
	//	}
	//	return count;
	//}

	//CK2::CKDWORD VxGetBitShift(CK2::CKDWORD dwMask) {
	//	if (dwMask == 0u) return 0;
	//	CK2::CKDWORD count = 0;
	//	while ((dwMask & 1u) != 0u) {
	//		dwMask >>= 1u;
	//		++count;
	//	}
	//	return count;
	//}

	//CK2::CKDWORD VxScaleFactor(CK2::CKDWORD val, CK2::CKDWORD srcBitCount, CK2::CKDWORD dstBitCount) {
	//	if (srcBitCount == dstBitCount) return val;
	//	if (srcBitCount > dstBitCount) {
	//		return val >> (srcBitCount - dstBitCount);
	//	} else {
	//		return val << (dstBitCount - srcBitCount);
	//	}
	//}

	void VxDoAlphaBlit(VxImageDescEx* dst_desc, CK2::CKBYTE AlphaValue) {
		if (dst_desc == nullptr) return;

		CK2::CKDWORD* pixels = dst_desc->GetPixels();
		CK2::CKDWORD pixelcount = dst_desc->GetPixelCount();

		for (CK2::CKDWORD i = 0; i < pixelcount; ++i) {
			*pixels = (*pixels) & 0xFF000000 | AlphaValue;
			++pixels;
		}

		//CK2::CKDWORD* pixels = dst_desc->GetPixels();
		//CK2::CKDWORD pixelcount = dst_desc->GetPixelCount(),
		//	alphashift = VxGetBitShift(dst_desc->m_AlphaMask),
		//	alphacount = VxGetBitCount(dst_desc->m_AlphaMask);

		//CK2::CKDWORD av = VxScaleFactor(AlphaValue, static_cast<CK2::CKDWORD>(sizeof(CK2::CKBYTE) * 8), alphacount) << alphashift;

		//for (CK2::CKDWORD i = 0; i < pixelcount; ++i) {
		//	*pixels = (*pixels) & (~dst_desc->m_AlphaMask) | av;
		//	++pixels;
		//}
	}

	void VxDoAlphaBlit(VxImageDescEx* dst_desc, CK2::CKBYTE* AlphaValues) {
		if (dst_desc == nullptr) return;
		
		CK2::CKDWORD* pixels = dst_desc->GetPixels();
		CK2::CKDWORD pixelcount = dst_desc->GetPixelCount();

		for (CK2::CKDWORD i = 0; i < pixelcount; ++i) {
			*pixels =  (*pixels) & 0xFF000000 | (*AlphaValues);
			++pixels;
			++AlphaValues;
		}
		
		//CK2::CKDWORD* pixels = dst_desc->GetPixels();
		//CK2::CKDWORD pixelcount = dst_desc->GetPixelCount(),
		//	alphashift = VxGetBitShift(dst_desc->m_AlphaMask),
		//	alphacount = VxGetBitCount(dst_desc->m_AlphaMask);

		//for (CK2::CKDWORD i = 0; i < pixelcount; ++i) {
		//	*pixels = (*pixels) & (~dst_desc->m_AlphaMask) | (VxScaleFactor(*AlphaValues, static_cast<CK2::CKDWORD>(sizeof(CK2::CKBYTE) * 8), alphacount) << alphashift);
		//	++pixels;
		//	++AlphaValues;
		//}
	}

#pragma endregion



}
