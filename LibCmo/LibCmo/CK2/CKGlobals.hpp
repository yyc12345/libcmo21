#pragma once

#include "CKTypes.hpp"
#include "../XContainer/XTypes.hpp"
#include <functional>
#include <memory>

namespace LibCmo::CK2 {

	// ========== Compression utilities ==========

	/**
	 * @brief Compress a buffer
	 * @param[in] Data A pointer to the buffer to compress. nullptr is not allowed.
	 * @param[in] size Size of the source buffer.
	 * @param[out] NewSize A reference that will be filled with the size of the compressed buffer. 0 if failed.
	 * @param[in] compressionlevel 0-9 Greater level smaller result size.
	 * @return 
	 * A pointer to the compressed buffer. nullptr if failed.
	 * The return pointer should be freed by \c delete[] manually.
	 * @remarks
	 * The size of allocated return value may greater than the passed value of NewSize. 
	 * NewSize only indicate the size of the part storing useful data in return value.
	 * @exception LogicException Raised if given buffer is nullptr and size is not equal to zero.
	 * @see CKUnPackData(), CKComputeDataCRC()
	*/
	void* CKPackData(const void* Data, CKDWORD size, CKDWORD& NewSize, CKINT compressionlevel);
	/**
	 * @brief Decompress a buffer
	 * @param[in] DestSize Expected size of the decompressed buffer.
	 * @param[in] SrcBuffer Compressed buffer. nullptr is not allowed.
	 * @param[in] SrcSize Size of the compressed buffer.
	 * @return 
	 * A pointer to the decompressed buffer or nullptr if there was a error.
	 * The return pointer should be freed by \c delete[] manually.
	 * @exception LogicException Raised if given buffer is nullptr and size is not equal to zero.
	 * @see CKPackData(), CKComputeDataCRC()
	*/
	void* CKUnPackData(CKDWORD DestSize, const void* SrcBuffer, CKDWORD SrcSize);
	/**
	 * @brief Computes a CRC for a buffer.
	 * @param[in] data A pointer to the buffer to create a CRC for. nullptr is not allowed.
	 * @param[in] size Size of the source buffer.
	 * @param[in] PreviousCRC 
	 * The first time a CRC is computed this value should be 0, 
	 * but it can be use to compute a single CRC for a several buffers 
	 * by using the currently computed CRC for previous buffers in this value.
	 * @return CRC of the buffer.
	 * @exception LogicException Raised if given buffer is nullptr and size is not equal to zero.
	 * @see CKPackData(), CKUnPackData()
	*/
	CKDWORD CKComputeDataCRC(const void* data, CKDWORD size, CKDWORD PreviousCRC = 0);

	// ========== String Utilities ==========

	/**
	 * @brief Check whether two string is equal. Case senstive.
	 * @param[in] str1 First string. nullptr is allowed but not suggested.
	 * @param[in] str2 Second string. nullptr is allowed but not suggested.
	 * @return True if two string is equal, otherwise false.
	 * @remarks
	 * nullptr string is not equal to any other string.
	 * However two nullptr string is equal.
	 * @see CKStrEqualI()
	*/
	bool CKStrEqual(CKSTRING str1, CKSTRING str2);
	/**
	 * @brief Check whther two string is equal. Case insenstive.
	 * @param[in] str1 First string. nullptr is allowed but not suggested.
	 * @param[in] str2 Second string. nullptr is allowed but not suggested.
	 * @return True if two string is equal, otherwise false.
	 * @remarks
	 * nullptr string is not equal to any other string.
	 * However two nullptr string is equal.
	 * @see CKStrEqual()
	*/
	bool CKStrEqualI(CKSTRING str1, CKSTRING str2);
	/**
	 * @brief Check whether string is empty
	 * @param[in] strl String for checking. nullptr is allowed but not suggested.
	 * @return True if string is empty, otherwise false.
	 * @remarks nullptr string is seen as empty string.
	*/
	bool CKStrEmpty(CKSTRING strl);
	/**
	 * @brief Get the length of given string.
	 * @param[in] strl String for getting length. nullptr is allowed but not suggested.
	 * @return String length in UTF8 code unit.
	 * @remarks nullptr string will return 0 instead.
	 * @exception RuntimeException Raised if the length of string exceed the maximum value of CKDWORD.
	*/
	CKDWORD CKStrLen(CKSTRING strl);

