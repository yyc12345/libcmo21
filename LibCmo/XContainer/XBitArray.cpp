#include "XBitArray.hpp"

namespace LibCmo::XContainer::XBitArrayPatch {

	template<bool _Cond>
	bool GeneralGetBitPosition(XBitArray& ba, CK2::CKDWORD n, CK2::CKDWORD& got) {
		CK2::CKDWORD counter = 0;
		for (size_t i = 0; i < ba.size(); ++i) {
			if (ba[i] == _Cond) {
				if (counter == n) {
					got = static_cast<CK2::CKDWORD>(i);
					return true;
				} else {
					++counter;
				}
			}
		}

		return false;
	}

	bool GetSetBitPosition(XBitArray& ba, CK2::CKDWORD n, CK2::CKDWORD& got) {
		return GeneralGetBitPosition<true>(ba, n, got);
	}

	bool GetUnsetBitPosition(XBitArray& ba, CK2::CKDWORD n, CK2::CKDWORD& got) {
		return GeneralGetBitPosition<false>(ba, n, got);
	}

}
