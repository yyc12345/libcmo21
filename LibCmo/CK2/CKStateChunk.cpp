#include "VTUtils.hpp"
#include "CKStateChunk.hpp"
#include "CKMinContext.hpp"
#include "CKFile.hpp"

namespace LibCmo::CK2 {

#pragma region Ctor Dtor

	//CKStateChunk::CKStateChunk() :
	//	m_ClassId(CK_CLASSID::CKCID_OBJECT), m_DataDwSize(0u), m_pData(nullptr),
	//	m_DataVersion(CK_STATECHUNK_DATAVERSION::CHUNKDATA_CURRENTVERSION), m_ChunkVersion(CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4),
	//	m_Parser{ CKStateChunkStatus::IDLE, 0u, 0u, 0u },
	//	m_ObjectList(), m_ChunkList(), m_ManagerList(),
	//	m_BindDoc(nullptr), m_BindContext(nullptr)
	//{
	//	;
	//}
	CKStateChunk::CKStateChunk(CKFileDocument* doc, CKMinContext* ctx) :
		m_ClassId(CK_CLASSID::CKCID_OBJECT), m_DataDwSize(0u), m_pData(nullptr),
		m_DataVersion(CK_STATECHUNK_DATAVERSION::CHUNKDATA_CURRENTVERSION), m_ChunkVersion(CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4),
		m_Parser{ CKStateChunkStatus::IDLE, 0u, 0u, 0u },
		m_ObjectList(), m_ChunkList(), m_ManagerList(),
		m_BindDoc(doc), m_BindContext(ctx)
	{
		;
	}
	CKStateChunk::CKStateChunk(const CKStateChunk& rhs) :
		m_ClassId(rhs.m_ClassId), m_DataVersion(rhs.m_DataVersion), m_ChunkVersion(rhs.m_ChunkVersion),
		m_Parser(rhs.m_Parser),
		m_ObjectList(rhs.m_ObjectList), m_ManagerList(rhs.m_ManagerList), m_ChunkList(rhs.m_ChunkList),
		m_pData(nullptr), m_DataDwSize(rhs.m_DataDwSize),
		m_BindDoc(rhs.m_BindDoc), m_BindContext(rhs.m_BindContext) {
		// copy buffer
		if (rhs.m_pData != nullptr) {
			this->m_pData = new(std::nothrow) CKDWORD[rhs.m_DataDwSize];
			if (this->m_pData != nullptr) {
				std::memcpy(this->m_pData, rhs.m_pData, sizeof(CKDWORD) * rhs.m_DataDwSize);
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

		this->m_BindDoc = rhs.m_BindDoc;
		this->m_BindContext = rhs.m_BindContext;

		// copy buffer
		if (rhs.m_pData != nullptr) {
			this->m_pData = new(std::nothrow) CKDWORD[rhs.m_DataDwSize];
			if (this->m_pData != nullptr) {
				std::memcpy(this->m_pData, rhs.m_pData, sizeof(CKDWORD) * rhs.m_DataDwSize);
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
		this->m_DataVersion = CK_STATECHUNK_DATAVERSION::CHUNK_DEV_2_1;
		this->m_ChunkVersion = CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4;

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

	CK_STATECHUNK_DATAVERSION CKStateChunk::GetDataVersion() {
		return this->m_DataVersion;
	}

	void CKStateChunk::SetDataVersion(CK_STATECHUNK_DATAVERSION version) {
		this->m_DataVersion = version;
	}

	void CKStateChunk::DeleteBuffer(void* buf) {
		if (buf == nullptr) return;
		delete[] reinterpret_cast<char*>(buf);
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



	size_t CKStateChunk::GetCeilDwordSize(size_t char_size) {
		return (char_size + 3) >> 2;
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
			CKDWORD* newbuf = new(std::nothrow) CKDWORD[new_dwsize];
			if (newbuf == nullptr) return false;	// if fail to create, return

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

				std::memcpy(this->m_pData, dwbuf + bufpos, sizeof(CKDWORD) * this->m_DataDwSize);
				bufpos += this->m_DataDwSize;
			}
			if (EnumsHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_FILE)) {
				// MARK: set ckfile = nullptr;
				;
			}
			if (EnumsHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_IDS)) {
				this->m_ObjectList.resize(dwbuf[bufpos]);
				bufpos += 1u;
				std::memcpy(this->m_ObjectList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ObjectList.size());
				bufpos += this->m_ObjectList.size();
			}
			if (EnumsHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_CHN)) {
				this->m_ChunkList.resize(dwbuf[bufpos]);
				bufpos += 1u;
				std::memcpy(this->m_ChunkList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ChunkList.size());
				bufpos += this->m_ChunkList.size();
			}
			if (EnumsHelper::FlagEnumHas(options, CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_MAN)) {
				this->m_ManagerList.resize(dwbuf[bufpos]);
				bufpos += 1u;
				std::memcpy(this->m_ManagerList.data(), dwbuf + bufpos, sizeof(CKDWORD) * this->m_ManagerList.size());
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
	//			std::memcpy(this->m_pData, buffer, this->m_DataDwSize * sizeof(CKDWORD));
	//		}
	//	}
	//	delete[] buffer;
	//	return true;
	//}

#pragma region Read Functions

	void CKStateChunk::StartRead(void) {
		if (this->m_Parser.m_Status != CKStateChunkStatus::IDLE) return;

		this->m_Parser.m_CurrentPos = 0u;
		this->m_Parser.m_DataSize = this->m_DataDwSize;
		this->m_Parser.m_PrevIdentifierPos = 0u;
		this->m_Parser.m_Status = CKStateChunkStatus::READ;
	}

	void CKStateChunk::StopRead(void) {
		if (this->m_Parser.m_Status != CKStateChunkStatus::READ) return;

		this->m_Parser.m_CurrentPos = 0u;
		this->m_Parser.m_DataSize = this->m_DataDwSize;
		this->m_Parser.m_PrevIdentifierPos = 0u;
		this->m_Parser.m_Status = CKStateChunkStatus::IDLE;
	}

	/* ========== Identifier Functions ==========*/

	bool CKStateChunk::SeekIdentifierDword(CKDWORD identifier) {
		CKDWORD cache;
		return SeekIdentifierDwordAndReturnSize(identifier, &cache);
	}

	bool CKStateChunk::SeekIdentifierDwordAndReturnSize(CKDWORD identifier, CKDWORD* out_size) {
		if (this->m_Parser.m_Status != CKStateChunkStatus::READ) return false;

		CKDWORD pos = 0u;
		if (this->m_DataDwSize < 2) return false;	// impossible to have a identifier

		// search identifier
		while (this->m_pData[pos] != identifier) {
			pos = this->m_pData[pos + 1];
			if (pos == 0u) return false;	// got tail. no more identifier
			if (pos + 1 >= this->m_DataDwSize) return false;	// out of buffer
		}

		// got identifier
		this->m_Parser.m_PrevIdentifierPos = pos;
		this->m_Parser.m_CurrentPos = pos + 2;

		// calc size
		CKDWORD nextptr = this->m_pData[pos + 1];
		if (nextptr == 0) {
			// the last identifier, use chunk size instead
			nextptr = this->m_DataDwSize;
		}
		*out_size = sizeof(CKDWORD) * (nextptr - pos);
		return true;
	}

	/* ========== Basic Data Read Functions ==========*/

	bool CKStateChunk::ReadByteData(void* data_ptr, CKDWORD size_in_byte) {
		if (this->m_Parser.m_Status != CKStateChunkStatus::READ) return false;
		if (data_ptr == nullptr) return false;

		CKDWORD size_in_dword = this->GetCeilDwordSize(size_in_byte);
		if (this->EnsureReadSpace(size_in_dword)) {
			std::memcpy(data_ptr, this->m_pData + this->m_Parser.m_CurrentPos, size_in_byte);
			return true;
		} else {
			// failed, report to context
			m_BindContext->Printf("CKStateChunk read length error at %" PRICKdword ".", this->m_Parser.m_CurrentPos);
			return false;
		}
	}

	bool CKStateChunk::ReadString(std::string* strl) {
		if (strl == nullptr) return false;

		// get byte based size
		CKDWORD strByteSize = 0u;
		if (!this->ReadStruct(strByteSize)) {
			strl->clear();
			return false;
		}

		// read data
		strl->resize(strByteSize);
		if (!this->ReadByteData(strl->data(), strByteSize)) {
			strl->clear();
			return false;
		}
		return true;
	}

	/* ========== Complex Data Read Functions ==========*/

	bool CKStateChunk::ReadObjectID(CK_ID* id) {
		if (id == nullptr) return false;

		// get basic value
		CKINT gotten_id = 0;
		if (!this->ReadStruct(gotten_id)) return false;

		// different strategy according to chunk ver
		if (this->m_ChunkVersion >= CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION1) {
			// new file

			// if no doc associated, return directly
			if (this->m_BindDoc == nullptr) {
				*id = static_cast<CK_ID>(gotten_id);
				return true;
			}
			// if it is positive, return corresponding value
			if (gotten_id >= 0) {
				*id = this->m_BindDoc->m_FileObjects[gotten_id].CreatedObject;
				return true;
			}

		} else {
			// old file
			// i don't know why I need skip 2 DWORD
			// just copy IDA code.

			if (gotten_id) {
				this->Skip(2);
				return this->ReadStruct(id);
			}
		}

		// all failed
		*id = 0u;
		return false;
	}

	bool CKStateChunk::ReadManagerInt(CKGUID* guid, CKINT* intval) {
		if (guid == nullptr || intval == nullptr) return false;

		// read guid first
		if (!this->ReadStruct(guid)) return false;
		// then read int value
		if (!this->ReadStruct(intval)) return false;

		return true;
	}

	CKStateChunk* CKStateChunk::ReadSubChunk(void) {
		CKStateChunk* subchunk = nullptr;

		// get size and do a enough space check
		CKDWORD subChunkSize;
		if (!this->ReadStruct(subChunkSize)) goto subchunk_defer;
		if (!this->EnsureReadSpace(subChunkSize)) goto subchunk_defer;

		// create statechunk
		subchunk = new(std::nothrow) CKStateChunk(this->m_BindDoc, this->m_BindContext);
		if (subchunk == nullptr) goto subchunk_defer;

		// start read data
		// read class id
		if (!this->ReadStruct(subchunk->m_ClassId)) goto subchunk_defer;

		// different read strategy by chunk version
		if (this->m_ChunkVersion >= CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION1) {
			// new file

			// read combined version
			CKDWORD versionInfo;
			if (!this->ReadStruct(versionInfo)) goto subchunk_defer;
			subchunk->m_DataVersion = static_cast<CK_STATECHUNK_DATAVERSION>(versionInfo & 0xffff);
			subchunk->m_ChunkVersion = static_cast<CK_STATECHUNK_CHUNKVERSION>((versionInfo >> 16) & 0xffff);

			// read data size and create it
			if (!this->ReadStruct(subchunk->m_DataDwSize)) goto subchunk_defer;
			subchunk->m_pData = new(std::nothrow) CKDWORD[subchunk->m_DataDwSize];
			if (subchunk->m_pData == nullptr) goto subchunk_defer;

			// has bind file?
			CKDWORD hasBindFile;
			if (!this->ReadStruct(hasBindFile)) goto subchunk_defer;
			if (hasBindFile == 1) subchunk->m_BindDoc = nullptr;

			// 3 list size
			// manager only existed when ver > 4
			CKDWORD lssize;
			if (!this->ReadStruct(lssize)) goto subchunk_defer;
			subchunk->m_ObjectList.resize(lssize);
			if (!this->ReadStruct(lssize)) goto subchunk_defer;
			subchunk->m_ChunkList.resize(lssize);
			if (this->m_ChunkVersion > CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION1) {
				if (!this->ReadStruct(lssize)) goto subchunk_defer;
				subchunk->m_ManagerList.resize(lssize);
			}

			// core data
			if (subchunk->m_DataDwSize != 0) {
				if (!this->ReadByteData(subchunk->m_pData, subchunk->m_DataDwSize * sizeof(CKDWORD))) goto subchunk_defer;
			}

			// 3 list data
			if (!subchunk->m_ObjectList.empty()) {
				if (!this->ReadByteData(
					subchunk->m_ObjectList.data(),
					subchunk->m_ObjectList.size() * sizeof(CKDWORD)
					)) goto subchunk_defer;
			}
			if (!subchunk->m_ChunkList.empty()) {
				if (!this->ReadByteData(
					subchunk->m_ChunkList.data(),
					subchunk->m_ChunkList.size() * sizeof(CKDWORD)
					)) goto subchunk_defer;
			}
			if (!subchunk->m_ManagerList.empty()) {
				if (!this->ReadByteData(
					subchunk->m_ManagerList.data(),
					subchunk->m_ManagerList.size() * sizeof(CKDWORD)
					)) goto subchunk_defer;
			}

		} else {
			// old file

			// read data size and create it
			if (!this->ReadStruct(subchunk->m_DataDwSize)) goto subchunk_defer;
			subchunk->m_pData = new(std::nothrow) CKDWORD[subchunk->m_DataDwSize];
			if (subchunk->m_pData == nullptr) goto subchunk_defer;

			// skip one?
			// I don't know why
			this->Skip(1u);

			// read core buf
			if (!this->ReadByteData(subchunk->m_pData, subchunk->m_DataDwSize * sizeof(CKDWORD))) goto subchunk_defer;

		}

		return subchunk;
	subchunk_defer:
		if (subchunk != nullptr) delete subchunk;
		return nullptr;
	}

	/* ========== Buffer Functions ==========*/

	bool CKStateChunk::ReadNoSizeBuffer(CKDWORD size_in_byte, void* allocatedBuf) {
		if (allocatedBuf == nullptr) return false;
		return this->ReadByteData(allocatedBuf, size_in_byte);
	}

	bool CKStateChunk::ReadBuffer(void** buf, CKDWORD* len_in_byte) {
		if (buf == nullptr || len_in_byte == nullptr) return false;

		// get buffer size.
		CKDWORD bufByteSize = 0u;
		if (!this->ReadStruct(bufByteSize)) {
			*buf = nullptr;
			*len_in_byte = 0;
			return false;
		}
		*len_in_byte = bufByteSize;

		// create buffer
		*buf = new(std::nothrow) char[bufByteSize];
		if (*buf == nullptr) {
			*len_in_byte = 0;
			return false;
		}

		// read data
		if (!this->ReadByteData(*buf, bufByteSize)) {
			this->DeleteBuffer(*buf);
			*buf = nullptr;
			*len_in_byte = 0;
			return false;
		}

		return true;
	}

	/* ========== Sequence Functions ==========*/

	bool CKStateChunk::ReadObjectIDSequence(std::vector<CK_ID>* ls) {
		if (ls == nullptr) return false;
		ls->clear();

		// read count
		CKDWORD count;
		if (!this->ReadStruct(count)) return false;

		// resize list and read it
		ls->resize(count);
		for (size_t i = 0; i < count; ++i) {
			if (!this->ReadObjectID(ls->data() + i)) {
				ls->clear();
				return false;
			}
		}

		return true;
	}

	bool CKStateChunk::ReadManagerIntSequence(CKGUID* guid, std::vector<CKINT>* ls) {
		if (guid == nullptr || ls == nullptr) return false;

		// read count
		CKDWORD count;
		if (!this->ReadStruct(count)) return false;

		// read guid
		if (!this->ReadStruct(guid))  return false;

		// resize list and read it
		ls->resize(count);
		for (size_t i = 0; i < count; ++i) {
			if (!this->ReadStruct(ls->data() + i)) {
				ls->clear();
				return false;
			}
		}

		return true;
	}

	bool CKStateChunk::ReadSubChunkSequence(std::vector<CKStateChunk*>* ls) {
		if (ls == nullptr) return false;

		// clear first
		for (auto& item : *ls) {
			if (item != nullptr)
				delete (item);
		}
		ls->clear();

		// read count
		CKDWORD count;
		if (!this->ReadStruct(count)) return false;

		// resize list and read it
		ls->resize(count, nullptr);
		for (size_t i = 0; i < count; ++i) {
			(*ls)[i] = this->ReadSubChunk();
			if ((*ls)[i] == nullptr) {
				// fail. remove all created statechunk and clear it
				for (auto& item : *ls) {
					if (item != nullptr)
						delete (item);
				}
				ls->clear();
				// return
				return false;
			}
		}

		return true;
	}

	bool CKStateChunk::ReadObjectArray(std::vector<CK_ID>* ls) {
		if (ls == nullptr) return false;
		ls->clear();

		// read count
		CKDWORD count;
		if (!this->ReadStruct(count)) return false;
		if (!count) return true;	// 0 size array
		
		// old file size correction
		if (this->m_ChunkVersion < CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION1) {
			// skip 4. but I don't know why!!!
			this->Skip(4);
			if (!this->ReadStruct(count)) return false;
		}

		// resize list and read
		ls->resize(count);
		for (auto& id : *ls) {
			// read ID first
			CKINT cache;
			if (!this->ReadStruct(cache)) {
				ls->clear();
				return false;
			}

			// remap id
			if (this->m_BindDoc != nullptr) {
				if (cache < 0) {
					id = 0u;
				} else {
					id = this->m_BindDoc->m_FileObjects[cache].CreatedObject;
				}
			} else {
				id = static_cast<CK_ID>(cache);
			}
		}

		return true;
	}

#pragma endregion


#pragma region Write Functions

	void CKStateChunk::StartWrite() {
		if (this->m_Parser.m_Status != CKStateChunkStatus::IDLE) return;

		// delete all current buffer
		if (this->m_pData != nullptr) {
			delete[] this->m_pData;
			this->m_pData = nullptr;
		}
		this->m_DataDwSize = 0u;

		// reset parser
		this->m_Parser.m_CurrentPos = 0u;
		this->m_Parser.m_DataSize = this->m_DataDwSize;
		this->m_Parser.m_PrevIdentifierPos = 0u;

		// force chunk version
		this->m_ChunkVersion = CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4;

		// switch status
		this->m_Parser.m_Status = CKStateChunkStatus::WRITE;
	}

	void CKStateChunk::StopWrite(void) {
		if (this->m_Parser.m_Status != CKStateChunkStatus::WRITE) return;

		// update buffer size
		this->m_DataDwSize = this->m_Parser.m_CurrentPos;
		// shrink it
		ResizeBuffer(this->m_DataDwSize);

		// shrink 3 vector also
		this->m_ObjectList.shrink_to_fit();
		this->m_ManagerList.shrink_to_fit();
		this->m_ChunkList.shrink_to_fit();

		// reset parser
		this->m_Parser.m_CurrentPos = 0u;
		this->m_Parser.m_DataSize = this->m_DataDwSize;
		this->m_Parser.m_PrevIdentifierPos = 0u;
		this->m_Parser.m_Status = CKStateChunkStatus::IDLE;
	}

#pragma endregion


}
