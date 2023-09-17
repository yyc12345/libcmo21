#pragma once

#include "CKTypes.hpp"
#include "../XContainer/XTypes.hpp"
#include <functional>

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
	void* CKPackData(const void* Data, CKDWORD size, CKDWORD& NewSize, CKINT compressionlevel);
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
	void* CKUnPackData(CKDWORD DestSize, const void* SrcBuffer, CKDWORD SrcSize);
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
	CKDWORD CKComputeDataCRC(const void* data, CKDWORD size, CKDWORD PreviousCRC = 0);

	// ========== String Utilities ==========

	/**
	 * @brief Check whether 2 string is equal. Case senstive.
	 * @param str1[in] String 1
	 * @param str2[in] String 2
	 * @return True if 2 string is equal.
	 * @see CKStrIEqual
	*/
	bool CKStrEqual(CKSTRING str1, CKSTRING str2);
	/**
	 * @brief Check whther 2 string is equal. Case insenstive.
	 * @param str1 
	 * @param str2 
	 * @return True if 2 string is equal.
	 * @see CKStrEqual
	*/
	bool CKStrEqualI(CKSTRING str1, CKSTRING str2);
	/**
	 * @brief Check whether string is empty
	 * @param strl 
	 * @return True if string is empty.
	*/
	bool CKStrEmpty(CKSTRING strl);

	// ========== Class registration utilities ==========

	using CKClassRegisterFct = std::function<void()>;
	using CKClassCreationFct = std::function<ObjImpls::CKObject* (CKContext*, CK_ID, CKSTRING)>;
	using CKClassReleaseFct = std::function<void(CKContext*, ObjImpls::CKObject*)>;
	using CKClassNameFct = std::function<CKSTRING()>;
	//using CKClassDependenciesFct = std::function<CKSTRING(CKINT, CKINT)>;
	//using CKClassDependenciesCountFct = std::function<CKINT(CKINT)>;

	struct CKClassDesc {
		bool IsValid; /**< True if this CKClassDesc is a valid one. Because CK_CLASSID may not be consecutive. */
		bool Done;
		// Initialized upon class registration
		CK_CLASSID Self;
		CK_CLASSID Parent; // Class Identifier of parent class
		CKClassRegisterFct RegisterFct; // Pointer to Class Specific Registration function
		CKClassCreationFct CreationFct; // Pointer to Class instance creation function
		CKClassReleaseFct ReleaseFct; // Pointer to Class instance release function
		CKClassNameFct NameFct; // Pointer to Class name function
		//CKClassDependenciesFct DependsFct; // Pointer to Class dependencies function (Copy,delete,replace...)
		//CKClassDependenciesCountFct DependsCountFct; // Pointer to Class dependencies Count function (Copy,delete,replace...)

		//// Initialized by class specific registration function
		//CKDWORD DefaultOptions; // Default options for this class	
		//CKDWORD DefaultCopyDependencies;
		//CKDWORD DefaultDeleteDependencies;
		//CKDWORD DefaultReplaceDependencies;
		//CKDWORD DefaultSaveDependencies;
		//CKGUID Parameter; // Associated parameter GUID

		// Initialized when building class hierarchy table
		CKINT DerivationLevel; // O => CKObject , etc..
		XContainer::XBitArray Parents; // Bit Mask of parents classes
		XContainer::XBitArray Children;	 // Bit Mask of children classes
		XContainer::XBitArray ToBeNotify; // User specified notify list, only for current class. If any deleted objects match class id in this XBitArray, notify the host of this XBitArray.
		XContainer::XBitArray CommonToBeNotify; // Same as ToBeNotify, but merging all parents' notify list.
		XContainer::XBitArray ToNotify; // The ClassID to notify when an object of this class is deleted (inverse of ToBeNotify)

		CKClassDesc() :
			IsValid(false),
			Done(false),
			Self(CK_CLASSID::CKCID_OBJECT), Parent(CK_CLASSID::CKCID_OBJECT),
			RegisterFct(nullptr), CreationFct(nullptr), ReleaseFct(nullptr), NameFct(nullptr),
			DerivationLevel(0),
			Parents(), Children(), ToBeNotify(), CommonToBeNotify()
		{}
		LIBCMO_DEFAULT_COPY_MOVE(CKClassDesc);
	};

	// ========== CKClass Registration ==========

	void CKClassNeedNotificationFrom(CK_CLASSID listener, CK_CLASSID listenTo);
	CK_CLASSID CKClassGetNewIdentifier();
	void CKClassRegister(CK_CLASSID cid, CK_CLASSID parentCid,
		CKClassRegisterFct regFct, CKClassCreationFct createFct, CKClassReleaseFct relFct, CKClassNameFct nameFct);

	// ========== Class Hierarchy Management ==========

	CKDWORD CKGetClassCount();
	const CKClassDesc* CKGetClassDesc(CK_CLASSID cid);
	CKSTRING CKClassIDToString(CK_CLASSID cid);

	bool CKIsChildClassOf(CK_CLASSID child, CK_CLASSID parent);
	CK_CLASSID CKGetParentClassID(CK_CLASSID child);
	CK_CLASSID CKGetCommonParent(CK_CLASSID cid1, CK_CLASSID cid2);

	/**
	 * @brief Check whether 'listener' need notified by the deletion of 'deletedObjCid'
	 * @param listener 
	 * @param deletedObjCid 
	 * @return true if need notify
	*/
	bool CKIsNeedNotify(CK_CLASSID listener, CK_CLASSID deletedObjCid);
	/**
	 * @brief Get all object cid need to be notified when 'delObjCids' matched objects are deleting.
	 * @param delObjCids 
	 * @param cidCount 
	 * @return 
	*/
	XContainer::XBitArray CKGetAllNotifyClassID(const XContainer::XBitArray& delObjCids);

	// ========== Initializations functions ==========
	CKERROR CKStartUp();
	CKERROR CKShutdown();

}
