#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <cinttypes>
#include "CKTypes.hpp"

namespace LibCmo::CK2 {
	/**
	 * @brief Current Version of CK Engine (Day/Month/Year)
	*/
	constexpr const CKDWORD CKVERSION = 0x05082002u;
	/**
	 * @brief Current Version of Dev
	*/
	constexpr const CKDWORD DEVVERSION = 0x02050000u;
	constexpr const CKGUID VIRTOOLS_GUID = CKGUID(0x56495254u, 0x4f4f4c53u);
}
