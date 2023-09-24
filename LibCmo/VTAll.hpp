#pragma once

/*
The general including header for LibCom self developer.
Every hpp or cpp file should include this first except
the headers including this file.

This header only fulfill type requirements. If you want
some implement based operations, such as calling 
CKStateChunk or CKContext function. You should include them manually.

All header or cpp file should include this file first.
Except the file listed in there.
The include list of all files list in there should be carefully managed,
to make sure there are no including loop and each header can fufill its functions.

All 'VT' started file is not a part of Virtools SDK.
They just assist Virtools SDK. So they will use native type, not CK type for decl and impl.
Take VTEncoding.hpp for example, All string is std::string, not XContainer::XString.
The file starts with 'CK', 'Vx', and 'X' is a part of Virtools SDK.
They should use Virtools type anywhere, except with one situation, 
Virtools type can not fulfill its requirement.

*/

/*
Including various basic define.
+ LIBCMO_OS_xxx macro to distinguish different OS and architecture
+ General types such as CKDWORD and their format constant
+ Class / struct ctor helper: LIBCMO_DISABLE_COPY_MOVE and LIBCMO_DEFAULT_COPY_MOVE
+ Library panic function
+ Enum helper for add, remove flags for enum.
*/
#include "VTUtils.hpp"
/*
System independent encoding system.
Use native Win32 function in Windows.
And use iconv under other OS.
*/
#include "VTEncoding.hpp"

// Define the basic type of CK2.
#include "CK2/CKTypes.hpp"
// Then we include all XContainer types.
#include "XContainer/XTypes.hpp"
// Then we include all Vx types.
#include "VxMath/VxTypes.hpp"
// Thus all basic type is OK.

// Include CKGlobals which including various essential functions
#include "CK2/CKGlobals.hpp"
// Then load VxMath functions
#include "VxMath/VxMath.hpp"
// OK, all essential functions loaded.

// Load various enums, const value
// Load CK2 first
#include "CK2/CKEnums.hpp"
#include "CK2/CKIdentifiers.hpp"
// load Vx
#include "VxMath/VxEnums.hpp"

// Last, load some custom struct used in program.
#include "CK2/CKDefines.hpp"



