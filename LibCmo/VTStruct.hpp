#pragma once

#include "VTConstants.hpp"
#include "VTEncoding.hpp"
#include <vector>

// only expose boost in libcom self. do not let it be seen by any program using libcmo.
#if defined(LIBCMO_EXPORTING)
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#endif

namespace LibCmo {

	struct CKGUID {
		union {
			struct {
				CKDWORD d1, d2;
			};
			CKDWORD d[2];
		};
		CKGUID(CKDWORD gd1 = 0, CKDWORD gd2 = 0) { d[0] = gd1; d[1] = gd2; }
	};

	// define some mapped file to make boost is invisible for 
	// any program using this library
#if defined(LIBCMO_EXPORTING)
	using P_FILE_MAPPING = boost::interprocess::file_mapping*;
	using P_MAPPED_REGION = boost::interprocess::mapped_region*;
#else
	using P_FILE_MAPPING = void*;
	using P_MAPPED_REGION = void*;
#endif

	class VxMemoryMappedFile {
	public:
		VxMemoryMappedFile(const char* u8_filepath);
		~VxMemoryMappedFile(void);

		void* GetBase(void);
		size_t GetFileSize(void);
		bool IsValid(void);

	private:

#if defined(LIBCMO_OS_WIN32)
		std::wstring m_szFilePath;
#else
		std::string m_szFilePath;
#endif

		P_FILE_MAPPING m_hFile;
		P_MAPPED_REGION m_hFileMapping;
		bool m_bIsValid;
	};

	class CKBufferParser {
	private:
		char* m_ReaderBegin;
		size_t m_ReaderPos;
		bool m_NeedManualFree;
		size_t m_ReaderSize;

	public:
		CKBufferParser(void* ptr, size_t rsize, bool need_manual_free) :
			m_ReaderBegin(static_cast<char*>(ptr)),
			m_ReaderPos(0u), m_ReaderSize(rsize),
			m_NeedManualFree(need_manual_free) {
			;
		}
		~CKBufferParser() {
			if (this->m_NeedManualFree) free(this->m_ReaderBegin);
		}

		inline void* GetPtr(void) { return (this->m_ReaderBegin + m_ReaderPos); }
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
		CKDWORD FileSize;			// Size of file in bytes.
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
		CKDWORD m_PluginCategory;
		XArray<CKGUID> m_Guids;
		XBitArray ValidGuids;
	};

	class CKFile {
	public:
		CKFile();
		~CKFile();


		int32_t m_SaveIDMax;
		XArray<CKFileObject> m_FileObject;
		//XArray<CKFileManagerData> m_ManagersData;
		XClassArray<CKFilePluginDependencies> m_PluginDep;
		//XClassArray<XIntArray> m_IndexByClassId;
		//XClassArray<XString> m_IncludedFiles;

		CKFileInfo m_FileInfo;

		CK_LOAD_FLAGS m_Flags;
		std::string m_FileName;
		CKBufferParser* m_Parser;
		VxMemoryMappedFile* m_MappedFile;

		bool m_ReadFileDataDone;

	};

}
