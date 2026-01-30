#pragma once

#include "../VTInternal.hpp"
#include <yycc/macro/class_copy_move.hpp>
#include <memory>
#include <functional>
#include <type_traits>

namespace LibCmo::CK2 {

	/**
	 * @remark
	 * + We make sure m_BindContext and m_BindFile always are not nullptr. So some code of BindFile check and write different data struct has been removed.
	 * + Calling StartRead multiple times is illegal. 
	 *   - The solution is that use StartRead and StopRead to warp the real CKStateChunk consumer. And just calling read functions in real consumer directly.
	 *   - See CKFileReader for more infomation.
	 * + Same as StartRead, calling StartWrite multiple times also is illegal. We also remove CKStateChunk merge function, AddChunkAndDelete and AddChunk.
	 *   - The solution is same as StartRead solution. Use StartWrite and StopWrite warp the real CKStateChunk writer. Call write function in consumer directly.
	 *   - Every inherited CKObject::Save must call SetClassId at the end of function if they have data to write.
	 *   - See CKFileWrite for more infomation.
	*/
	class CKStateChunk {
	public:
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
			YYCC_DEFAULT_COPY_MOVE(LockedReadBufferDeleter)

			void operator()(const void* /*buf*/);
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
			YYCC_DEFAULT_COPY_MOVE(LockedWriteBufferDeleter)

			void operator()(const void* /*buf*/);
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
			YYCC_DEFAULT_COPY_MOVE(BufferDeleter)

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
			/**
			 * @brief The runtime size of CKStateChunk internal buf in DWORD unit.
			 * Usually be used and changed when resizing buffer in writing mode.
			*/
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
		/**
		 * @brief Convert byte based size to DWORD based size.
		 *
		 * Becase CKStateChunk use DWORD based buffer, so all data should be aligned to DWORD boundary.
		 * This function can convert byte based size to DWORD based size while keeping its size aligned with DWORD boundary.
		 * For example, caller want to allocate 3 bytes for data storing, this function will first align it to DWORD boundary, 4 bytes.
		 * Then convert it in DWORD size, 1 DWORD.
		 *
		 * @param char_size[in] The size in byte unit.
		 * @return The size in DWORD unit.
		*/
		CKDWORD GetCeilDwordSize(size_t char_size);
		bool ResizeBuffer(CKDWORD new_dwsize);
		/**
		 * @brief Check whether there are enough buffer to read.
		 *
		 * This function will check whether current CKStateChunk is in read mode and
		 * whether data area is enough to write.
		 * However, it is different with EnsureReadSpace. If no space to write, this function will
		 * try calling ResizeBuffer to get a enough buffer. Only when resize failed,
		 * this function will return false.
		 *
		 * @param dwsize[in] Required buffer size in DWORD unit.
		 * @return True if have enough space to write.
		 * @see EnsureReadSpace
		*/
		bool EnsureWriteSpace(CKDWORD dwsize);
		/**
		 * @brief Check whether there are enough buffer to read.
		 *
		 * This function will check whether current CKStateChunk is in read mode and
		 * whether data area is enough to read.
		 *
		 * @param dword_required[in] Required buffer size in DWORD unit.
		 * @return True if have enough space to read.
		 * @see EnsureWriteSpace
		*/
		bool EnsureReadSpace(CKDWORD dword_required);

		/**
		 * @brief The helper writer of m_ObjectList, m_ManagerList and m_ChunkList.
		 * 
		 * Add a position indicator into list which represent a single data.
		 * 
		 * @param entry_ls[in] The list to be written.
		 * @param pos[int] The written position data.
		 * @remark:
		 * + IntList::AddEntry() redirect to this.
		*/
		void AddEntry(XContainer::XArray<CKDWORD>& entry_ls, CKDWORD pos);
		/**
		 * @brief The helper writer of m_ObjectList, m_ManagerList and m_ChunkList.
		 * 
		 * Add a position indicator into list which represent a series of data.
		 * 
		 * @param entry_ls[in] The list to be written.
		 * @param pos[int] The written position data.
		 * @remark:
		 * + IntList::AddEntries() redirect to this.
		*/
		void AddEntries(XContainer::XArray<CKDWORD>& entry_ls, CKDWORD pos);

#pragma endregion

#pragma region Read Function

