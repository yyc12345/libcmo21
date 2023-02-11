#include "VTUtils.hpp"
#if defined(LIBCMO_OS_WIN32)
#define ZLIB_WINAPI
#include "zconf.h"
#endif

#include "VTStruct.hpp"
#include <cstdlib>
#include <zlib.h>

namespace LibCmo {

	void* CKUnPackData(CKINT DestSize, const void* SrcBuffer, CKINT SrcSize) {
		char* DestBuffer = (char*)malloc(DestSize);
		if (DestBuffer == nullptr) return nullptr;

		uLongf cache = DestSize;
		if (uncompress(
			reinterpret_cast<Bytef*>(DestBuffer), &cache,
			reinterpret_cast<const Bytef*>(SrcBuffer), SrcSize) != Z_OK) {
			free(DestBuffer);
			return nullptr;
		}

		return DestBuffer;
	}

#pragma region VxMemoryMappedFile

	VxMemoryMappedFile::VxMemoryMappedFile(const char* u8_filepath) :
		m_szFilePath(),
		m_hFile(nullptr), m_hFileMapping(nullptr),
		m_bIsValid(false) {

		// save file path and set region option
		boost::interprocess::map_options_t region_option;
#if defined(LIBCMO_OS_WIN32)
		Encoding::CharToWchar(u8_filepath, this->m_szFilePath, CP_UTF8);
		region_option = FILE_MAP_READ;
#else
		this->m_szFilePath = u8_filepath;
		region_option = boost::interprocess::default_map_options;
#endif

		// open file
		this->m_hFile = new boost::interprocess::file_mapping(
			this->m_szFilePath.c_str(), boost::interprocess::read_only
		);

		// open region
		this->m_hFileMapping = new boost::interprocess::mapped_region(
			*(this->m_hFile), boost::interprocess::read_only,
			0, 0, nullptr,
			region_option
		);

		// set valid
		this->m_bIsValid = true;

	}

	VxMemoryMappedFile::~VxMemoryMappedFile(void) {
		this->m_bIsValid = false;

		delete this->m_hFileMapping;
		boost::interprocess::file_mapping::remove(this->m_szFilePath.c_str());
		delete this->m_hFile;
	}

#pragma endregion

#pragma region CKBufferParser

	CKBufferParser::CKBufferParser(void* ptr, size_t rsize, bool need_manual_free) :
		m_ReaderBegin(static_cast<char*>(ptr)),
		m_ReaderPos(0u), m_ReaderSize(rsize),
		m_NeedManualFree(need_manual_free) {
		;
	}
	CKBufferParser::~CKBufferParser() {
		if (this->m_NeedManualFree) free(this->m_ReaderBegin);
	}

#pragma endregion

#pragma region CKFile Misc

	CKFile::CKFile(const Utils::VirtoolsContext& cfg) :
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
