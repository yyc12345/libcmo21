#include "CKStateChunk.hpp"
#include "CKFile.hpp"
#include "CKContext.hpp"

namespace LibCmo::CK2 {

#pragma region Ctor Dtor

	CKStateChunk::CKStateChunk(CKFileVisitor* visitor, CKContext* ctx) :
		m_ClassId(CK_CLASSID::CKCID_OBJECT), m_DataDwSize(0u), m_pData(nullptr),
		m_DataVersion(CK_STATECHUNK_DATAVERSION::CHUNKDATA_CURRENTVERSION), m_ChunkVersion(CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4),
		m_Parser { CKStateChunkStatus::IDLE, 0u, 0u, 0u },
		m_ObjectList(), m_ChunkList(), m_ManagerList(),
		m_BindFile(visitor), m_BindContext(ctx)
	{}

	CKStateChunk::CKStateChunk(const CKStateChunk& rhs) :
		m_ClassId(rhs.m_ClassId), m_DataVersion(rhs.m_DataVersion), m_ChunkVersion(rhs.m_ChunkVersion),
		m_Parser(rhs.m_Parser),
		m_ObjectList(rhs.m_ObjectList), m_ManagerList(rhs.m_ManagerList), m_ChunkList(rhs.m_ChunkList),
		m_pData(nullptr), m_DataDwSize(rhs.m_DataDwSize),
		m_BindFile(rhs.m_BindFile), m_BindContext(rhs.m_BindContext) {
		// copy buffer
		if (rhs.m_pData != nullptr) {
			this->m_pData = new CKDWORD[rhs.m_DataDwSize];
				std::memcpy(this->m_pData, rhs.m_pData, sizeof(CKDWORD) * rhs.m_DataDwSize);
		}
	}

	CKStateChunk::CKStateChunk(CKStateChunk&& rhs) :
		m_ClassId(rhs.m_ClassId), m_DataVersion(rhs.m_DataVersion), m_ChunkVersion(rhs.m_ChunkVersion),
		m_Parser(rhs.m_Parser),
		m_ObjectList(std::move(rhs.m_ObjectList)), m_ManagerList(std::move(rhs.m_ManagerList)), m_ChunkList(std::move(rhs.m_ChunkList)),
		m_pData(rhs.m_pData), m_DataDwSize(rhs.m_DataDwSize),
		m_BindFile(rhs.m_BindFile), m_BindContext(rhs.m_BindContext) {
		// set to null after steal data
		rhs.m_pData = nullptr;
		// and clear it
		rhs.Clear();
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

		this->m_BindFile = rhs.m_BindFile;
		this->m_BindContext = rhs.m_BindContext;

		// copy buffer
		if (rhs.m_pData != nullptr) {
			this->m_pData = new CKDWORD[rhs.m_DataDwSize];
				std::memcpy(this->m_pData, rhs.m_pData, sizeof(CKDWORD) * rhs.m_DataDwSize);
		}
		this->m_DataDwSize = rhs.m_DataDwSize;

		return *this;
	}

	CKStateChunk& CKStateChunk::operator=(CKStateChunk&& rhs) {
		this->Clear();

		this->m_DataVersion = rhs.m_DataVersion;
		this->m_ChunkVersion = rhs.m_ChunkVersion;
		this->m_ClassId = rhs.m_ClassId;

		this->m_Parser = rhs.m_Parser;

		this->m_ObjectList = rhs.m_ObjectList;
		this->m_ManagerList = rhs.m_ManagerList;
		this->m_ChunkList = rhs.m_ChunkList;

		this->m_BindFile = rhs.m_BindFile;
		this->m_BindContext = rhs.m_BindContext;

		// steal buffer
		this->m_pData = rhs.m_pData;
		rhs.m_pData = nullptr;
		this->m_DataDwSize = rhs.m_DataDwSize;

		// clear steal chunk
		rhs.Clear();

		return *this;
	}

	CKStateChunk::~CKStateChunk() {
		if (this->m_pData != nullptr)
			delete[] this->m_pData;
	}

#pragma endregion

#pragma region Self Used Data Struct

	void CKStateChunk::LockedReadBufferDeleter::operator()(LIBCMO_UNUSED const void* buf) {
		if (m_Host == nullptr) return;
		m_Host->UnLockReadBuffer(m_ConsumedSize);
	}

	void CKStateChunk::LockedReadBufferDeleter::SetConsumedSize(CKDWORD newsize) {
		m_ConsumedSize = newsize;
	}
	
	void CKStateChunk::LockedWriteBufferDeleter::operator()(LIBCMO_UNUSED const void* buf) {
		if (m_Host == nullptr) return;
		m_Host->UnLockWriteBuffer(m_ConsumedSize);
	}

