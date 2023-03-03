#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"
#include <type_traits>
#include <cinttypes>

namespace LibCmo::CK2 {

	class CKStateChunk {
	public:
		CKStateChunk();
		CKStateChunk(const CKStateChunk&);
		CKStateChunk& operator=(const CKStateChunk&);
		~CKStateChunk();

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
		inline size_t GetCeilDwordSize(size_t char_size) {
			return (char_size + 3) >> 3;
		}
		bool ResizeBuffer(CKDWORD new_dwsize);
		bool EnsureWriteSpace(CKDWORD dwsize);
		bool EnsureReadSpace(CKDWORD dword_required);

	public:
		bool ConvertFromBuffer(const void* buf);
		CKDWORD ConvertToBuffer(void* buf);

		//bool UnPack(CKDWORD DestSize);
		void Clear(void);
		CKDWORD GetDataSize(void);
		CK_STATECHUNK_DATAVERSION GetDataVersion();
		void SetDataVersion(CK_STATECHUNK_DATAVERSION version);
		bool Skip(CKDWORD DwordCount);

#pragma region Read Function

	public:
		void StartRead(void);
		bool SeekIdentifierDword(CKDWORD identifier);
		bool SeekIdentifierDwordAndReturnSize(CKDWORD identifier, CKDWORD* out_size);
		template<typename TEnum>
		inline bool SeekIdentifier(TEnum enum_v) {
			return SeekIdentifierDword(static_cast<CKDWORD>(enum_v));
		}
		template<typename TEnum>
		inline bool SeekIdentifierAndReturnSize(TEnum enum_v, CKDWORD* out_size) {
			return SeekIdentifierDwordAndReturnSize(static_cast<CKDWORD>(enum_v), out_size);
		}

		/*
		* Read Struct
		Primitive type: ReadInt, ReadByte, ReadWord, ReadDword, ReadFloat, etc...
		Struct type: ReadGuid, ReadVector, ReadMatrix, etc...
		Both of them are redirected to this.
		*/
		template<typename T>
		CKERROR ReadStructPtr(T* data) {
			size_t size = GetCeilDwordSize(sizeof(T));
			if (EnsureReadSpace(static_cast<CKDWORD>(size))) {
				std::memcpy(data, this->m_pData + this->m_Parser.m_CurrentPos, size);

			} else return CKERROR::CKERR_OUTOFMEMORY;
			return CKERROR::CKERR_OK;
		}
		/*
		* Read Struct
		A wrapper for ReadStructPtr.
		Use reference, not pointer.
		*/
		template<typename T>
		inline CKERROR ReadStructRef(T& data) {
			return ReadStructPtr(&data);
		}
		///*
		//* Read Enum Data
		//A wrapper for ReadStructPtr.
		//All Enum read redirect to this.
		//*/
		//template<typename T>
		//inline CKERROR ReadEnum(T& data) {
		//	return ReadStructPtr(reinterpret_cast<std::underlying_type_t<T>*>(&data));
		//}

		CKERROR ReadString(std::string& strl);

		CKERROR ReadBuffer(void* allocatedBuf);
		CKERROR ReadNoSizeBuffer(CKDWORD size, void* allocatedBuf);

		//int		ReadInt();
		//int 		StartReadSequence();
		//CK_ID	ReadObjectID();
		//CKStateChunk* ReadSubChunk();
		//int 		StartManagerReadSequence(CKGUID* guid);
		//CKGUID	ReadGuid();
		//void ReadAndFillBuffer_LEndian(void* buffer);
		//void ReadAndFillBuffer_LEndian16(void* buffer);
		//float	ReadFloat();
		//CKWORD	ReadWord();
		//CKDWORD	ReadDword();
		//CKDWORD	ReadDwordAsWords();
		//void		ReadVector(VxMath::VxVector* v);
		//void		ReadMatrix(VxMath::VxMatrix& mat);
		//CKObjectImplements::CKObject* ReadObject(CKMinContext*);
		//void ReadAndFillBuffer(void* buffer);
		//CKBYTE* ReadRawBitmap(VxMath::VxImageDescEx& desc);
		//XObjectArray ReadXObjectArray(void);
		void StopRead(void);

#pragma endregion

#pragma region Write Function

	public:
		void StartWrite();
		//void WriteIdentifier(CKDWORD id);
		//void AddChunkAndDelete(CKStateChunk*);
		//void StartObjectIDSequence(int count);
		//void WriteObjectSequence(CKObjectImplements::CKObject* obj);
		//void WriteInt(int data);
		//void WriteFloat(float data);
		//void WriteDword(CKDWORD data);
		//void WriteDwordAsWords(CKDWORD data);
		//void WriteVector(const VxMath::VxVector* v);
		//void WriteMatrix(const VxMath::VxMatrix& mat);
		//void WriteObject(CKObjectImplements::CKObject* obj);
		//void WriteBuffer_LEndian(int size, void* buf);
		//void WriteBuffer_LEndian16(int size, void* buf);
		//void WriteBufferNoSize_LEndian(int size, void* buf);
		///*void UpdateDataSize();*/
		//void* LockWriteBuffer(int DwordCount);

		/*
		* Old Name: CloseChunk();
		*/
		void StopWrite(void);

#pragma endregion



	};

}
