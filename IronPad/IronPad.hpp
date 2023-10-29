#pragma once

// only include VTUtils to get essential macro
#include <VTUtils.hpp>

#if defined(LIBCMO_BUILD_RELEASE) && defined(LIBCMO_OS_WIN32)
#define IRONPAD_ENABLED 1
#endif
 
// Debug IronPad used. Force enable IronPad.
#define IRONPAD_ENABLED 1

namespace IronPad {

	/**
	 * @brief Register IronPad.
	 * @detail This function frequently called at the start of program.
	*/
	void IronPadRegister();
	/**
	 * @brief Unregiister IronPad
	 * @detail This function frequently called at the end of program.
	*/
	void IronPadUnregister();

}
