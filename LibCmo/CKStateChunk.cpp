#include "VTUtils.hpp"
#if defined(LIBCMO_OS_WIN32)
#define ZLIB_WINAPI
#include <zconf.h>
#endif

#include <zlib.h>
#include "CKStateChunk.hpp"

namespace LibCmo {

#pragma region Ctor Dtor

	CKStateChunk::CKStateChunk() :
		m_ClassId(CK_CLASSID::CKCID_OBJECT), m_DataDwSize(0u), m_pData(nullptr),
		m_DataVersion(CK_STATECHUNK_DATAVERSION::CHUNKDATA_CURRENTVERSION), m_ChunkVersion(CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4),
		m_Parser{ CKStateChunkStatus::IDLE, 0u, 0u, 0u },
		m_ObjectList(), m_ChunkList(), m_ManagerList()
	{
		;
	}
	CKStateChunk::CKStateChunk(const CKStateChunk& rhs) :
		m_ClassId(rhs.m_ClassId), m_DataVersion(rhs.m_DataVersion), m_ChunkVersion(rhs.m_ChunkVersion),
		m_Parser(rhs.m_Parser),
		m_ObjectList(rhs.m_ObjectList), m_ManagerList(rhs.m_ManagerList), m_ChunkList(rhs.m_ChunkList),
		m_pData(nullptr), m_DataDwSize(rhs.m_DataDwSize)
	{
		// copy buffer
		if (rhs.m_pData != nullptr) {
			this->m_pData = new(std::nothrow) CKDWORD[rhs.m_DataDwSize];
			if (this->m_pData != nullptr) {
				memcpy(this->m_pData, rhs.m_pData, sizeof(CKDWORD) * rhs.m_DataDwSize);
			}
		}
	}
	CKStateChunk& CKStateChunk::operator=(const CKStateChunk& rhs) {
		this->Clear();

		this->m_DataVersion = rhs.m_DataVersion;
		this->m_ChunkVersion = rhs.m_ChunkVersion;
		this->m_ClassId = rhs.m_ClassId;

		this->m_Parser = rhs.m_Parser;

		this->m_ObjectList = rhs.m_ObjectList;
		this->m_ManagerList = rhs.m_ManagerList;
		this->m_ChunkList = rhs.m_ChunkList;

		// copy buffer
		if (rhs.m_pData != nullptr) {
			this->m_pData = new(std::nothrow) CKDWORD[rhs.m_DataDwSize];
			if (this->m_pData != nullptr) {
				memcpy(this->m_pData, rhs.m_pData, sizeof(CKDWORD) * rhs.m_DataDwSize);
			}
		}
		this->m_DataDwSize = rhs.m_DataDwSize;

		return *this;
	}
	CKStateChunk::~CKStateChunk() {
		if (this->m_pData != nullptr)
			delete[] this->m_pData;
	}

#pragma endregion

#pragma region Misc Funcs

	void CKStateChunk::Clear(void) {
		this->m_ClassId = CK_CLASSID::CKCID_OBJECT;
		//this->m_DataVersion = CK_STATECHUNK_DATAVERSION::CHUNK_DEV_2_1;
		//this->m_ChunkVersion = CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4;

		this->m_Parser.m_CurrentPos = 0;
		this->m_Parser.m_DataSize = 0;
		this->m_Parser.m_PrevIdentifierPos = 0;

		this->m_DataDwSize = 0;
		if (this->m_pData != nullptr) {
			delete[] this->m_pData;
			this->m_pData = nullptr;
		}

		this->m_ObjectList.clear();
		this->m_ManagerList.clear();
		this->m_ChunkList.clear();
	}

	CKDWORD CKStateChunk::GetDataSize(void) {
		return sizeof(CKDWORD) * this->m_DataDwSize;
	}

	void LibCmo::CKStateChunk::_EnsureWriteSpace(CKDWORD size) {
		;
	}


#pragma endregion

#pragma region Buffer Related

