#include "CKFile.hpp"
#include "CKStateChunk.hpp"
#include "ObjImpls/CKObject.hpp"
#include "../VxMath/VxMemoryMappedFile.hpp"
#include "CKContext.hpp"
#include <memory>

namespace LibCmo::CK2 {

	/*
	* NOTE:
	* We onlt support read Virtools file with FileVersion >= 7.
	* The file with FileVersion < 7 is older than NeMo 1.0 (CK 1.1).
	* No need to support them.
	*/

	CKERROR CKFileReader::ShallowLoad(CKSTRING u8_filename) {
		// check document status
		if (this->m_Done) CKERROR::CKERR_CANCELLED;

		// check file and open memory
		if (u8_filename == nullptr) return CKERROR::CKERR_INVALIDPARAMETER;
		std::unique_ptr<VxMath::VxMemoryMappedFile> mappedFile(new VxMath::VxMemoryMappedFile(u8_filename));
		if (!mappedFile->IsValid()) {
			this->m_Ctx->OutputToConsoleEx("Fail to create Memory File for \"%s\".", u8_filename);
			return CKERROR::CKERR_INVALIDFILE;
		}

		// create buffer and start loading
		std::unique_ptr<CKBufferParser> parser(new CKBufferParser(mappedFile->GetBase(), mappedFile->GetFileSize(), false));
		CKERROR err = this->ReadFileHeader(parser.get());
		if (err != CKERROR::CKERR_OK) return err;
		err = this->ReadFileData(parser.get());
		if (err != CKERROR::CKERR_OK) return err;

		// other data will be free automatically
		return CKERROR::CKERR_OK;
	}

