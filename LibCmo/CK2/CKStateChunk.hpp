#pragma once

#include "../VTAll.hpp"
#include <memory>
#include <functional>

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

#pragma region Self Used Data Struct

	public:
		struct ProfileStateChunk_t {
			CK_CLASSID m_ClassId;
			CKDWORD m_DataDwSize;
			CKDWORD* m_pData;

			CK_STATECHUNK_DATAVERSION m_DataVersion;
			CK_STATECHUNK_CHUNKVERSION m_ChunkVersion;

			size_t m_ObjectListSize, m_ChunkListSize, m_ManagerListSize;

			CKFileVisitor* m_BindFile;
			CKContext* m_BindContext;
		};
		struct ProfileIdentifier_t {
			CKDWORD m_Identifier;
			void* m_DataPtr;
			CKDWORD m_AreaSize;
		};

		class LockedReadBufferDeleter {
		public:
			LockedReadBufferDeleter() : m_Host(nullptr), m_ConsumedSize(0) {}
			LockedReadBufferDeleter(CKStateChunk* host, CKDWORD init_size) :
				m_Host(host), m_ConsumedSize(init_size) {}
			LIBCMO_DEFAULT_COPY_MOVE(LockedReadBufferDeleter);

			void operator()(LIBCMO_UNUSED const void* buf);
			void SetConsumedSize(CKDWORD newsize);
		private:
			CKStateChunk* m_Host;
			CKDWORD m_ConsumedSize;
		};
		
		class LockedWriteBufferDeleter {
		public:
			LockedWriteBufferDeleter() : m_Host(nullptr), m_ConsumedSize(0) {}
			LockedWriteBufferDeleter(CKStateChunk* host, CKDWORD init_size) :
				m_Host(host), m_ConsumedSize(init_size) {}
			LIBCMO_DEFAULT_COPY_MOVE(LockedWriteBufferDeleter);

			void operator()(LIBCMO_UNUSED const void* buf);
			void SetConsumedSize(CKDWORD newsize);
		private:
			CKStateChunk* m_Host;
			CKDWORD m_ConsumedSize;
		};
		
		class BufferDeleter {
		public:
			BufferDeleter() : m_Host(nullptr), m_BufSize(0) {}
			BufferDeleter(CKStateChunk* host, CKDWORD bufsize) : 
				m_Host(host), m_BufSize(bufsize) {}
			LIBCMO_DEFAULT_COPY_MOVE(BufferDeleter);

			void operator()(const void* buf);
			CKDWORD GetBufferSize() const;
		private:
			CKStateChunk* m_Host;
			CKDWORD m_BufSize;
		};

		using LockedReadBuffer_t = std::unique_ptr<const void, LockedReadBufferDeleter>;
		using LockedWriteBuffer_t = std::unique_ptr<void, LockedWriteBufferDeleter>;
		using Buffer_t = std::unique_ptr<void, BufferDeleter>;

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

#pragma endregion

#pragma region Member Decl

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

#pragma endregion

#pragma region Buffer Related

	public:
		bool ConvertFromBuffer(const void* buf);
		CKDWORD ConvertToBuffer(void* buf);

#pragma endregion

