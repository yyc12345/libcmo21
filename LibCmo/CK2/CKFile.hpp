#pragma once

#include "../VTAll.hpp"

namespace LibCmo::XContainer {
	using XIntArray = XArray<CKINT>;
	using XFileObjectsTable = XHashTable<CK2::CK_ID, CKINT>;
}

namespace LibCmo::CK2 {

	class CKBufferParser {
	private:
		CKBYTE* m_MemBegin;
		CKDWORD m_MemPos;
		bool m_NeedManualFree;
		CKDWORD m_MemSize;

	public:
		/**
		 * @brief Create CKBufferParser from a existed buffer.
		 * @param ptr The start pointer of buffer. This buffer should be allocated by 'new[]', not 'new' or 'malloc()'.
		 * @param rsize The size of buffer.
		 * @param need_manual_free True if provided buffer need freed by CKBufferParser automatically.
		*/
		CKBufferParser(const void* ptr, CKDWORD rsize, bool need_manual_free) :
			m_MemBegin(const_cast<CKBYTE*>(static_cast<const CKBYTE*>(ptr))),
			m_MemPos(0u), m_MemSize(rsize),
			m_NeedManualFree(need_manual_free) 
		{}
		/**
		 * @brief Create CKBufferParser from a new created buffer.
		 * @param newsize The size of new buffer.
		*/
		CKBufferParser(CKDWORD newsize) :
			m_MemBegin(new CKBYTE[newsize]),
			m_MemPos(0u), m_MemSize(newsize),
			m_NeedManualFree(true)
		{}
		~CKBufferParser() {
			if (this->m_NeedManualFree) delete[](this->m_MemBegin);
		}
		LIBCMO_DISABLE_COPY_MOVE(CKBufferParser);

		const void* GetPtr(CKINT extraoff = 0) { return (this->m_MemBegin + m_MemPos + extraoff); }
		void* GetMutablePtr(CKINT extraoff = 0) { return (this->m_MemBegin + m_MemPos + extraoff); }
		void* GetBase(void) { return this->m_MemBegin; }
		CKDWORD GetSize(void) { return this->m_MemSize; }
		CKDWORD GetCursor(void) { return this->m_MemPos; }
		void MoveCursor(CKINT off) { this->m_MemPos += off; }
		void SetCursor(CKDWORD off) { this->m_MemPos = off; }
		void Read(void* data, CKDWORD data_size) {
			std::memcpy(data, (this->m_MemBegin + m_MemPos), data_size);
			this->m_MemPos += data_size;
		}
		template<class _Ty>
		void Read(_Ty* data) {
			Read(data, CKSizeof(_Ty));
		}
		void Write(const void* data, CKDWORD data_size) {
			std::memcpy((this->m_MemBegin + m_MemPos), data, data_size);
			this->m_MemPos += data_size;
		}
		template<class _Ty>
		void Write(const _Ty* data) {
			Write(data, CKSizeof(_Ty));
		}
	};

#pragma pack(push)
#pragma pack(1)
	struct CKRawFileInfo {
		CKBYTE NeMo[8];
		CKDWORD Crc;
		CKDWORD CKVersion;
		CKDWORD FileVersion;
		CKDWORD Zero;
		CKDWORD FileWriteMode;
		CKDWORD Hdr1PackSize;

		CKDWORD DataPackSize;
		CKDWORD DataUnPackSize;
		CKDWORD ManagerCount;
		CKDWORD ObjectCount;
		CKDWORD MaxIDSaved;
		CKDWORD ProductVersion;
		CKDWORD ProductBuild;
		CKDWORD Hdr1UnPackSize;
	};
#pragma pack(pop)

	class CKFileInfo {
	public:
		CKFileInfo() :
			ProductVersion(0u), ProductBuild(0x01010000u), FileWriteMode(CK_FILE_WRITEMODE::CKFILE_UNCOMPRESSED),
			FileVersion(8u), CKVersion(CKVERSION), FileSize(0u),
			ObjectCount(0u), ManagerCount(0u), MaxIDSaved(0u), Crc(0u),
			Hdr1PackSize(0u), Hdr1UnPackSize(0u), DataPackSize(0u), DataUnPackSize(0u) {}
		~CKFileInfo() {}
		LIBCMO_DEFAULT_COPY_MOVE(CKFileInfo);

