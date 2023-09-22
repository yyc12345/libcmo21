#pragma once

#include "../CK2/CKTypes.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <list>

/**
 * @brief The X container part of LibCmo.
 * These classes are prefixed with X in original Virtools SDK.
*/
namespace LibCmo::XContainer {

	/**
	@brief Class representation of a string (an array of character ended by NULL).
	@remark This class now use std::string.
	*/
	using XString = std::string;

	/**
	@brief Set of bit flags.
	@remark
	+ This class now use specialized std::vector<bool>.
	*/
	using XBitArray = std::vector<bool>;

	/**
	@brief Class representation of an array.
	@tparam T Element Type.
	@remark 
	+ This class now use std::vector<T>.
	+ XSArray, XClassArray now redirect to this.
	*/
	template<typename T>
	using XArray = std::vector<T>;
	
	/**
	 * @brief Double-linked list.
	 * @tparam T Element Type.
	*/
	template<typename T>
	using XList = std::list<T>;

	/**
	@brief Container class for CKObject Id's.
	@remark
	+ This class use the template container XArray to contain object CK_ID's.
	+ Supports for Check, Load, Save, Add, Remove, Find functions in the Object CK_ID array.
	+ XSObjectArray now redirect to this.
	@todo May independ this class to implement the functions introduced in remarks.
	*/
	using XObjectArray = XArray<CK2::CK_ID>;

	/**
	@brief Container class for CKObject pointers.
	@remark
	+ Exactly same as XObjectArray class, but uses XArray (Pre-allocated items)
	for storing pointers, and not IDs (more efficient to retrieve the objects).
	+ Supports for Check, Load, Save, Add, Remove, Find functions in the CKObject Pointer array.
	+ XSObjectPointerArray now redirect to this.
	@todo May independ this class to implement the functions introduced in remarks.
	@see XObjectArray, CKObjectArray
	*/
	using XObjectPointerArray = XArray<CK2::ObjImpls::CKObject*>;

	/**
	@brief Class representation of an Hash Table container.
	@tparam K The type of the key
	@tparam T The type of element to insert
	@tparam H The hash function to hash the key
	@tparam Eq The equal function to the key
	@remark 
	+ This class now use std::unordered_map<T>.
	+ XNHashTable, XSHashTable now redirect to this.
	*/
	template<class K, class T, class H = std::hash<K>, class Eq = std::equal_to<K>>
	using XHashTable = std::unordered_map<K, T, H, Eq>;

	// ========== Patch Section ==========

	namespace NSXBitArray {
		
		/**
		 * @brief Resize XBitArray to the new size. Initial value is false.
		 * @param ba The XBitArray
		 * @param newsize New Size
		*/
		void Resize(XBitArray& ba, CKDWORD newsize);

		/**
		 * @brief Merge other XBitArray to this XBitArray with OR condition. 
		 * This XBitArray will be resized at least equal with merged XBitArray if its size is lower than merged XBitArray
		 * @param thisba This XBitArray
		 * @param thatba Other XBitArray will be merged.
		*/
		void Or(XBitArray& thisba, const XBitArray& thatba);

		/**
		 * @brief Check whether bit flag is set.
		 * @param ba The XBitArray
		 * @param n The check position.
		 * @return True if set. False if not set or out of range.
		*/
		bool IsSet(const XBitArray& ba, CKDWORD n);
		/**
		 * @brief Set specified position to true in XBitArray. Auto resize if no space to set.
		 * @param ba The XBitArray
		 * @param n The set position.
		*/
		void Set(XBitArray& ba, CKDWORD n);
		/**
		 * @brief Unset specified position to true in XBitArray. If out of range, simply return.
		 * @param ba The XBitArray
		 * @param n The unset position.
		*/
		void Unset(XBitArray& ba, CKDWORD n);

		/**
		* @brief Returns the position of the n-th set(1) bit
		* @return false if not found.
		*/
		bool GetSetBitPosition(const XBitArray& ba, CKDWORD n, CKDWORD& got);
		/**
		* @brief Returns the position of the n-th unset(0) bit
		* @return false if not found.
		*/
		bool GetUnsetBitPosition(const XBitArray& ba, CKDWORD n, CKDWORD& got);

	}

	namespace NSXString {

		/**
		 * @brief Return CKSTRING statement of XString.
		 * @param strl The XString need to be output.
		 * @return Return the CKSTRING format of XString. if XString is blank, return nullptr.
		*/
		CKSTRING ToCKSTRING(const XString& strl);

		/**
		 * @brief Copy CKSTRING to XString.
		 * @param strl The string dest.
		 * @param s The CKSTRING need to be copied. Pass nullptr will clear string dest.
		*/
		void FromCKSTRING(XString& strl, CKSTRING s);

	}

	namespace NSXObjectArray {

		/**
		 * @brief Check Object ID validation and remove invalid IDs before deletion.
		 * @param objarray 
		 * @param ctx 
		*/
		void PreDeletedCheck(XObjectArray& objarray, CK2::CKContext* ctx);
		/**
		 * @brief Check Object ID validation and remove invalid IDs after deletion.
		 * @param objarray 
		 * @param ctx 
		*/
		void PostDeletedCheck(XObjectArray& objarray, CK2::CKContext* ctx);
		
		/**
		 * @brief Check Object pointer validation and remove invalid pointers before deletion.
		 * @param objarray 
		 * @param ctx 
		*/
		void PreDeletedCheck(XList<CK2::CK_ID>& objarray, CK2::CKContext* ctx);
		/**
		 * @brief Check Object pointer validation and remove invalid pointers after deletion.
		 * @param objarray 
		 * @param ctx 
		 * @remark The performance of this function is extremely low. Use it carefully.
		*/
		void PostDeletedCheck(XList<CK2::CK_ID>& objarray, CK2::CKContext* ctx);

	}
	
	namespace NSXObjectPointerArray {

		/**
		 * @brief Inserts the object at the end of the array, if it is not yet present.
		 * @param objarray 
		 * @param obj 
		 * @return True if the object was already present, false otherwise
		*/
		bool AddIfNotHere(XObjectPointerArray& objarray, CK2::ObjImpls::CKObject* obj);

		/**
		 * @brief Check Object pointer validation and remove invalid pointers before deletion.
		 * @param objarray 
		 * @param ctx 
		*/
		void PreDeletedCheck(XObjectPointerArray& objarray, CK2::CKContext* ctx);
		/**
		 * @brief Check Object pointer validation and remove invalid pointers after deletion.
		 * @param objarray 
		 * @param ctx 
		 * @remark The performance of this function is extremely low. Use it carefully.
		*/
		void PostDeletedCheck(XObjectPointerArray& objarray, CK2::CKContext* ctx);

		/**
		 * @brief Check Object pointer validation and remove invalid pointers before deletion.
		 * @param objarray 
		 * @param ctx 
		*/
		void PreDeletedCheck(XList<CK2::ObjImpls::CKObject*>& objarray, CK2::CKContext* ctx);
		/**
		 * @brief Check Object pointer validation and remove invalid pointers after deletion.
		 * @param objarray 
		 * @param ctx 
		 * @remark The performance of this function is extremely low. Use it carefully.
		*/
		void PostDeletedCheck(XList<CK2::ObjImpls::CKObject*>& objarray, CK2::CKContext* ctx);

	}


}