	void CKStateChunk::LockedWriteBufferDeleter::SetConsumedSize(CKDWORD newsize) {
		m_ConsumedSize = newsize;
	}
	
	void CKStateChunk::BufferDeleter::operator()(const void* buf) {
		if (m_Host == nullptr) return;
		m_Host->DeleteBuffer(buf);
	}

	CKDWORD CKStateChunk::BufferDeleter::GetBufferSize() const {
		return m_BufSize;
	}

#pragma endregion
	
#pragma region Misc Funcs

	// ========== Public Funcs ==========

	const CKStateChunk::ProfileStateChunk_t CKStateChunk::GetStateChunkProfile() {
		return CKStateChunk::ProfileStateChunk_t {
			.m_ClassId = this->m_ClassId,
			.m_DataDwSize = this->m_DataDwSize,
			.m_pData = this->m_pData,

			.m_DataVersion = this->m_DataVersion,
			.m_ChunkVersion = this->m_ChunkVersion,

			.m_ObjectListSize = this->m_ObjectList.size(),
			.m_ChunkListSize = this->m_ChunkList.size(),
			.m_ManagerListSize = this->m_ManagerList.size(),

			.m_BindFile = this->m_BindFile,
			.m_BindContext = this->m_BindContext,
		};
	}
	
	const XContainer::XArray<CKStateChunk::ProfileIdentifier_t> CKStateChunk::GetIdentifiersProfile() {
		XContainer::XArray<CKStateChunk::ProfileIdentifier_t> collection;
		if (this->m_Parser.m_Status != CKStateChunkStatus::READ) return collection;

		CKDWORD pos = 0u;
		if (this->m_DataDwSize < 2u) return collection;	// impossible to have a identifier

		// iterate identifier
		while (true) {
			// add current identifier
			CKDWORD nextptr = this->m_pData[pos + 1];
			if (nextptr == 0u) {
				nextptr = this->m_DataDwSize;	// got tail. no more identifier
			}
			collection.emplace_back(CKStateChunk::ProfileIdentifier_t{
				this->m_pData[pos],
				this->m_pData + pos + 2,
				CKSizeof(CKDWORD) * (nextptr - pos - 2u)
			});

			// move to next identifier or exit
			// got tail. no more identifier
			if (this->m_pData[pos + 1] == 0u) break;

			pos = this->m_pData[pos + 1];

			// out of buffer
			if (pos + 1 >= this->m_DataDwSize) break;
		};
		return collection;

	}

