#include "../CK2/CKTypes.hpp"
#include "VxTypes.hpp"

namespace LibCmo::VxMath {

	// ========== Structure copying ==========
	
	/**
	 * @brief Fills a memory buffer with a source buffer pattern.
	 * @param Count[in] Number of element to set in the destination buffer
	 * @param Dst[out] Destination buffer
	 * @param Stride[in] Amount in bytes between each element in the destination buffer
	 * @param SizeSrc[in] Size in bytes of an element int the Src buffer
	 * @param Src[in] Source buffer
	 * @remark This function can be used to initialized an array of structure when only some members should be modified.
	*/
	void VxFillStructure(CKDWORD Count, void* Dst, CKDWORD Stride, CKDWORD SizeSrc, const void* Src); 
	/**
	 * @brief copies an array of elements between two memory buffers.
	 * @param Count[in] Number of element to copy in the destination buffer
	 * @param Dst[out] Destination buffer
	 * @param OutStride[in] Amount in bytes between each element in the destination buffer
	 * @param SizeSrc[in] Size in bytes of an element
	 * @param Src[in] Source buffer.
	 * @param InStride[in] Amount in bytes between each element in the source buffer
	 * @remark This function can be used to initialized an array of structure when only some members should be modified.
	*/
	void VxCopyStructure(CKDWORD Count, void* Dst, CKDWORD OutStride, CKDWORD SizeSrc, const void* Src, CKDWORD InStride);
	
	// ========== Graphic Utilities ==========
	
	/**
	 * @brief Performs a blit between two images. This method can resize (shrink or grow) images.
	 * @remark The source image must be in a 32 bit ARGB8888 per pixel format.
	 * @param dest[out] The dest image.
	 * @param origin[in] The origin image.
	*/
	void VxDoBlit(const VxImageDescEx* origin, VxImageDescEx* dst);
	/**
	 * @brief Performs a blit between two images. This method can inverts the destination image.
	 * @remark 
		+ The source image must be in a 32 bit ARGB8888 per pixel format.
		+ This function usually used in the covertion between texture coordinate
		system and UV coordinate system.
	 * @param dest[out] The dest image.
	 * @param origin[in] The origin image.
	*/
	void VxDoBlitUpsideDown(const VxImageDescEx* origin, VxImageDescEx* dst);

	/**
	 * @brief Counts number of bits to representing a value in dwMask
	*/
	CKDWORD VxGetBitCount(CKDWORD dwMask);
	/**
	 * @brief Counts number of bits to shift to acces a non zero value in dwMask.
	*/
	CKDWORD VxGetBitShift(CKDWORD dwMask);

	///**
	// * @brief scale the integer to a new range.
	// * @param val The int need to be scale
	// * @param srcBitCount The bit count which source integer consumed.
	// * @param dstBitCount The bit count which dest integer consumed.
	// * @remark This function usually used in image color factor assign with mask.
	// * @return The result integer.
	//*/
	//CKDWORD VxScaleFactor(CKDWORD val, CKDWORD srcBitCount, CKDWORD dstBitCount);

	/**
	 * @brief Sets the alpha component of an image.
	 * @remark The source image must be in a 32 bit ARGB8888 per pixel format.
	 * @param dst_desc[in] A pointer to a structure describing the destination image format.
	 * @param AlphaValue[in] A CKBYTE value containing the alpha value to set to the whole image
	 * @remark If the destination image does not have alpha information the function returns immediatly.
	*/
	void VxDoAlphaBlit(VxImageDescEx* dst_desc, CKBYTE AlphaValue);
	/**
	 * @brief Sets the alpha component of an image.
	 * @remark The source image must be in a 32 bit ARGB8888 per pixel format.
	 * @param dst_desc[in] A pointer to a structure describing the destination image format.
	 * @param AlphaValues[in] A BYTE array containing the alpha values for each pixel. This array should be allocated to Width*Height bytes.
	 * @remark If the destination image does not have alpha information the function returns immediatly.
	*/
	void VxDoAlphaBlit(VxImageDescEx* dst_desc, const CKBYTE* AlphaValues);


	// ========== Patch Section ==========

	namespace NSVxVector {

		CKFLOAT DotProduct(const VxVector2& lhs, const VxVector2& rhs);
		CKFLOAT DotProduct(const VxVector3& lhs, const VxVector3& rhs);
		CKFLOAT DotProduct(const VxVector4& lhs, const VxVector4& rhs);

		VxVector3 CrossProduct(const VxVector3& lhs, const VxVector3& rhs);
		
		void Abs(VxVector3& lhs);

	}

}