		CKDWORD ProductVersion;	 /**< Virtools Version (Dev/Creation). (CK_VIRTOOLS_VERSION) */
		CKDWORD ProductBuild; /**< Virtools Build Number. */
		CK_FILE_WRITEMODE FileWriteMode; /**< Options used to save this file. (CK_FILE_WRITEMODE) */
		CKDWORD FileVersion; /**< Version of file format when file was saved. */
		CKDWORD CKVersion; /**< Version of CK when file was saved. */
		CKDWORD FileSize; /**< Size of file in bytes. */
		CKDWORD ObjectCount; /**< Number of objects stored in the file. */
		CKDWORD ManagerCount; /**< Number of managers which saved data in the file. */
		CKDWORD MaxIDSaved; /**< Maximum Object identifier saved */
		CKDWORD Crc; /**< Crc of data */
		CKDWORD Hdr1PackSize; /**< The compressed size of Header section. */
		CKDWORD Hdr1UnPackSize; /**< The uncompressed size of Header section. */
		CKDWORD DataPackSize; /**< The compressed size of Data section. */
		CKDWORD DataUnPackSize; /**< The uncompressed size of Data section. */
	};

	class CKFileObject {
	public:
		CKFileObject();
		CKFileObject(const CKFileObject&);
		CKFileObject(CKFileObject&&);
		CKFileObject& operator=(const CKFileObject&);
		CKFileObject& operator=(CKFileObject&&);
		~CKFileObject();

		CK_ID ObjectId; /**< ID of the object being load/saved (as it will be/was saved in the file) */
		CK_ID CreatedObjectId; /**< ID of the object being created */
		CK_CLASSID ObjectCid; /**< Class Identifier of the object */
		ObjImpls::CKObject* ObjPtr; /**< A pointer to the object itself (as CreatedObject when loading) */
		XContainer::XString Name; /**< Name of the Object */
		CKStateChunk* Data; /**< A CKStateChunk that contains object information */
		CKDWORD PackSize;  /**< The CKStateChunk data size */
		//CKINT PostPackSize; /**< When compressed chunk by chunk : size of Data after compression */
		//CKINT PrePackSize; /**< When compressed chunk by chunk : size of Data before compression */
		CK_FO_OPTIONS Options; /**< When loading an object it may be renamed , use to replace another object */
		CKDWORD FileIndex; /**< Position of the object data inside uncompressed file buffer */
		CKDWORD SaveFlags; /**< Flags used when this object was saved. */
	};

	class CKFileManagerData {
	public:
		CKFileManagerData();
		CKFileManagerData(const CKFileManagerData&);
		CKFileManagerData(CKFileManagerData&&);
		CKFileManagerData& operator=(const CKFileManagerData&);
		CKFileManagerData& operator=(CKFileManagerData&&);
		~CKFileManagerData();

		CKStateChunk* Data;
		CKGUID Manager;
	};

	class CKFilePluginDependencies {
	public:
		CKFilePluginDependencies() :
			m_PluginCategory(CK_PLUGIN_TYPE::CKPLUGIN_MANAGER_DLL), m_Guids() {}
		~CKFilePluginDependencies() {}
		LIBCMO_DEFAULT_COPY_MOVE(CKFilePluginDependencies);

		CK_PLUGIN_TYPE m_PluginCategory;
		XContainer::XArray<CKGUID> m_Guids;
		//XContainer::XBitArray ValidGuids;
	};

	class CKFileVisitor {
	public:
		CKFileVisitor(CKFileReader* reader);
		CKFileVisitor(CKFileWriter* writer);
		CKFileVisitor(const CKFileVisitor&);
		CKFileVisitor(CKFileVisitor&&);
		CKFileVisitor& operator=(const CKFileVisitor&);
		CKFileVisitor& operator=(CKFileVisitor&&);

