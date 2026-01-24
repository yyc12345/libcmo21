#include "CKPathManager.hpp"

namespace LibCmo::CK2::MgrImpls {

	static constexpr char8_t g_UniqueFolder[] = u8"LibCmo";

	CKPathManager::CKPathManager(CKContext* ctx) :
		CKBaseManager(ctx, PATH_MANAGER_GUID, u8"Path Manager"),
		m_TempFolder(), m_ExtraPathes() {
		// preset for temp folder
		// todo: add current CKContext pointer as the part of temp path.
		// thus multiple CKContext can work.
		m_TempFolder = std::filesystem::temp_directory_path();
		m_TempFolder /= std::filesystem::path(g_UniqueFolder);
		std::filesystem::create_directories(m_TempFolder);

	}
	CKPathManager::~CKPathManager() {}

	bool CKPathManager::SetTempFolder(CKSTRING u8_temp) {
		if (u8_temp == nullptr) return false;

		std::filesystem::path cache(u8_temp);
		if (std::filesystem::is_directory(cache)) {
			m_TempFolder = cache;
			return true;
		} else {
			return false;
		}
	}

	XContainer::XString CKPathManager::GetTempFolder() {
		return this->m_TempFolder.u8string();
	}

	XContainer::XString CKPathManager::GetTempFilePath(CKSTRING u8_filename) {
		if (u8_filename == nullptr) return XContainer::XString();

		std::filesystem::path stdfilename(u8_filename);
		auto realfile = this->m_TempFolder / stdfilename;

		return realfile.u8string();
	}

	bool CKPathManager::AddPath(CKSTRING u8path) {
		if (u8path == nullptr) return false;

		std::filesystem::path newpath(u8path);
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
		std::filesystem::path filepath(u8_filename);

		// if it is absolute path, return it directly
		if (filepath.is_absolute()) {
			return true;
		}

		// test in temp folder
		auto tempfile = m_TempFolder / filepath;
		if (std::filesystem::is_regular_file(tempfile)) {
			u8_filename = tempfile.u8string();
			return true;
		}

		// otherwise check it in extra path
		for (const auto& extrapath : m_ExtraPathes) {
			auto combinedpath = extrapath / filepath;
			if (std::filesystem::is_regular_file(combinedpath)) {
				// this is correct
				u8_filename = combinedpath.u8string();
				return true;
			}
		}

		// failed
		return false;
	}

	void CKPathManager::GetFileName(XContainer::XString& u8path) {
		std::filesystem::path filepath(u8path);

		auto result = filepath.filename();
		u8path = result.u8string();
	}

	void CKPathManager::GetExtension(XContainer::XString& u8path) {
		std::filesystem::path filepath(u8path);

		auto result = filepath.extension();
		u8path = result.u8string();
	}

}

