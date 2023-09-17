#include "XTypes.hpp"
#include "../CK2/CKContext.hpp"
#include "../CK2/MgrImpls/CKObjectManager.hpp"
#include "../CK2/ObjImpls/CKObject.hpp"
#include <type_traits>

namespace LibCmo::XContainer {

	namespace NSXBitArray {

		void Resize(XBitArray& ba, CKDWORD newsize) {
			ba.resize(newsize, false);
		}

		void Or(XBitArray& thisba, const XBitArray& thatba) {
			if (thisba.size() < thatba.size()) {
				Resize(thisba, static_cast<CKDWORD>(thatba.size()));
			}

			for (size_t i = 0; i < thisba.size(); ++i) {
				thisba[i] = thisba[i] || thatba[i];
			}
		}

		bool IsSet(const XBitArray& ba, CKDWORD n) {
			if (n >= ba.size()) return false;
			return ba[n];
		}

		void Set(XBitArray& ba, CKDWORD n)  {
			if (n >= ba.size()) {
				ba.resize(n + 1);
			}
			ba[n] = true;
		}

		void Unset(XBitArray& ba, CKDWORD n)  {
			if (n >= ba.size()) return;
			ba[n] = false;
		}

		template<bool _Cond>
		bool GeneralGetBitPosition(const XBitArray& ba, CKDWORD n, CKDWORD& got) {
			CKDWORD counter = 0;
			for (size_t i = 0; i < ba.size(); ++i) {
				if (ba[i] == _Cond) {
					if (counter == n) {
						got = static_cast<CKDWORD>(i);
						return true;
					} else {
						++counter;
					}
				}
			}

			return false;
		}

		bool GetSetBitPosition(const XBitArray& ba, CKDWORD n, CKDWORD& got) {
			return GeneralGetBitPosition<true>(ba, n, got);
		}

		bool GetUnsetBitPosition(const XBitArray& ba, CKDWORD n, CKDWORD& got) {
			return GeneralGetBitPosition<false>(ba, n, got);
		}

	}

	namespace NSXString {

		CKSTRING ToCKSTRING(const XString& strl) {
			if (strl.empty()) return nullptr;
			else return strl.c_str();
		}

		void FromCKSTRING(XString& strl, CKSTRING s) {
			if (s == nullptr) strl.clear();
			else strl = s;
		}

	}

	template<class _Ty>
	constexpr bool GeneralXArrayCheck_TypeCheck() {
		return std::is_same_v<_Ty, CK2::CK_ID> || std::is_same_v<_Ty, CK2::ObjImpls::CKObject*>;
	}
	template<class _Ty, bool _IsPre>
	bool GeneralXArrayCheck_ItemCheck(const _Ty& item, CK2::CKContext* ctx) {
		static_assert(GeneralXArrayCheck_TypeCheck<_Ty>());
		if (ctx == nullptr) return false;

		if constexpr (_IsPre) {
			CK2::ObjImpls::CKObject* obj = nullptr;
			if constexpr (std::is_same_v<_Ty, CK2::CK_ID>) {
				obj = ctx->GetObject(item);
				if (obj == nullptr) return false;
			} else {
				obj = item;
			}
			if (EnumsHelper::Has(obj->GetObjectFlags(), CK2::CK_OBJECT_FLAGS::CK_OBJECT_TOBEDELETED)) return false;
		} else {
			CK2::MgrImpls::CKObjectManager* objmgr = ctx->GetObjectManager();
			if constexpr (std::is_same_v<_Ty, CK2::CK_ID>) {
				if (!objmgr->IsObjectSafe(item)) return false;
			} else {
				if (!objmgr->IsObjectPointerSafe(item)) return false;
			}
		}

		return true;
	}
	namespace NSXObjectArray {

		void PreDeletedCheck(XObjectArray& objarray, CK2::CKContext* ctx) {
			if (ctx == nullptr) return;
			std::erase_if(objarray, [ctx](const CK2::CK_ID& item) -> bool {
				return GeneralXArrayCheck_ItemCheck<CK2::CK_ID, true>(item, ctx);
				});
		}

		void PostDeletedCheck(XObjectArray& objarray, CK2::CKContext* ctx) {
			if (ctx == nullptr) return;
			std::erase_if(objarray, [ctx](const CK2::CK_ID& item) -> bool {
				return GeneralXArrayCheck_ItemCheck<CK2::CK_ID, false>(item, ctx);
				});
		}
		

		void PreDeletedCheck(XList<CK2::CK_ID>& objarray, CK2::CKContext* ctx) {
			if (ctx == nullptr) return;
			std::erase_if(objarray, [ctx](const CK2::CK_ID& item) -> bool {
				return GeneralXArrayCheck_ItemCheck<CK2::CK_ID, true>(item, ctx);
				});
		}

		void PostDeletedCheck(XList<CK2::CK_ID>& objarray, CK2::CKContext* ctx) {
			if (ctx == nullptr) return;
			std::erase_if(objarray, [ctx](const CK2::CK_ID& item) -> bool {
				return GeneralXArrayCheck_ItemCheck<CK2::CK_ID, false>(item, ctx);
				});
		}

	}
	namespace NSXObjectPointerArray {

		void PreDeletedCheck(XObjectPointerArray& objarray, CK2::CKContext* ctx) {
			if (ctx == nullptr) return;
			std::erase_if(objarray, [ctx](CK2::ObjImpls::CKObject* const& item) -> bool {
				return GeneralXArrayCheck_ItemCheck<CK2::ObjImpls::CKObject*, true>(item, ctx);
				});
		}

		void PostDeletedCheck(XObjectPointerArray& objarray, CK2::CKContext* ctx) {
			if (ctx == nullptr) return;
			std::erase_if(objarray, [ctx](CK2::ObjImpls::CKObject* const& item) -> bool {
				return GeneralXArrayCheck_ItemCheck<CK2::ObjImpls::CKObject*, false>(item, ctx);
				});
		}


		void PreDeletedCheck(XList<CK2::ObjImpls::CKObject*>& objarray, CK2::CKContext* ctx) {
			if (ctx == nullptr) return;
			std::erase_if(objarray, [ctx](CK2::ObjImpls::CKObject* const& item) -> bool {
				return GeneralXArrayCheck_ItemCheck<CK2::ObjImpls::CKObject*, true>(item, ctx);
				});
		}

		void PostDeletedCheck(XList<CK2::ObjImpls::CKObject*>& objarray, CK2::CKContext* ctx) {
			if (ctx == nullptr) return;
			std::erase_if(objarray, [ctx](CK2::ObjImpls::CKObject* const& item) -> bool {
				return GeneralXArrayCheck_ItemCheck<CK2::ObjImpls::CKObject*, false>(item, ctx);
				});
		}

	}


}