	void CKStateChunk::Clear() {
		this->m_ClassId = CK_CLASSID::CKCID_OBJECT;
		this->m_DataVersion = CK_STATECHUNK_DATAVERSION::CHUNK_DEV_2_1;
		this->m_ChunkVersion = CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4;

		this->m_Parser.m_Status = CKStateChunkStatus::IDLE;
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

	CKDWORD CKStateChunk::GetDataSize() const {
		return CKSizeof(CKDWORD) * this->m_DataDwSize;
	}

	CK_STATECHUNK_DATAVERSION CKStateChunk::GetDataVersion() const {
		return this->m_DataVersion;
	}

	void CKStateChunk::SetDataVersion(CK_STATECHUNK_DATAVERSION version) {
		this->m_DataVersion = version;
	}

	CK_CLASSID CKStateChunk::GetClassId() const {
		return this->m_ClassId;
	}

	void CKStateChunk::SetClassId(CK_CLASSID cid) {
		this->m_ClassId = cid;
	}

	bool CKStateChunk::Skip(CKDWORD DwordCount) {
		bool result;
		switch (this->m_Parser.m_Status) {
			case CKStateChunkStatus::READ:
				result = EnsureReadSpace(DwordCount);
				break;
			case CKStateChunkStatus::WRITE:
				result = EnsureWriteSpace(DwordCount);
				break;
			case CKStateChunkStatus::IDLE:
			default:
				result = false;
				break;
		}

		// if success, move cursor
		if (result) {
			this->m_Parser.m_CurrentPos += DwordCount;
		}
		return result;
	}

	// ========== Private Funcs ==========

	CKDWORD CKStateChunk::GetCeilDwordSize(size_t char_size) {
		return static_cast<CKDWORD>((char_size + 3) >> 2);
	}

	bool CKStateChunk::ResizeBuffer(CKDWORD new_dwsize) {
		if (new_dwsize == 0u) {
			// if reuqired size is zero, we just delete it
			if (this->m_pData != nullptr) {
				delete[] this->m_pData;
				this->m_pData = nullptr;
			}
		} else {
			// otherwise, we create a new buffer instead it
			CKDWORD* newbuf = new CKDWORD[new_dwsize];

			// if no original data, we do not need copy it and free it
			if (this->m_pData != nullptr) {
				std::memcpy(newbuf, this->m_pData, sizeof(CKDWORD) * new_dwsize);
				delete[] this->m_pData;
			}

			// assign new buffer
			this->m_pData = newbuf;
		}

		return true;
	}

	bool CKStateChunk::EnsureWriteSpace(CKDWORD dwsize) {
		if (this->m_Parser.m_Status != CKStateChunkStatus::WRITE) return false;

		// check whether need enlarge
		CKDWORD needed = dwsize + this->m_Parser.m_CurrentPos;
		if (needed > this->m_Parser.m_DataSize) {
			// add a very enough space to buffer
			if (dwsize < 512) dwsize = 512;
			needed = dwsize + this->m_Parser.m_CurrentPos;

			// try resizing it
			if (!this->ResizeBuffer(needed)) return false;

			// update size
			this->m_Parser.m_DataSize = needed;
		}

		return true;
	}

	bool CKStateChunk::EnsureReadSpace(CKDWORD dword_required) {
		return (m_Parser.m_Status == CKStateChunkStatus::READ) &&
			(this->m_Parser.m_CurrentPos + dword_required <= this->m_Parser.m_DataSize);
	}


#pragma endregion

#pragma region Buffer Related

	bool CKStateChunk::ConvertFromBuffer(const void* buf) {
		if (buf == nullptr) return false;
		this->Clear();

		// read chunk ver and data ver first
		// chunk ver always set in the 3rd BYTE in every format
		this->m_ChunkVersion = static_cast<CK_STATECHUNK_CHUNKVERSION>(
			reinterpret_cast<const CKBYTE*>(buf)[2]
			);
		// data ver always set in the 1st BYTE in every format
		this->m_DataVersion = static_cast<CK_STATECHUNK_DATAVERSION>(
			reinterpret_cast<const CKBYTE*>(buf)[0]
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
				this->m_pData = new CKDWORD[this->m_DataDwSize];
				std::memcpy(this->m_pData, dwbuf + bufpos, sizeof(CKDWORD) * this->m_DataDwSize);
				bufpos += this->m_DataDwSize;
			}
			if (!this->m_ObjectList.empty()) {
				std::memcpy(this->m_ObjectList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ObjectList.size());
				bufpos += this->m_ObjectList.size();
			}
			if (!this->m_ChunkList.empty()) {
				std::memcpy(this->m_ChunkList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ChunkList.size());
				bufpos += this->m_ChunkList.size();
			}
			
			// no bind file
			this->m_BindFile = nullptr;

		} else if (this->m_ChunkVersion == CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION2) {
			// medium ver file

			this->m_ClassId = static_cast<CK_CLASSID>(dwbuf[1]);
			this->m_DataDwSize = dwbuf[2];

			this->m_ObjectList.resize(dwbuf[4]);
			this->m_ChunkList.resize(dwbuf[5]);
			this->m_ManagerList.resize(dwbuf[6]);
			bufpos = 7u;

			if (this->m_DataDwSize != 0) {
				this->m_pData = new CKDWORD[this->m_DataDwSize];
				std::memcpy(this->m_pData, dwbuf + bufpos, sizeof(CKDWORD) * this->m_DataDwSize);
				bufpos += this->m_DataDwSize;
			}
			if (!this->m_ObjectList.empty()) {
				std::memcpy(this->m_ObjectList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ObjectList.size());
				bufpos += this->m_ObjectList.size();
			}
			if (!this->m_ChunkList.empty()) {
				std::memcpy(this->m_ChunkList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ChunkList.size());
				bufpos += this->m_ChunkList.size();
			}
			if (!this->m_ManagerList.empty()) {
				std::memcpy(this->m_ManagerList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ManagerList.size());
				bufpos += this->m_ManagerList.size();
			}

			// no bind file
			this->m_BindFile = nullptr;

		} else if (this->m_ChunkVersion <= CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4) {
			// the latest file

			// re-read some extra data
			// class id located the 2nd BYTE
			this->m_ClassId = static_cast<CK_CLASSID>(
				reinterpret_cast<const CKBYTE*>(buf)[1]
				);
			// options located the 4th BYTE
			CK_STATECHUNK_CHUNKOPTIONS options = static_cast<CK_STATECHUNK_CHUNKOPTIONS>(
				reinterpret_cast<const CKBYTE*>(buf)[3]
				);

			// read normal data
			this->m_DataDwSize = dwbuf[1];
			bufpos = 2u;

			if (this->m_DataDwSize != 0) {
				this->m_pData = new CKDWORD[this->m_DataDwSize];
				std::memcpy(this->m_pData, dwbuf + bufpos, sizeof(CKDWORD) * this->m_DataDwSize);
				bufpos += this->m_DataDwSize;
			}
			if (!EnumsHelper::Has(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_FILE)) {
				// forced no bind file
				this->m_BindFile = nullptr;
			}
			if (EnumsHelper::Has(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_IDS)) {
				this->m_ObjectList.resize(dwbuf[bufpos]);
				bufpos += 1u;
				std::memcpy(this->m_ObjectList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ObjectList.size());
				bufpos += this->m_ObjectList.size();
			}
			if (EnumsHelper::Has(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_CHN)) {
				this->m_ChunkList.resize(dwbuf[bufpos]);
				bufpos += 1u;
				std::memcpy(this->m_ChunkList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ChunkList.size());
				bufpos += this->m_ChunkList.size();
			}
			if (EnumsHelper::Has(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_MAN)) {
				this->m_ManagerList.resize(dwbuf[bufpos]);
				bufpos += 1u;
				std::memcpy(this->m_ManagerList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ManagerList.size());
				bufpos += this->m_ManagerList.size();
			}


		} else {
			// too new. can not read, skip
			return true;
		}

		return true;
	}

