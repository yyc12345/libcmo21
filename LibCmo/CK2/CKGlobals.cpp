#include "VTUtils.hpp"
#if defined(LIBCMO_OS_WIN32)
#define ZLIB_WINAPI
#endif
#include <zconf.h>

#include "CKGlobals.hpp"
#include <zlib.h>

namespace LibCmo::CK2 {
	void* CKPackData(const void* Data, CKINT size, CKINT& NewSize, CKINT compressionlevel) {
		uLong boundary = compressBound(static_cast<uLong>(size));
		char* DestBuffer = new char[boundary];

		uLongf _destLen = static_cast<uLongf>(boundary);
		if (compress2(
			reinterpret_cast<Bytef*>(DestBuffer), &_destLen,
			reinterpret_cast<const Bytef*>(Data), static_cast<uLong>(size),
			static_cast<int>(compressionlevel)) != Z_OK) {
			NewSize = 0;
			delete[] DestBuffer;
			return nullptr;
		}

		NewSize = static_cast<CKINT>(_destLen);
		return DestBuffer;
	}

	void* CKUnPackData(CKINT DestSize, const void* SrcBuffer, CKINT SrcSize) {
		char* DestBuffer = new char[DestSize];

		uLongf cache = DestSize;
		if (uncompress(
			reinterpret_cast<Bytef*>(DestBuffer), &cache,
			reinterpret_cast<const Bytef*>(SrcBuffer), SrcSize) != Z_OK) {
			delete[] DestBuffer;
			return nullptr;
		}

		return DestBuffer;
	}

	CKDWORD CKComputeDataCRC(const void* data, size_t size, CKDWORD PreviousCRC) {
		return static_cast<CKDWORD>(adler32(
			static_cast<uLong>(PreviousCRC),
			reinterpret_cast<const Bytef*>(data),
			static_cast<uInt>(size)
			));
	}

}
