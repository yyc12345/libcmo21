#include "CKPathManager.hpp"

namespace LibCmo::CK2::MgrImpls {

#if defined(LIBCMO_OS_WIN32)
	static wchar_t g_UniqueFolder[] = L"LibCmo";
#else
	static char g_UniqueFolder[] = "LibCmo";
#endif

	CKPathManager::CKPathManager(CKContext* ctx) :
		CKBaseManager(ctx, PATH_MANAGER_GUID, "Path Manager"),
		m_TempFolder() {
		// preset for temp folder
		// todo: add current CKContext pointer as the part of temp path.
		// thus multiple CKContext can work.
		m_TempFolder = std::filesystem::temp_directory_path();
		m_TempFolder /= g_UniqueFolder;
		std::filesystem::create_directories(m_TempFolder);

	}
	CKPathManager::~CKPathManager() {}

	void CKPathManager::SetTempFolder(CKSTRING u8_temp) {
		EncodingHelper::U8PathToStdPath(this->m_TempFolder, u8_temp);
	}

	std::string CKPathManager::GetTempFolder() {
		std::string result;
		EncodingHelper::StdPathToU8Path(result, this->m_TempFolder);
		return result;
	}

	std::string CKPathManager::GetTempFilePath(CKSTRING u8_filename) {
		std::filesystem::path stdfilename;
		EncodingHelper::U8PathToStdPath(stdfilename, u8_filename);
		auto realfile = this->m_TempFolder / stdfilename;

		std::string result;
		EncodingHelper::StdPathToU8Path(result, realfile);
		return result;
	}

	bool CKPathManager::ResolveFileName(std::string& u8_filename) {
		// todo: finish resolve file name
		return false;
	}

}

