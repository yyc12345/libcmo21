#include "VTUtils.hpp"
#if defined(LIBCMO_OS_WIN32)
#define ZLIB_WINAPI
#include "zconf.h"
#endif

#include "VTStruct.hpp"
#include <zlib.h>

namespace LibCmo {

	CKERROR CKFile::Load(CKSTRING u8_filename, /*CKObjectArray list, */ CK_LOAD_FLAGS flags) {
		CKERROR result = this->OpenFile(u8_filename, flags);
		if (result == CKERROR::CKERR_OK || result == CKERROR::CKERR_PLUGINSMISSING) {
			result = this->LoadFileData();
		}

		return result;
	}

	CKERROR CKFile::OpenFile(CKSTRING u8_filename, CK_LOAD_FLAGS flags) {
		this->ClearData();
		if (u8_filename == nullptr) return CKERROR::CKERR_INVALIDPARAMETER;

		this->m_FileName = u8_filename;
		this->m_MappedFile = new VxMemoryMappedFile(this->m_FileName.c_str());
		if (!this->m_MappedFile->IsValid()) return CKERROR::CKERR_INVALIDFILE;

		return this->OpenMemory(this->m_MappedFile->GetBase(), this->m_MappedFile->GetFileSize(), flags);
	}

	CKERROR CKFile::OpenMemory(void* MemoryBuffer, size_t BufferSize, CK_LOAD_FLAGS Flags) {
		if (MemoryBuffer == nullptr) return CKERROR::CKERR_INVALIDPARAMETER;
		// compare magic words
		if (BufferSize < 0x20 || memcmp(MemoryBuffer, "Nemo", 4u)) return CKERROR::CKERR_INVALIDFILE;

		this->m_Parser = new CKBufferParser(MemoryBuffer, BufferSize, false);

		this->m_Flags = Flags;
		this->m_IndexByClassId.resize(static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID));
		return this->ReadFileHeaders(&(this->m_Parser));
	}

	CKERROR CKFile::ReadFileHeaders(CKBufferParser** ParserPtr) {
		CKBufferParser* parser = *ParserPtr;
		this->m_IncludedFiles.clear();

		// ========== read header1 ==========
		CKDWORD fhdr1[8];
		CKDWORD fhdr2[8];
		if (parser->GetSize() < sizeof(fhdr1)) return CKERROR::CKERR_INVALIDFILE;
		memcpy(fhdr1, parser->GetPtr(), sizeof(fhdr1));
		parser->MoveCursor(sizeof(fhdr1));

		if (fhdr1[5]) {	// it seems that there is a ZERO checker?
			memset(fhdr1, 0, sizeof(fhdr1));
		}
		// check outdated
		if (fhdr1[4] > 9) return CKERROR::CKERR_OBSOLETEVIRTOOLS;

		// ========== read header2 ==========
		// file ver < 5 do not have second header
		if (fhdr1[4] < 5) {
			memset(fhdr2, 0, sizeof(fhdr2));
		} else {
			if (parser->GetSize() < sizeof(fhdr1) + sizeof(fhdr2)) return CKERROR::CKERR_INVALIDFILE;
			memcpy(fhdr2, parser->GetPtr(), sizeof(fhdr2));
			parser->MoveCursor(sizeof(fhdr2));
		}

		// forcely reset too big product ver
		if (fhdr2[5] >= 12) {
			fhdr2[5] = 0;
			fhdr2[6] = 0x1010000;
		}

		// ========== assign value ==========
		this->m_FileInfo.ProductVersion = fhdr2[5];
		this->m_FileInfo.ProductBuild = fhdr2[6];
		this->m_FileInfo.FileWriteMode = static_cast<CK_FILE_WRITEMODE>(fhdr1[6]);
		this->m_FileInfo.CKVersion = fhdr1[3];
		this->m_FileInfo.FileVersion = fhdr1[4];
		this->m_FileInfo.FileSize = parser->GetSize();
		this->m_FileInfo.ManagerCount = fhdr2[2];
		this->m_FileInfo.ObjectCount = fhdr2[3];
		this->m_FileInfo.MaxIDSaved = fhdr2[4];
		this->m_FileInfo.Hdr1PackSize = fhdr1[7];
		this->m_FileInfo.Hdr1UnPackSize = fhdr2[7];
		this->m_FileInfo.DataPackSize = fhdr2[0];
		this->m_FileInfo.DataUnPackSize = fhdr2[1];
		this->m_FileInfo.Crc = fhdr1[3];

		// ========== crc and body unpacker ==========
		if (this->m_FileInfo.FileVersion >= 8) {
			// crc checker for file ver >= 8 
			// reset crc field of header
			fhdr1[2] = 0;

			// compute crc
			uLong gotten_crc = adler32(0u, reinterpret_cast<const Bytef*>(&fhdr1), sizeof(fhdr1));
			parser->SetCursor(sizeof(fhdr1));
			gotten_crc = adler32(gotten_crc, reinterpret_cast<const Bytef*>(parser->GetPtr()), sizeof(fhdr2));
			parser->MoveCursor(sizeof(fhdr2));
			gotten_crc = adler32(gotten_crc, reinterpret_cast<const Bytef*>(parser->GetPtr()), this->m_FileInfo.Hdr1PackSize);
			parser->MoveCursor(this->m_FileInfo.Hdr1PackSize);
			gotten_crc = adler32(gotten_crc, reinterpret_cast<const Bytef*>(parser->GetPtr()), this->m_FileInfo.DataPackSize);
			parser->SetCursor(sizeof(fhdr1) + sizeof(fhdr2));

			if (gotten_crc != static_cast<uLong>(this->m_FileInfo.Crc)) return CKERROR::CKERR_FILECRCERROR;

			// compare size to decide wheher use compress feature
			void* decomp_buffer = CKUnPackData(this->m_FileInfo.Hdr1UnPackSize, parser->GetPtr(), this->m_FileInfo.Hdr1PackSize);
			if (decomp_buffer != nullptr) {
				parser = new CKBufferParser(decomp_buffer, this->m_FileInfo.Hdr1UnPackSize, true);
			}
		}

		// ========== object list read ==========
		// file ver >= 7 have this features
		if (this->m_FileInfo.FileVersion >= 7) {
			// apply max id saved
			this->m_SaveIDMax = this->m_FileInfo.MaxIDSaved;
			// resize
			this->m_FileObject.resize(this->m_FileInfo.ObjectCount);

			// read data
			for (auto it = this->m_FileObject.begin(); it != this->m_FileObject.end(); ++it) {
				CKFileObject* fileobj = &(*it);
				// setup useless fields
				fileobj->ObjPtr = nullptr;
				fileobj->Data = nullptr;

				// read basic fields
				memcpy(&(fileobj->Object), parser->GetPtr(), sizeof(CK_ID));
				parser->MoveCursor(sizeof(CK_ID));
				memcpy(&(fileobj->ObjectCid), parser->GetPtr(), sizeof(CK_CLASSID));
				parser->MoveCursor(sizeof(CK_CLASSID));
				memcpy(&(fileobj->FileIndex), parser->GetPtr(), sizeof(CKDWORD));
				parser->MoveCursor(sizeof(CKDWORD));

				CKDWORD namelen;
				memcpy(&namelen, parser->GetPtr(), sizeof(CKDWORD));
				parser->MoveCursor(sizeof(CKDWORD));
				if (namelen != 0) {
					fileobj->Name.resize(namelen);
					memcpy(fileobj->Name.data(), parser->GetPtr(), namelen);
					parser->MoveCursor(namelen);
				}
			}
		}

		return CKERROR::CKERR_OK;
	}

	CKERROR CKFile::ReadFileData(CKBufferParser** ParserPtr) {
		return CKERROR::CKERR_OK;
	}

	CKERROR CKFile::LoadFileData(void/*CKObjectArray list*/) {
		return CKERROR::CKERR_OK;
	}
}
