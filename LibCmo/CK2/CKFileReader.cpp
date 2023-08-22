#include "CKFile.hpp"
#include "CKGlobals.hpp"
#include "CKStateChunk.hpp"
#include "CKObjects.hpp"
#include "VxMemoryMappedFile.hpp"
#include "CKMinContext.hpp"
#include <memory>

namespace LibCmo::CK2 {

	/*
	* NOTE:
	* We onlt support read Virtools file with FileVersion >= 7.
	* The file with FileVersion < 7 is older than NeMo 1.0 (CK 1.1).
	* No need to support them.
	*/

	CKERROR CKFile::ShallowLoad(CKSTRING u8_filename, CKFileDocument** out_doc) {
		// preset value
		*out_doc = nullptr;

		// check file and open memory
		if (u8_filename == nullptr) return CKERROR::CKERR_INVALIDPARAMETER;
		std::unique_ptr<VxMath::VxMemoryMappedFile> mappedFile(new(std::nothrow) VxMath::VxMemoryMappedFile(u8_filename));
		if (mappedFile == nullptr) {
			this->m_MinCtx->Printf("Out of memory when creating Memory File.");
			return CKERROR::CKERR_OUTOFMEMORY;
		}
		if (!mappedFile->IsValid()) {
			this->m_MinCtx->Printf("Fail to create Memory File for \"%s\".", u8_filename);
			return CKERROR::CKERR_INVALIDFILE;
		}

		// create document
		std::unique_ptr<CKFileDocument> doc(new(std::nothrow) CKFileDocument());
		if (doc == nullptr) return CKERROR::CKERR_OUTOFMEMORY;

		// create buffer and start loading
		std::unique_ptr<CKBufferParser> parser(new(std::nothrow) CKBufferParser(mappedFile->GetBase(), mappedFile->GetFileSize(), false));
		CKERROR err = this->ReadFileHeader(parser.get(), doc.get());
		if (err != CKERROR::CKERR_OK) return err;
		err = this->ReadFileData(parser.get(), doc.get());
		if (err != CKERROR::CKERR_OK) return err;

		// unbind document and assign it
		*out_doc = doc.release();
		// other data will be free automatically
		return CKERROR::CKERR_OK;
	}

