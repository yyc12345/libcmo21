#include "CKPathManager.hpp"

namespace LibCmo::CK2::MgrImpls {

#if defined(LIBCMO_OS_WIN32)
	static wchar_t g_UniqueFolder[] = L"LibCmo";
#else
	static char g_UniqueFolder[] = "LibCmo";
#endif

	CKPathManager::CKPathManager(CKContext* ctx) :
		CKBaseManager(ctx, PATH_MANAGER_GUID, "Path Manager"),
		m_TempFolder(), m_ExtraPathes() {
		// preset for temp folder
		// todo: add current CKContext pointer as the part of temp path.
		// thus multiple CKContext can work.
		m_TempFolder = std::filesystem::temp_directory_path();
		m_TempFolder /= g_UniqueFolder;
		std::filesystem::create_directories(m_TempFolder);

	}
	CKPathManager::~CKPathManager() {}

	bool CKPathManager::SetTempFolder(CKSTRING u8_temp) {
		if (u8_temp == nullptr) return false;

		std::filesystem::path cache;
		EncodingHelper::U8PathToStdPath(cache, u8_temp);
		if (std::filesystem::is_directory(cache)) {
			m_TempFolder = cache;
			return true;
		} else {
			return false;
		}
	}

	XContainer::XString CKPathManager::GetTempFolder() {
		XContainer::XString result;
		EncodingHelper::StdPathToU8Path(result, this->m_TempFolder);
		return result;
	}

	XContainer::XString CKPathManager::GetTempFilePath(CKSTRING u8_filename) {
		if (u8_filename == nullptr) return XContainer::XString();

		std::filesystem::path stdfilename;
		EncodingHelper::U8PathToStdPath(stdfilename, u8_filename);
		auto realfile = this->m_TempFolder / stdfilename;

		XContainer::XString result;
		EncodingHelper::StdPathToU8Path(result, realfile);
		return result;
	}

	bool CKPathManager::AddPath(CKSTRING u8path) {
		if (u8path == nullptr) return false;

		std::filesystem::path newpath;
		EncodingHelper::U8PathToStdPath(newpath, u8path);
		if (std::filesystem::is_directory(newpath)) {
			m_ExtraPathes.emplace_back(std::move(newpath));
			return true;
		} else {
			return false;
		}
	}

	void CKPathManager::ClearPath() {
		m_ExtraPathes.clear();
	}

	bool CKPathManager::ResolveFileName(XContainer::XString& u8_filename) {
		std::filesystem::path filepath;
		EncodingHelper::U8PathToStdPath(filepath, u8_filename.c_str());

		// if it is absolute path, return it directly
		if (filepath.is_absolute()) {
			return true;
		}

		// test in temp folder
		auto tempfile = m_TempFolder / filepath;
		if (std::filesystem::is_regular_file(tempfile)) {
			EncodingHelper::StdPathToU8Path(u8_filename, tempfile);
			return true;
		}

		// otherwise check it in extra path
		for (const auto& extrapath : m_ExtraPathes) {
			auto combinedpath = extrapath / filepath;
			if (std::filesystem::is_regular_file(combinedpath)) {
				// this is correct
				EncodingHelper::StdPathToU8Path(u8_filename, combinedpath);
				return true;
			}
		}

		// failed
		return false;
	}

	void CKPathManager::GetFileName(XContainer::XString& u8path) {
		std::filesystem::path filepath;
		EncodingHelper::U8PathToStdPath(filepath, u8path.c_str());

		auto result = filepath.filename();
		EncodingHelper::StdPathToU8Path(u8path, result);
	}

	void CKPathManager::GetExtension(XContainer::XString& u8path) {
		std::filesystem::path filepath;
		EncodingHelper::U8PathToStdPath(filepath, u8path.c_str());

		auto result = filepath.extension();
		EncodingHelper::StdPathToU8Path(u8path, result);
	}

}

