#include "CKStateChunk.hpp"
#include "CKFile.hpp"
#include "CKContext.hpp"

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

	bool CKStateChunk::LockWriteBuffer(const void** ppData, CKDWORD size_in_byte) {
		return false;
	}

	bool CKStateChunk::UnLockWriteBuffer(CKDWORD size_in_byte) {
		return false;
	}

	CKStateChunk::LockedWriteBuffer_t CKStateChunk::LockWriteBufferWrapper(CKDWORD size_in_byte) {
		return LockedWriteBuffer_t();
	}

}