	CKERROR CKFile::ReadFileHeader(CKBufferParser* ParserPtr, CKFileDocument* doc) {
		std::unique_ptr<CKBufferParser> parser(new(std::nothrow) CKBufferParser(ParserPtr->GetBase(), ParserPtr->GetSize(), false));
		if (parser == nullptr) return CKERROR::CKERR_OUTOFMEMORY;
		parser->SetCursor(ParserPtr->GetCursor());

		std::string name_conv;

		// ========== read header ==========
		// check header size
		if (parser->GetSize() < sizeof(CKRawFileInfo)) return CKERROR::CKERR_INVALIDFILE;
		if (std::memcmp(parser->GetPtr(), "Nemo Fi", sizeof(CKRawFileInfo::NeMo))) return CKERROR::CKERR_INVALIDFILE;
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
		doc->m_FileInfo.ProductVersion = rawHeader.ProductVersion;
		doc->m_FileInfo.ProductBuild = rawHeader.ProductBuild;
		doc->m_FileInfo.FileWriteMode = static_cast<CK_FILE_WRITEMODE>(rawHeader.FileWriteMode);
		doc->m_FileInfo.CKVersion = rawHeader.CKVersion;
		doc->m_FileInfo.FileVersion = rawHeader.FileVersion;
		doc->m_FileInfo.FileSize = parser->GetSize();
		doc->m_FileInfo.ManagerCount = rawHeader.ManagerCount;
		doc->m_FileInfo.ObjectCount = rawHeader.ObjectCount;
		doc->m_FileInfo.MaxIDSaved = rawHeader.MaxIDSaved;
		doc->m_FileInfo.Hdr1PackSize = rawHeader.FileVersion >= 8 ? rawHeader.Hdr1PackSize : 0u;
		doc->m_FileInfo.Hdr1UnPackSize = rawHeader.FileVersion >= 8 ? rawHeader.Hdr1UnPackSize : 0u;
		doc->m_FileInfo.DataPackSize = rawHeader.DataPackSize;
		doc->m_FileInfo.DataUnPackSize = rawHeader.DataUnPackSize;
		doc->m_FileInfo.Crc = rawHeader.Crc;

		// ========== crc and body unpacker ==========
		if (doc->m_FileInfo.FileVersion >= 8) {
			// crc checker for file ver >= 8 
			// reset crc field of header
			rawHeader.Crc = 0u;

			// compute crc
			CKDWORD gotten_crc = CKComputeDataCRC(&rawHeader, sizeof(CKRawFileInfo), 0u);
			parser->SetCursor(sizeof(CKRawFileInfo));
			gotten_crc = CKComputeDataCRC(parser->GetPtr(), doc->m_FileInfo.Hdr1PackSize, gotten_crc);
			parser->MoveCursor(doc->m_FileInfo.Hdr1PackSize);
			gotten_crc = CKComputeDataCRC(parser->GetPtr(), doc->m_FileInfo.DataPackSize, gotten_crc);

			if (gotten_crc != doc->m_FileInfo.Crc) {
				this->m_MinCtx->Printf("Virtools file CRC error.");
				return CKERROR::CKERR_FILECRCERROR;
			}

			// reset cursor
			parser->SetCursor(sizeof(CKRawFileInfo));

			// compare size to decide wheher use compress feature
			void* decomp_buffer = CKUnPackData(doc->m_FileInfo.Hdr1UnPackSize, parser->GetPtr(), doc->m_FileInfo.Hdr1PackSize);
			if (decomp_buffer != nullptr) {
				parser = std::unique_ptr<CKBufferParser>(new(std::nothrow) CKBufferParser(decomp_buffer, doc->m_FileInfo.Hdr1UnPackSize, true));
				if (parser == nullptr) {
					delete[] reinterpret_cast<char*>(decomp_buffer);
					return CKERROR::CKERR_OUTOFMEMORY;
				}
			}
		}

		// ========== object list read ==========
		// file ver >= 7 have this features
		{
			// apply max id saved
			doc->m_SaveIDMax = doc->m_FileInfo.MaxIDSaved;
			// resize
			doc->m_FileObjects.resize(doc->m_FileInfo.ObjectCount);

			// read data
			for (auto& fileobj : doc->m_FileObjects) {
				// read basic fields
				parser->Read(&(fileobj.ObjectId), sizeof(CK_ID));
				parser->Read(&(fileobj.ObjectCid), sizeof(CK_CLASSID));
				parser->Read(&(fileobj.FileIndex), sizeof(CKDWORD));

				CKDWORD namelen;
				parser->Read(&namelen, sizeof(CKDWORD));
				if (namelen != 0) {
					name_conv.resize(namelen);
					parser->Read(name_conv.data(), namelen);
					m_MinCtx->GetUtf8String(name_conv, fileobj.Name);
				}
			}
		}

		// ========== dep list read ==========
		// file ver >= 8 have this feature
		if (doc->m_FileInfo.FileVersion >= 8) {
			// get size and resize
			CKDWORD depSize;
			parser->Read(&depSize, sizeof(CKDWORD));
			doc->m_PluginDep.resize(depSize);

			CKDWORD guid_size;
			for (auto& dep : doc->m_PluginDep) {
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
		if (doc->m_FileInfo.FileVersion >= 8) {
			// MARK: i don't knwo what is this!
			int32_t hasIncludedFile;
			parser->Read(&hasIncludedFile, sizeof(int32_t));

			if (hasIncludedFile > 0) {
				// read included file size and resize
				CKDWORD includedFileCount;
				parser->Read(&includedFileCount, sizeof(CKDWORD));
				doc->m_IncludedFiles.resize(includedFileCount);

				hasIncludedFile -= 4;
			}

			// MARK: backward pos
			// backward with 0?
			parser->MoveCursor(hasIncludedFile);
		}

		// ========== sync main parser ==========
		if (doc->m_FileInfo.FileVersion >= 8) {
			// file ver >= 8, use header offset
			// because it have compress feature
			ParserPtr->SetCursor(doc->m_FileInfo.Hdr1PackSize + sizeof(CKRawFileInfo));
		} else {
			// otherwise, sync with current parser.
			ParserPtr->SetCursor(parser->GetCursor());
		}

		return CKERROR::CKERR_OK;
	}

	CKERROR CKFile::ReadFileData(CKBufferParser* ParserPtr, CKFileDocument* doc) {
		std::unique_ptr<CKBufferParser> parser(new(std::nothrow) CKBufferParser(ParserPtr->GetBase(), ParserPtr->GetSize(), false));
		if (parser == nullptr) return CKERROR::CKERR_OUTOFMEMORY;
		parser->SetCursor(ParserPtr->GetCursor());

		std::string name_conv;

		// ========== compress feature process ==========
		if (EnumsHelper::FlagEnumHas(doc->m_FileInfo.FileWriteMode, CK_FILE_WRITEMODE::CKFILE_CHUNKCOMPRESSED_OLD) ||
			EnumsHelper::FlagEnumHas(doc->m_FileInfo.FileWriteMode, CK_FILE_WRITEMODE::CKFILE_WHOLECOMPRESSED)) {

			void* decomp_buffer = CKUnPackData(doc->m_FileInfo.DataUnPackSize, parser->GetPtr(), doc->m_FileInfo.DataPackSize);
			if (decomp_buffer != nullptr) {
				parser = std::unique_ptr<CKBufferParser>(new(std::nothrow) CKBufferParser(decomp_buffer, doc->m_FileInfo.DataUnPackSize, true));

				if (parser == nullptr) {
					delete[] reinterpret_cast<char*>(decomp_buffer);
					return CKERROR::CKERR_OUTOFMEMORY;
				}
			}
		}

		// ========== old file crc and obj list read ==========
		// only file ver < 8 run this
		if (doc->m_FileInfo.FileVersion < 8) {
			// check crc
			CKDWORD gotten_crc = CKComputeDataCRC(
				parser->GetPtr(),
				parser->GetSize() - parser->GetCursor(),
				0u
			);
			if (gotten_crc != doc->m_FileInfo.Crc) {
				this->m_MinCtx->Printf("Virtools file CRC error.");
				return CKERROR::CKERR_FILECRCERROR;
			}

			// MARK: why read again? especially for file ver == 7.
			// get save id max
			parser->Read(&doc->m_SaveIDMax, sizeof(int32_t));
			// get object count and resize
			parser->Read(&doc->m_FileInfo.ObjectCount, sizeof(CKDWORD));
			if (doc->m_FileObjects.empty()) {
				doc->m_FileObjects.resize(doc->m_FileInfo.ObjectCount);
			}
		}

		// ========== manager read ==========
		// only file ver >= 6 have this
		if (doc->m_FileInfo.ManagerCount != 0) {
			doc->m_FileManagersData.resize(doc->m_FileInfo.ManagerCount);
			CKDWORD stateChunkLen = 0u;
			bool stateChkParseSuccess = false;

			for (auto& mgr : doc->m_FileManagersData) {
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
				mgr.Data = new(std::nothrow) CKStateChunk(doc, this->m_MinCtx);
				if (mgr.Data != nullptr) {
					stateChkParseSuccess = mgr.Data->ConvertFromBuffer(parser->GetPtr());
					if (!stateChkParseSuccess) {
						delete mgr.Data;
						mgr.Data = nullptr;
					}
				}
				parser->MoveCursor(stateChunkLen);
			}
		}

		// ========== object read ==========
		// only works file version >= 4. < 4 section has been removed.
		if (doc->m_FileInfo.ObjectCount != 0) {
			// new file reader section
			CKDWORD stateChunkLen = 0u;
			bool stateChkParseSuccess = false;
			for (auto& obj : doc->m_FileObjects) {
				// get statechunk len
				parser->Read(&stateChunkLen, sizeof(CKDWORD));
				// check state chunk len
				if (stateChunkLen == 0) {
					obj.Data = nullptr;
					continue;
				}

				// read state chunk
				obj.Data = new(std::nothrow) CKStateChunk(doc, this->m_MinCtx);
				if (obj.Data != nullptr) {
					stateChkParseSuccess = obj.Data->ConvertFromBuffer(parser->GetPtr());
					if (!stateChkParseSuccess) {
						delete obj.Data;
						obj.Data = nullptr;
					}
				}
				parser->MoveCursor(stateChunkLen);

			}
		}

		// ========== included file get ==========
		// before reading, we need switch back to original parser.
		// and skip data chunk size
		parser = std::unique_ptr<CKBufferParser>(new(std::nothrow) CKBufferParser(ParserPtr->GetBase(), ParserPtr->GetSize(), false));
		parser->MoveCursor(doc->m_FileInfo.DataPackSize);

		// then we can read it.
		if (doc->m_IncludedFiles.size() != 0) {
			for (auto& file : doc->m_IncludedFiles) {
				// get file name length and resize it
				CKDWORD filenamelen = 0u;
				parser->Read(&filenamelen, sizeof(CKDWORD));
				name_conv.resize(filenamelen);

				// read filename
				if (filenamelen != 0) {
					parser->Read(name_conv.data(), filenamelen);
					m_MinCtx->GetUtf8String(name_conv, file);
				}

				// read file body length
				CKDWORD filebodylen = 0u;
				parser->Read(&filebodylen, sizeof(CKDWORD));

				// read file body
				FILE* fp = m_MinCtx->OpenTempFile(file.c_str(), false);
				if (fp != nullptr) {
					StreamHelper::CopyStream(parser->GetPtr(), fp, filebodylen);
					fclose(fp);
				}

				// move to next
				parser->MoveCursor(filebodylen);
			}
		}

		return CKERROR::CKERR_OK;
	}

	CKERROR CKFile::DeepLoad(CKSTRING u8_filename, CKFileDocument** out_doc) {
		// ========== prepare work ==========
		// preset value
		*out_doc = nullptr;
		CKERROR err = CKERROR::CKERR_OK;

		// get shallow document first
		CKFileDocument* rawShallowDoc = nullptr;
		err = this->ShallowLoad(u8_filename, &rawShallowDoc);
		if (rawShallowDoc == nullptr) return err;
		// use unique ptr wrap it as a deep doc
		std::unique_ptr<CKFileDocument> deepDoc(rawShallowDoc);
		if (err != CKERROR::CKERR_OK) return err;

		// ========== create object first ==========
		for (auto& obj : deepDoc->m_FileObjects) {
			// todo: skip CK_LEVEL
			// todo: resolve references
			if (obj.Data == nullptr) continue;

			// create object and assign created obj ckid
			obj.ObjPtr = m_MinCtx->CreateCKObject(obj.ObjectId, obj.ObjectCid, obj.Name.c_str());
			if (obj.ObjPtr == nullptr) {
				obj.CreatedObject = 0u;
			} else {
				obj.CreatedObject = obj.ObjPtr->m_ID;
			}
		}

		// ========== CKStateChunk remap ==========
		// todo: remap
		// todo: CK_LEVEL special proc

		// ========== consume Managers ==========
		// todo...

		// ========== analyze objects CKStateChunk ==========
		for (auto& obj : deepDoc->m_FileObjects) {
			if (obj.Data == nullptr || obj.ObjPtr == nullptr) continue;

			// todo: special treat for CK_LEVEL
			// try parsing data
			obj.Data->StartRead();
			bool success = obj.ObjPtr->Load(obj.Data, deepDoc.get());
			obj.Data->StopRead();
			if (success) {
				// if success, clear CKStateChunk*
				delete obj.Data;
				obj.Data = nullptr;
			} else {
				// if failed, delete it
				m_MinCtx->DestroyCKObject(obj.ObjectId);
				obj.ObjPtr = nullptr;
				obj.CreatedObject = 0u;
			}
		}

		// ========== finalize work ==========
		// detach and return
		*out_doc = deepDoc.release();
		return CKERROR::CKERR_OK;
	}

}
