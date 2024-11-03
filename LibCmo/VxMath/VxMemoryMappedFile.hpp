#pragma once

#include "../VTInternal.hpp"
#if YYCC_OS == YYCC_OS_WINDOWS
#include <YYCC/WinImportPrefix.hpp>
#include <Windows.h>
#include <YYCC/WinImportSuffix.hpp>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <string>

namespace LibCmo::VxMath {

	/**
	 * @brief Utility class for memory mapped file reading.
	 * @details The VxMemoryMappedFile can be used have a mapping of a file into a memory buffer for reading purposes.
	 * @remarks Due to Virtools limitation, we disallow opening any file whose size exceed the maximum value of CKDWORD.
	*/
	class VxMemoryMappedFile {
	private:
#if YYCC_OS == YYCC_OS_WINDOWS
		HANDLE m_hFile;
		LARGE_INTEGER m_dwFileSize;
		HANDLE m_hFileMapping;
		LPVOID m_hFileMapView;
#else
		int m_hFile;
		off_t m_offFileSize;
		void* m_pFileAddr;
#endif

		std::u8string m_szFilePath;
		void* m_pMemoryMappedFileBase;
		CKDWORD m_cbFile;
		bool m_bIsValid;

	public:
		/**
		 * @brief Create a new VxMemoryMappedFile
		 * @param[in] u8_filepath The path to file to be opened. nullptr is allowed but not suggested.
		 * Because it must create an invalid VxMemoryMappedFile.
		*/
		VxMemoryMappedFile(CKSTRING u8_filepath);
		~VxMemoryMappedFile();
		YYCC_DEL_CLS_COPY_MOVE(VxMemoryMappedFile);

		/**
		 * @brief Returns a pointer to the mapped memory buffer.
		 * @return The pointer to the mapped memory buffer for reading.
		 * @remarks
		 * The returned pointer should not be deleted nor should it be used for writing purpose.
		 * If you did, it will cause undefined behavior.
		 * @exception RuntimeException Raised when calling this on an invalid file mapping.
		*/
		const void* GetBase() const;
		/**
		 * @brief Returns the file size in bytes.
		 * @return The file size in bytes.
		 * @exception RuntimeException Raised when calling this on an invalid file mapping.
		*/
		CKDWORD GetFileSize() const;
		/**
		 * @brief Check whether this file mapping is valid to use.
		 * @return True if it is, otherwise false.
		*/
		bool IsValid() const;
	};

}
