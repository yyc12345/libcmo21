#pragma once

/**
 * \file
 * The top header file for LibCom self development.
 * Every C++ header or source file located in this project should include this first 
 * except the headers included in this file.
 * The header files included by this header should be carefully managed,
 * to make sure there are no include loop and each header files can work correctly.
 * 
 * This header should only be used as internal header file.
 * It only contains all necessary stuff used by this project self.
 * Any other program linked to this library should not include this header file directly.
 * The best way to include this library is include VTAll.hpp which include this header file at the first position.
 * 
 * All \c VT started files are not a part of Virtools SDK. They just assist Virtools SDK. 
 * So they will use native type, not CK type for declaration and implementation.
 * Take VTEncoding.hpp for example, All string used in it is \c std::u8string, not LibCmo::XContainer::XString.
 * The file starts with \c CK, \c Vx, and \c X is a part of Virtools SDK.
 * They should use Virtools type anywhere, except that Virtools type can not fulfill their requirements.
*/

// The version info header of LibCmo
#include "VTVersion.hpp"
// The base header of LibCmo.
// It provides various convenient stuff, for example:
// - General LibCmo specific custom exception.
// - Enum Helper for convenient C++ enum class type logic operations.
#include "VTUtils.hpp"
// Platform independent encoding system.
// Use native Win32 functions in Windows,
// or Iconv in non-Windows to provide encoding convertion services.
#include "VTEncoding.hpp"

// Include CK2 types first.
// Because it also include some types or structs used by other module.
#include "CK2/CKTypes.hpp"
// Then include all XContainer types.
#include "XContainer/XTypes.hpp"
// Then include all VxMath types.
#include "VxMath/VxTypes.hpp"
// Now all basic types are included.

// Include CKGlobals which including various essential functions
#include "CK2/CKGlobals.hpp"
// Then load VxMath functions
#include "VxMath/VxMath.hpp"
// OK, all essential functions loaded.

// Load various enums, constant values
// Load CK2 Enums and CKStateChunk Identifier.
#include "CK2/CKEnums.hpp"
#include "CK2/CKIdentifiers.hpp"
// Load VxMath Enums
#include "VxMath/VxEnums.hpp"

// Last, load some custom struct used in program.
#include "CK2/CKDefines.hpp"



