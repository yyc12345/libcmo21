#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"

#include "VxMemoryMappedFile.hpp"
#include "CKStateChunk.hpp"
#include "CKMinContext.hpp"

namespace LibCmo {

	class CKBufferParser {
	private:
		char* m_MemBegin;
		size_t m_MemPos;
		bool m_NeedManualFree;
		size_t m_MemSize;

	public:
		CKBufferParser(void* ptr, size_t rsize, bool need_manual_free);
		CKBufferParser(const CKBufferParser&) = delete;
		CKBufferParser& operator=(const CKBufferParser&) = delete;
		~CKBufferParser();

		inline const void* GetPtr(void) { return (this->m_MemBegin + m_MemPos); }
		inline void Read(void* data, size_t data_size) {
			memcpy(data, (this->m_MemBegin + m_MemPos), data_size);
			this->m_MemPos += data_size;
		}
		inline void Write(const void* data, size_t data_size) {
			memcpy((this->m_MemBegin + m_MemPos), data, data_size);
			this->m_MemPos += data_size;
		}
		inline void* GetBase(void) { return this->m_MemBegin; }
		inline size_t GetSize(void) { return this->m_MemSize; }
		inline size_t GetCursor(void) { return this->m_MemPos; }
		inline void MoveCursor(size_t off) { this->m_MemPos += off; }
		inline void SetCursor(size_t off) { this->m_MemPos = off; }
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
		CKFileInfo();
		CKFileInfo(const CKFileInfo&) = delete;
		CKFileInfo& operator=(const CKFileInfo&) = delete;
		~CKFileInfo();

		CKDWORD ProductVersion;		// Virtools Version (Dev/Creation). (CK_VIRTOOLS_VERSION)
		CKDWORD ProductBuild;		// Virtools Build Number.
		CK_FILE_WRITEMODE FileWriteMode;		// Options used to save this file. (CK_FILE_WRITEMODE)
		CKDWORD FileVersion;		// Version of file format when file was saved.
		CKDWORD CKVersion;			// Version of CK when file was saved.
		size_t FileSize;			// Size of file in bytes.
		CKDWORD ObjectCount;		// Number of objects stored in the file. 	
		CKDWORD ManagerCount;		// Number of managers which saved data in the file.
		CKDWORD MaxIDSaved;			// Maximum Object identifier saved
		CKDWORD Crc;				// Crc of data
		CKDWORD Hdr1PackSize;		// Reserved
		CKDWORD Hdr1UnPackSize;		// Reserved
		CKDWORD DataPackSize;		// Reserved
		CKDWORD DataUnPackSize;		// Reserved
	private:

	};

	class CKFileObject {
	public:
		CKFileObject();
		//CKFileObject(const CKFileObject&) = delete;
		//CKFileObject& operator=(const CKFileObject&) = delete;
		~CKFileObject();

		CK_ID ObjectId;
		CK_CLASSID ObjectCid;
		std::string Name;
		CKStateChunk* Data;
		CKDWORD FileIndex;
	private:

	};

	class CKFileManagerData {
	public:
		CKFileManagerData();
		//CKFileManagerData(const CKFileManagerData&) = delete;
		//CKFileManagerData& operator=(const CKFileManagerData&) = delete;
		~CKFileManagerData();

		CKStateChunk* Data;
		CKGUID Manager;
	private:

	};

	class CKFilePluginDependencies {
	public:
		CKFilePluginDependencies();
		//CKFilePluginDependencies(const CKFilePluginDependencies&) = delete;
		//CKFilePluginDependencies& operator=(const CKFilePluginDependencies&) = delete;
		~CKFilePluginDependencies();

		CK_PLUGIN_TYPE m_PluginCategory;
		XArray<CKGUID> m_Guids;
	private:

	};