	// ========== Class registration utilities ==========

	/// @brief Function pointer which do extra stuff when registry this class.
	using CKClassRegisterFct = std::function<void()>;
	/// @brief Function pointer which create ObjImpls::CKObject pointer by given CKContext, CK_ID and name.
	using CKClassCreationFct = std::function<ObjImpls::CKObject* (CKContext*, CK_ID, CKSTRING)>;
	/// @brief Function pointer which free given ObjImpls::CKObject pointer.
	using CKClassReleaseFct = std::function<void(CKContext*, ObjImpls::CKObject*)>;
	/// @brief Function pointer which return the name of class.
	using CKClassNameFct = std::function<CKSTRING()>;
	//using CKClassDependenciesFct = std::function<CKSTRING(CKINT, CKINT)>;
	//using CKClassDependenciesCountFct = std::function<CKINT(CKINT)>;

	/**
	 * @brief The representation of a registered class.
	*/
	struct CKClassDesc {
		// Variables used when building class hierarchy table
		bool IsValid; /**< True if this CKClassDesc is a valid one. Because CK_CLASSID may not be consecutive. */
		bool Done; /**< Temporary variable indicating this item has been processed when creating table. */

		// Variables initialized upon class registration
		CK_CLASSID Self; /**< Class identifier of self */
		CK_CLASSID Parent; /**< Class identifier of parent class */
		CKClassRegisterFct RegisterFct; /**< Function pointer which do extra stuff when registry this class. */
		CKClassCreationFct CreationFct; /**< Function pointer which create ObjImpls::CKObject pointer by given CKContext, CK_ID and name. */
		CKClassReleaseFct ReleaseFct; /**< Function pointer which free given ObjImpls::CKObject pointer. */
		CKClassNameFct NameFct; /**< Function pointer which return the name of class. */
		//CKClassDependenciesFct DependsFct; // Pointer to Class dependencies function (Copy,delete,replace...)
		//CKClassDependenciesCountFct DependsCountFct; // Pointer to Class dependencies Count function (Copy,delete,replace...)

		//// Initialized by class specific registration function
		//CKDWORD DefaultOptions; // Default options for this class	
		//CKDWORD DefaultCopyDependencies;
		//CKDWORD DefaultDeleteDependencies;
		//CKDWORD DefaultReplaceDependencies;
		//CKDWORD DefaultSaveDependencies;
		//CKGUID Parameter; // Associated parameter GUID

		// Variables initialized after building class hierarchy table
		CKINT DerivationLevel; /**< How many parent level it has. 0 for CKObject, etc.. */
		XContainer::XBitArray Parents; /**< Bit Mask of parents classes */
		XContainer::XBitArray Children;	 /**< Bit Mask of children classes */
		XContainer::XBitArray ToBeNotify; /**< User specified notify list, only for current class. If any deleted objects match class id in this XBitArray, notify the host of this XBitArray. */
		XContainer::XBitArray CommonToBeNotify; /**< Same as ToBeNotify, but merging all parents' notify list. */
		XContainer::XBitArray ToNotify; /**< The ClassID to notify when an object of this class is deleted (inverse of ToBeNotify) */

		CKClassDesc() :
			IsValid(false),
			Done(false),
			Self(CK_CLASSID::CKCID_OBJECT), Parent(CK_CLASSID::CKCID_OBJECT),
			RegisterFct(nullptr), CreationFct(nullptr), ReleaseFct(nullptr), NameFct(nullptr),
			DerivationLevel(0),
			Parents(), Children(), ToBeNotify(), CommonToBeNotify()
		{}
		YYCC_DEF_CLS_COPY_MOVE(CKClassDesc);
	};

	// ========== CKClass Registration ==========

