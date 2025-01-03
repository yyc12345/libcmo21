#include "CKFile.hpp"
#include "CKStateChunk.hpp"
#include "ObjImpls/CKObject.hpp"
#include "MgrImpls/CKPathManager.hpp"
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
		if (this->m_Done) return CKERROR::CKERR_CANCELLED;
		// check CKContext encoding sequence
		if (!this->m_Ctx->IsValidEncoding()) return CKERROR::CKERR_CANCELLED;

		// check file and open memory
		if (u8_filename == nullptr) return CKERROR::CKERR_INVALIDPARAMETER;
		std::unique_ptr<VxMath::VxMemoryMappedFile> mappedFile(new VxMath::VxMemoryMappedFile(u8_filename));
		if (!mappedFile->IsValid()) {
			this->m_Ctx->OutputToConsoleEx(u8"Fail to create Memory File for \"%s\".", u8_filename);
			return CKERROR::CKERR_INVALIDFILE;
		}

		// create buffer and start loading
		std::unique_ptr<CKBufferParser> parser(new CKBufferParser(mappedFile->GetBase(), mappedFile->GetFileSize(), false));
		CKERROR err = this->ReadFileHeader(parser.get());
		if (err != CKERROR::CKERR_OK) return err;
		err = this->ReadFileData(parser.get());
		if (err != CKERROR::CKERR_OK) return err;

		// other data will be free automatically
		// set done flag and return
		this->m_Done = true;
		return CKERROR::CKERR_OK;
	}

	CKERROR CKFileReader::ReadFileHeader(CKBufferParser* ParserPtr) {
		std::unique_ptr<CKBufferParser> parser(new CKBufferParser(ParserPtr->GetBase(), ParserPtr->GetSize(), false));
		parser->SetCursor(ParserPtr->GetCursor());

		std::string name_conv;

		// ========== read header ==========
		// check header size
		if (parser->GetSize() < CKSizeof(CKRawFileInfo)) return CKERROR::CKERR_INVALIDFILE;
		if (std::memcmp(parser->GetPtr(), CKNEMOFI, sizeof(CKRawFileInfo::NeMo))) return CKERROR::CKERR_INVALIDFILE;
		// read header
		CKRawFileInfo rawHeader;
		parser->Read(&rawHeader);

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
		this->m_FileInfo.MaxIDSaved = static_cast<CK_ID>(rawHeader.MaxIDSaved);
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

			// Compute and check CRC in theory (< Virtools 4.0)
			CKDWORD gotten_crc = CKComputeDataCRC(&rawHeader, CKSizeof(CKRawFileInfo), 0u);
			parser->SetCursor(CKSizeof(CKRawFileInfo));
			gotten_crc = CKComputeDataCRC(parser->GetPtr(), this->m_FileInfo.Hdr1PackSize, gotten_crc);
			parser->MoveCursor(this->m_FileInfo.Hdr1PackSize);
			gotten_crc = CKComputeDataCRC(parser->GetPtr(), this->m_FileInfo.DataPackSize, gotten_crc);

			if (gotten_crc != this->m_FileInfo.Crc) {
				// MARK: 
				// If the CRC check failed, there is another way to compute CRC. (>= Virtools 4.0)
				// This is a patch for Dassault stupid programmer.
				// 
				// After Virtools 4.0, Dassault use a new way to compute the CRC of file.
				// Dassault introduces a new class called CKMemoryBufferWriter which use file and memory map to handle big file properly.
				// This algorithm splits the whole data body into 8 MB chunks and calculate them one by one to avoid instantaneous memory occupation.
				// However, there is a bug in virtual function CKMemoryBufferWriter::ComputeCRC.
				// It takes `PreviousCRC` as argument but never use it in function. 
				// In this function, the start value of CRC compution is hardcoded 0.
				// So, although Dassault programmer try to compute CRC for file header, header part and daat part in code, it actually only compute CRC for data part!
				// I 100% sure this is the mistake of Dassault stupid programmer and this bug cause more horrible result.
				// 
				// In Virtools 2.1, engine will check CRC of file first. If no matched CRC, engine will reject loading file.
				// So the obvious result is that we can not load file saved by Virtools 4.0 in Virtools 2.1.
				// But this is not the point which makes me indignant.
				// The real weird point is that we can use Virtools 3.5 to open file saved by Virtools 4.0, but why?
				// After some researches, I found that the programmer of Dassault totally removed CRC check when loading file, since some version which I don't know, to suppress this bug!
				// This is totally cheat and commercial-oriented behavior!
				// I guess Dassault programmer also found that they can not load new created file in old Virtools.
				// But they didn't find out what cause this bug, and just directly remove the whole of CRC checker to resolve this bug!
				// I can't believe that this thing happens on such official software.
				// This is the point which makes me indignant.
				gotten_crc = CKComputeDataCRC(parser->GetPtr(), this->m_FileInfo.DataPackSize, 0u);

				// Both CRC compute methods are failed. This file may be really broken.
				// Report exception directly.
				if (gotten_crc != this->m_FileInfo.Crc) {
					this->m_Ctx->OutputToConsole(u8"Virtools file CRC error.");
					return CKERROR::CKERR_FILECRCERROR;
				}
			}

			// reset cursor
			parser->SetCursor(CKSizeof(CKRawFileInfo));

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
				parser->Read(&fileobj.ObjectId);
				parser->Read(&fileobj.ObjectCid);
				parser->Read(&fileobj.FileIndex);

				CKDWORD namelen;
				parser->Read(&namelen);
				if (namelen != 0) {
					name_conv.resize(namelen);
					parser->Read(name_conv.data(), namelen);
					if (!m_Ctx->GetUTF8String(name_conv, fileobj.Name))
						m_Ctx->OutputToConsole(u8"Fail to get UTF8 name for CKObject when reading file header. Some objects name will leave to blank.");
				} else {
					XContainer::NSXString::FromCKSTRING(fileobj.Name, nullptr);
				}
			}
		}

		// ========== dep list read ==========
		// file ver >= 8 have this feature
		if (this->m_FileInfo.FileVersion >= 8) {
			// get size and resize
			CKDWORD depSize;
			parser->Read(&depSize);
			this->m_PluginsDep.resize(depSize);

			CKDWORD guid_size;
			for (auto& dep : this->m_PluginsDep) {
				// read category
				parser->Read(&dep.m_PluginCategory);
				// get size and resize
				parser->Read(&guid_size);
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
			CKINT hasIncludedFile;
			parser->Read(&hasIncludedFile);

			if (hasIncludedFile > 0) {
				// read included file size and resize
				CKDWORD includedFileCount;
				parser->Read(&includedFileCount);
				this->m_IncludedFiles.resize(includedFileCount);

				hasIncludedFile -= static_cast<CKINT>(sizeof(CKDWORD));
			}

			// MARK: backward pos
			// backward with 0?
			parser->MoveCursor(hasIncludedFile);
		}

		// ========== sync main parser ==========
		if (this->m_FileInfo.FileVersion >= 8) {
			// file ver >= 8, use header offset
			// because it have compress feature
			ParserPtr->SetCursor(this->m_FileInfo.Hdr1PackSize + CKSizeof(CKRawFileInfo));
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
		if (YYCC::EnumHelper::Has(this->m_FileInfo.FileWriteMode, CK_FILE_WRITEMODE::CKFILE_CHUNKCOMPRESSED_OLD) ||
			YYCC::EnumHelper::Has(this->m_FileInfo.FileWriteMode, CK_FILE_WRITEMODE::CKFILE_WHOLECOMPRESSED)) {

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
				this->m_Ctx->OutputToConsole(u8"Virtools file CRC error.");
				return CKERROR::CKERR_FILECRCERROR;
			}

			// MARK: why read again? especially for file ver == 7.
			// get save id max
			parser->Read(&this->m_SaveIDMax);
			// get object count and resize
			parser->Read(&this->m_FileInfo.ObjectCount);
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
				parser->Read(&mgr.Manager);

				// read statechunk len
				parser->Read(&stateChunkLen);
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
				parser->Read(&obj.PackSize);
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
				parser->Read(&filenamelen);
				name_conv.resize(filenamelen);

				// read filename
				if (filenamelen != 0) {
					parser->Read(name_conv.data(), filenamelen);
					if (!m_Ctx->GetUTF8String(name_conv, file))
						m_Ctx->OutputToConsole(u8"Fail to get UTF8 name for included file when reading file body. Some included files may be stripped.");
				}

				// read file body length
				CKDWORD filebodylen = 0u;
				parser->Read(&filebodylen);

				// read file body
				XContainer::XString tempfilename = m_Ctx->GetPathManager()->GetTempFilePath(file.c_str());
				FILE* fp = YYCC::IOHelper::UTF8FOpen(tempfilename.c_str(), u8"wb");
				if (fp != nullptr) {
					std::fwrite(parser->GetPtr(), sizeof(CKBYTE), filebodylen, fp);
					std::fclose(fp);
				} else {
					m_Ctx->OutputToConsoleEx(u8"Fail to open temp file: %s", tempfilename.c_str());
				}

				// move to next
				parser->MoveCursor(filebodylen);
			}
		}

		return CKERROR::CKERR_OK;
	}

	CKERROR CKFileReader::DeepLoad(CKSTRING u8_filename) {
		// check document status
		if (this->m_Done) return CKERROR::CKERR_CANCELLED;
		// check CKContext encoding sequence
		if (!this->m_Ctx->IsValidEncoding()) return CKERROR::CKERR_CANCELLED;

		// ========== prepare work ==========
		CKERROR err = CKERROR::CKERR_OK;

		// get shallow document first
		err = this->ShallowLoad(u8_filename);
		if (err != CKERROR::CKERR_OK) return err;
		// reset done flag because we need further processing
		this->m_Done = false;

		// ========== create object first ==========
		for (auto& obj : this->m_FileObjects) {
			// todo: skip CK_LEVEL
			// todo: resolve references
			if (obj.Data == nullptr) continue;

			// create object and assign created obj ckid
			obj.ObjPtr = m_Ctx->CreateObject(obj.ObjectCid, XContainer::NSXString::ToCKSTRING(obj.Name));
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


		// set done flag and return
		this->m_Done = true;
		return CKERROR::CKERR_OK;
	}

}
