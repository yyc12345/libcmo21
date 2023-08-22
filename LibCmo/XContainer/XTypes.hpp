#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <cinttypes>
#include "../CK2/CKTypes.hpp"

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
	@remark This class now use specialized std::vector<bool>.
	*/
	using XBitArray = std::vector<bool>;

	/**
	@brief Class representation of an array.
	@tparam T Element Type.
	@remark This class now use std::vector<T>.
	@see XClassArray, XSArray
	*/
	template<typename T>
	using XArray = std::vector<T>;
	/**
	@brief Class representation of an array.
	@tparam T Element Type.
	@details Equivalent to XArray becasue memory reducing is useless.
	@see XArray
	*/
	template<typename T>
	using XSArray = XArray<T>;
	/**
	@brief Class representation of an array.
	@tparam T Element Type.
	@details Equivalent to XArray because std::vector<T> can fufill the requirements.
	@see XArray
	*/
	template<typename T>
	using XClassArray = XArray<T>;

	/**
	@brief Container class for CKObject Id's.
	@remark
	+ This class use the template container XArray to contain object CK_ID's.
	+ Supports for Check, Load, Save, Add, Remove, Find functions in the Object CK_ID array.
	@todo May independ this class to implement the functions introduced in remarks.
	@see XObjectPointerArray, CKObjectArray
	*/
	using XObjectArray = XArray<CK2::CK_ID>;
	/**
	@brief Container class for CKObject Id's
	@details Equivalent to XObjectArray becasue memory reducing is useless.
	@see XObjectArray
	*/
	using XSObjectArray = XObjectArray;

	/**
	@brief Container class for CKObject pointers.
	@remark
	+ Exactly same as XObjectArray class, but uses XArray (Pre-allocated items)
	for storing pointers, and not IDs (more efficient to retrieve the objects).
	+ Supports for Check, Load, Save, Add, Remove, Find functions in the CKObject Pointer array.
	@todo May independ this class to implement the functions introduced in remarks.
	@see XObjectArray, CKObjectArray
	*/
	using XObjectPointerArray = XArray<CK2::CKObject*>;
	/**
	@brief Container class for CKObject pointers.
	@details Equivalent to XObjectPointerArray becasue memory reducing is useless.s
	*/
	using XSObjectPointerArray = XObjectPointerArray;

	/**
	@brief Class representation of an Hash Table container.
	@tparam K The type of the key
	@tparam T The type of element to insert
	@tparam H The hash function to hash the key
	@tparam Eq The equal function to the key
	@remark This class now use std::unordered_map<T>.
	*/
	template<class K, class T, class H = std::hash<K>, class Eq = std::equal_to<K>>
	using XHashTable = std::unordered_map<K, T, H, Eq>;
	/**
	@copydoc XHashTable
	@details Equivalent to XHashTable
	@see XHashTable
	*/
	template<class K, class T, class H = std::hash<K>, class Eq = std::equal_to<K>>
	using XNHashTable = XHashTable<K, T, H, Eq>;
	/**
	@copydoc XHashTable
	@details Equivalent to XHashTable because static allocation is useless.
	@see XHashTable
	*/
	template<class K, class T, class H = std::hash<K>, class Eq = std::equal_to<K>>
	using XSHashTable = XHashTable<K, T, H, Eq>;


}
