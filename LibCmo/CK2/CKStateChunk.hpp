#pragma once

#include "../VTAll.hpp"

namespace LibCmo::CK2 {

	class CKStateChunk {
	public:
		//CKStateChunk();
		CKStateChunk(CKFileVisitor* visitor, CKContext* ctx);
		CKStateChunk(const CKStateChunk&);
		CKStateChunk(CKStateChunk&&);
		CKStateChunk& operator=(const CKStateChunk&);
		CKStateChunk& operator=(CKStateChunk&&);
		~CKStateChunk();

	private:
		enum class CKStateChunkStatus : int32_t {
			IDLE,
			READ,
			WRITE
		};
		struct ChunkParser {
			CKStateChunkStatus m_Status;
			CKDWORD m_CurrentPos;
			CKDWORD m_DataSize;
			CKDWORD m_PrevIdentifierPos;
		};

		CK_CLASSID m_ClassId;
		CKDWORD m_DataDwSize;
		CKDWORD* m_pData;

		CK_STATECHUNK_DATAVERSION m_DataVersion;
		CK_STATECHUNK_CHUNKVERSION m_ChunkVersion;

		ChunkParser m_Parser;

		std::vector<CKDWORD> m_ObjectList;
		std::vector<CKDWORD> m_ChunkList;
		std::vector<CKDWORD> m_ManagerList;

		CKFileVisitor* m_BindFile;
		CKContext* m_BindContext;

#pragma region Buffer Related

	public:
		bool ConvertFromBuffer(const void* buf);
		CKDWORD ConvertToBuffer(void* buf);

#pragma endregion

#pragma region Misc Functions

	public:
		//bool UnPack(CKDWORD DestSize);
		void Clear(void);
		CKDWORD GetDataSize(void);
		CK_STATECHUNK_DATAVERSION GetDataVersion();
		void SetDataVersion(CK_STATECHUNK_DATAVERSION version);
		void DeleteBuffer(void* buf);
		bool Skip(CKDWORD DwordCount);

	private:
		size_t GetCeilDwordSize(size_t char_size);
		bool ResizeBuffer(CKDWORD new_dwsize);
		bool EnsureWriteSpace(CKDWORD dwsize);
		bool EnsureReadSpace(CKDWORD dword_required);

#pragma endregion


#pragma region Read Function

	public:
		void StartRead(void);
		void StopRead(void);

		/* ========== Identifier Functions ==========*/

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

		/* ========== Basic Data Read Functions ==========*/

	private:
		/// <summary>
		/// The base read function for all data.
		/// <para>This function will check all read requirements.</para>
		/// <para>If you have use this function or functions calling this function. You do not need check any reading requirements anymore</para>
		/// </summary>
		/// <param name="data_ptr">the pointer to data. must be allocated first.</param>
		/// <param name="size_in_byte">the size of data in byte.</param>
		/// <returns></returns>
		bool ReadByteData(void* data_ptr, CKDWORD size_in_byte);
	public:
		/// <summary>
		/// Read Struct
		/// <para>Primitive type: ReadInt, ReadByte, ReadWord, ReadDword, ReadFloat, etc...</para>
		/// <para>Struct type: ReadGuid, ReadVector, ReadMatrix, etc...</para>
		/// <para>Both of them are redirected to this.</para>
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="data"></param>
		/// <returns></returns>
		template<typename T>
		bool ReadStruct(T* data) {
			return ReadByteData(data, static_cast<CKDWORD>(sizeof(T)));
		}
		/// <summary>
		/// Read Struct
		/// <para>A wrapper for ReadStructPtr.</para>
		/// <para>Use reference, not pointer.</para>
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="data"></param>
		/// <returns></returns>
		template<typename T>
		inline bool ReadStruct(T& data) {
			return ReadByteData(&data, static_cast<CKDWORD>(sizeof(T)));
		}

		/// <summary>
		/// Read string
		/// </summary>
		/// <param name="strl"></param>
		/// <returns></returns>
		bool ReadString(std::string* strl);
		inline bool ReadString(std::string& strl) {
			return ReadString(&strl);
		}

		/* ========== Complex Data Read Functions ==========*/

		bool ReadObjectID(CK_ID* id);
		inline bool ReadObjectID(CK_ID& id) {
			return ReadObjectID(&id);
		}

		bool ReadManagerInt(CKGUID* guid, CKINT* intval);
		inline bool ReadManagerInt(CKGUID& guid, CKINT& intval) {
			return ReadManagerInt(&guid, &intval);
		}

