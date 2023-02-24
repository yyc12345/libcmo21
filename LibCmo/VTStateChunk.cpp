#include "VTUtils.hpp"
#if defined(LIBCMO_OS_WIN32)
#define ZLIB_WINAPI
#include <zconf.h>
#endif

#include <zlib.h>
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
	LibCmo::CKStateChunk::CKStateChunk(CK_CLASSID clsid) :
		m_ClassId(clsid), m_DataDwSize(0u), m_pData(nullptr),
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

	bool CKStateChunk::ConvertFromOldBuffer(const void* buf, CKDWORD buf_size, CKDWORD blk_size, CK_FILE_WRITEMODE mode) {
		// if not use compress or 2 size is equal, it mean that this buffer is raw.
		// assign it directly
		if (!EnumHelper::FlagEnumHas(mode, CK_FILE_WRITEMODE::CKFILE_CHUNKCOMPRESSED_OLD) ||
			buf_size == blk_size) {

			this->m_DataDwSize = buf_size / sizeof(CKDWORD);
			this->m_pData = new(std::nothrow) CKDWORD[this->m_DataDwSize];
			if (this->m_pData != nullptr) {
				memcpy(this->m_pData, buf, this->m_DataDwSize * sizeof(CKDWORD));
			}

			return true;

		} else {
			// otherwise, we need UnPack
			// prepare UnPack requirement
			this->m_DataDwSize = buf_size;	// NOTE: store as BYTE length!!!
			this->m_pData = reinterpret_cast<CKDWORD*>(new(std::nothrow) char[buf_size]);
			if (this->m_pData == nullptr) return false;
			memcpy(this->m_pData, buf, buf_size);

			// call UnPack
			return UnPack(blk_size);
		}
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
			bufpos = 2u;

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

	bool CKStateChunk::UnPack(CKDWORD DestSize) {
		// NOTE: in UnPack. pData store the compressed buffer, and 
		// dwSize store the length of compressed buffer as CHAR size, not DWORD size!

		// create a enough buffer
		char* buffer = new(std::nothrow) char[DestSize];
		if (buffer == nullptr) return false;
		uLongf destSize = DestSize;
		// uncompress it
		auto err = uncompress(
			reinterpret_cast<Bytef*>(buffer), &destSize,
			reinterpret_cast<const Bytef*>(this->m_pData), static_cast<uLong>(this->m_DataDwSize)
		);
		// if no error, assign data
		if (err == Z_OK) {
			// get dw size and copy it to remove useless blank data
			this->m_DataDwSize = static_cast<CKDWORD>(destSize) / sizeof(CKDWORD);

			delete[] this->m_pData;
			this->m_pData = nullptr;
			this->m_pData = new(std::nothrow) CKDWORD[this->m_DataDwSize];
			if (this->m_pData != nullptr) {
				memcpy(this->m_pData, buffer, this->m_DataDwSize * sizeof(CKDWORD));
			}
		}
		delete[] buffer;
		return true;
	}

	CKDWORD CKStateChunk::GetDataSize(void) {
		return sizeof(CKDWORD) * this->m_DataDwSize;
	}

	bool CKStateChunk::SeekIdentifier(CKDWORD identifier) {
		return false;
	}

	void CKStateChunk::ReadString(std::string& strl) {
		;
	}

	void LibCmo::CKStateChunk::_EnsureEnoughSpace(CKDWORD size) {
		;
	}

}
