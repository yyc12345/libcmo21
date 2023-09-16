#pragma once

#include "../CK2/CKTypes.hpp"
#include <string>
#include <vector>
#include <unordered_map>

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
	+ Do not use CKBOOL, because we want make sure it enable specialized std::vector<bool>.
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


}