	namespace CKFileData {
		class ShallowDocument {
		public:
			ShallowDocument();
			ShallowDocument(const ShallowDocument&) = delete;
			ShallowDocument& operator=(const ShallowDocument&) = delete;
			~ShallowDocument();

			int32_t m_SaveIDMax;
			CKFileInfo m_FileInfo;

			XArray<CKFileObject> m_FileObjects;
			XArray<CKFileManagerData> m_FileManagersData;
			XClassArray<CKFilePluginDependencies> m_PluginDep;
			XClassArray<XIntArray> m_IndexByClassId;
			XClassArray<XString> m_IncludedFiles;

		private:

		};

		class DeepDocument {
		public:
			DeepDocument();
			DeepDocument(const DeepDocument&) = delete;
			DeepDocument& operator=(const DeepDocument&) = delete;
			~DeepDocument();

			int32_t m_SaveIDMax;
			CKFileInfo m_FileInfo;

			XArray<ObjsImpl::CKObject*> m_Objects;
			XClassArray<XIntArray> m_IndexByClassId;
			XClassArray<XString> m_IncludedFiles;
		private:

		};

		class HybridDocument {
		public:
			HybridDocument();
			HybridDocument(const HybridDocument&) = delete;
			HybridDocument& operator=(const HybridDocument&) = delete;
			~HybridDocument();

		private:

		};

	}

	class CKFile {
	public:
		CKFile(CKMinContext* ctx);
		CKFile(const CKFile&) = delete;
		CKFile& operator=(const CKFile&) = delete;
		~CKFile();

		void ClearData(void);

		CKERROR ShallowLoad(CKSTRING u8_filename, CKFileData::ShallowDocument** out_doc);
		CKERROR DeepLoad(CKSTRING u8_filename, CKFileData::DeepDocument** out_doc);

		CKERROR ShallowSave(CKSTRING u8_filename, CKFileData::ShallowDocument* in_doc);
		CKERROR DeepSave(CKSTRING u8_filename, CKFileData::DeepDocument* out_doc);
		CKERROR HybridSave(CKSTRING u8_filename, CKFileData::HybridDocument* out_doc);

		CKERROR DestroyDocument(CKFileData::ShallowDocument* in_doc);
		CKERROR DestroyDocument(CKFileData::DeepDocument* in_doc);
		CKERROR DestroyDocument(CKFileData::HybridDocument* in_doc);

		//CKERROR Load(CKSTRING u8_filename, /*CKObjectArray list, */ CK_LOAD_FLAGS flags);
		//CKERROR OpenFile(CKSTRING u8_filename, CK_LOAD_FLAGS flags);
		//CKERROR OpenMemory(void* MemoryBuffer, size_t BufferSize, CK_LOAD_FLAGS Flags);
		//CKERROR ReadFileHeaders(CKBufferParser** ParserPtr);
		//CKERROR ReadFileData(CKBufferParser** ParserPtr);
		//CKERROR LoadFileData(void/*CKObjectArray list*/);
		//CKERROR FinishLoading(/*CKObjectArray list, */CK_LOAD_FLAGS flags);

		//int32_t m_SaveIDMax;
		//XArray<CKFileObject> m_FileObject;
		//XArray<CKFileManagerData> m_ManagersData;
		//XClassArray<CKFilePluginDependencies> m_PluginDep;
		//XClassArray<XIntArray> m_IndexByClassId;
		//XClassArray<XString> m_IncludedFiles;

		//CKFileInfo m_FileInfo;

		//CK_LOAD_FLAGS m_Flags;


		//bool m_ReadFileDataDone;

	private:
		// reader function and variables
		CKERROR ReadFileHeader(CKBufferParser* ParserPtr, CKFileData::ShallowDocument* doc);
		CKERROR ReadFileData(CKBufferParser* ParserPtr, CKFileData::ShallowDocument* doc);

		// writer function and varibales

		// shared function and variables
		std::string m_FileName;
		CKMinContext* m_MinCtx;
	};

}
