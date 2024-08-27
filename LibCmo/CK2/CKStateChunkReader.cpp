#include "CKStateChunk.hpp"
#include "CKFile.hpp"
#include "CKContext.hpp"

namespace LibCmo::CK2 {

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
		if (this->m_DataDwSize < 2u) return false;	// impossible to have a identifier

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
		*out_size = CKSizeof(CKDWORD) * (nextptr - pos - 2u);
		return true;
	}

	bool CKStateChunk::LockReadBuffer(const void** ppData, CKDWORD size_in_byte) {
		// check self status
		if (this->m_Parser.m_Status != CKStateChunkStatus::READ) return false;
		// check arguments
		if (ppData == nullptr) return false;
		*ppData = nullptr;

		// get corresponding size
		CKDWORD size_in_dword = this->GetCeilDwordSize(size_in_byte);
		// ensure space
		if (this->EnsureReadSpace(size_in_dword)) {
			*ppData = this->m_pData + this->m_Parser.m_CurrentPos;
			return true;
		} else {
			// failed, report to context
			m_BindContext->OutputToConsoleEx(u8"CKStateChunk::LockReadBuffer at buffer pos %" PRIuCKDWORD ".", this->m_Parser.m_CurrentPos);
			return false;
		}
	}

	bool CKStateChunk::UnLockReadBuffer(CKDWORD size_in_byte) {
		// check self status
		if (this->m_Parser.m_Status != CKStateChunkStatus::READ) return false;

		// get corresponding size
		CKDWORD size_in_dword = this->GetCeilDwordSize(size_in_byte);
		// ensure space
		if (this->EnsureReadSpace(size_in_dword)) {
			this->m_Parser.m_CurrentPos += size_in_dword;
			return true;
		} else {
			// failed, report to context
			m_BindContext->OutputToConsoleEx(u8"CKStateChunk::UnLockReadBuffer at buffer pos %" PRIuCKDWORD ".", this->m_Parser.m_CurrentPos);
			return false;
		}
	}

	CKStateChunk::LockedReadBuffer_t CKStateChunk::LockReadBufferWrapper(CKDWORD size_in_byte) {
		const void* pData = nullptr;
		bool ret = LockReadBuffer(&pData, size_in_byte);
		if (ret) {
			return LockedReadBuffer_t(pData, LockedReadBufferDeleter(this, size_in_byte));
		} else {
			return LockedReadBuffer_t();
		}
	}

	/* ========== Basic Data Read Functions ==========*/


	bool CKStateChunk::ReadByteData(void* data_ptr, CKDWORD size_in_byte) {
		if (data_ptr == nullptr) return false;

		const void* pData = nullptr;
		bool ret = LockReadBuffer(&pData, size_in_byte);
		if (ret) {
			std::memcpy(data_ptr, pData, size_in_byte);
			UnLockReadBuffer(size_in_byte);
			return true;
		} else {
			return false;
		}
	}

	bool CKStateChunk::ReadString(XContainer::XString* strl) {
		if (strl == nullptr) return false;

		// get byte based size
		CKDWORD strByteSize = 0u;
		if (!this->ReadStruct(strByteSize)) {
			strl->clear();
			return false;
		}

		// check blank string
		if (strByteSize == 0) {
			strl->clear();
			return true;
		}

		// read data
		// MARK: the written string has NULL terminal.
		// strByteSize also include NULL terminal, 
		// so we need minus 1 when resizing (not ReadByteData, because we still need read NULL terminal to skip it.)
		std::string cache;
		cache.resize(strByteSize - 1);
		if (!this->ReadByteData(cache.data(), strByteSize)) {
			strl->clear();
			return false;
		}

		// convert encoding
		if (!m_BindContext->GetUTF8String(cache, *strl)) {
			m_BindContext->OutputToConsole(u8"Fail to get UTF8 string when reading CKStateChunk. Some objects may be loaded incorrectly.");
			strl->clear();
			return false;
		}

		// okey
		return true;
	}

	/* ========== Complex Data Read Functions ==========*/

	bool CKStateChunk::ReadObjectID(CK_ID* id) {
		if (id == nullptr) return false;

		// get basic value
		CKDWORD gotten_id = 0;
		if (!this->ReadStruct(gotten_id)) return false;

		// different strategy according to chunk ver
		if (this->m_ChunkVersion >= CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION1) {
			// new file

			// if no doc associated, return directly
			if (this->m_BindFile == nullptr) {
				*id = static_cast<CK_ID>(gotten_id);
				return true;
			}
			// if it is positive, return corresponding value
			if ((gotten_id & 0x80000000) == 0) {
				*id = this->m_BindFile->GetFileObjectByIndex(gotten_id)->CreatedObjectId;
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

	bool CKStateChunk::ReadObjectPointer(ObjImpls::CKObject** obj) {
		CK_ID cache;
		bool ret = ReadObjectID(&cache);
		if (ret) {
			*obj = m_BindContext->GetObject(cache);
		} else {
			*obj = nullptr;
		}
		return ret;
	}

	bool CKStateChunk::ReadManagerInt(CKGUID* guid, CKINT* intval) {
		if (guid == nullptr || intval == nullptr) return false;

		// read guid first
		if (!this->ReadStruct(guid)) return false;
		// then read int value
		if (!this->ReadStruct(intval)) return false;

		return true;
	}

	//CKStateChunk* CKStateChunk::ReadSubChunk(void) {
	//	CKStateChunk* subchunk = nullptr;

	//	// get size and do a enough space check
	//	CKDWORD subDwordChunkSize;
	//	if (!this->ReadStruct(subDwordChunkSize)) goto subchunk_defer;
	//	if (!this->EnsureReadSpace(subDwordChunkSize)) goto subchunk_defer;

	//	// create statechunk
	//	subchunk = new CKStateChunk(this->m_BindFile, this->m_BindContext);

	//	// start read data
	//	// read class id
	//	if (!this->ReadStruct(subchunk->m_ClassId)) goto subchunk_defer;

	//	// different read strategy by chunk version
	//	if (this->m_ChunkVersion >= CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION1) {
	//		// new file

	//		// read combined version
	//		CKDWORD versionInfo;
	//		if (!this->ReadStruct(versionInfo)) goto subchunk_defer;
	//		subchunk->m_DataVersion = static_cast<CK_STATECHUNK_DATAVERSION>(versionInfo & 0xffff);
	//		subchunk->m_ChunkVersion = static_cast<CK_STATECHUNK_CHUNKVERSION>((versionInfo >> 16) & 0xffff);

	//		// read data size and create it
	//		if (!this->ReadStruct(subchunk->m_DataDwSize)) goto subchunk_defer;
	//		subchunk->m_pData = new CKDWORD[subchunk->m_DataDwSize];

	//		// has bind file?
	//		CKDWORD hasBindFile;
	//		if (!this->ReadStruct(hasBindFile)) goto subchunk_defer;
	//		if (hasBindFile == 1) subchunk->m_BindFile = nullptr;

	//		// 3 list size
	//		// manager only existed when ver > 4
	//		CKDWORD lssize;
	//		if (!this->ReadStruct(lssize)) goto subchunk_defer;
	//		subchunk->m_ObjectList.resize(lssize);
	//		if (!this->ReadStruct(lssize)) goto subchunk_defer;
	//		subchunk->m_ChunkList.resize(lssize);
	//		if (this->m_ChunkVersion > CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION1) {
	//			if (!this->ReadStruct(lssize)) goto subchunk_defer;
	//			subchunk->m_ManagerList.resize(lssize);
	//		}

	//		// core data
	//		if (subchunk->m_DataDwSize != 0) {
	//			if (!this->ReadByteData(subchunk->m_pData, subchunk->m_DataDwSize * CKSizeof(CKDWORD))) goto subchunk_defer;
	//		}

	//		// 3 list data
	//		if (!subchunk->m_ObjectList.empty()) {
	//			if (!this->ReadByteData(
	//				subchunk->m_ObjectList.data(),
	//				static_cast<CKDWORD>(subchunk->m_ObjectList.size()) * CKSizeof(CKDWORD)
	//				)) goto subchunk_defer;
	//		}
	//		if (!subchunk->m_ChunkList.empty()) {
	//			if (!this->ReadByteData(
	//				subchunk->m_ChunkList.data(),
	//				static_cast<CKDWORD>(subchunk->m_ChunkList.size()) * CKSizeof(CKDWORD)
	//				)) goto subchunk_defer;
	//		}
	//		if (!subchunk->m_ManagerList.empty()) {
	//			if (!this->ReadByteData(
	//				subchunk->m_ManagerList.data(),
	//				static_cast<CKDWORD>(subchunk->m_ManagerList.size()) * CKSizeof(CKDWORD)
	//				)) goto subchunk_defer;
	//		}

	//	} else {
	//		// old file

	//		// read data size and create it
	//		if (!this->ReadStruct(subchunk->m_DataDwSize)) goto subchunk_defer;
	//		subchunk->m_pData = new CKDWORD[subchunk->m_DataDwSize];

	//		// skip one?
	//		// I don't know why
	//		this->Skip(1u);

	//		// read core buf
	//		if (!this->ReadByteData(subchunk->m_pData, subchunk->m_DataDwSize * CKSizeof(CKDWORD))) goto subchunk_defer;

	//	}

	//	return subchunk;
	//subchunk_defer:
	//	if (subchunk != nullptr) delete subchunk;
	//	return nullptr;
	//}

	/* ========== Buffer Functions ==========*/

	bool CKStateChunk::ReadBuffer(void** ppData, CKDWORD* size_in_byte) {
		if (ppData == nullptr || size_in_byte == nullptr) return false;
		*ppData = nullptr;
		*size_in_byte = 0;

		// read size first
		if (!this->ReadStruct(size_in_byte)) {
			return false;
		}

		// if it is empty buffer,
		// simply return it but need return true to notify read okey.
		if (*size_in_byte == 0) {
			return true;
		}

		// use LockReadBuffer to get pointer
		auto locker = LockReadBufferWrapper(*size_in_byte);
		if (locker == nullptr) {
			*size_in_byte = 0;
			return false;
		}

		// copy data and unlock buffer
		*ppData = new CKBYTE[*size_in_byte];
		std::memcpy(*ppData, locker.get(), *size_in_byte);
		locker.reset();
		return true;
	}
	
	void CKStateChunk::DeleteBuffer(const void* buf) {
		if (buf == nullptr) return;
		delete[] static_cast<const CKBYTE*>(buf);
	}

	CKStateChunk::Buffer_t CKStateChunk::ReadBufferWrapper() {
		void* cache = nullptr;
		CKDWORD size = 0;
		if (!ReadBuffer(&cache, &size)) {
			return Buffer_t();
		}
		return Buffer_t(cache, BufferDeleter(this, size));
	}

	bool CKStateChunk::ReadAndFillBuffer(void* pData) {
		if (pData == nullptr) return false;

		// get buffer size.
		CKDWORD size_in_byte;
		if (!this->ReadStruct(size_in_byte)) {
			return false;
		}

		// use LockReadBuffer to get pointer
		// this step can process zero length buffer
		// so we do not treat it specially
		auto locker = LockReadBufferWrapper(size_in_byte);
		if (locker == nullptr) {
			return false;
		}

		// then copy the data
		std::memcpy(pData, locker.get(), size_in_byte);
		locker.reset();
		return true;
	}

	bool CKStateChunk::ReadAndFillBuffer(void* pData, CKDWORD size_in_byte) {
		if (pData == nullptr) return false;

		// directly use LockReadBuffer to get pointer
		auto locker = LockReadBufferWrapper(size_in_byte);
		if (locker == nullptr) {
			return false;
		}

		// then copy the data
		std::memcpy(pData, locker.get(), size_in_byte);
		locker.reset();
		return true;
	}

	/* ========== Sequence Functions ==========*/

	bool CKStateChunk::ReadObjectIDSequence(XContainer::XObjectArray* ls) {
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

	bool CKStateChunk::ReadManagerIntSequence(CKGUID* guid, XContainer::XArray<CKINT>* ls) {
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

	//bool CKStateChunk::ReadSubChunkSequence(XContainer::XArray<CKStateChunk*>* ls) {
	//	if (ls == nullptr) return false;

	//	// clear first
	//	for (auto& item : *ls) {
	//		if (item != nullptr)
	//			delete (item);
	//	}
	//	ls->clear();

	//	// read count
	//	CKDWORD count;
	//	if (!this->ReadStruct(count)) return false;

	//	// resize list and read it
	//	ls->resize(count, nullptr);
	//	for (size_t i = 0; i < count; ++i) {
	//		(*ls)[i] = this->ReadSubChunk();
	//		if ((*ls)[i] == nullptr) {
	//			// fail. remove all created statechunk and clear it
	//			for (auto& item : *ls) {
	//				if (item != nullptr)
	//					delete (item);
	//			}
	//			ls->clear();
	//			// return
	//			return false;
	//		}
	//	}

	//	return true;
	//}

	bool CKStateChunk::ReadXObjectArray(XContainer::XObjectArray* ls) {
		if (ls == nullptr) return false;
		ls->clear();

		// read count
		CKDWORD count;
		if (!this->ReadStruct(count)) return false;
		if (count == 0) return true;	// 0 size array

		// old file size correction
		bool old_file = this->m_ChunkVersion < CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION1;
		if (old_file) {
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

			// in old file or no bind file, the read data directly is CK_ID.
			// in new file or has bind file, the read data is the index in FileObjects
			if (old_file || this->m_BindFile == nullptr) {
				id = static_cast<CK_ID>(cache);
			} else {
				if (cache < 0) id = 0;
				else id = this->m_BindFile->GetFileObjectByIndex(cache)->CreatedObjectId;
			}
		}

		return true;
	}

	bool CKStateChunk::ReadXObjectPointerArray(XContainer::XObjectPointerArray* ls) {
		if (ls == nullptr) return false;
		ls->clear();

		// very very similar to ReadXObjectArray
		// we execute it first.
		XContainer::XObjectArray idarr;
		if (!ReadXObjectArray(idarr)) return false;

		// then convert it to pointer list
		ls->resize(idarr.size());
		for (size_t i = 0; i < idarr.size(); ++i) {
			(*ls)[i] = m_BindContext->GetObject(idarr[i]);
		}

		return true;
	}

}