	public:
		void StartRead();
		void StopRead();

		/* ========== Identifier Functions ==========*/

	public:
		bool SeekIdentifierDword(CKDWORD identifier);
		bool SeekIdentifierDwordAndReturnSize(CKDWORD identifier, CKDWORD* out_size);
		template<typename TEnum>
			requires std::is_enum_v<TEnum>
		inline bool SeekIdentifier(TEnum enum_v) {
			return SeekIdentifierDword(static_cast<CKDWORD>(enum_v));
		}
		template<typename TEnum>
			requires std::is_enum_v<TEnum>
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
		 * @return LockedReadBuffer_t (actually is a std::unique_ptr with custom deleter)
		 * @remark
		 * + The return value is just a std::unique_ptr but its deleter have more features.
		 * + If return value contained data is nullptr, it mean that this function is failed.
		 * + If you only use the pointer provided by the return value, the final moving forward byte count is the value passed in this function.
		 * + You also can use LockedReadBuffer_t.get_deleter().SetConsumedSize() to set the final moving forward byte count before the return value free itself.
		 * + The value passed to LockedReadBuffer_t..get_deleter().SetConsumedSize() will finally be passed to UnLockReadBuffer.
		 * + You can use LockedReadBuffer_t.reset() to force free the return value.
		 * @remark Here is a example.
		 * ```
		 * auto buf = chunk->LockReadBufferWrapper(1919810);
		 * if (buf) {
		 *     stuff(buf.get()); // do some operation...
		 *     buf.get_deleter().SetConsumedSize(114514); // i only consume these bytes.
		 *     buf.reset(); // immediately free it.
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
		bool ReadObjectPointer(ObjImpls::CKObject** obj);
		inline bool ReadObjectID(CK_ID& id) {
			return ReadObjectID(&id);
		}
		inline bool ReadObjectPointer(ObjImpls::CKObject*& id) {
			return ReadObjectPointer(&id);
		}

		bool ReadManagerInt(CKGUID* guid, CKINT* intval);
		inline bool ReadManagerInt(CKGUID& guid, CKINT& intval) {
			return ReadManagerInt(&guid, &intval);
		}

		///**
		// * @brief Read sub chunk
		// * @return Returned a new created of CKStateChunk if success, otherwise nullptr.
		// * Returned CKStateChunk should be manually released!
		//*/
		//CKStateChunk* ReadSubChunk();

		/* ========== Buffer Functions ==========*/

		/*
		Buffer related function implements:

		ReadBuffer(void**)						Read Byte based size.		-> ReadBuffer(void**, CKDWORD*)
		ReadAndFillBuffer(int, void*)			User give Byte based size.	-> ReadAndFillBuffer(const void*, CKDWORD)
		ReadAndFillBuffer(void*)				Read Byte based size.		-> ReadAndFillBuffer(const void*)
		ReadAndFillBuffer_LEndian(int, void*)	User give Byte based size.	-> ReadAndFillBuffer(const void*, CKDWORD)
		ReadAndFillBuffer_LEndian(void*)		Read Byte based size.		-> ReadAndFillBuffer(const void*)
		ReadAndFillBuffer_LEndian16(int, void*)	User give Byte based size.	-> ReadAndFillBuffer(const void*, CKDWORD)
		ReadAndFillBuffer_LEndian16(void*)		Read Byte based size.		-> ReadAndFillBuffer(const void*)
		*/

		/**
		 * @brief Read buffer and copy it.
		 *
		 * The size of buffer will be read from CKStateChunk internally.
		 * It mean the read buffer must be written by WriteBuffer().
		 * 
		 * The copied buffer and the size of buffer will be returned to caller.
		 * Caller should free the buffer by calling CKStateChunk::DeleteBuffer(void*).
		 *
		 * @param ppData[out] The pointer to pointer holding the new copied data.
		 * @param size_in_byte[out] Set to the size of buffer when success.
		 * @return True if success.
		 * @remark Following original Virtools functions can use this function to implement:
		 * + ReadBuffer(void**)
		*/
		bool ReadBuffer(void** ppData, CKDWORD* size_in_byte);
		/**
		 * @brief Free the buffer allocated by CKStateChunk reading functions.
		 * @param buf[in] The buffer need to be free.
		 * @see ReadBuffer
		*/
		void DeleteBuffer(const void* buf);
		/**
		 * @brief A RAII wrapper for ReadAndCopyBuffer(void**, CKDWORD*)
		 * @return Buffer_t (actually is a std::unique_ptr with custom deleter)
		 * @remark
		 * + The return value is std::unique_ptr but its deleter have more features.
		 * + If return value containing value is nullptr, it mean this function is failed.
		 * + Use Buffer_t.get_deleter().GetBufferSize() to get the size of buffer.
		 * + You can use Buffer_t.reset() to force free the return value.
		 * @remark Here is a exmaple about how to use this function
		 * ```
		 * Buffer_t buf = chunk->ReadBufferWrapper(114);
		 * if (buf) {
		 *     stuff(buf.get(), buf.get_deleter().GetBufferSize()); // do some operation...
		 *     buf.reset(); // immediately free it.
		 * }
		 * ```
		*/
		Buffer_t ReadBufferWrapper();

		/**
		 * @brief Read buffer and fill user struct.
		 *
		 * The size of buffer will be read from CKStateChunk internally and return to caller.
		 * It mean the read buffer must be written by WriteBuffer().
		 *
		 * @param pData[out] The pointer holding the data.
		 * @return True if success.
		 * @remark Following original Virtools functions can use this function to implement:
		 * + ReadAndFillBuffer(void*)
		 * + ReadAndFillBuffer_LEndian(void*)
		 * + ReadAndFillBuffer_LEndian16(void*)
		*/
		bool ReadAndFillBuffer(void* pData);
		/**
		 * @brief Read buffer and fill user struct.
		 *
		 * The size of buffer is provided by user.
		 * It mean the read buffer must be written by WriteBufferNoSize().
		 *
		 * @param pData[out] The pointer holding the data.
		 * @param size_in_byte[in] The size of data which you want to read in byte unit
		 * @return True if success.
		 * @remark Following original Virtools functions can use this function to implement:
		 * + ReadAndFillBuffer(int, void*)
		 * + ReadAndFillBuffer_LEndian(int, void*)
		 * + ReadAndFillBuffer_LEndian16(int, void*)
		*/
		bool ReadAndFillBuffer(void* pData, CKDWORD size_in_byte);

		/* ========== Sequence Functions ==========*/

		/// <summary>
		/// Read Object ID Sequence
		/// <para>The combination using of StartReadSequence(), ReadObjectID(), and ReadObject() redirect to this.</para>
		/// </summary>
		/// <param name="ls"></param>
		/// <returns></returns>
		bool ReadObjectIDSequence(XContainer::XObjectArray* ls);
		inline bool ReadObjectIDSequence(XContainer::XObjectArray& ls) {
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

		///// <summary>
		///// Read Sub Chunk Sequence
		///// <para>The combination using of StartReadSequence() and ReadSubChunk() redirect to this.</para>
		///// <para>The item of returned CKStateChunk* list should be manually released!</para>
		///// </summary>
		///// <param name="ls"></param>
		///// <returns></returns>
		//bool ReadSubChunkSequence(XContainer::XArray<CKStateChunk*>* ls);
		//inline bool ReadSubChunkSequence(XContainer::XArray<CKStateChunk*>& ls) {
		//	return ReadSubChunkSequence(&ls);
		//}

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

#pragma endregion

#pragma region Write Function

	public:
		void StartWrite();
		/*
		* Actually this function mix various functions, including CloseChunk(), UpdateSize() and etc.
		*/
		void StopWrite();


		/* ========== Identifier Functions ==========*/

	public:
		bool WriteIdentifierDword(CKDWORD identifier);
		template<typename TEnum>
			requires std::is_enum_v<TEnum>
		inline bool WriteIdentifier(TEnum enum_v) {
			return WriteIdentifierDword(static_cast<CKDWORD>(enum_v));
		}

		/* ========== Write Buffer Controller ==========*/

	public:
		bool LockWriteBuffer(void** ppData, CKDWORD size_in_byte);
		bool UnLockWriteBuffer(CKDWORD size_in_byte);
		LockedWriteBuffer_t LockWriteBufferWrapper(CKDWORD size_in_byte);

		/* ========== Basic Data Write Functions ==========*/

	private:
		bool WriteByteData(const void* data_ptr, CKDWORD size_in_byte);

	public:
		template<typename T>
		bool WriteStruct(const T* data) {
			return WriteByteData(data, CKSizeof(T));
		}
		template<typename T>
		inline bool WriteStruct(const T& data) {
			return WriteByteData(&data, CKSizeof(T));
		}

		bool WriteString(const XContainer::XString* strl);
		inline bool WriteString(const XContainer::XString& strl) {
			return WriteString(&strl);
		}

		
		/* ========== Complex Data Read Functions ==========*/

	public:
		bool WriteObjectID(const CK_ID* id);
		bool WriteObjectPointer(ObjImpls::CKObject* obj);
		inline bool WriteObjectID(const CK_ID& id) {
			return WriteObjectID(&id);
		}

		bool WriteManagerInt(const CKGUID* guid, CKINT intval);
		inline bool WriteManagerInt(const CKGUID& guid, CKINT intval) {
			return WriteManagerInt(&guid, intval);
		}

		// Sub Chunk not support now.
		// Too complex and I even don't use it in my code.
		//CKStateChunk* ReadSubChunk();

		/* ========== Buffer Functions ==========*/

		/*
		Buffer related function implements:

		WriteBuffer(int, void*)						Write buffer with size.		-> WriteBuffer(const void*, CKDWORD)
		WriteBufferNoSize(int, void*)				Write buffer without size.	-> WriteBufferNoSize(const void*, CKDWORD)
		WriteBuffer_LEndian(int, void*)				Write buffer with size.		-> WriteBuffer(const void*, CKDWORD)
		WriteBuffer_LEndian16(int, void*)			Write buffer with size.		-> WriteBuffer(const void*, CKDWORD)
		WriteBufferNoSize_LEndian(int, void*)		Write buffer without size.	-> WriteBufferNoSize(const void*, CKDWORD)
		WriteBufferNoSize_LEndian16(int, void*)		Write buffer without size.	-> WriteBufferNoSize(const void*, CKDWORD)
		*/

		// MARK: buf can be nullptr, but buf size must is 0. and WriteBuffer will only write a zero to indicate there is a zero buffer.

		bool WriteBuffer(const void* buf, CKDWORD size_in_byte);
		bool WriteBufferNoSize(const void* buf, CKDWORD size_in_byte);

		/* ========== Sequence Functions ==========*/
		
	public:
		bool WriteObjectIDSequence(const XContainer::XObjectArray* ls);
		inline bool WriteObjectIDSequence(const XContainer::XObjectArray& ls) {
			return WriteObjectIDSequence(&ls);
		}

		bool WriteManagerIntSequence(const CKGUID* guid, const XContainer::XArray<CKINT>* ls);
		inline bool WriteManagerIntSequence(const CKGUID& guid, const XContainer::XArray<CKINT>& ls) {
			return WriteManagerIntSequence(&guid, &ls);
		}

		// Sub chunk is not available now
		// Because my code never use it and it is too complex.
		//bool ReadSubChunkSequence(XContainer::XArray<CKStateChunk*>* ls);
		//inline bool ReadSubChunkSequence(XContainer::XArray<CKStateChunk*>& ls) {
		//	return ReadSubChunkSequence(&ls);
		//}

		bool WriteXObjectArray(const XContainer::XObjectArray* ls);
		inline bool WriteXObjectArray(const XContainer::XObjectArray& ls) {
			return WriteXObjectArray(&ls);
		}

		bool WriteXObjectPointerArray(const XContainer::XObjectPointerArray* ls);
		inline bool WriteXObjectPointerArray(const XContainer::XObjectPointerArray& ls) {
			return WriteXObjectPointerArray(&ls);
		}


#pragma endregion

	};

}
