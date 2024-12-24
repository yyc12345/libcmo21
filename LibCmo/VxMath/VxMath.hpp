#include "../CK2/CKTypes.hpp"
#include "VxTypes.hpp"

namespace LibCmo::VxMath {

	// ========== Structure copying ==========
	
	/**
	 * @brief Fills a memory buffer with a source buffer pattern.
	 * @details This function can be used to initialized an array of structure when only some members should be modified.
	 * @param[in] Count Number of element to set in the destination buffer
	 * @param[out] Dst Destination buffer
	 * @param[in] Stride Amount in bytes between each element in the destination buffer
	 * @param[in] SizeSrc Size in bytes of an element int the Src buffer
	 * @param[in] Src Source buffer
	 * @exception LogicException Raised if source or destination buffer is nullptr, and count is not zero.
	 * @remarks If given buffer is not sufficient to perform operations, it will cause undefined behavior.
	*/
	void VxFillStructure(CKDWORD Count, void* Dst, CKDWORD Stride, CKDWORD SizeSrc, const void* Src); 
	/**
	 * @brief copies an array of elements between two memory buffers.
	 * @details This function can be used to initialized an array of structure when only some members should be modified.
	 * @param[in] Count Number of element to copy in the destination buffer
	 * @param[out] Dst Destination buffer
	 * @param[in] OutStride Amount in bytes between each element in the destination buffer
	 * @param[in] SizeSrc Size in bytes of an element
	 * @param[in] Src Source buffer.
	 * @param[in] InStride Amount in bytes between each element in the source buffer
	 * @exception LogicException Raised if source or destination buffer is nullptr, and count is not zero.
	 * @remarks If given buffer is not sufficient to perform operations, it will cause undefined behavior.
	*/
	void VxCopyStructure(CKDWORD Count, void* Dst, CKDWORD OutStride, CKDWORD SizeSrc, const void* Src, CKDWORD InStride);
	
	// ========== Graphic Utilities ==========
	
	/**
	 * @brief Performs a blit between two images. This method can resize (shrink or grow) images.
	 * @param[out] dst The destination image.
	 * @param[in] origin The origin image.
	 * @remarks The source image must be in a 32 bit ARGB8888 per pixel format.
	 * @exception LogicException Raised if given image description is nullptr or invalid.
	*/
	void VxDoBlit(const VxImageDescEx* origin, VxImageDescEx* dst);
	/**
	 * @brief Performs a blit between two images. This method can inverts the destination image.
	 * @param[out] dst The destination image.
	 * @param[in] origin The origin image.
	 * @remarks
	 * \li The source image must be in a 32 bit ARGB8888 per pixel format.
	 * \li This function usually used in the covertion between texture coordinate system and UV coordinate system.
	 * @exception LogicException Raised if given image description is nullptr or invalid.
	*/
	void VxDoBlitUpsideDown(const VxImageDescEx* origin, VxImageDescEx* dst);

	/**
	 * @brief Get the total count of set(1) bit in given value.
	 * @param[in] dwMask The value for counting.
	 * @return The total count of set(1) bit.
	*/
	CKDWORD VxGetBitCount(CKDWORD dwMask);
	/**
	 * @brief Get the offset in bit to first set(1) bit when performing right shift.
	 * @param[in] dwMask The value for fetching offset.
	 * @return The offset in bit to first set(1) bit.
	 * If there is no set(1) bit in given value, return 0 instead.
	*/
	CKDWORD VxGetBitShift(CKDWORD dwMask);

	// /**
	// * @brief Scale the integer to a new range.
	// * @param[in] val The int need to be scale
	// * @param[in] srcBitCount The bit count which source integer consumed.
	// * @param[in] dstBitCount The bit count which dest integer consumed.
	// * @remarks This function usually used in image color factor assign with mask.
	// * @return The result integer.
	//*/
	//CKDWORD VxScaleFactor(CKDWORD val, CKDWORD srcBitCount, CKDWORD dstBitCount);

	/**
	 * @brief Sets the alpha component of an image.
	 * @param[in] dst_desc A pointer to a structure describing the destination image format.
	 * @param[in] AlphaValue A CKBYTE value containing the alpha value to set to the whole image
	 * @remarks The source image must be in a 32 bit ARGB8888 per pixel format.
	 * @exception LogicException Raised if given image description is nullptr or invalid.
	*/
	void VxDoAlphaBlit(VxImageDescEx* dst_desc, CKBYTE AlphaValue);
	/**
	 * @brief Sets the alpha component of an image.
	 * @param[in] dst_desc A pointer to a structure describing the destination image format.
	 * @param[in] AlphaValues A BYTE array containing the alpha values for each pixel. This array should be allocated to Width * Height bytes.
	 * @remarks
	 * \li The source image must be in a 32 bit ARGB8888 per pixel format.
	 * \li If given alpha channel buffer do not have correct length, it will cause undefined behavior.
	 * @exception LogicException Raised if given image description or alpha channel buffer is nullptr or invalid.
	*/
	void VxDoAlphaBlit(VxImageDescEx* dst_desc, const CKBYTE* AlphaValues);

}