	CKERROR CKFileReader::ReadFileHeader(CKBufferParser* ParserPtr) {
		std::unique_ptr<CKBufferParser> parser(new CKBufferParser(ParserPtr->GetBase(), ParserPtr->GetSize(), false));
		parser->SetCursor(ParserPtr->GetCursor());

		std::string name_conv, name_dest;

		// ========== read header ==========
		// check header size
		if (parser->GetSize() < CKSizeof(CKRawFileInfo)) return CKERROR::CKERR_INVALIDFILE;
		if (std::memcmp(parser->GetPtr(), CKNEMOFI, sizeof(CKRawFileInfo::NeMo))) return CKERROR::CKERR_INVALIDFILE;
		// read header
		CKRawFileInfo rawHeader;
		parser->Read(&rawHeader, sizeof(CKRawFileInfo));

		// ========== header checker ==========
		// check zero flag?
		if (rawHeader.Zero) return CKERROR::CKERR_INVALIDFILE;
		// check file version
		if (rawHeader.FileVersion > 9 || rawHeader.FileVersion < 7) return CKERROR::CKERR_OBSOLETEVIRTOOLS;
		// force reset too big product ver?
		if (rawHeader.ProductVersion >= 12u) {
			rawHeader.ProductVersion = 0u;
			rawHeader.ProductBuild = 0x01010000u;
		}

		// ========== assign value ==========
		this->m_FileInfo.ProductVersion = rawHeader.ProductVersion;
		this->m_FileInfo.ProductBuild = rawHeader.ProductBuild;
		this->m_FileInfo.FileWriteMode = static_cast<CK_FILE_WRITEMODE>(rawHeader.FileWriteMode);
		this->m_FileInfo.CKVersion = rawHeader.CKVersion;
		this->m_FileInfo.FileVersion = rawHeader.FileVersion;
		this->m_FileInfo.FileSize = parser->GetSize();
		this->m_FileInfo.ManagerCount = rawHeader.ManagerCount;
		this->m_FileInfo.ObjectCount = rawHeader.ObjectCount;
		this->m_FileInfo.MaxIDSaved = rawHeader.MaxIDSaved;
		this->m_FileInfo.Hdr1PackSize = rawHeader.FileVersion >= 8 ? rawHeader.Hdr1PackSize : 0u;
		this->m_FileInfo.Hdr1UnPackSize = rawHeader.FileVersion >= 8 ? rawHeader.Hdr1UnPackSize : 0u;
		this->m_FileInfo.DataPackSize = rawHeader.DataPackSize;
		this->m_FileInfo.DataUnPackSize = rawHeader.DataUnPackSize;
		this->m_FileInfo.Crc = rawHeader.Crc;

		// ========== crc and body unpacker ==========
		if (this->m_FileInfo.FileVersion >= 8) {
			// crc checker for file ver >= 8 
			// reset crc field of header
			rawHeader.Crc = 0u;

			// compute crc
			CKDWORD gotten_crc = CKComputeDataCRC(&rawHeader, CKSizeof(CKRawFileInfo), 0u);
			parser->SetCursor(sizeof(CKRawFileInfo));
			gotten_crc = CKComputeDataCRC(parser->GetPtr(), this->m_FileInfo.Hdr1PackSize, gotten_crc);
			parser->MoveCursor(this->m_FileInfo.Hdr1PackSize);
			gotten_crc = CKComputeDataCRC(parser->GetPtr(), this->m_FileInfo.DataPackSize, gotten_crc);

			if (gotten_crc != this->m_FileInfo.Crc) {
				this->m_Ctx->OutputToConsole("Virtools file CRC error.");
				return CKERROR::CKERR_FILECRCERROR;
			}

			// reset cursor
			parser->SetCursor(sizeof(CKRawFileInfo));

			// compare size to decide wheher use compress feature
			if (this->m_FileInfo.Hdr1PackSize != this->m_FileInfo.Hdr1UnPackSize) {
				void* decomp_buffer = CKUnPackData(this->m_FileInfo.Hdr1UnPackSize, parser->GetPtr(), this->m_FileInfo.Hdr1PackSize);
				if (decomp_buffer != nullptr) {
					parser = std::unique_ptr<CKBufferParser>(new CKBufferParser(decomp_buffer, this->m_FileInfo.Hdr1UnPackSize, true));
				}
			}
		}

		// ========== object list read ==========
		// file ver >= 7 have this features
		{
			// apply max id saved
			this->m_SaveIDMax = this->m_FileInfo.MaxIDSaved;
			// resize
			this->m_FileObjects.resize(this->m_FileInfo.ObjectCount);

			// read data
			for (auto& fileobj : this->m_FileObjects) {
				// read basic fields
				parser->Read(&(fileobj.ObjectId), sizeof(CK_ID));
				parser->Read(&(fileobj.ObjectCid), sizeof(CK_CLASSID));
				parser->Read(&(fileobj.FileIndex), sizeof(CKDWORD));

				CKDWORD namelen;
				parser->Read(&namelen, sizeof(CKDWORD));
				if (namelen != 0) {
					name_conv.resize(namelen);
					parser->Read(name_conv.data(), namelen);
					m_Ctx->GetUtf8String(name_conv, name_dest);
					fileobj.Name = name_dest;
				}
			}
		}

		// ========== dep list read ==========
		// file ver >= 8 have this feature
		if (this->m_FileInfo.FileVersion >= 8) {
			// get size and resize
			CKDWORD depSize;
			parser->Read(&depSize, sizeof(CKDWORD));
			this->m_PluginsDep.resize(depSize);

			CKDWORD guid_size;
			for (auto& dep : this->m_PluginsDep) {
				// read category
				parser->Read(&(dep.m_PluginCategory), sizeof(CK_PLUGIN_TYPE));
				// get size and resize
				parser->Read(&guid_size, sizeof(CKDWORD));
				dep.m_Guids.resize(guid_size);
				// read data
				if (guid_size != 0) {
					parser->Read(dep.m_Guids.data(), sizeof(CKGUID) * guid_size);
				}

			}
		}

		// ========== included file list read ==========
		// file ver >= 8 have this feature
		if (this->m_FileInfo.FileVersion >= 8) {
			// MARK: i don't knwo what is this!
			int32_t hasIncludedFile;
			parser->Read(&hasIncludedFile, sizeof(int32_t));

			if (hasIncludedFile > 0) {
				// read included file size and resize
				CKDWORD includedFileCount;
				parser->Read(&includedFileCount, sizeof(CKDWORD));
				this->m_IncludedFiles.resize(includedFileCount);

				hasIncludedFile -= static_cast<int32_t>(sizeof(CKDWORD));
			}

			// MARK: backward pos
			// backward with 0?
			parser->MoveCursor(hasIncludedFile);
		}

		// ========== sync main parser ==========
		if (this->m_FileInfo.FileVersion >= 8) {
			// file ver >= 8, use header offset
			// because it have compress feature
			ParserPtr->SetCursor(this->m_FileInfo.Hdr1PackSize + sizeof(CKRawFileInfo));
		} else {
			// otherwise, sync with current parser.
			ParserPtr->SetCursor(parser->GetCursor());
		}

		return CKERROR::CKERR_OK;
	}

