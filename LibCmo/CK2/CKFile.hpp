#pragma once

#include "../VTAll.hpp"

namespace LibCmo::XContainer {
	using XIntArray = XArray<CK2::CKINT>;
	using XFileObjectsTable = XHashTable<CK2::CK_ID, CK2::CKINT>;
}

namespace LibCmo::CK2 {

	class CKBufferParser {
	private:
		char* m_MemBegin;
		size_t m_MemPos;
		bool m_NeedManualFree;
		size_t m_MemSize;

	public:
		CKBufferParser(void* ptr, size_t rsize, bool need_manual_free) :
			m_MemBegin(static_cast<char*>(ptr)),
			m_MemPos(0u), m_MemSize(rsize),
			m_NeedManualFree(need_manual_free) {
			;
		}
		~CKBufferParser() {
			if (this->m_NeedManualFree) delete[](this->m_MemBegin);
		}
		LIBCMO_DISABLE_COPY_MOVE(CKBufferParser);

		const void* GetPtr(void) { return (this->m_MemBegin + m_MemPos); }
		void Read(void* data, size_t data_size) {
			std::memcpy(data, (this->m_MemBegin + m_MemPos), data_size);
			this->m_MemPos += data_size;
		}
		void Write(const void* data, size_t data_size) {
			std::memcpy((this->m_MemBegin + m_MemPos), data, data_size);
			this->m_MemPos += data_size;
		}
		void* GetBase(void) { return this->m_MemBegin; }
		size_t GetSize(void) { return this->m_MemSize; }
		size_t GetCursor(void) { return this->m_MemPos; }
		void MoveCursor(size_t off) { this->m_MemPos += off; }
		void SetCursor(size_t off) { this->m_MemPos = off; }
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
		CKObject* ObjPtr; /**< A pointer to the object itself (as CreatedObject when loading) */
		TypeHelper::MKString Name; /**< Name of the Object */
		CKStateChunk* Data; /**< A CKStateChunk that contains object information */
		//CKINT PostPackSize; /**< When compressed chunk by chunk : size of Data after compression */
		//CKINT PrePackSize; /**< When compressed chunk by chunk : size of Data before compression */
		CK_FO_OPTIONS Options; /**< When loading an object it may be renamed , use to replace another object */
		CKINT FileIndex; /**< Position of the object data inside uncompressed file buffer */
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
		CKBOOL m_IsReader;
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
		const XContainer::XArray<CKFileObject>& GetFileObjects();
		const XContainer::XArray<XContainer::XString>& GetIncludedFiles();

	protected:
		CKBOOL m_Done;
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
		CKBOOL AddSavedObject(CKObject* obj, CKDWORD flags = CK_STATESAVE_ALL);
		CKBOOL AddSavedObjects(CKObjectArray* objarray, CKDWORD flags = CK_STATESAVE_ALL);
		CKBOOL AddSavedFile(CKSTRING u8FileName);

		// ========== Saving ==========
		CKERROR Save(CKSTRING u8_filename);

	protected:
		CKContext* m_Ctx;
		CKFileVisitor m_Visitor;
	};

}
