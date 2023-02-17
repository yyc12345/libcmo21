#include "VTUtils.hpp"
#if defined(LIBCMO_OS_WIN32)
#define ZLIB_WINAPI
#include <zconf.h>
#endif

#include "VTStruct.hpp"
#include <cstdlib>
#include <zlib.h>

namespace LibCmo {

#pragma region CKZLib Func

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

	CKDWORD LibCmo::CKComputeDataCRC(const void* data, size_t size, CKDWORD PreviousCRC) {
		return static_cast<CKDWORD>(adler32(
			static_cast<uLong>(PreviousCRC),
			reinterpret_cast<const Bytef*>(data),
			static_cast<uInt>(size)
			));
	}

#pragma endregion

#pragma region VxMemoryMappedFile

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

#pragma endregion

#pragma region CKBufferParser

	CKBufferParser::CKBufferParser(void* ptr, size_t rsize, bool need_manual_free) :
		m_MemBegin(static_cast<char*>(ptr)),
		m_MemPos(0u), m_MemSize(rsize),
		m_NeedManualFree(need_manual_free) {
		;
	}
	CKBufferParser::~CKBufferParser() {
		if (this->m_NeedManualFree) delete[](this->m_MemBegin);
	}

#pragma endregion

#pragma region CKFile Misc

	CKFile::CKFile(const Utils::VirtoolsEnvironment& cfg) :
		m_Parser(nullptr), m_MappedFile(nullptr),
		m_UserCfg(cfg) {
		;
	}

	CKFile::~CKFile() {
	}


	void CKFile::ClearData(void) {
		m_SaveIDMax = 0;
		m_FileObject.clear();
		m_PluginDep.clear();

		memset(&m_FileInfo, 0, sizeof(CKFileInfo));

		m_Flags = CK_LOAD_FLAGS::CK_LOAD_DEFAULT;
		m_FileName.clear();
		if (m_Parser != nullptr) {
			delete m_Parser;
			m_Parser = nullptr;
		}
		if (m_MappedFile != nullptr) {
			delete m_MappedFile;
			m_MappedFile = nullptr;
		}
	}

#pragma endregion


}
