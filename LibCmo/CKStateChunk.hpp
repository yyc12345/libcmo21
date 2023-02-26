#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"

namespace LibCmo {

	class CKStateChunk {
	public:
		CKStateChunk();
		CKStateChunk(const CKStateChunk&);
		CKStateChunk& operator=(const CKStateChunk&);
		~CKStateChunk();

		void Clear(void);

		bool ConvertFromBuffer(const void* buf);
		CKDWORD ConvertToBuffer(void* buf);

		//bool UnPack(CKDWORD DestSize);
		CKDWORD GetDataSize(void);

		bool SeekIdentifier(CKDWORD identifier);

		void StartRead(void);
		void ReadString(std::string& strl);

	private:
		enum class CKStateChunkStatus : int32_t {
			IDLE,
			READ,
			WRITE
		};

		CK_CLASSID m_ClassId;
		CKDWORD m_DataDwSize;
		CKDWORD* m_pData;

		CK_STATECHUNK_DATAVERSION m_DataVersion;
		CK_STATECHUNK_CHUNKVERSION m_ChunkVersion;

		struct {
			CKStateChunkStatus m_Status;
			CKDWORD m_CurrentPos;
			CKDWORD m_DataSize;
			CKDWORD m_PrevIdentifierPos;
		}m_Parser;

		std::vector<CKDWORD> m_ObjectList;
		std::vector<CKDWORD> m_ChunkList;
		std::vector<CKDWORD> m_ManagerList;

	private:
		void _EnsureWriteSpace(CKDWORD size);
		inline bool _EnsureReadSpace(CKDWORD required) {
			return (this->m_Parser.m_CurrentPos <= this->m_Parser.m_DataSize) && (required <= (this->m_Parser.m_DataSize - this->m_Parser.m_CurrentPos));
		}
	};

}