	CKERROR CKFileReader::ReadFileData(CKBufferParser* ParserPtr) {
		std::unique_ptr<CKBufferParser> parser(new CKBufferParser(ParserPtr->GetBase(), ParserPtr->GetSize(), false));
		parser->SetCursor(ParserPtr->GetCursor());

		std::string name_conv;

		// ========== compress feature process ==========
		if (EnumsHelper::Has(this->m_FileInfo.FileWriteMode, CK_FILE_WRITEMODE::CKFILE_CHUNKCOMPRESSED_OLD) ||
			EnumsHelper::Has(this->m_FileInfo.FileWriteMode, CK_FILE_WRITEMODE::CKFILE_WHOLECOMPRESSED)) {

			void* decomp_buffer = CKUnPackData(this->m_FileInfo.DataUnPackSize, parser->GetPtr(), this->m_FileInfo.DataPackSize);
			if (decomp_buffer != nullptr) {
				parser = std::unique_ptr<CKBufferParser>(new CKBufferParser(decomp_buffer, this->m_FileInfo.DataUnPackSize, true));
			}
		}

		// ========== old file crc and obj list read ==========
		// only file ver < 8 run this
		if (this->m_FileInfo.FileVersion < 8) {
			// check crc
			CKDWORD gotten_crc = CKComputeDataCRC(
				parser->GetPtr(),
				parser->GetSize() - parser->GetCursor(),
				0u
			);
			if (gotten_crc != this->m_FileInfo.Crc) {
				this->m_Ctx->OutputToConsole("Virtools file CRC error.");
				return CKERROR::CKERR_FILECRCERROR;
			}

			// MARK: why read again? especially for file ver == 7.
			// get save id max
			parser->Read(&this->m_SaveIDMax, sizeof(int32_t));
			// get object count and resize
			parser->Read(&this->m_FileInfo.ObjectCount, sizeof(CKDWORD));
			if (this->m_FileObjects.empty()) {
				this->m_FileObjects.resize(this->m_FileInfo.ObjectCount);
			}
		}

		// ========== manager read ==========
		// only file ver >= 6 have this
		if (this->m_FileInfo.ManagerCount != 0) {
			this->m_ManagersData.resize(this->m_FileInfo.ManagerCount);
			CKDWORD stateChunkLen = 0u;
			bool stateChkParseSuccess = false;

			for (auto& mgr : this->m_ManagersData) {
				// read guid
				parser->Read(&(mgr.Manager), sizeof(CKGUID));

				// read statechunk len
				parser->Read(&stateChunkLen, sizeof(CKDWORD));
				// check len
				if (stateChunkLen == 0) {
					mgr.Data = nullptr;
					continue;
				}

				// read statechunk
				mgr.Data = new CKStateChunk(&this->m_Visitor, this->m_Ctx);
				stateChkParseSuccess = mgr.Data->ConvertFromBuffer(parser->GetPtr());
				if (!stateChkParseSuccess) {
					delete mgr.Data;
					mgr.Data = nullptr;
				}
				parser->MoveCursor(stateChunkLen);
			}
		}

		// ========== object read ==========
		// only works file version >= 4. < 4 section has been removed.
		if (this->m_FileInfo.ObjectCount != 0) {
			// new file reader section
			bool stateChkParseSuccess = false;
			for (auto& obj : this->m_FileObjects) {
				// get statechunk len
				parser->Read(&obj.PackSize, sizeof(CKDWORD));
				// check state chunk len
				if (obj.PackSize == 0) {
					obj.Data = nullptr;
					continue;
				}

				// read state chunk
				obj.Data = new CKStateChunk(&this->m_Visitor, this->m_Ctx);
				stateChkParseSuccess = obj.Data->ConvertFromBuffer(parser->GetPtr());
				if (!stateChkParseSuccess) {
					delete obj.Data;
					obj.Data = nullptr;
				}
				parser->MoveCursor(obj.PackSize);

			}
		}

		// ========== included file get ==========
		// before reading, we need switch back to original parser.
		// and skip data chunk size
		parser = std::unique_ptr<CKBufferParser>(new CKBufferParser(ParserPtr->GetBase(), ParserPtr->GetSize(), false));
		parser->SetCursor(ParserPtr->GetCursor());
		parser->MoveCursor(this->m_FileInfo.DataPackSize);

		// then we can read it.
		if (this->m_IncludedFiles.size() != 0) {
			for (auto& file : this->m_IncludedFiles) {
				// get file name length and resize it
				CKDWORD filenamelen = 0u;
				parser->Read(&filenamelen, sizeof(CKDWORD));
				name_conv.resize(filenamelen);

				// read filename
				if (filenamelen != 0) {
					parser->Read(name_conv.data(), filenamelen);
					m_Ctx->GetUtf8String(name_conv, file);
				}

				// read file body length
				CKDWORD filebodylen = 0u;
				parser->Read(&filebodylen, sizeof(CKDWORD));

				// read file body
				std::string tempfilename = m_Ctx->GetTempFilePath(file.c_str());
				FILE* fp = EncodingHelper::U8FOpen(tempfilename.c_str(), "wb");
				if (fp != nullptr) {
					std::fwrite(parser->GetPtr(), sizeof(char), filebodylen, fp);
					std::fclose(fp);
				} else {
					m_Ctx->OutputToConsoleEx("Fail to open temp file: %s", tempfilename.c_str());
				}

				// move to next
				parser->MoveCursor(filebodylen);
			}
		}

		return CKERROR::CKERR_OK;
	}

