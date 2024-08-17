#pragma once

#include "../VTInternal.hpp"
#if YYCC_OS == YYCC_OS_WINDOWS
#include <Windows.h>
// disable annoy macro at the same time
#undef GetObject
#undef GetClassName
#undef LoadImage
#undef GetTempPath
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <string>
#include <filesystem>

namespace LibCmo::VxMath {

	class VxMemoryMappedFile {
	private:

#if YYCC_OS == YYCC_OS_WINDOWS
		HANDLE m_hFile;
		DWORD m_dwFileSizeLow, m_dwFileSizeHigh;
		HANDLE m_hFileMapping;
		LPVOID m_hFileMapView;
#else
		int m_hFile;
		off_t m_offFileSize;
		void* m_pFileAddr;
#endif

		std::filesystem::path m_szFilePath;
		void* m_pMemoryMappedFileBase;
		size_t m_cbFile;
		bool m_bIsValid;
	public:
		VxMemoryMappedFile(CKSTRING u8_filepath);
		VxMemoryMappedFile(const VxMemoryMappedFile&) = delete;
		VxMemoryMappedFile& operator=(const VxMemoryMappedFile&) = delete;
		~VxMemoryMappedFile(void);

		void* GetBase(void) { return this->m_pMemoryMappedFileBase; }
		CKDWORD GetFileSize(void) { return static_cast<CKDWORD>(this->m_cbFile); }
		bool IsValid(void) { return this->m_bIsValid; }
	};

}
