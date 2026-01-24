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
	 * @brief The representation of an UTF8 string (an array of UTF8 character ended by a '\0').
	 * @remarks
	 * \li This class now is implemented by \c std::u8string.
	*/
	using XString = std::u8string;
	using XOrdinaryString = std::string;

	/**
	 * @brief The representation of a set of bit flags (memory optimized to reduce occupied size).
	 * @remarks 
	 * \li This class is implemented by specialized \c std::vector<bool>.
	 * \li This class define a set of bit flags that may be treated as a virtual array but are stored in an efficient manner.
	 * \li The class has methods to set, clear and return the i-th bit, resize the array, etc.
	*/
	using XBitArray = std::vector<bool>;

	/**
	 * @brief The representation of an array.
	 * @tparam T Element type.
	 * @remarks
	 * \li This class now use \c std::vector<T>.
	 * \li XSArray now redirect to this. Because XSArray behaves exactly like the XArray,
	 * but it only occupies less memory. This feature also can be done by executing \c std::vector<T>::shrink_to_fit().
	 * \li XClassArray now redirect to this. Because it is designed to hold structure or class 
	 * which have something specific to do on the construction, deletion or recopy.
	 * However this feature can be resolved by writing proper ctor, copy ctor, move ctor and dtor.
	*/
	template<typename T>
	using XArray = std::vector<T>;
	
	/**
	 * @brief The representation of an double-linked list.
	 * @tparam T Element type.
	*/
	template<typename T>
	using XList = std::list<T>;

	/**
	 * @brief The representation of an CKObject ID array.
	 * @remarks
	 * \li This class is implemented as \c XArray<CK2::CK_ID>
	 * \li Supports for Check, Load, Save, Add, Remove, Find functions in the Object CK_ID array.
	 * \li XSObjectArray now redirect to this. See the relation between XArray and XSArray for more reason.
	 * \li CKObjectArray now redirect to this according to its requirements (use pointer or use ID).
	 * @see XObjectPointerArray
	 * @todo Add implementations of functions introduced in remarks to the patch namespace.
	*/
	using XObjectArray = XArray<CK2::CK_ID>;

	/**
	 * @brief The representation of an CKObject pointer array.
	 * @remarks
	 * \li This class is implemented as \c XArray<CK2::ObjImpls::CKObject*>
	 * \li Exactly same as XObjectArray class, but stores pointers, not IDs (more efficient to retrieve the objects).
	 * \li Supports for Check, Load, Save, Add, Remove, Find functions in the CKObject pointer array.
	 * \li XSObjectPointerArray now redirect to this. See the relation between XArray and XSArray for more reason.
	 * \li CKObjectArray now redirect to this according to its requirements (use pointer or use ID).
	 * @see XObjectArray
	 * @todo Add implementations of functions introduced in remarks to the patch namespace.
	*/
	using XObjectPointerArray = XArray<CK2::ObjImpls::CKObject*>;

	/**
	 * @brief The representation of an hash table container.
	 * @tparam K The type of the key
	 * @tparam T The type of element to insert
	 * @tparam H The hash function to hash the key
	 * @tparam Eq The equal function to the key
	 * @remarks
	 * \li This class now is implemented by \c std::unordered_map<K,T,H,Eq>.
	 * \li XNHashTable now redirect to this. Because it seems like the old version of XHashTable.
	 * \li XSHashTable now redirect to this. See the relation between XArray and XSArray for more reason.
	*/
	template<class K, class T, class H = std::hash<K>, class Eq = std::equal_to<K>>
	using XHashTable = std::unordered_map<K, T, H, Eq>;

	// ========== Patch Section ==========

	/**
	 * @brief The patch namespace for XBitArray
	 * @details This namespace provides XBitArray functions which presented in original Virtools SDK,
	 * because XBitArray use standard library container as its implementation.
	*/
	namespace NSXBitArray {
		
		/**
		 * @brief Resize XBitArray to the new size. Initial value is false.
		 * @param[in] ba The XBitArray to be resized.
		 * @param[in] newsize New size (the count of bit flags, \b not the space in byte).
		*/
		void Resize(XBitArray& ba, CKDWORD newsize);

		/**
		 * @brief Merge 2 XBitArray like performing <TT>thisba | thatba</TT> for each flags.
		 * @details
		 * This XBitArray will be resized at least equal with merged XBitArray if its size is lower than merged XBitArray
		 * @param[in] thisba The merging XBitArray.
		 * If this XBitArray will be resized at least equal with merged XBitArray before merging.
		 * @param[in] thatba Other XBitArray will be merged.
		*/
		void Or(XBitArray& thisba, const XBitArray& thatba);

		/**
		 * @brief Check whether bit flag is set.
		 * @param[in] ba The XBitArray for checking
		 * @param[in] n The position for checking.
		 * @return True if set. False if not set or out of range.
		*/
		bool IsSet(const XBitArray& ba, CKDWORD n);
		/**
		 * @brief Set specified position to true in XBitArray. Auto resize if no space to set.
		 * @param[in] ba The XBitArray for setting.
		 * @param[in] n The position for setting.
		*/
		void Set(XBitArray& ba, CKDWORD n);
		/**
		 * @brief Unset specified position to true in XBitArray. Do nothing if out of range.
		 * @param[in] ba The XBitArray for unsetting.
		 * @param[in] n The position for unsetting.
		*/
		void Unset(XBitArray& ba, CKDWORD n);

		/**
		 * @brief Find the n-th bit which is set(1) bit from the lowest position.
		 * @param[in] ba The The XBitArray for finding.
		 * @param[in] n N-th
		 * @param[out] got The position we found.
		 * @return True if we found, otherwise false.
		*/
		bool GetSetBitPosition(const XBitArray& ba, CKDWORD n, CKDWORD& got);
		/**
		 * @brief Find the n-th bit which is unset(0) bit from the lowest position.
		 * @param[in] ba The The XBitArray for finding.
		 * @param[in] n N-th
		 * @param[out] got The position we found.
		 * @return True if we found, otherwise false.
		*/
		bool GetUnsetBitPosition(const XBitArray& ba, CKDWORD n, CKDWORD& got);

	}
	
	/**
	 * @brief The patch namespace for XString
	 * @details This namespace provides XString functions which presented in original Virtools SDK,
	 * because XString use standard library container as its implementation.
	*/
	namespace NSXString {

		/**
		 * @brief Return CKSTRING representation of XString.
		 * @param[in] strl The XString need to be output.
		 * @return Return the CKSTRING representation of XString. 
		 * If XString is blank, return nullptr instead.
		*/
		CKSTRING ToCKSTRING(const XString& strl);

		/**
		 * @brief Copy CKSTRING representation to XString.
		 * @param[in] strl The string dest.
		 * @param[in] s The CKSTRING need to be copied. nullptr will clear string dest.
		*/
		void FromCKSTRING(XString& strl, CKSTRING s);

	}
	
	/**
	 * @brief The patch namespace for XObjectArray
	 * @details This namespace provides XObjectArray functions which presented in original Virtools SDK,
	 * because XObjectArray use standard library container as its implementation.
	*/
	namespace NSXObjectArray {

		/**
		 * @brief Check Object ID validation and remove invalid IDs before deletion.
		 * @param[in] objarray The array to be checked.
		 * @param[in] ctx Associated CKContext.
		*/
		void PreDeletedCheck(XObjectArray& objarray, CK2::CKContext* ctx);
		/**
		 * @brief Check Object ID validation and remove invalid IDs after deletion.
		 * @param[in] objarray The array to be checked.
		 * @param[in] ctx Associated CKContext.
		 * @warning The performance of this function is extremely bad. Use this carefully.
		*/
		void PostDeletedCheck(XObjectArray& objarray, CK2::CKContext* ctx);
		
		/**
		 * @brief Check Object pointer validation and remove invalid pointers before deletion.
		 * @param[in] objarray The array to be checked.
		 * @param[in] ctx Associated CKContext.
		*/
		void PreDeletedCheck(XList<CK2::CK_ID>& objarray, CK2::CKContext* ctx);
		/**
		 * @brief Check Object pointer validation and remove invalid pointers after deletion.
		 * @param[in] objarray The array to be checked.
		 * @param[in] ctx Associated CKContext.
		 * @warning The performance of this function is extremely bad. Use this carefully.
		*/
		void PostDeletedCheck(XList<CK2::CK_ID>& objarray, CK2::CKContext* ctx);

	}
	
	/**
	 * @brief The patch namespace for XObjectPointer
	 * @details This namespace provides XObjectPointer functions which presented in original Virtools SDK,
	 * because XObjectPointer use standard library container as its implementation.
	*/
	namespace NSXObjectPointerArray {

		/**
		 * @brief Inserts the object at the end of the array, if it is not yet present.
		 * @param[in] objarray The array for adding.
		 * @param[in] obj The CKObject to be added.
		 * @return True if the object was already present, false otherwise
		*/
		bool AddIfNotHere(XObjectPointerArray& objarray, CK2::ObjImpls::CKObject* obj);

		/**
		 * @brief Check Object pointer validation and remove invalid pointers before deletion.
		 * @param objarray The array to be checked.
		 * @param ctx Associated CKContext.
		*/
		void PreDeletedCheck(XObjectPointerArray& objarray, CK2::CKContext* ctx);
		/**
		 * @brief Check Object pointer validation and remove invalid pointers after deletion.
		 * @param objarray The array to be checked.
		 * @param ctx Associated CKContext.
		 * @warning The performance of this function is extremely bad. Use this carefully.
		*/
		void PostDeletedCheck(XObjectPointerArray& objarray, CK2::CKContext* ctx);

		/**
		 * @brief Check Object pointer validation and remove invalid pointers before deletion.
		 * @param objarray The array to be checked.
		 * @param ctx Associated CKContext.
		*/
		void PreDeletedCheck(XList<CK2::ObjImpls::CKObject*>& objarray, CK2::CKContext* ctx);
		/**
		 * @brief Check Object pointer validation and remove invalid pointers after deletion.
		 * @param objarray The array to be checked.
		 * @param ctx Associated CKContext.
		 * @warning The performance of this function is extremely bad. Use this carefully.
		*/
		void PostDeletedCheck(XList<CK2::ObjImpls::CKObject*>& objarray, CK2::CKContext* ctx);

	}


}
