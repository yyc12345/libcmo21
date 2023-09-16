#pragma once
#include "XTypes.hpp"

namespace LibCmo::XContainer {

	namespace NSXBitArray {

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

}
