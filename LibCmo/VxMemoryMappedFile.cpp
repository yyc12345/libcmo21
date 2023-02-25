#include "VxMemoryMappedFile.hpp"
#include "VTEncoding.hpp"

namespace LibCmo {

	VxMemoryMappedFile::VxMemoryMappedFile(const char* u8_filepath) :
		// init members
#if defined(LIBCMO_OS_WIN32)
		m_hFile(NULL), m_hFileMapping(NULL), m_hFileMapView(NULL),
		m_dwFileSizeLow(0), m_dwFileSizeHigh(0),
#else
#error NO IMPLEMENTATION FOR LINUX MMAP!
#endif
		m_szFilePath(),
		m_bIsValid(false), m_pMemoryMappedFileBase(nullptr), m_cbFile(0u) {

		// save file path
#if defined(LIBCMO_OS_WIN32)
		Encoding::CharToWchar(u8_filepath, this->m_szFilePath, CP_UTF8);
#else
		this->m_szFilePath = u8_filepath;
#endif

		// real mapping work
#if defined(LIBCMO_OS_WIN32)

		// open file
		this->m_hFile = CreateFileW(
			this->m_szFilePath.c_str(),
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

		// get size
		m_dwFileSizeLow = ::GetFileSize(this->m_hFile, &this->m_dwFileSizeHigh);
		if (m_dwFileSizeLow == INVALID_FILE_SIZE) {
			CloseHandle(this->m_hFile);
			return;
		}

		// open mapping
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

		// open map view
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

		// write member data
		m_pMemoryMappedFileBase = m_hFileMapView;
		m_cbFile = static_cast<size_t>(static_cast<uint64_t>(m_dwFileSizeLow) | (static_cast<uint64_t>(m_dwFileSizeHigh) << 32));

#else
#error NO IMPLEMENTATION FOR LINUX MMAP!
#endif


		// set valid
		this->m_bIsValid = true;
	}

	VxMemoryMappedFile::~VxMemoryMappedFile(void) {
		if (this->m_bIsValid) {
			// only success mapping need free
			this->m_bIsValid = false;
			m_cbFile = 0;
			m_pMemoryMappedFileBase = nullptr;

#if defined(LIBCMO_OS_WIN32)
			UnmapViewOfFile(this->m_hFileMapView);
			CloseHandle(m_hFileMapping);
			CloseHandle(m_hFile);
#else
#error NO IMPLEMENTATION FOR LINUX MMAP!
#endif
		}
	}


}