	/**
	 * @brief Order that first class id will be notified when deleting object whose class id is second argument
	 * @param[in] listener The id of class will be notified.
	 * @param[in] listenTo The id of class which first argument interested in.
	 * @remarks If one of given class ids is invalid, this function simply return and do nothing.
	*/
	void CKClassNeedNotificationFrom(CK_CLASSID listener, CK_CLASSID listenTo);
	/**
	 * @brief Get an usable class id for registration.
	 * @details This function is usually used by plugin to registering classes.
	 * Because all embedded Virtools classes has constant class id.
	 * @return An usable class id for registration.
	*/
	CK_CLASSID CKClassGetNewIdentifier();
	/**
	 * @brief Register a new class.
	 * @param[in] cid The id of this class.
	 * @param[in] parentCid The id of parent class.
	 * @param[in] regFct Pointer to class specific registration function which do extra stuff when registry this class.
	 * @param[in] createFct Pointer to class instance creation function
	 * @param[in] relFct Pointer to class instance release function
	 * @param[in] nameFct Pointer to class name function
	*/
	void CKClassRegister(CK_CLASSID cid, CK_CLASSID parentCid,
		CKClassRegisterFct regFct, CKClassCreationFct createFct, CKClassReleaseFct relFct, CKClassNameFct nameFct);

	// ========== Class Hierarchy Management ==========

	/**
	 * @brief Get total count of registered classes.
	 * @return The total count of registered classes.
	*/
	CKDWORD CKGetClassCount();
	/**
	 * @brief Get the class description struct by given class id.
	 * @param[in] cid Class id for fetching.
	 * @return The pointer to corresponding class description.
	 * If given class id is invalid, this function will return nullptr.
	 * According to this, caller can utilize this function to validate class id.
	*/
	const CKClassDesc* CKGetClassDesc(CK_CLASSID cid);
	/**
	 * @brief Get the name representation of given class id.
	 * @param[in] cid Class id for fetching.
	 * @return The name of given class id.
	 * If given class id is invalid, it return a predefined name.
	*/
	CKSTRING CKClassIDToString(CK_CLASSID cid);

	/**
	 * @brief Check whether first class is second class' child class.
	 * @param[in] child The id of first class assumed as child class.
	 * @param[in] parent The id of second class assumed as parent class.
	 * @return True if relation is satisfied, otherwise false.
	 * If one of given class ids is invalid, this function always return false.
	*/
	bool CKIsChildClassOf(CK_CLASSID child, CK_CLASSID parent);
	/**
	 * @brief Get the parent class id of given class id.
	 * @param[in] child The id to class which need to find parent class.
	 * @return The parent class id.
	 * If given class id is invalid, this function always return the class id of CKObject.
	*/
	CK_CLASSID CKGetParentClassID(CK_CLASSID child);
	/**
	 * @brief Get the cloest common parent of given two classes.
	 * @param[in] cid1 The id of first class finding common parent.
	 * @param[in] cid2 The id of second class finding common parent.
	 * @return The cloest common parent class id.
	*/
	CK_CLASSID CKGetCommonParent(CK_CLASSID cid1, CK_CLASSID cid2);

	/**
	 * @brief Check whether object whose class id is first argument, need to be notified when the objects whose class id is second argument, is deleting.
	 * @param[in] listener The class id of checking whether need to be notified.
	 * @param[in] deletedObjCid The class id of deleting object.
	 * @return True if it need to be notified, otherwise false.
	 * If the class id of checking is invalid, this function always return false.
	*/
	bool CKIsNeedNotify(CK_CLASSID listener, CK_CLASSID deletedObjCid);
	/**
	 * @brief Get all class ids need to be notified when objects whose class id included in \c delObjCids are deleting.
	 * @param[in] delObjCids The bit array representing class ids which need to be queried.
	 * @return The queried bit array representing class ids need to be notified.
	 * @see CKIsNeedNotify()
	*/
	XContainer::XBitArray CKGetAllNotifyClassID(const XContainer::XBitArray& delObjCids);

	// ========== Initializations Functions ==========

	/**
	 * @brief Initialize CK engine.
	 * @details You must call this function before anything.
	 * @return Error code about initializing.
	 * @see CKShutdown()
	*/
	CKERROR CKStartUp();
	/**
	 * @brief Shutdown CK engine.
	 * @details You must call this function after you have done all things.
	 * @return Error code about shutdown.
	 * @see CKStartUp()
	*/
	CKERROR CKShutdown();

}
