#include "VTStruct.hpp"

namespace LibCmo {

	CKStateChunk::CKStateChunk() :
		m_ClassId(CK_CLASSID::CKCID_OBJECT), m_DataDwSize(0u), m_pData(nullptr),
		m_DataVersion(CK_STATECHUNK_DATAVERSION::CHUNKDATA_CURRENTVERSION), m_ChunkVersion(CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4),
		m_Parser{ 0u, 0u, 0u },
		m_ObjectList(), m_ChunkList(), m_ManagerList()
	{
		;
	}

	CKStateChunk::~CKStateChunk() {
		if (this->m_pData != nullptr)
			delete[] this->m_pData;
	}

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
			bufpos = 2;

			if (this->m_DataDwSize != 0) {
				this->m_pData = new(std::nothrow) CKDWORD[this->m_DataDwSize];
				if (m_pData == nullptr) return false;

				memcpy(this->m_pData, dwbuf + bufpos, sizeof(CKDWORD) * this->m_DataDwSize);
				bufpos += this->m_DataDwSize;
			}
			if (EnumHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_FILE)) {
				// MARK: set ckfile = nullptr;
				;
			}
			if (EnumHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_IDS)) {
				this->m_ObjectList.resize(dwbuf[bufpos]);
				bufpos += 1u;
				memcpy(this->m_ObjectList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ObjectList.size());
				bufpos += this->m_ObjectList.size();
			}
			if (EnumHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_CHN)) {
				this->m_ChunkList.resize(dwbuf[bufpos]);
				bufpos += 1u;
				memcpy(this->m_ChunkList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ChunkList.size());
				bufpos += this->m_ChunkList.size();
			}
			if (EnumHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_MAN)) {
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

	void LibCmo::CKStateChunk::_EnsureEnoughSpace(CKDWORD size) {
		;
	}

}
