#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"

namespace LibCmo {

	class CKStateChunk {
	public:
		CKStateChunk();
		CKStateChunk(CK_CLASSID clsid);
		CKStateChunk(const CKStateChunk&) = delete;
		CKStateChunk& operator=(const CKStateChunk&) = delete;
		~CKStateChunk();

		bool ConvertFromOldBuffer(const void* buf, CKDWORD buf_size, CKDWORD blk_size, CK_FILE_WRITEMODE mode);
		bool ConvertFromBuffer(const void* buf);
		CKDWORD ConvertToBuffer(void* buf);

		bool UnPack(CKDWORD DestSize);
		CKDWORD GetDataSize(void);

		bool SeekIdentifier(CKDWORD identifier);

		void ReadString(std::string& strl);

	private:
		CK_CLASSID m_ClassId;
		CKDWORD m_DataDwSize;
		CKDWORD* m_pData;

		CK_STATECHUNK_DATAVERSION m_DataVersion;
		CK_STATECHUNK_CHUNKVERSION m_ChunkVersion;

		struct {
			CKDWORD m_CurrentPos;
			CKDWORD m_DataSize;
			CKDWORD m_PrevIdentifierPos;
		}m_Parser;

		std::vector<CKDWORD> m_ObjectList;
		std::vector<CKDWORD> m_ChunkList;
		std::vector<CKDWORD> m_ManagerList;

	private:
		void _EnsureEnoughSpace(CKDWORD size);

	};

}
