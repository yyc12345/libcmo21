#include "CKStateChunk.hpp"
#include "CKFile.hpp"
#include "CKContext.hpp"
#include "ObjImpls/CKObject.hpp"

namespace LibCmo::CK2 {

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

	/* ========== Identifier Functions ==========*/

	bool CKStateChunk::WriteIdentifierDword(CKDWORD identifier) {
		// check self status
		if (this->m_Parser.m_Status != CKStateChunkStatus::WRITE) return false;
		// make sure there are 2 DWORD space for writing identifier header
		if (!EnsureWriteSpace(2)) return false;

		// update the last identifier header to fill its length indicator
		if (m_Parser.m_PrevIdentifierPos < m_Parser.m_CurrentPos) {
			m_pData[m_Parser.m_PrevIdentifierPos + 1] = m_Parser.m_CurrentPos;
		}

		// set prev ident to this new created ident
		m_Parser.m_PrevIdentifierPos = m_Parser.m_CurrentPos;
		// write identifier and set default next ident data
		m_pData[m_Parser.m_CurrentPos++] = identifier;
		m_pData[m_Parser.m_CurrentPos++] = 0;
		return true;
	}

	/* ========== Write Buffer Controller ==========*/

	bool CKStateChunk::LockWriteBuffer(void** ppData, CKDWORD size_in_byte) {
		// same as LockReadBuffer with slight difference.
		if (this->m_Parser.m_Status != CKStateChunkStatus::WRITE) return false;
		if (*ppData == nullptr) return false;
		*ppData = nullptr;

		CKDWORD size_in_dword = this->GetCeilDwordSize(size_in_byte);
		if (this->EnsureWriteSpace(size_in_dword)) {
			*ppData = this->m_pData + this->m_Parser.m_CurrentPos;
			return true;
		} else {
			m_BindContext->OutputToConsoleEx("CKStateChunk::LockWriteBuffer at buffer pos %" PRIuCKDWORD ".", this->m_Parser.m_CurrentPos);
			return false;
		}
	}

	bool CKStateChunk::UnLockWriteBuffer(CKDWORD size_in_byte) {
		// same as UnLockReadBuffer with slight difference.
		if (this->m_Parser.m_Status != CKStateChunkStatus::WRITE) return false;

		CKDWORD size_in_dword = this->GetCeilDwordSize(size_in_byte);
		if (this->EnsureWriteSpace(size_in_dword)) {
			this->m_Parser.m_CurrentPos += size_in_dword;
			return true;
		} else {
			m_BindContext->OutputToConsoleEx("CKStateChunk::UnLockWriteBuffer at buffer pos %" PRIuCKDWORD ".", this->m_Parser.m_CurrentPos);
			return false;
		}
	}

	CKStateChunk::LockedWriteBuffer_t CKStateChunk::LockWriteBufferWrapper(CKDWORD size_in_byte) {
		// same as LockReadBufferWrapper with slight difference.
		void* pData;
		bool ret = LockWriteBuffer(&pData, size_in_byte);
		if (ret) {
			return LockedWriteBuffer_t(pData, LockedWriteBufferDeleter(this, size_in_byte));
		} else {
			return LockedWriteBuffer_t();
		}
	}

	/* ========== Basic Data Write Functions ==========*/

	bool CKStateChunk::WriteByteData(const void* data_ptr, CKDWORD size_in_byte) {
		// same as ReadByteData with slight difference.
		if (data_ptr == nullptr) return false;

		void* pData;
		bool ret = LockWriteBuffer(&pData, size_in_byte);
		if (ret) {
			std::memcpy(pData, data_ptr, size_in_byte);
			UnLockWriteBuffer(size_in_byte);
			return true;
		} else {
			return false;
		}
	}

	bool CKStateChunk::WriteString(const XContainer::XString* strl) {
		if (strl == nullptr) return;

		// convert encoding
		XContainer::XString cache;
		m_BindContext->GetNativeString(*strl, cache);

		// get size
		CKDWORD strByteSize = static_cast<CKDWORD>(cache.size());
		if (!this->WriteStruct(strByteSize)) {
			return false;
		}

		// write data
		if (!this->WriteByteData(cache.c_str(), strByteSize)) {
			return false;
		}

		return true;
	}

	bool CKStateChunk::WriteObjectID(const CK_ID* id) {
		// MARK: if BindFile is not nullptr, no need to push this obj into obj list according to IDA code.
		// but we assume BindFile always it not nullptr, so I remove that pushing code.
		return this->WriteStruct(m_BindFile->GetIndexByObjectID(*id));
	}

	bool CKStateChunk::WriteObjectPointer(ObjImpls::CKObject* obj) {
		CK_ID objid;
		if (obj != nullptr) {
			objid = obj->GetID();
		}

		return WriteObjectID(objid);
	}

	bool CKStateChunk::WriteManagerInt(const CKGUID* guid, CKINT intval) {
		// push into manager list
		m_ManagerList.emplace_back(m_Parser.m_CurrentPos);
		// write data
		if (!this->WriteStruct(guid)) return false;
		if (!this->WriteStruct(intval)) return false;
		return true;
	}
	
	/* ========== Buffer Functions ==========*/

	bool CKStateChunk::WriteBuffer(const void* buf, CKDWORD size_in_byte) {
		if (buf != nullptr) {
			// write size
			if (!this->WriteStruct(size_in_byte)) return false;
			// write data
			auto locker = LockWriteBufferWrapper(size_in_byte);
			if (locker == nullptr) return false;
			std::memcpy(locker.get(), buf, size_in_byte);
			locker.reset();
		} else {
			// write blank data
			if (!this->WriteStruct(0)) return false;
		}

		return true;
	}

	bool CKStateChunk::WriteBufferNoSize(const void* buf, CKDWORD size_in_byte) {
		if (buf != nullptr) {
			// write data
			auto locker = LockWriteBufferWrapper(size_in_byte);
			if (locker == nullptr) return false;
			std::memcpy(locker.get(), buf, size_in_byte);
			locker.reset();
		}
		// if nosize buffer is nullptr, nothing need to write.
		return true;
	}

	/* ========== Sequence Functions ==========*/

	bool CKStateChunk::WriteObjectIDSequence(const XContainer::XObjectArray* ls) {
		return false;
	}

	bool CKStateChunk::WriteManagerIntSequence(const CKGUID* guid, const XContainer::XArray<CKINT>* ls) {
		return false;
	}

	bool CKStateChunk::WriteXObjectArray(const XContainer::XObjectArray* ls) {
		return false;
	}

	bool CKStateChunk::WriteXObjectPointerArray(const XContainer::XObjectPointerArray* ls) {
		return false;
	}

}
