#include "VTUtils.hpp"
#include "CKMinContext.hpp"

namespace LibCmo {

#if defined(LIBCMO_OS_WIN32)
	static wchar_t g_UniqueFolder[] = L"LibCmo";
#else
	static char g_UniqueFolder[] = "LibCmo";
#endif


	CKMinContext::CKMinContext() :
		NameEncoding(), NameEncodingToken(Encoding::ENCODING_TOKEN_DEFAULT),
		TempFolder()
	{
		// preset for temp folder
		TempFolder = std::filesystem::temp_directory_path();
		TempFolder /= g_UniqueFolder;
		std::filesystem::create_directory(TempFolder);
	}
	CKMinContext::~CKMinContext() {

	}

	void CKMinContext::Printf(CKSTRING fmt, ...) {
		va_list argptr;
		va_start(argptr, fmt);
		vfprintf(stdout, fmt, argptr);
		va_end(argptr);
	}

	void CKMinContext::GetUtf8ObjectName(std::string& native_name, std::string& u8_name) {
		Encoding::GetUtf8VirtoolsName(native_name, u8_name, this->NameEncodingToken);
	}

	void CKMinContext::GetNativeObjectName(std::string& u8_name, std::string& native_name) {
		Encoding::GetNativeVirtoolsName(u8_name, native_name, this->NameEncodingToken);
	}

	void CKMinContext::SetEncoding(CKSTRING encoding) {
		this->NameEncoding = encoding;
		this->RefetchEncodingToken();
	}

	void CKMinContext::SetTempPath(CKSTRING u8_temp) {
		Encoding::SetStdPathFromU8Path(this->TempFolder, u8_temp);
	}

	FILE* CKMinContext::OpenTempFile(CKSTRING u8_filename, bool is_read) {
		std::filesystem::path stdfilename;
		Encoding::SetStdPathFromU8Path(stdfilename, u8_filename);

		auto realfile = this->TempFolder / stdfilename;
		return Encoding::OpenStdPathFile(realfile, is_read);
	}

	void CKMinContext::RefetchEncodingToken(void) {
		Encoding::DestroyEncodingToken(this->NameEncodingToken);
		this->NameEncodingToken = Encoding::CreateEncodingToken(this->NameEncoding);
	}

}