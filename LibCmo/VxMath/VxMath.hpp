#include "../VTUtils.hpp"
#include "../CK2/CKTypes.hpp"
#include "VxTypes.hpp"

namespace LibCmo::VxMath {

	// ========== Structure copying ==========
	
	/**
	 * @brief Fills a memory buffer with a source buffer pattern.
	 * @param Count[in] Number of element to set in the destination buffer
	 * @param Dst[out] Destination buffer
	 * @param Stride[in] Amount in bytes between each element in the destination buffer
	 * @param SizeSrc[in] Size in bytes (but must be a multiple of 4) of an element int the Src buffer
	 * @param Src[in] Source buffer
	 * @remark This function can be used to initialized an array of structure when only some members should be modified.
	*/
	void VxFillStructure(CK2::CKDWORD Count, void* Dst, CK2::CKDWORD Stride, CK2::CKDWORD SizeSrc, const void* Src); 
	/**
	 * @brief copies an array of elements between two memory buffers.
	 * @param Count[in] Number of element to copy in the destination buffer
	 * @param Dst[out] Destination buffer
	 * @param OutStride[in] Amount in bytes between each element in the destination buffer
	 * @param SizeSrc[in] Size in bytes (but must be a multiple of 4) of an element
	 * @param Src[in] Source buffer.
	 * @param InStride[in] Amount in bytes between each element in the source buffer
	 * @remark This function can be used to initialized an array of structure when only some members should be modified.
	*/
	void VxCopyStructure(CK2::CKDWORD Count, void* Dst, CK2::CKDWORD OutStride, CK2::CKDWORD SizeSrc, const void* Src,CK2::CKDWORD InStride);
	
	// ========== Graphic Utilities ==========
	
	/**
	 * @brief Sets the alpha component of an image.
	 * @param dst_desc[in] A pointer to a structure describing the destination image format.
	 * @param AlphaValue[in] A CKBYTE value containing the alpha value to set to the whole image
	 * @remark If the destination image does not have alpha information the function returns immediatly.
	*/
	void VxDoAlphaBlit(const VxImageDescEx* dst_desc, CK2::CKBYTE AlphaValue);
	/**
	 * @brief Sets the alpha component of an image.
	 * @param dst_desc[in] A pointer to a structure describing the destination image format.
	 * @param AlphaValues[in] A BYTE array containing the alpha values for each pixel. This array should be allocated to Width*Height bytes.
	 * @remark If the destination image does not have alpha information the function returns immediatly.
	*/
	void VxDoAlphaBlit(const VxImageDescEx* dst_desc, CK2::CKBYTE* AlphaValues);

}

