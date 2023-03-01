#include "VTUtils.hpp"
#if defined(LIBCMO_OS_WIN32)
#define ZLIB_WINAPI
#endif
#include <zconf.h>


#include "CKGlobals.hpp"
#include <zlib.h>

namespace LibCmo::CK2 {

	void* CKUnPackData(CKINT DestSize, const void* SrcBuffer, CKINT SrcSize) {
		char* DestBuffer = new(std::nothrow) char[DestSize];
		if (DestBuffer == nullptr) return nullptr;

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
