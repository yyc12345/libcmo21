#pragma once
#include "XTypes.hpp"

namespace LibCmo::XContainer::XBitArrayPatch {

	/**
	 * @brief Returns the position of the n-th set(1) bit
	 * @return false if not found.
	*/
	bool GetSetBitPosition(XBitArray& ba, CK2::CKDWORD n, CK2::CKDWORD& got);
	/**
	 * @brief Returns the position of the n-th unset(0) bit
	 * @return false if not found.
	*/
	bool GetUnsetBitPosition(XBitArray& ba, CK2::CKDWORD n, CK2::CKDWORD& got);

}
