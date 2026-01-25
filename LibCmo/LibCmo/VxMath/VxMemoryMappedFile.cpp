#include "VxMemoryMappedFile.hpp"
#include <yycc/num/safe_cast.hpp>
#if defined(YYCC_OS_WINDOWS)
#include <yycc/encoding/windows.hpp>
#else
#include <yycc/string/reinterpret.hpp>
#endif

namespace LibCmo::VxMath {

	VxMemoryMappedFile::VxMemoryMappedFile(CKSTRING u8_filepath) :
		// Initialize members
#if defined(YYCC_OS_WINDOWS)
		// Initialize Windows specific.
		m_hFile(NULL), m_hFileMapping(NULL), m_hFileMapView(NULL),
		m_dwFileSize(),
#else
		// Initialize Linux specific.
		m_hFile(-1), m_offFileSize(0), m_pFileAddr((void*)-1),
#endif
		// Initialize common members.
		m_szFilePath(),
		m_bIsValid(false), m_pMemoryMappedFileBase(nullptr), m_cbFile(0u) {

		// Setup file path first
		if (u8_filepath == nullptr) return;
		m_szFilePath = u8_filepath;

		// Do real mapping work according to different platform.
#if defined(YYCC_OS_WINDOWS)

		// Parse file name to wchar_t
        auto rv_w_filename = yycc::encoding::windows::to_wchar(m_szFilePath);
        if (!rv_w_filename.has_value()) return;
        std::wstring w_filename = std::move(rv_w_filename.value());

		// Open file
		this->m_hFile = ::CreateFileW(
			w_filename.c_str(),
			GENERIC_READ,
			0,	// do not share
			NULL,	// no security
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,	// no attr
			NULL	// no template
		);
		if (this->m_hFile == INVALID_HANDLE_VALUE) {
			return;
		}

		// Get size and check its range.
		if (!(::GetFileSizeEx(this->m_hFile, &m_dwFileSize))) {
			CloseHandle(this->m_hFile);
			return;
		}
		if (m_dwFileSize.HighPart != 0) {
			CloseHandle(this->m_hFile);
			return;
		}
		m_cbFile = m_dwFileSize.LowPart;

		// Open mapping
		this->m_hFileMapping = CreateFileMappingW(
			this->m_hFile,
			NULL,	// default security
			PAGE_READONLY,
			0, 0,	// expand to file size
			NULL	// no name
		);
		if (this->m_hFileMapping == NULL) {
			CloseHandle(this->m_hFile);
			return;
		}

		// Open map view
		this->m_hFileMapView = MapViewOfFile(
			this->m_hFileMapping,
			FILE_MAP_READ,
			0, 0,	//no offset
			0	// expand to full file size
		);
		if (this->m_hFileMapView == NULL) {
			CloseHandle(m_hFileMapping);
			CloseHandle(m_hFile);
		}
		// Set base address
		m_pMemoryMappedFileBase = m_hFileMapView;

#else
		// create file
		// we do not need provide mode_t, because is served for new created file.
		// we are opening a existed file.
        this->m_hFile = open(yycc::string::reinterpret::as_ordinary(m_szFilePath.c_str()), O_RDONLY);
		if (m_hFile == -1) {
			return;
		}

		// calculate file size
		struct stat sb;
		int err = fstat(m_hFile, &sb);
		// if failed or not a regular file, exit
		if (err == -1 || (sb.st_mode & S_IFMT) != S_IFREG) {
			close(m_hFile);
			return;
		}
		// Setup size with checking its range
		this->m_offFileSize = sb.st_size;
        auto rv_cbFile = yycc::num::safe_cast::try_to<CKDWORD>(this->m_offFileSize);
        if (rv_cbFile.has_value()) {
            this->m_cbFile = rv_cbFile.value();
        } else {
            close(m_hFile);
            return;
		}

		// map file
		this->m_pFileAddr = mmap(
			nullptr,	// request linux distribute one
			this->m_offFileSize,	// map the full file
			PROT_READ,	// only for reading
			MAP_PRIVATE,
			this->m_hFile,
			0	// no offset
		);
		if (this->m_pFileAddr == MAP_FAILED) {
			close(m_hFile);
			return;
		}
		// set base address
		this->m_pMemoryMappedFileBase = m_pFileAddr;
		
#endif

		// set valid
		this->m_bIsValid = true;
	}

	VxMemoryMappedFile::~VxMemoryMappedFile() {
		if (this->m_bIsValid) {
			// only success mapping need free
			this->m_bIsValid = false;
			m_cbFile = 0;
			m_pMemoryMappedFileBase = nullptr;

#if defined(YYCC_OS_WINDOWS)
			UnmapViewOfFile(this->m_hFileMapView);
			CloseHandle(m_hFileMapping);
			CloseHandle(m_hFile);
#else
			munmap(this->m_pFileAddr, this->m_offFileSize);
			close(m_hFile);
#endif
		}
	}

	const void* VxMemoryMappedFile::GetBase() const {
		if (!this->IsValid())
			throw RuntimeException("Try to get file base address on an invalid VxMemoryMappedFile");
		return this->m_pMemoryMappedFileBase;
	}

	CKDWORD VxMemoryMappedFile::GetFileSize() const {
		if (!this->IsValid())
			throw RuntimeException("Try to get file size on an invalid VxMemoryMappedFile");
		return this->m_cbFile;
	}

	bool VxMemoryMappedFile::IsValid() const {
		return this->m_bIsValid;
	}

}