	CKERROR CKFileReader::DeepLoad(CKSTRING u8_filename) {
		// check document status
		if (this->m_Done) CKERROR::CKERR_CANCELLED;

		// ========== prepare work ==========
		CKERROR err = CKERROR::CKERR_OK;

		// get shallow document first
		err = this->ShallowLoad(u8_filename);
		if (err != CKERROR::CKERR_OK) return err;

		// ========== create object first ==========
		for (auto& obj : this->m_FileObjects) {
			// todo: skip CK_LEVEL
			// todo: resolve references
			if (obj.Data == nullptr) continue;

			// create object and assign created obj ckid
			obj.ObjPtr = m_Ctx->CreateObject(obj.ObjectCid, obj.Name.toCKSTRING());
			if (obj.ObjPtr == nullptr) {
				obj.CreatedObjectId = 0u;
			} else {
				obj.CreatedObjectId = obj.ObjPtr->GetID();
			}
		}

		// ========== CKStateChunk remap ==========
		// todo: remap
		// todo: CK_LEVEL special proc

		// ========== consume Managers ==========
		// todo...

		// ========== analyze objects CKStateChunk ==========
		for (auto& obj : this->m_FileObjects) {
			if (obj.Data == nullptr || obj.ObjPtr == nullptr) continue;

			// todo: special treat for CK_LEVEL
			// try parsing data
			obj.Data->StartRead();
			bool success = obj.ObjPtr->Load(obj.Data, &this->m_Visitor);
			obj.Data->StopRead();
			if (success) {
				// if success, clear CKStateChunk*
				delete obj.Data;
				obj.Data = nullptr;
			} else {
				// if failed, delete it
				m_Ctx->DestroyObject(obj.ObjectId);
				obj.ObjPtr = nullptr;
				obj.CreatedObjectId = 0u;
			}
		}

		// ========== finalize work ==========
		return CKERROR::CKERR_OK;
	}

}
