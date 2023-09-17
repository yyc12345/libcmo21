#pragma once

#include "../VTAll.hpp"
#include <memory>
#include <functional>

namespace LibCmo::CK2 {

	struct ChunkProfile {
		CK_CLASSID m_ClassId;
		CKDWORD m_DataDwSize;
		CKDWORD* m_pData;

		CK_STATECHUNK_DATAVERSION m_DataVersion;
		CK_STATECHUNK_CHUNKVERSION m_ChunkVersion;

		size_t m_ObjectListSize, m_ChunkListSize, m_ManagerListSize;

		CKFileVisitor* m_BindFile;
		CKContext* m_BindContext;
	};
	struct IdentifierProfile {
		CKDWORD m_Identifier;
		void* m_DataPtr;
		CKDWORD m_AreaSize;
	};

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
		enum class CKStateChunkStatus : CKDWORD {
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

		XContainer::XArray<CKDWORD> m_ObjectList;
		XContainer::XArray<CKDWORD> m_ChunkList;
		XContainer::XArray<CKDWORD> m_ManagerList;

		CKFileVisitor* m_BindFile;
		CKContext* m_BindContext;

#pragma region Buffer Related

	public:
		bool ConvertFromBuffer(const void* buf);
		CKDWORD ConvertToBuffer(void* buf);

#pragma endregion

#pragma region Misc Functions

	public:
		const ChunkProfile GetStateChunkProfile();

		//bool UnPack(CKDWORD DestSize);
		void Clear(void);
		CKDWORD GetDataSize(void);
		CK_STATECHUNK_DATAVERSION GetDataVersion();
		void SetDataVersion(CK_STATECHUNK_DATAVERSION version);
		bool Skip(CKDWORD DwordCount);

	private:
		CKDWORD GetCeilDwordSize(size_t char_size);
		void* GetCurrentPointer();
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
		const XContainer::XArray<IdentifierProfile> GetIdentifierProfile();

		/* ========== Basic Data Read Functions ==========*/

	private:
		/**
		 * @brief The base read function for all data.
		 * This function will check all read requirements.
		 * If you have use this function or functions calling this function. You do not need check any reading requirements anymore.
		 * @param data_ptr[out] the pointer to data. must be allocated first.
		 * @param size_in_byte[in] the size of data in byte.
		 * @return True if success.
		*/
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
			return ReadByteData(data, CKSizeof(T));
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
			return ReadByteData(&data, CKSizeof(T));
		}

		/// <summary>
		/// Read string. The content of string will automatically converted into UTF8 format.
		/// </summary>
		/// <param name="strl"></param>
		/// <returns></returns>
		bool ReadString(XContainer::XString* strl);
		inline bool ReadString(XContainer::XString& strl) {
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

		/**
		 * @brief The deleter for std::unique_ptr of CKStateChunk created buffer.
		*/
		struct BufferDeleter {
			BufferDeleter() = default;
			BufferDeleter(const BufferDeleter&) noexcept {}
			void operator()(void* buf);
		};
		/**
		 * @brief The type of CKStateChunk auto free buffer.
		*/
		using TBuffer = std::unique_ptr<void, BufferDeleter>;

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
		/**
		 * @brief A auto free wrapper for ReadBuffer
		 * @param uptr The pointer to unique_ptr receiving data.
		 * @param len_in_byte The size of gotten buffer.
		 * @return 
		*/
		bool ReadBufferWrapper(TBuffer* uptr, CKDWORD* len_in_byte);
		/**
		 * @brief Perform a dry buffer reading.
		 * This function will only make sure there is enough space for your reading.
		 * And return the start memory address to you.
		 * And will not create any extra memory like ReadBuffer.
		 * @param buf[out] a pointer to the pointer receiving data start address.
		 * @param ordered_sizepin] your expected length of this buffer.
		 * @return 
		*/
		bool ReadDryBuffer(const void** buf, CKDWORD ordered_size);
		
		/**
		 * @brief Free the buffer allocated by CKStateChunk reading functions.
		 * @param buf The buffer need to be free.
		*/
		void DeleteBuffer(const void* buf);

		/* ========== Sequence Functions ==========*/

		/// <summary>
		/// Read Object ID Sequence
		/// <para>The combination using of StartReadSequence(), ReadObjectID(), and ReadObject() redirect to this.</para>
		/// </summary>
		/// <param name="ls"></param>
		/// <returns></returns>
		bool ReadObjectIDSequence(XContainer::XArray<CK_ID>* ls);
		inline bool ReadObjectIDSequence(XContainer::XArray<CK_ID>& ls) {
			return ReadObjectIDSequence(&ls);
		}

		/// <summary>
		/// Read Manager Sequence
		/// <para>The combination using of StartManagerReadSequence() and ReadManagerIntSequence() redirect to this.</para>
		/// </summary>
		/// <param name="guid"></param>
		/// <param name="ls"></param>
		/// <returns></returns>
		bool ReadManagerIntSequence(CKGUID* guid, XContainer::XArray<CKINT>* ls);
		inline bool ReadManagerIntSequence(CKGUID& guid, XContainer::XArray<CKINT>& ls) {
			return ReadManagerIntSequence(&guid, &ls);
		}

		/// <summary>
		/// Read Sub Chunk Sequence
		/// <para>The combination using of StartReadSequence() and ReadSubChunk() redirect to this.</para>
		/// <para>The item of returned CKStateChunk* list should be manually released!</para>
		/// </summary>
		/// <param name="ls"></param>
		/// <returns></returns>
		bool ReadSubChunkSequence(XContainer::XArray<CKStateChunk*>* ls);
		inline bool ReadSubChunkSequence(XContainer::XArray<CKStateChunk*>& ls) {
			return ReadSubChunkSequence(&ls);
		}

		/**
		 * @brief Read Object Array (actually still is CK_ID)
		 * @remark ReadObjectArray() and XObjectArray::Load redirect to this.
		 * @param ls The list
		 * @return True if success.
		*/
		bool ReadXObjectArray(XContainer::XObjectArray* ls);
		inline bool ReadXObjectArray(XContainer::XObjectArray& ls) {
			return ReadXObjectArray(&ls);
		}

		/**
		 * @brief Read Object Array (actually is CKObject*)
		 * @remark ReadXObjectArray() and XObjectPointerArray::Load redirect to this.
		 * @param ls The list
		 * @return True if success
		*/
		bool ReadXObjectPointerArray(XContainer::XObjectPointerArray* ls);
		inline bool ReadXObjectPointerArray(XContainer::XObjectPointerArray& ls) {
			return ReadXObjectPointerArray(&ls);
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