		const CKFileObject* GetFileObjectByIndex(size_t index);
	protected:
		bool m_IsReader;
		CKFileReader* m_Reader;
		CKFileWriter* m_Writer;
		CKContext* m_Ctx;
	};

	class CKFileReader {
		friend class CKFileVisitor;
	public:
		CKFileReader(CKContext* ctx);
		~CKFileReader();
		LIBCMO_DISABLE_COPY_MOVE(CKFileReader);

		// ========== Loading ==========
		CKERROR ShallowLoad(CKSTRING u8_filename);
		CKERROR DeepLoad(CKSTRING u8_filename);

		// ========== Loading Result ==========
		CKINT GetSaveIdMax();
		const XContainer::XArray<CKFileObject>& GetFileObjects();
		const XContainer::XArray<CKFileManagerData>& GetManagersData();
		const XContainer::XArray<CKFilePluginDependencies>& GetPluginsDep();
		const XContainer::XArray<XContainer::XString>& GetIncludedFiles();
		const CKFileInfo GetFileInfo();

	protected:
		bool m_Done;
		CKINT m_SaveIDMax; /**< Maximum CK_ID found when saving or loading objects */
		XContainer::XArray<CKFileObject> m_FileObjects; /**< List of objects being saved / loaded */
		XContainer::XArray<CKFileManagerData> m_ManagersData; /**< Manager Data loaded */
		XContainer::XArray<CKFilePluginDependencies> m_PluginsDep;	/**< Plugins dependencies for this file */
		// XContainer::XClassArray<XContainer::XIntArray> m_IndexByClassId; /**< List of index in the m_FileObjects table sorted by ClassID */
		XContainer::XArray<XContainer::XString> m_IncludedFiles; /**< List of files that should be inserted in the CMO file. */
		CKFileInfo m_FileInfo; /**< Headers summary */

		CKERROR ReadFileHeader(CKBufferParser* ParserPtr);
		CKERROR ReadFileData(CKBufferParser* ParserPtr);

		CKContext* m_Ctx;
		CKFileVisitor m_Visitor;
	};

	class CKFileWriter {
		friend class CKFileVisitor;
	public:
		CKFileWriter(CKContext* ctx);
		CKFileWriter(CKContext* ctx, CKFileReader* reader);
		~CKFileWriter();
		LIBCMO_DISABLE_COPY_MOVE(CKFileWriter);

		// ========== Saving Preparing ==========
		bool AddSavedObject(ObjImpls::CKObject* obj, CKDWORD flags = CK_STATESAVE_ALL);
		bool AddSavedObjects(CKObjectArray* objarray, CKDWORD flags = CK_STATESAVE_ALL);
		bool AddSavedFile(CKSTRING u8FileName);

		// ========== Saving ==========
		CKERROR Save(CKSTRING u8_filename);

	protected:
		bool m_Done;
		/**
		 * True if this writer is copy from reader.
		 * The data copied from reader mean that calling just only do some small modification.
		 * So we don't need try getting some managers or save file options from CKContext.
		 * Just apply the data coming from reader.
		 * Also, Add object functions is not allowed when writer copying from reader. 
		*/
		bool m_IsCopyFromReader;
		
		CKINT m_SaveIDMax; /**< Maximum CK_ID found when saving or loading objects */
		XContainer::XArray<CKFileObject> m_FileObjects; /**< List of objects being saved / loaded */
		XContainer::XArray<CKFileManagerData> m_ManagersData; /**< Manager Data loaded */
		XContainer::XArray<CKFilePluginDependencies> m_PluginsDep;	/**< Plugins dependencies for this file */
		XContainer::XArray<XContainer::XString> m_IncludedFiles; /**< List of files that should be inserted in the CMO file. */
		CKFileInfo m_FileInfo; /**< Headers summary */

		CKERROR PrepareFile(CKSTRING filename);

		CKContext* m_Ctx;
		CKFileVisitor m_Visitor;
	};

}
