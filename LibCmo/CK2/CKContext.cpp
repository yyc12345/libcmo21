#include "CKContext.hpp"
#include "ObjImpls/CKObject.hpp"
#include "../XContainer/XBitArray.hpp"
#include <cstdarg>

namespace LibCmo::CK2 {

#if defined(LIBCMO_OS_WIN32)
	static wchar_t g_UniqueFolder[] = L"LibCmo";
#else
	static char g_UniqueFolder[] = "LibCmo";
#endif

#pragma region Ctor Dtor

	CKContext::CKContext() :
		m_ObjectsList(), m_ReturnedObjectOffsets(),
		m_GroupGlobalIndex(), m_SceneGlobalIndex(),
		m_CompressionLevel(5), m_FileWriteMode(CK_FILE_WRITEMODE::CKFILE_UNCOMPRESSED),
		m_NameEncoding(), m_TempFolder(),
		m_OutputCallback(nullptr)
	{
		// preset for temp folder
		// todo: add current CKContext pointer as the part of temp path.
		// thus multiple CKContext can work.
		m_TempFolder = std::filesystem::temp_directory_path();
		m_TempFolder /= g_UniqueFolder;
		std::filesystem::create_directory(m_TempFolder);
	}

	CKContext::~CKContext() {
		ClearAll();
	}

#pragma endregion

#pragma region Objects Management

	void CKContext::ClearAll() {

	}

#pragma endregion

#pragma region Common Manager Functions
	
	CKDWORD CKContext::GetManagerCount() {
		return m_ManagerList.size();
	}

	MgrImpls::CKBaseManager* CKContext::GetManager(CKDWORD index) {
		if (index >= m_ManagerList.size()) return nullptr;
		return m_ManagerList[index];
	}

#pragma endregion

#pragma region File Save/Load Options

	void CKContext::SetCompressionLevel(CKINT level) {
		if (level > 0 && level < 10) {
			m_CompressionLevel = level;
		}
	}

	CKINT CKContext::GetCompressionLevel() {
		return m_CompressionLevel;
	}

	void CKContext::SetFileWriteMode(CK_FILE_WRITEMODE mode) {
		m_FileWriteMode = mode;
	}

	CK_FILE_WRITEMODE CKContext::GetFileWriteMode() {
		return m_FileWriteMode;
	}


#pragma endregion

#pragma region Output utilities

	void CKContext::OutputToConsole(CKSTRING str) {
		if (m_OutputCallback == nullptr) return;
		m_OutputCallback(str);
	}

	void CKContext::OutputToConsoleEx(CKSTRING fmt, ...) {
		if (m_OutputCallback == nullptr) return;

		va_list argptr;
		va_start(argptr, fmt);

		std::string result;
		int count = std::vsnprintf(nullptr, 0, fmt, argptr);
		result.resize(count);
		int write_result = std::vsnprintf(result.data(), count, fmt, argptr);

		if (write_result < 0 || write_result > count) return;

		va_end(argptr);

		m_OutputCallback(result.c_str());
	}

	void CKContext::SetOutputCallback(OutputCallback cb) {
		m_OutputCallback = cb;
	}

#pragma endregion

#pragma region Temp IO utilities

	void CKContext::SetTempPath(CKSTRING u8_temp) {
		EncodingHelper::U8PathToStdPath(this->m_TempFolder, u8_temp);
	}

	std::string CKContext::GetTempFilePath(CKSTRING u8_filename) {
		std::filesystem::path stdfilename;
		EncodingHelper::U8PathToStdPath(stdfilename, u8_filename);
		auto realfile = this->m_TempFolder / stdfilename;

		std::string result;
		EncodingHelper::StdPathToU8Path(result, realfile);
		return result;
	}

#pragma endregion

#pragma region Encoding utilities

	void CKContext::GetUtf8String(const std::string& native_name, std::string& u8_name) {
		bool success = false;
		for (const auto& token : this->m_NameEncoding) {
			success = LibCmo::EncodingHelper::GetUtf8VirtoolsName(native_name, u8_name, token);
			if (success) break;
		}

		// fallback
		if (!success) {
			u8_name = native_name;
			this->OutputToConsole("Error when converting to UTF8 string.");
		}
	}

	void CKContext::GetNativeString(const std::string& u8_name, std::string& native_name) {
		bool success = false;
		for (const auto& token : this->m_NameEncoding) {
			success = LibCmo::EncodingHelper::GetNativeVirtoolsName(u8_name, native_name, token);
			if (success) break;
		}
		
		// fallback
		if (!success) {
			native_name = u8_name;
			this->OutputToConsole("Error when converting to native string.");
		}
	}

	void CKContext::SetEncoding(const std::vector<std::string> encoding_series) {
		// free all current series
		for (const auto& encoding : this->m_NameEncoding) {
			LibCmo::EncodingHelper::DestroyEncodingToken(encoding);
		}
		this->m_NameEncoding.clear();

		// add new encoding
		for (const auto& encoding_str : encoding_series) {
			this->m_NameEncoding.push_back(LibCmo::EncodingHelper::CreateEncodingToken(encoding_str));
		}
	}



#pragma endregion

}