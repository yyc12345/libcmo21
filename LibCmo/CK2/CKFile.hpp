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

		CKDWORD ProductVersion;	 ///< Virtools Version (Dev/Creation). (CK_VIRTOOLS_VERSION)
		CKDWORD ProductBuild; ///< Virtools Build Number.
		CK_FILE_WRITEMODE FileWriteMode; ///< Options used to save this file. (CK_FILE_WRITEMODE)
		CKDWORD FileVersion; ///< Version of file format when file was saved.
		CKDWORD CKVersion; ///< Version of CK when file was saved.
		CKDWORD FileSize; ///< Size of file in bytes.
		CKDWORD ObjectCount; ///< Number of objects stored in the file. 	
		CKDWORD ManagerCount; ///< Number of managers which saved data in the file.
		CKDWORD MaxIDSaved; ///< Maximum Object identifier saved
		CKDWORD Crc; ///< Crc of data
		CKDWORD Hdr1PackSize; ///< Reserved
		CKDWORD Hdr1UnPackSize; ///< Reserved
		CKDWORD DataPackSize; ///< Reserved
		CKDWORD DataUnPackSize; ///< Reserved
	};

	class CKFileObject {
	public:
		CKFileObject();
		CKFileObject(const CKFileObject&);
		CKFileObject(CKFileObject&&);
		CKFileObject& operator=(const CKFileObject&);
		CKFileObject& operator=(CKFileObject&&);
		~CKFileObject();

		CK_ID ObjectId; ///< ID of the object being load/saved (as it will be/was saved in the file)
		CK_ID CreatedObjectId; ///< ID of the object being created
		CK_CLASSID ObjectCid; ///< Class Identifier of the object
		CKObject* ObjPtr; ///< A pointer to the object itself (as CreatedObject when loading)
		TypeHelper::MKString Name; ///< Name of the Object
		CKStateChunk* Data; ///< A CKStateChunk that contains object information
		//CKINT PostPackSize; ///< When compressed chunk by chunk : size of Data after compression
		//CKINT PrePackSize; ///< When compressed chunk by chunk : size of Data before compression
		CK_FO_OPTIONS Options; ///< When loading an object it may be renamed , use to replace another object 	
		CKINT FileIndex; ///< Position of the object data inside uncompressed file buffer
		CKDWORD SaveFlags; ///< Flags used when this object was saved.
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
		XContainer::XBitArray ValidGuids;
	};

	/**
	@brief CKFile provides functions to save/load files in Virtools format.
	@remark
		+ Use CKFile just like a normal C++ class with passing CKCotext pointer.
		+ Once created a CKFile can be used to save one or more objects to disk.
		+ Only can load objects when this CKFile is empty. Use ClearData() to clear any existed CKFile.
		+ In any case, CKFile can run Save(). So you can create a CKFile, add some obejcts and save it.
		Or you can load a file by CKFile and add/modify something then save it.
	*/
	class CKFile {
	public:
		CKFile(CKContext* ctx);
		~CKFile();
		LIBCMO_DISABLE_COPY_MOVE(CKFile);

		// ========== Reset CKFile ==========
		void ClearData(void);

		// ========== Loading ==========
		CKERROR ShallowLoad(CKSTRING u8_filename);
		CKERROR DeepLoad(CKSTRING u8_filename);

		// ========== Loading Result ==========
		const XContainer::XArray<CKFileObject>& GetFileObjects();
		const XContainer::XArray<XContainer::XString>& GetIncludedFiles();

		// ========== Saving Preparing ==========
		CKBOOL AddSavedObject(CKObject* obj, CKDWORD flags = CK_STATESAVE_ALL);
		CKBOOL AddSavedObjects(CKObjectArray* objarray, CKDWORD flags = CK_STATESAVE_ALL);
		CKBOOL AddSavedFile(CKSTRING u8FileName);

		// ========== Saving ==========
		CKERROR Save(CKSTRING u8_filename);

		//CKERROR Load(CKSTRING u8_filename, /*CKObjectArray list, */ CK_LOAD_FLAGS flags);
		//CKERROR OpenFile(CKSTRING u8_filename, CK_LOAD_FLAGS flags);
		//CKERROR OpenMemory(void* MemoryBuffer, size_t BufferSize, CK_LOAD_FLAGS Flags);
		//CKERROR ReadFileHeaders(CKBufferParser** ParserPtr);
		//CKERROR ReadFileData(CKBufferParser** ParserPtr);
		//CKERROR LoadFileData(void/*CKObjectArray list*/);
		//CKERROR FinishLoading(/*CKObjectArray list, */CK_LOAD_FLAGS flags);

		CKINT m_SaveIDMax; // Maximum CK_ID found when saving or loading objects
		XContainer::XArray<CKFileObject> m_FileObjects; // List of objects being saved / loaded
		XContainer::XArray<CKFileManagerData> m_ManagersData; // Manager Data loaded
		XContainer::XArray<CKFilePluginDependencies> m_PluginsDep;	// Plugins dependencies for this file
		// XContainer::XClassArray<XContainer::XIntArray> m_IndexByClassId; // List of index in the m_FileObjects table sorted by ClassID
		XContainer::XArray<XContainer::XString> m_IncludedFiles; // List of files that should be inserted in the CMO file.
		CKFileInfo m_FileInfo; // Headers summary

		//XContainer::XFileObjectsTable m_ObjectsHashTable;
		//CKBOOL m_ReadFileDataDone;
		//CKBOOL m_SceneSaved;
		//XContainer::XIntArray m_DuplicateNameFounds;	// A List of file object index for which a existing object with the same name has been
		//													// found, this list is build if the load option contains CK_LOAD_AUTOMATICMODE or CK_LOAD_DODIALOG
		//XContainer::XBitArray m_AlreadySavedMask;			// BitArray of IDs already saved  {secret}
		//XContainer::XBitArray m_AlreadyReferencedMask;	// BitArray of IDs already referenced  {secret}
		//XContainer::XObjectPointerArray	 m_ReferencedObjects;

	private:
		// reader function and variables
		CKERROR ReadFileHeader(CKBufferParser* ParserPtr);
		CKERROR ReadFileData(CKBufferParser* ParserPtr);

		// writer function and varibales

		// shared function and variables
		bool mCanLoad;
		CKContext* m_Ctx;
	};

}
