#include "VTStruct.hpp"

namespace LibCmo {

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
			this->m_hFile, boost::interprocess::read_only,
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

	void* VxMemoryMappedFile::GetBase(void) { return this->m_hFileMapping->get_address(); }
	size_t VxMemoryMappedFile::GetFileSize(void) { return this->m_hFileMapping->get_size(); }
	bool VxMemoryMappedFile::IsValid(void) { return this->m_bIsValid; }


	CKFile::CKFile() {
	}

	CKFile::~CKFile() {
	}

}
