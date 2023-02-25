#include "CKFile.hpp"
#include <cstdarg>

namespace LibCmo {


#pragma region CKMinContext

	CKMinContext::CKMinContext() {

	}
	CKMinContext::~CKMinContext() {

	}

	void CKMinContext::Printf(CKSTRING fmt, ...) {
		va_list argptr;
		va_start(argptr, fmt);
		vfprintf(stdout, fmt, argptr);
		va_end(argptr);
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

#pragma region CKFileObject

	LibCmo::CKFileObject::CKFileObject() :
	Data(nullptr) {
		;
	}

	LibCmo::CKFileObject::~CKFileObject() {
		;
	}

#pragma endregion


#pragma region ShallowDocument

	LibCmo::CKFileData::ShallowDocument::ShallowDocument() {
		this->m_IndexByClassId.resize(static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID));
	}

	LibCmo::CKFileData::ShallowDocument::~ShallowDocument() {

	}

#pragma endregion

#pragma region DeepDocument

	LibCmo::CKFileData::DeepDocument::DeepDocument() {
		this->m_IndexByClassId.resize(static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID));
	}

	LibCmo::CKFileData::DeepDocument::~DeepDocument() {

	}

#pragma endregion


#pragma region CKFile Misc

	CKFile::CKFile(CKMinContext* ctx) :
		m_MappedFile(nullptr), m_FileName(),
		m_MinCtx(ctx) {
		;
	}

	CKFile::~CKFile() {
	}



#pragma endregion


}
