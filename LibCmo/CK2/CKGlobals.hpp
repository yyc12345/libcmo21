#pragma once

#include "CkDefines.hpp"

namespace LibCmo::CK2 {

	// ========== Compression utilities ==========

	/**
	 * @brief Compress a buffer
	 * @param[in] Data A pointer to the buffer to coompress
	 * @param[in] size Size of the source buffer.
	 * @param[out] NewSize A reference that will be filled with the size of the compressed buffer. 0 if failed.
	 * @param[in] compressionlevel 0-9
	 * @return 
	 * A pointer to the compressed buffer. nullptr if failed.
	 * The return pointer should be freed by `delete[]` manually.
	 * @remark
	 * The size of allocated return value may greater than the passed value of NewSize. 
	 * NewSize only indicate the size of the part storing useful data in return value.
	 * @see CKUnPackData, CKComputeDataCRC
	*/
	void* CKPackData(const void* Data, CKINT size, CKINT& NewSize, CKINT compressionlevel);
	/**
	 * @brief Decompress a buffer
	 * @param[in] DestSize Expected size of the decompressed buffer.
	 * @param[in] SrcBuffer Compressed buffer.
	 * @param[in] SrcSize Size of the compressed buffer.
	 * @return 
	 * A pointer to the decompressed buffer or nullptr if there was a error.
	 * The return pointer should be freed by `delete[]` manually.
	 * @see CKPackData, CKComputeDataCRC
	*/
	void* CKUnPackData(CKINT DestSize, const void* SrcBuffer, CKINT SrcSize);
	/**
	 * @brief Computes a CRC for a buffer.
	 * @param[in] data A pointer to the buffer to create a CRC for.
	 * @param[in] size Size of the source buffer.
	 * @param[in] PreviousCRC 
	 * The first time a CRC is computed this value should be 0, 
	 * but it can be use to compute a single CRC for a several buffers 
	 * by using the currently computed CRC for previous buffers in this value.
	 * @return CRC of the buffer.
	 * @see CKPackData, CKUnPackData
	*/
	CKDWORD CKComputeDataCRC(const void* data, CKINT size, CKDWORD PreviousCRC = 0);

	// ========== CKClass Registration ==========
	void CKClassRegister(CK_CLASSID cid, CK_CLASSID parentCid,
		CKClassCreationFct createFct, CKClassReleaseFct relFct, CKClassNameFct nameFct);

	// ========== Class Hierarchy Management ==========
	CKINT CKGetClassCount();
	const CKClassDesc* CKGetClassDesc(CK_CLASSID cid);
	CKSTRING CKClassIDToString(CK_CLASSID cid);

	CKBOOL CKIsChildClassOf(CK_CLASSID child, CK_CLASSID parent);
	CK_CLASSID CKGetParentClassID(CK_CLASSID child);
	CK_CLASSID CKGetCommonParent(CK_CLASSID cid1, CK_CLASSID cid2);

	// ========== Initializations functions ==========
	CKERROR CKStartUp();
	CKERROR CKShutdown();

}
