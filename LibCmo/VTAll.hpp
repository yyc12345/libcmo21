#pragma once

/*
The general including header for LibCom self developer.
Every hpp or cpp file should include this first except
the headers including this file.

This header only fulfill type requirements. If you want
some implement based operations, such as calling 
CKStateChunk or CKContext function. You should include them manually.

All header or cpp file should include this file first.
Except the file listed in there, they should include "VTUtils.hpp" first.

*/
#include "VTUtils.hpp"
#include "VTEncoding.hpp"

#include "CK2/CKTypes.hpp"	// the basic type of Virtools.
#include "CK2/CKDefines.hpp"	// some useful define or constexpr for Virtools.
#include "CK2/CKEnums.hpp"	// All CK used enums except CKStateChunk identifiers.
#include "CK2/CKIdentifiers.hpp"	// CKStateChunk identifiers.
#include "CK2/CKGlobals.hpp"	// CK global functions, such as CKUnPack and etc.

#include "VxMath/VxEnums.hpp"
#include "VxMath/VxTypes.hpp"
#include "VxMath/VxMath.hpp"

#include "XContainer/XTypes.hpp"
