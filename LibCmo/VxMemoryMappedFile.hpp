#pragma once

#include "VTUtils.hpp"
#if defined(LIBCMO_OS_WIN32)
#include <Windows.h>
#else
#include <sys/mman.h>
#endif

#include <string>
#include <filesystem>

namespace LibCmo::VxMath {

	class VxMemoryMappedFile {
	private:


#if defined(LIBCMO_OS_WIN32)
		HANDLE m_hFile;
		DWORD m_dwFileSizeLow, m_dwFileSizeHigh;
		HANDLE m_hFileMapping;
		LPVOID m_hFileMapView;
#else
		
#error NO IMPLEMENTATION FOR LINUX MMAP!
#endif

		std::filesystem::path m_szFilePath;
		void* m_pMemoryMappedFileBase;
		size_t m_cbFile;
		bool m_bIsValid;
	public:
		VxMemoryMappedFile(const char* u8_filepath);
		VxMemoryMappedFile(const VxMemoryMappedFile&) = delete;
		VxMemoryMappedFile& operator=(const VxMemoryMappedFile&) = delete;
		~VxMemoryMappedFile(void);

		inline void* GetBase(void) { return this->m_pMemoryMappedFileBase; }
		inline size_t GetFileSize(void) { return this->m_cbFile; }
		inline bool IsValid(void) { return this->m_bIsValid; }
	};

}