	bool CKStateChunk::ConvertFromBuffer(const void* buf) {
		if (buf == nullptr) return false;

		// read chunk ver and data ver first
		// chunk ver always set in the 3rd BYTE in every format
		this->m_ChunkVersion = static_cast<CK_STATECHUNK_CHUNKVERSION>(
			reinterpret_cast<const char*>(buf)[2]
			);
		// data ver always set in the 1st BYTE in every format
		this->m_DataVersion = static_cast<CK_STATECHUNK_DATAVERSION>(
			reinterpret_cast<const char*>(buf)[0]
			);

		// switch according to chunk ver
		const CKDWORD* dwbuf = reinterpret_cast<const CKDWORD*>(buf);
		size_t bufpos = 0u;
		if (this->m_ChunkVersion < CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION2) {
			// very old file

			this->m_ClassId = static_cast<CK_CLASSID>(dwbuf[1]);
			this->m_DataDwSize = dwbuf[2];

			this->m_ObjectList.resize(dwbuf[4]);
			this->m_ChunkList.resize(dwbuf[5]);
			bufpos = 6u;

			if (this->m_DataDwSize != 0) {
				this->m_pData = new(std::nothrow) CKDWORD[this->m_DataDwSize];
				if (m_pData == nullptr) return false;

				memcpy(this->m_pData, dwbuf + bufpos, sizeof(CKDWORD) * this->m_DataDwSize);
				bufpos += this->m_DataDwSize;
			}
			if (!this->m_ObjectList.empty()) {
				memcpy(this->m_ObjectList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ObjectList.size());
				bufpos += this->m_ObjectList.size();
			}
			if (!this->m_ChunkList.empty()) {
				memcpy(this->m_ChunkList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ChunkList.size());
				bufpos += this->m_ChunkList.size();
			}

		} else if (this->m_ChunkVersion == CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION2) {
			// medium ver file

			this->m_ClassId = static_cast<CK_CLASSID>(dwbuf[1]);
			this->m_DataDwSize = dwbuf[2];

			this->m_ObjectList.resize(dwbuf[4]);
			this->m_ChunkList.resize(dwbuf[5]);
			this->m_ManagerList.resize(dwbuf[6]);
			bufpos = 7u;

			if (this->m_DataDwSize != 0) {
				this->m_pData = new(std::nothrow) CKDWORD[this->m_DataDwSize];
				if (m_pData == nullptr) return false;

				memcpy(this->m_pData, dwbuf + bufpos, sizeof(CKDWORD) * this->m_DataDwSize);
				bufpos += this->m_DataDwSize;
			}
			if (!this->m_ObjectList.empty()) {
				memcpy(this->m_ObjectList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ObjectList.size());
				bufpos += this->m_ObjectList.size();
			}
			if (!this->m_ChunkList.empty()) {
				memcpy(this->m_ChunkList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ChunkList.size());
				bufpos += this->m_ChunkList.size();
			}
			if (!this->m_ManagerList.empty()) {
				memcpy(this->m_ManagerList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ManagerList.size());
				bufpos += this->m_ManagerList.size();
			}

		} else if (this->m_ChunkVersion <= CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4) {
			// the latest file

			// re-read some extra data
			// class id located the 2nd BYTE
			this->m_ClassId = static_cast<CK_CLASSID>(
				reinterpret_cast<const char*>(buf)[1]
				);
			// options located the 4th BYTE
			CK_STATECHUNK_CHUNKOPTIONS options = static_cast<CK_STATECHUNK_CHUNKOPTIONS>(
				reinterpret_cast<const char*>(buf)[3]
				);

			// read normal data
			this->m_DataDwSize = dwbuf[1];
			bufpos = 2u;

			if (this->m_DataDwSize != 0) {
				this->m_pData = new(std::nothrow) CKDWORD[this->m_DataDwSize];
				if (m_pData == nullptr) return false;

				memcpy(this->m_pData, dwbuf + bufpos, sizeof(CKDWORD) * this->m_DataDwSize);
				bufpos += this->m_DataDwSize;
			}
			if (EnumsHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_FILE)) {
				// MARK: set ckfile = nullptr;
				;
			}
			if (EnumsHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_IDS)) {
				this->m_ObjectList.resize(dwbuf[bufpos]);
				bufpos += 1u;
				memcpy(this->m_ObjectList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ObjectList.size());
				bufpos += this->m_ObjectList.size();
			}
			if (EnumsHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_CHN)) {
				this->m_ChunkList.resize(dwbuf[bufpos]);
				bufpos += 1u;
				memcpy(this->m_ChunkList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ChunkList.size());
				bufpos += this->m_ChunkList.size();
			}
			if (EnumsHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_MAN)) {
				this->m_ManagerList.resize(dwbuf[bufpos]);
				bufpos += 1u;
				memcpy(this->m_ManagerList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ManagerList.size());
				bufpos += this->m_ManagerList.size();
			}


		} else {
			// too new. can not read, skip
			;
		}

		return true;
	}

	CKDWORD CKStateChunk::ConvertToBuffer(void* buf) {
		return 0u;
	}

#pragma endregion


	//bool CKStateChunk::UnPack(CKDWORD DestSize) {
	//	// NOTE: in UnPack. pData store the compressed buffer, and 
	//	// dwSize store the length of compressed buffer as CHAR size, not DWORD size!

	//	// create a enough buffer
	//	char* buffer = new(std::nothrow) char[DestSize];
	//	if (buffer == nullptr) return false;
	//	uLongf destSize = DestSize;
	//	// uncompress it
	//	auto err = uncompress(
	//		reinterpret_cast<Bytef*>(buffer), &destSize,
	//		reinterpret_cast<const Bytef*>(this->m_pData), static_cast<uLong>(this->m_DataDwSize)
	//	);
	//	// if no error, assign data
	//	if (err == Z_OK) {
	//		// get dw size and copy it to remove useless blank data
	//		this->m_DataDwSize = static_cast<CKDWORD>(destSize) / sizeof(CKDWORD);

	//		delete[] this->m_pData;
	//		this->m_pData = nullptr;
	//		this->m_pData = new(std::nothrow) CKDWORD[this->m_DataDwSize];
	//		if (this->m_pData != nullptr) {
	//			memcpy(this->m_pData, buffer, this->m_DataDwSize * sizeof(CKDWORD));
	//		}
	//	}
	//	delete[] buffer;
	//	return true;
	//}

	bool CKStateChunk::SeekIdentifier(CKDWORD identifier) {
		return false;
	}

	void CKStateChunk::StartRead(void) {
		if (this->m_Parser.m_Status != CKStateChunkStatus::IDLE) return;

		this->m_Parser.m_CurrentPos = 0u;
		this->m_Parser.m_DataSize = this->m_DataDwSize;
		this->m_Parser.m_PrevIdentifierPos = 0u;
		this->m_Parser.m_Status = CKStateChunkStatus::READ;
	}

	void CKStateChunk::ReadString(std::string& strl) {
		;
	}

}
