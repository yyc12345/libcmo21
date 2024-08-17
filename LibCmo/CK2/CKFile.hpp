#pragma once

#include "../VTInternal.hpp"

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
		YYCC_DEL_CLS_COPY_MOVE(CKBufferParser);

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
		YYCC_DEF_CLS_COPY_MOVE(CKFileInfo);

		CKDWORD ProductVersion;	 /**< Virtools Version (Dev/Creation). (CK_VIRTOOLS_VERSION) */
		CKDWORD ProductBuild; /**< Virtools Build Number. */
		CK_FILE_WRITEMODE FileWriteMode; /**< Options used to save this file. (CK_FILE_WRITEMODE) */
		CKDWORD FileVersion; /**< Version of file format when file was saved. */
		CKDWORD CKVersion; /**< Version of CK when file was saved. */
		CKDWORD FileSize; /**< Size of file in bytes. */
		CKDWORD ObjectCount; /**< Number of objects stored in the file. */
		CKDWORD ManagerCount; /**< Number of managers which saved data in the file. */
		CK_ID MaxIDSaved; /**< Maximum Object identifier saved */
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
		YYCC_DEF_CLS_COPY_MOVE(CKFilePluginDependencies);

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

		const CKFileObject* GetFileObjectByIndex(CKDWORD index);
		CKDWORD GetIndexByObjectID(CK_ID objid);
		bool AddSavedFile(CKSTRING u8FileName);

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
		YYCC_DEL_CLS_COPY_MOVE(CKFileReader);

		// ========== Loading ==========
		CKERROR ShallowLoad(CKSTRING u8_filename);
		CKERROR DeepLoad(CKSTRING u8_filename);

		// ========== Loading Result ==========
		CK_ID GetSaveIdMax();
		const XContainer::XArray<CKFileObject>& GetFileObjects();
		const XContainer::XArray<CKFileManagerData>& GetManagersData();
		const XContainer::XArray<CKFilePluginDependencies>& GetPluginsDep();
		const XContainer::XArray<XContainer::XString>& GetIncludedFiles();
		const CKFileInfo GetFileInfo();

	protected:
		bool m_Done;
		CK_ID m_SaveIDMax; /**< Maximum CK_ID found when saving or loading objects */
		XContainer::XArray<CKFileObject> m_FileObjects; /**< List of objects being saved / loaded */
		XContainer::XArray<CKFileManagerData> m_ManagersData; /**< Manager Data loaded */
		XContainer::XArray<CKFilePluginDependencies> m_PluginsDep;	/**< Plugins dependencies for this file */
		// XContainer::XClassArray<XContainer::XIntArray> m_IndexByClassId; /**< List of index in the m_FileObjects table sorted by ClassID */
		/**
		 * @brief List of files that should be inserted in the CMO file.
		 * @remark Each item is just file name, not the full path to file.
		*/
		XContainer::XArray<XContainer::XString> m_IncludedFiles;
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
		CKFileWriter(CKContext* ctx, CKFileReader* reader, bool is_shallow);
		~CKFileWriter();
		YYCC_DEL_CLS_COPY_MOVE(CKFileWriter);

		// ========== Saving Preparing ==========
		bool AddSavedObject(ObjImpls::CKObject* obj, CKDWORD flags = CK_STATESAVE_ALL);
		bool AddSavedObjects(const XContainer::XObjectPointerArray& objarray, CKDWORD flags = CK_STATESAVE_ALL);
		bool AddSavedFile(CKSTRING u8FileName);

		// ========== Saving ==========
		CKERROR Save(CKSTRING u8_filename);

	protected:
		/**
		 * @brief A helper function to check whether given file can be written.
		 * @param filename[in] The name of file to be wriiten
		 * @return CKERROR::CK_OK if can write.
		*/
		CKERROR PrepareFile(CKSTRING filename);
		/**
		 * @brief Internal used Object Adder.
		 * 
		 * This function is used by AddSavedObject() and Copy from reader constructor.
		 * This function accept a object pointer, and try adding them.
		 * And set m_IncludedFiles and m_ObjectsHashTable properly.
		 * 
		 * @param obj The pointer to added object.
		 * @param flags The flag used when saving this object.
		 * @return True if success.
		*/
		bool InternalObjectAdder(ObjImpls::CKObject* obj, CKDWORD flags = CK_STATESAVE_ALL);

	protected:
		bool m_Done;	/**< True if this writer is already written into file. A written CKFileWriter can no be written again. */
		/**
		 * @brief True if this writer is not allowed to add objects.
		 * @remark
		 * + This field should be false in default.
		 * + This field usually be set when importing from reader.
		*/
		bool m_DisableAddingObject;
		/**
		 * @brief True if this writer is not allowed to add files.
		 * @remark
		 * + This field should be false in default.
		 * + This field usually be set when importing from reader.
		*/
		bool m_DisableAddingFile;
		
		CK_ID m_SaveIDMax; /**< Maximum CK_ID found when saving or loading objects */
		XContainer::XArray<CKFileObject> m_FileObjects; /**< List of objects being saved / loaded */
		XContainer::XArray<CKFileManagerData> m_ManagersData; /**< Manager Data loaded */
		XContainer::XArray<CKFilePluginDependencies> m_PluginsDep;	/**< Plugins dependencies for this file */
		/**
		 * @brief List of files that should be inserted in the CMO file.
		 * @remark Each item is the full path to file.
		*/
		XContainer::XArray<XContainer::XString> m_IncludedFiles;
		XContainer::XHashTable<CK_ID, CKDWORD> m_ObjectsHashTable; /**< A Object ID to save index hash table. */
		XContainer::XBitArray m_AlreadySavedMask; /**< Field recording saved object id. If this object is saved, set m_AlreadySavedMask[id] to true. Also used to check whether object already is in save list. */
		CKFileInfo m_FileInfo; /**< Headers summary */

		CKContext* m_Ctx;
		CKFileVisitor m_Visitor;
	};

}