	CKDWORD CKStateChunk::ConvertToBuffer(void* buf) {
		// calc size and setup options first
		// size = buffer + buffer_size + header
		CKDWORD size = (m_DataDwSize * CKSizeof(CKDWORD)) + CKSizeof(CKDWORD) + CKSizeof(CKDWORD);
		CK_STATECHUNK_CHUNKOPTIONS options = static_cast<CK_STATECHUNK_CHUNKOPTIONS>(0);

		if (!m_ObjectList.empty()) {
			size += CKSizeof(CKDWORD) * static_cast<CKDWORD>(m_ObjectList.size()) + sizeof(CKDWORD);
			EnumsHelper::Add(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_IDS);
		}
		if (!m_ChunkList.empty()) {
			size += CKSizeof(CKDWORD) * static_cast<CKDWORD>(m_ChunkList.size()) + sizeof(CKDWORD);
			EnumsHelper::Add(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_CHN);
		}
		if (!m_ManagerList.empty()) {
			size += CKSizeof(CKDWORD) * static_cast<CKDWORD>(m_ManagerList.size()) + sizeof(CKDWORD);
			EnumsHelper::Add(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_MAN);
		}

		if (this->m_BindFile != nullptr) {
			EnumsHelper::Add(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_FILE);
		}

		// if buffer provided, write it
		if (buf != nullptr) {
			// write header
			reinterpret_cast<CKBYTE*>(buf)[0] = static_cast<CKBYTE>(this->m_DataVersion);
			reinterpret_cast<CKBYTE*>(buf)[1] = static_cast<CKBYTE>(this->m_ClassId);
			reinterpret_cast<CKBYTE*>(buf)[2] = static_cast<CKBYTE>(this->m_ChunkVersion);
			reinterpret_cast<CKBYTE*>(buf)[3] = static_cast<CKBYTE>(options);

			CKDWORD* dwbuf = reinterpret_cast<CKDWORD*>(buf);
			// write buffer length
			dwbuf[1] = this->m_DataDwSize;
			size_t bufpos = 2u;
			// write buffer
			std::memcpy(dwbuf + bufpos, this->m_pData, this->m_DataDwSize * sizeof(CKDWORD));
			bufpos += this->m_DataDwSize;

			// write list
			if (!m_ObjectList.empty()) {
				dwbuf[bufpos] = static_cast<CKDWORD>(m_ObjectList.size());
				std::memcpy(dwbuf + bufpos + 1, m_ObjectList.data(), m_ObjectList.size() * sizeof(CKDWORD));
				bufpos += m_ObjectList.size() + 1;
			}
			if (!m_ChunkList.empty()) {
				dwbuf[bufpos] = static_cast<CKDWORD>(m_ChunkList.size());
				std::memcpy(dwbuf + bufpos + 1, m_ChunkList.data(), m_ChunkList.size() * sizeof(CKDWORD));
				bufpos += m_ChunkList.size() + 1;
			}
			if (!m_ManagerList.empty()) {
				dwbuf[bufpos] = static_cast<CKDWORD>(m_ManagerList.size());
				std::memcpy(dwbuf + bufpos + 1, m_ManagerList.data(), m_ManagerList.size() * sizeof(CKDWORD));
				bufpos += m_ManagerList.size() + 1;
			}

		}

		// return expected size.
		return size;
	}

#pragma endregion

}
