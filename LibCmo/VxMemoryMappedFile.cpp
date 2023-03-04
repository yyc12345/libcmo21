#include "VxMemoryMappedFile.hpp"
#include "VTEncoding.hpp"

namespace LibCmo::VxMath {

	VxMemoryMappedFile::VxMemoryMappedFile(const char* u8_filepath) :
		// init members
#if defined(LIBCMO_OS_WIN32)
		m_hFile(NULL), m_hFileMapping(NULL), m_hFileMapView(NULL),
		m_dwFileSizeLow(0), m_dwFileSizeHigh(0),
#else
		m_hFile(-1), m_offFileSize(0), m_pFileAddr((void*)-1),
#endif
		m_szFilePath(),
		m_bIsValid(false), m_pMemoryMappedFileBase(nullptr), m_cbFile(0u) {

		// save file path
#if defined(LIBCMO_OS_WIN32)
		EncodingHelper::SetStdPathFromU8Path(m_szFilePath, u8_filepath);
#else
		this->m_szFilePath = u8_filepath;
#endif

		// real mapping work
#if defined(LIBCMO_OS_WIN32)

		// open file
		this->m_hFile = CreateFileW(
			this->m_szFilePath.wstring().c_str(),
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
		// create file
		// we do not need provide mode_t, because is served for new created file.
		// we are opening a existed file.
		this->m_hFile = open(m_szFilePath.string().c_str(), O_RDONLY);
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
		// setup size
		this->m_offFileSize = sb.st_size;

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

		// write member data
		m_pMemoryMappedFileBase = m_pFileAddr;
		m_cbFile = static_cast<size_t>(this->m_offFileSize);
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
			munmap(this->m_pFileAddr, this->m_offFileSize);
			close(m_hFile);
#endif
		}
	}


}