#pragma region Misc Functions

	public:
		// ===== 2 Profiles Getter used by Unvirt =====

		/**
		 * @brief Get basic info of this CKStateChunk.
		 * @return The struct describing this CKStateChunk.
		*/
		const ProfileStateChunk_t GetStateChunkProfile();
		/**
		 * @brief Get all indentifier infos of this CKStateChunk, 
		 * including identifier self, data area size and address.
		 * @return A arrary, each item describe a single identifier's info.
		 * @remark The detail of implement can be seen in SeekIdentifierAndReturnSize()
		 * @see SeekIdentifierAndReturnSize
		*/
		const XContainer::XArray<ProfileIdentifier_t> GetIdentifiersProfile();

		/**
		 * @brief Clear this CKStateChunk, restore it to original status for other using.
		*/
		void Clear();
		/**
		 * @brief Get the size of data buffer in this CKStateChunk.
		 * @return The data buffer's size in CKBYTE.
		*/
		CKDWORD GetDataSize() const;
		/**
		 * @brief Get data version in this CKStateChunk.
		 * @remark Data version is frequently used by calling of CKStateChunk to distinguish different data layout due to compatibility reason.
		 * @return The data version.
		*/
		CK_STATECHUNK_DATAVERSION GetDataVersion() const;
		/**
		 * @brief Set data version of this CKStateChunk.
		 * @param version The set data version
		 * @remark Frequently used when saving someing in CKStateChunk.
		*/
		void SetDataVersion(CK_STATECHUNK_DATAVERSION version);
		/**
		 * @brief Get associated CK_CLASSID of this CKStateChunk
		 * @return Associated CK_CLASSID
		*/
		CK_CLASSID GetClassId() const;
		/**
		 * @brief Set associated CK_CLASSID of this CKStateChunk.
		 * @param cid The set CK_CLASSID
		 * @remark Frequently used at the end of CKObject::Save() override.
		*/
		void SetClassId(CK_CLASSID cid);


		bool Skip(CKDWORD DwordCount);

	private:
		CKDWORD GetCeilDwordSize(size_t char_size);
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

		/* ========== Read Buffer Controller ==========*/

	public:
		/**
		 * @brief Lock read buffer and make sure it has at least some bytes to read.
		 * @param ppData[out] The pointer to pointer receiving data address.
		 * @param size_in_byte[in] The number of bytes present in the read buffer needs to be ensured.
		 * @return Treu if success (can read)
		 * @remark
		 * + It actually not lock the buffer, just make sure that the read area is okey.
		 * + Do not forget calling UnLockReadBuffer after all read work done.
		 * @see UnLockReadBuffer, ReadBufferLocker
		*/
		bool LockReadBuffer(const void** ppData, CKDWORD size_in_byte);
		/**
		 * @brief Unlock read buffer and move forward with specified bytes.
		 * @param size_in_byte[in] The number of bytes you wish to move forward.
		 * This value can be different with the value passed to LockReadBuffer but should not greater than it.
		 * @return True if success (have enough space to move forward)
		 * @remark
		 * + It actually not unlock the buffer, just move forward reading pointer.
		 * + Used together with LockReadBuffer.
		 * @see LockReadBuffer
		*/
		bool UnLockReadBuffer(CKDWORD size_in_byte);
		/**
		 * @brief A RAII wrapper for LockReadBuffer and UnLockReadBuffer.
		 * @param size_in_byte[in] The value passed to LockReadBuffer.
		 * @return A read-only buffer with RAII feature (more like std::unique_ptr).
		 * @remark
		 * + The return value is more like std::unique_ptr but it have more features.
		 * + If GeneralBuffer_t::GetPtr return nullptr, it mean this function is failed.
		 * + If you only use the pointer-getter provided by the return value, the final moving forward byte count is the value passed in this function.
		 * + You also can use GeneralBuffer_t::SetSize to set the final moving forward byte count before the return value free itself.
		 * + The value passed to GeneralBuffer_t::SetSize will finally be passed to UnLockReadBuffer.
		 * + You can use GeneralBuffer_t::Reset to force free the return value.
		 * @example
		 * ```
		 * LockedReadBuffer_t buf = ReadBufferLocker(1919810);
		 * if (buf.GetPtr() == nullptr) { 
		 *     // failed
		 * } else {
		 *     stuff(buf); // do some operation...
		 *     buf.SetSize(114514); // i only consume these bytes.
		 *     buf.Reset(); // immediately free it.
		 * }
		 * ```
		 * @see LockReadBuffer, UnLockReadBuffer, LockedReadBuffer_t
		*/
		LockedReadBuffer_t LockReadBufferWrapper(CKDWORD size_in_byte);
		
		/* ========== Basic Data Read Functions ==========*/

	private:
		/**
		 * @brief A struct-read-friendly wrapper for LockReadBuffer and UnLockReadBuffer.
		 * All following struct reading function use this function as a underlaying calling.
		 * @param data_ptr[out] the pointer to data. must be allocated first.
		 * @param size_in_byte[in] the size of data in byte.
		 * @return True if success.
		*/
		bool ReadByteData(void* data_ptr, CKDWORD size_in_byte);
	public:
		/**
		 * @brief Read Struct
		 * @tparam T The reading type.
		 * @param data Data pointer for reading. Can not be bullptr
		 * @return True if reading success.
		 * @remark
		 * + This function is a reinterpter of a bunch of original Virtools SDK reading functions, including:
		 *   - Primitive type: ReadInt, ReadByte, ReadWord, ReadDword, ReadFloat, etc...
		 *   - Struct type: ReadGuid, ReadVector, ReadMatrix, etc...
		 * @see ReadStruct(T&)
		*/
		template<typename T>
		bool ReadStruct(T* data) {
			return ReadByteData(data, CKSizeof(T));
		}
		/**
		 * @brief Read Struct (Reference Type)
		 * @tparam T  The reading type.
		 * @param data Data reference for reading.
		 * @return True if reading success.
		 * @see ReadStruct(T*)
		*/
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

		/**
		 * @brief Read sub chunk
		 * @return Returned a new created of CKStateChunk if success, otherwise nullptr.
		 * Returned CKStateChunk should be manually released!
		*/
		CKStateChunk* ReadSubChunk();

		/* ========== Buffer Functions ==========*/

		/*
		Buffer related function implements:

		ReadBuffer(void**)						Read Byte based size.		-> ReadAndCopyBuffer(void**, CKDWORD*)
		ReadAndFillBuffer(int, void*)			User give Byte based size.	-> ReadBuffer(const void**, CKDWORD)
		ReadAndFillBuffer(void*)				Read Byte based size.		-> ReadBuffer(const void**, CKDWORD*)
		ReadAndFillBuffer_LEndian(int, void*)	User give Byte based size.	-> ReadBuffer(const void**, CKDWORD)
		ReadAndFillBuffer_LEndian(void*)		Read Byte based size.		-> ReadBuffer(const void**, CKDWORD*)
		ReadAndFillBuffer_LEndian16(int, void*)	User give Byte based size.	-> ReadBuffer(const void**, CKDWORD)
		ReadAndFillBuffer_LEndian16(void*)		Read Byte based size.		-> ReadBuffer(const void**, CKDWORD*)
		*/

		/**
		 * @brief Read buffer and copy it.
		 * The copied buffer and the size of buffer will be returned to caller.
		 * Caller should free the buffer by calling CKStateChunk::DeleteBuffer(void*).
		 * @param ppData[out] The pointer to pointer holding the new copied data.
		 * @param size_in_byte[out] Set to the size of buffer when success.
		 * @return True if success.
		 * @remark
		 * + Following original Virtools functions can use this function to implement:
		 *   - ReadBuffer(void**)
		*/
		bool ReadBuffer(void** ppData, CKDWORD* size_in_byte);
		/**
		 * @brief Free the buffer allocated by CKStateChunk reading functions.
		 * @param buf[in] The buffer need to be free.
		 * @see ReadBuffer
		*/
		void DeleteBuffer(const void* buf);
		/**
		 * @brief A wrapper for ReadAndCopyBuffer(void**, CKDWORD*)
		 * @return 
		*/
		Buffer_t ReadBufferWrapper();

		/**
		 * @brief Read buffer and fill user struct.
		 * The size of buffer will be read from CKStateChunk internally and return to caller.
		 * @param pData[out] The pointer holding the data.
		 * @return True if success.
		 * @remark
		 * + Following original Virtools functions can use this function to implement:
		 *   - ReadAndFillBuffer(void*)
		 *   - ReadAndFillBuffer_LEndian(void*)
		 *   - ReadAndFillBuffer_LEndian16(void*)
		*/
		bool ReadAndFillBuffer(void* pData);
		/**
		 * @brief Read buffer and fill user struct.
		 * The size of buffer is provided by user.
		 * @param pData[out] The pointer holding the data.
		 * @param size_in_byte[in] The size of data which you want to read in byte unit
		 * @return True if success.
		 * @remark
		 * + Following original Virtools functions can use this function to implement:
		 *   - ReadAndFillBuffer(int, void*)
		 *   - ReadAndFillBuffer_LEndian(int, void*)
		 *   - ReadAndFillBuffer_LEndian16(int, void*)
		*/
		bool ReadAndFillBuffer(void* pData, CKDWORD size_in_byte);

		///**
		// * @brief A RAII wrapper for ReadBuffer and DeleteBuffer
		// * @param uptr The pointer to unique_ptr receiving data.
		// * @param len_in_byte The size of gotten buffer.
		// * @return A buffer with RAII feature (more like std::unique_ptr).
		// * @remark
		// * + The return value is more like std::unique_ptr but it have more features.
		// * + If Buffer_t::GetPtr return nullptr, it mean this function is failed.
		// * + Use Buffer_t::GetSize to get the size of buffer.
		// * + You can use Buffer_t::Reset to force free the return value.
		// * @example
		// * ```
		// * Buffer_t buf = ReadBufferWrapper(1919810);
		// * if (buf.GetPtr() == nullptr) { 
		// *     // failed
		// * } else {
		// *     stuff(buf); // do some operation...
		// *     buf.SetSize(114514); // i only consume these bytes.
		// *     buf.Reset(); // immediately free it.
		// * }
		// * ```
		//*/
		//Buffer_t ReadBufferWrapper();

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

		bool LockWriteBuffer(const void** ppData, CKDWORD size_in_byte);
		bool UnLockWriteBuffer(CKDWORD size_in_byte);
		LockedWriteBuffer_t LockWriteBufferWrapper(CKDWORD size_in_byte);

#pragma endregion



	};

}