		/// <summary>
		/// Read sub chunk
		/// <para>Return nullptr if failed.</para>
		/// <para>Returned CKStateChunk should be manually released!</para>
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		CKStateChunk* ReadSubChunk(void);

		/* ========== Buffer Functions ==========*/

		/*
		Buffer related function implements:

		ReadBuffer(void**)						Read Byte based size.		-> ReadBuffer
		ReadAndFillBuffer(int, void*)			User give Byte based size.	-> ReadNoSizeBuffer
		ReadAndFillBuffer(void*)				Read Byte based size.		-> ReadBuffer
		ReadAndFillBuffer_LEndian(int, void*)	User give Byte based size.	-> ReadNoSizeBuffer
		ReadAndFillBuffer_LEndian(void*)		Read Byte based size.		-> ReadBuffer
		ReadAndFillBuffer_LEndian16(int, void*)	User give Byte based size.	-> ReadNoSizeBuffer
		ReadAndFillBuffer_LEndian16(void*)		Read Byte based size.		-> ReadBuffer
		*/

		/// <summary>
		/// Read a buffer with unknow size (order user specific it).
		/// <para>ReadAndFillBuffer(int, void*), ReadAndFillBuffer_LEndian(int, void*), ReadAndFillBuffer_LEndian16(int, void*) are redirected to this.</para>
		/// <para>The buffer should be allocated by caller first.</para>
		/// </summary>
		/// <param name="size"></param>
		/// <param name="allocatedBuf"></param>
		/// <returns></returns>
		bool ReadNoSizeBuffer(CKDWORD size_in_byte, void* allocatedBuf);
		/// <summary>
		/// Read a buffer with knowen size stored in chunk.
		/// <para>ReadBuffer(void**), ReadAndFillBuffer(void*), ReadAndFillBuffer_LEndian(void*), ReadAndFillBuffer_LEndian16(void*) are redirected to this.</para>
		/// <para>The buffer will be allocated by function.</para>
		/// <para>Use CKStateChunk::DeleteBuffer() to delete it.</para>
		/// </summary>
		/// <param name="buf">a pointer to the pointer receiving data start address.</param>
		/// <param name="len">a pointer to the variable receiving the length of gotten buffer.</param>
		/// <returns></returns>
		bool ReadBuffer(void** buf, CKDWORD* len_in_byte);

		/* ========== Sequence Functions ==========*/

		/// <summary>
		/// Read Object ID Sequence
		/// <para>The combination using of StartReadSequence(), ReadObjectID(), and ReadObject() redirect to this.</para>
		/// </summary>
		/// <param name="ls"></param>
		/// <returns></returns>
		bool ReadObjectIDSequence(std::vector<CK_ID>* ls);
		inline bool ReadObjectIDSequence(std::vector<CK_ID>& ls) {
			return ReadObjectIDSequence(&ls);
		}

		/// <summary>
		/// Read Manager Sequence
		/// <para>The combination using of StartManagerReadSequence() and ReadManagerIntSequence() redirect to this.</para>
		/// </summary>
		/// <param name="guid"></param>
		/// <param name="ls"></param>
		/// <returns></returns>
		bool ReadManagerIntSequence(CKGUID* guid, std::vector<CKINT>* ls);
		inline bool ReadManagerIntSequence(CKGUID& guid, std::vector<CKINT>& ls) {
			return ReadManagerIntSequence(&guid, &ls);
		}

		/// <summary>
		/// Read Sub Chunk Sequence
		/// <para>The combination using of StartReadSequence() and ReadSubChunk() redirect to this.</para>
		/// <para>The item of returned CKStateChunk* list should be manually released!</para>
		/// </summary>
		/// <param name="ls"></param>
		/// <returns></returns>
		bool ReadSubChunkSequence(std::vector<CKStateChunk*>* ls);
		inline bool ReadSubChunkSequence(std::vector<CKStateChunk*>& ls) {
			return ReadSubChunkSequence(&ls);
		}

		/// <summary>
		/// Read Object Array (actually still is CK_ID)
		/// <para>ReadXObjectArray() and ReadObjectArray() redirect to this.</para>
		/// </summary>
		/// <param name="ls"></param>
		/// <returns></returns>
		bool ReadObjectArray(std::vector<CK_ID>* ls);
		inline bool ReadObjectArray(std::vector<CK_ID>& ls) {
			return ReadObjectArray(&ls);
		}

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
