#include "CKFile.hpp"
#include "CKStateChunk.hpp"
#include "CKMinContext.hpp"
#include <cstdarg>

namespace LibCmo::CK2 {

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

#pragma region CKFileInfo

	CKFileInfo::CKFileInfo() {
	}

	CKFileInfo::~CKFileInfo() {
	}

#pragma endregion

#pragma region CKFileObject

	CKFileObject::CKFileObject() :
		Data(nullptr) {
		;
	}

	CKFileObject::~CKFileObject() {
		if (Data != nullptr) delete Data;
	}

#pragma endregion

#pragma region CKFileManagerData

	CKFileManagerData::CKFileManagerData() :
		Data(nullptr) {
	}

	CKFileManagerData::~CKFileManagerData() {
		if (Data != nullptr) delete Data;
	}

#pragma endregion

#pragma region CKFilePluginDependencies

	CKFilePluginDependencies::CKFilePluginDependencies() {
	}

	CKFilePluginDependencies::~CKFilePluginDependencies() {
	}

#pragma endregion

	namespace CKFileData {

#pragma region ShallowDocument

		ShallowDocument::ShallowDocument() {
			/*this->m_IndexByClassId.resize(static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID));*/
		}

		ShallowDocument::~ShallowDocument() {

		}

#pragma endregion

#pragma region DeepDocument

		DeepDocument::DeepDocument() {
			/*this->m_IndexByClassId.resize(static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID));*/
		}

		DeepDocument::~DeepDocument() {

		}

#pragma endregion

	}

#pragma region CKFile Misc

	CKFile::CKFile(CKMinContext* ctx) :
		m_FileName(), m_MinCtx(ctx) {
		;
	}

	CKFile::~CKFile() {
	}


#pragma endregion


}
