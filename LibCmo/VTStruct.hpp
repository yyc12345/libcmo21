#pragma once

#include "VTUtils.hpp"
#include "VTConstants.hpp"
#include "VTEncoding.hpp"
#include <vector>

#if defined(LIBCMO_OS_WIN32)
#include <Windows.h>
#else
#include <sys/mman.h>
#endif

namespace LibCmo {

	void* CKUnPackData(CKINT DestSize, const void* SrcBuffer, CKINT SrcSize);

	struct CKGUID {
		union {
			struct {
				CKDWORD d1, d2;
			};
			CKDWORD d[2];
		};
		CKGUID(CKDWORD gd1 = 0, CKDWORD gd2 = 0) { d[0] = gd1; d[1] = gd2; }
	};

	class VxMemoryMappedFile {
	private:

#if defined(LIBCMO_OS_WIN32)
		std::wstring m_szFilePath;
		HANDLE m_hFile;
		DWORD m_dwFileSizeLow, m_dwFileSizeHigh;
		HANDLE m_hFileMapping;
		LPVOID m_hFileMapView;
#else
		std::string m_szFilePath;
#error NO IMPLEMENTATION FOR LINUX MMAP!
#endif

		void* m_pMemoryMappedFileBase;
		size_t m_cbFile;
		bool m_bIsValid;
	public:
		VxMemoryMappedFile(const char* u8_filepath);
		VxMemoryMappedFile(const VxMemoryMappedFile&) = delete;
		VxMemoryMappedFile& operator=(const VxMemoryMappedFile&) = delete;
		~VxMemoryMappedFile(void);

		inline void* GetBase(void) { return this->m_pMemoryMappedFileBase; }
		inline size_t GetFileSize(void) { return this->m_cbFile; }
		inline bool IsValid(void) { return this->m_bIsValid; }
	};

	class CKBufferParser {
	private:
		char* m_ReaderBegin;
		size_t m_ReaderPos;
		bool m_NeedManualFree;
		size_t m_ReaderSize;

	public:
		CKBufferParser(void* ptr, size_t rsize, bool need_manual_free);
		CKBufferParser(const CKBufferParser&) = delete;
		CKBufferParser& operator=(const CKBufferParser&) = delete;
		~CKBufferParser();

		inline const void* GetPtr(void) { return (this->m_ReaderBegin + m_ReaderPos); }
		inline void ReadAndMove(void* data, size_t data_size) {
			memcpy(data, (this->m_ReaderBegin + m_ReaderPos), data_size);
			this->m_ReaderPos += data_size;
		}
		inline size_t GetSize(void) { return this->m_ReaderSize; }
		inline void MoveCursor(size_t off) { this->m_ReaderPos += off; }
		inline void SetCursor(size_t off) { this->m_ReaderPos = off; }
	};

	struct CKFileInfo {
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
	};

	struct CKFileObject {
		CK_ID Object;
		CK_ID CreatedObject;
		CK_CLASSID ObjectCid;
		void* ObjPtr;
		std::string Name;
		void* Data;
		CKDWORD PostPackSize;
		CKDWORD PrePackSize;
		CK_FO_OPTIONS Options;
		CKDWORD FileIndex;
		CK_FILE_WRITEMODE SaveFlags;
	};

	struct CKFilePluginDependencies {
		CK_PLUGIN_TYPE m_PluginCategory;
		XArray<CKGUID> m_Guids;
		XBitArray ValidGuids;
	};

	class CKFile {
	public:
		CKFile(const Utils::VirtoolsEnvironment& cfg);
		CKFile(const CKFile&) = delete;
		CKFile& operator=(const CKFile&) = delete;
		~CKFile();

		void ClearData(void);
		CKERROR Load(CKSTRING u8_filename, /*CKObjectArray list, */ CK_LOAD_FLAGS flags);
		CKERROR OpenFile(CKSTRING u8_filename, CK_LOAD_FLAGS flags);
		CKERROR OpenMemory(void* MemoryBuffer, size_t BufferSize, CK_LOAD_FLAGS Flags);
		CKERROR ReadFileHeaders(CKBufferParser** ParserPtr);
		CKERROR ReadFileData(CKBufferParser** ParserPtr);
		CKERROR LoadFileData(void/*CKObjectArray list*/);

		int32_t m_SaveIDMax;
		XArray<CKFileObject> m_FileObject;
		//XArray<CKFileManagerData> m_ManagersData;
		XClassArray<CKFilePluginDependencies> m_PluginDep;
		XClassArray<XIntArray> m_IndexByClassId;
		XClassArray<XString> m_IncludedFiles;

		CKFileInfo m_FileInfo;

		CK_LOAD_FLAGS m_Flags;
		std::string m_FileName;
		CKBufferParser* m_Parser;
		VxMemoryMappedFile* m_MappedFile;

		bool m_ReadFileDataDone;

	private:
		Utils::VirtoolsEnvironment m_UserCfg;
	};

}
