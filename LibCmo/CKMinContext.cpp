#include "VTUtils.hpp"
#include "CKMinContext.hpp"

namespace LibCmo::CK2 {

#if defined(LIBCMO_OS_WIN32)
	static wchar_t g_UniqueFolder[] = L"LibCmo";
#else
	static char g_UniqueFolder[] = "LibCmo";
#endif


	CKMinContext::CKMinContext() :
		m_NameEncoding(), m_NameEncodingToken(EncodingHelper::ENCODING_TOKEN_DEFAULT),
		m_TempFolder(),
		m_PrintCallback(nullptr)
	{
		// preset for temp folder
		m_TempFolder = std::filesystem::temp_directory_path();
		m_TempFolder /= g_UniqueFolder;
		std::filesystem::create_directory(m_TempFolder);
	}
	CKMinContext::~CKMinContext() {

	}

	void CKMinContext::Printf(CKSTRING fmt, ...) {
		if (m_PrintCallback == nullptr) return;

		va_list argptr;
		va_start(argptr, fmt);

		std::string result;
		int count = std::vsnprintf(nullptr, 0, fmt, argptr);
		result.resize(count);
		int write_result = std::vsnprintf(result.data(), count, fmt, argptr);

		if (write_result < 0 || write_result > count) return;

		va_end(argptr);

		m_PrintCallback(result);
	}

	void CKMinContext::SetPrintCallback(PrintCallback cb) {
		m_PrintCallback = cb;
	}

	void CKMinContext::GetUtf8String(std::string& native_name, std::string& u8_name) {
		EncodingHelper::GetUtf8VirtoolsName(native_name, u8_name, this->m_NameEncodingToken);
	}

	void CKMinContext::GetNativeString(std::string& u8_name, std::string& native_name) {
		EncodingHelper::GetNativeVirtoolsName(u8_name, native_name, this->m_NameEncodingToken);
	}

	void CKMinContext::SetEncoding(CKSTRING encoding) {
		this->m_NameEncoding = encoding;
		this->RefetchEncodingToken();
	}

	void CKMinContext::SetTempPath(CKSTRING u8_temp) {
		EncodingHelper::SetStdPathFromU8Path(this->m_TempFolder, u8_temp);
	}

	FILE* CKMinContext::OpenTempFile(CKSTRING u8_filename, bool is_read) {
		std::filesystem::path stdfilename;
		EncodingHelper::SetStdPathFromU8Path(stdfilename, u8_filename);

		auto realfile = this->m_TempFolder / stdfilename;
		return EncodingHelper::OpenStdPathFile(realfile, is_read);
	}

	void CKMinContext::RefetchEncodingToken(void) {
		EncodingHelper::DestroyEncodingToken(this->m_NameEncodingToken);
		this->m_NameEncodingToken = EncodingHelper::CreateEncodingToken(this->m_NameEncoding);
	}

}