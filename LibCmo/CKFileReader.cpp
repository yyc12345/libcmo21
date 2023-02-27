#include "CKFile.hpp"
#include "CKGlobals.hpp"
#include "CKStateChunk.hpp"
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

	CKERROR CKFile::ShallowLoad(CKSTRING u8_filename, CKFileData::ShallowDocument** out_doc) {
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
			this->m_MinCtx->Printf("Fail to create Memory File for \"%s\".", this->m_FileName.c_str());
			return CKERROR::CKERR_INVALIDFILE;
		}

		// create document
		std::unique_ptr<CKFileData::ShallowDocument> doc(new(std::nothrow) CKFileData::ShallowDocument());
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

	CKERROR CKFile::ReadFileHeader(CKBufferParser* ParserPtr, CKFileData::ShallowDocument* doc) {
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
			rawHeader.ProductBuild = 0x01010000;
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
					delete[] decomp_buffer;
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
		bool noDepLost = true;
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

	CKERROR CKFile::ReadFileData(CKBufferParser* ParserPtr, CKFileData::ShallowDocument* doc) {
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
					delete[] decomp_buffer;
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
				mgr.Data = new(std::nothrow) CKStateChunk();
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
				obj.Data = new(std::nothrow) CKStateChunk();
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

	CKERROR CKFile::DeepLoad(CKSTRING u8_filename, CKFileData::DeepDocument** out_doc) {
		return CKERROR::CKERR_OK;
	}


	//CKERROR CKFile::Load(CKSTRING u8_filename, /*CKObjectArray list, */ CK_LOAD_FLAGS flags) {
	//	CKERROR result = this->OpenFile(u8_filename, flags);
	//	if (result == CKERROR::CKERR_OK || result == CKERROR::CKERR_PLUGINSMISSING) {
	//		result = this->LoadFileData();
	//	}

	//	return result;
	//}

	//CKERROR CKFile::OpenFile(CKSTRING u8_filename, CK_LOAD_FLAGS flags) {
	//	this->ClearData();
	//	if (u8_filename == nullptr) return CKERROR::CKERR_INVALIDPARAMETER;

	//	this->m_FileName = u8_filename;
	//	this->m_MappedFile = new(std::nothrow) VxMemoryMappedFile(this->m_FileName.c_str());
	//	if (this->m_MappedFile == nullptr) return CKERROR::CKERR_OUTOFMEMORY;
	//	if (!this->m_MappedFile->IsValid()) return CKERROR::CKERR_INVALIDFILE;

	//	// MARK: CKContext::SetLastCmoLoaded
	//	return this->OpenMemory(this->m_MappedFile->GetBase(), this->m_MappedFile->GetFileSize(), flags);
	//}

	//CKERROR CKFile::OpenMemory(void* MemoryBuffer, size_t BufferSize, CK_LOAD_FLAGS Flags) {
	//	if (MemoryBuffer == nullptr) return CKERROR::CKERR_INVALIDPARAMETER;
	//	// compare magic words
	//	if (BufferSize < 0x20 || memcmp(MemoryBuffer, "Nemo", 4u)) return CKERROR::CKERR_INVALIDFILE;

	//	this->m_Parser = new(std::nothrow) CKBufferParser(MemoryBuffer, BufferSize, false);
	//	if (this->m_Parser == nullptr) return CKERROR::CKERR_OUTOFMEMORY;
	//	// MARK: eliminate check of m_Parser->m_ReaderBegin

	//	// MARK: dword_2405F6C0 = 0;
	//	this->m_Flags = Flags;
	//	this->m_IndexByClassId.resize(static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID));
	//	return this->ReadFileHeaders(&(this->m_Parser));
	//}

	//CKERROR CKFile::ReadFileHeaders(CKBufferParser** ParserPtr) {
	//	CKBufferParser* parser = *ParserPtr;
	//	this->m_IncludedFiles.clear();

	//	// ========== read header1 ==========
	//	CKDWORD fhdr1[8];
	//	CKDWORD fhdr2[8];
	//	if (parser->GetSize() < sizeof(fhdr1)) return CKERROR::CKERR_INVALIDFILE;
	//	parser->Read(fhdr1, sizeof(fhdr1));

	//	if (fhdr1[5]) {	// it seems that there is a ZERO checker?
	//		memset(fhdr1, 0, sizeof(fhdr1));
	//	}
	//	// check outdated
	//	if (fhdr1[4] > 9) return CKERROR::CKERR_OBSOLETEVIRTOOLS;

	//	// ========== read header2 ==========
	//	// file ver < 5 do not have second header
	//	if (fhdr1[4] < 5) {
	//		memset(fhdr2, 0, sizeof(fhdr2));
	//	} else {
	//		if (parser->GetSize() < sizeof(fhdr1) + sizeof(fhdr2)) return CKERROR::CKERR_INVALIDFILE;
	//		parser->Read(fhdr2, sizeof(fhdr2));
	//	}

	//	// forcely reset too big product ver
	//	if (fhdr2[5] >= 12) {
	//		fhdr2[5] = 0;
	//		fhdr2[6] = 0x1010000;
	//	}

	//	// ========== assign value ==========
	//	this->m_FileInfo.ProductVersion = fhdr2[5];
	//	this->m_FileInfo.ProductBuild = fhdr2[6];
	//	this->m_FileInfo.FileWriteMode = static_cast<CK_FILE_WRITEMODE>(fhdr1[6]);
	//	this->m_FileInfo.CKVersion = fhdr1[3];
	//	this->m_FileInfo.FileVersion = fhdr1[4];
	//	this->m_FileInfo.FileSize = parser->GetSize();
	//	this->m_FileInfo.ManagerCount = fhdr2[2];
	//	this->m_FileInfo.ObjectCount = fhdr2[3];
	//	this->m_FileInfo.MaxIDSaved = fhdr2[4];
	//	this->m_FileInfo.Hdr1PackSize = fhdr1[7];
	//	this->m_FileInfo.Hdr1UnPackSize = fhdr2[7];
	//	this->m_FileInfo.DataPackSize = fhdr2[0];
	//	this->m_FileInfo.DataUnPackSize = fhdr2[1];
	//	this->m_FileInfo.Crc = fhdr1[2];

	//	// ========== crc and body unpacker ==========
	//	if (this->m_FileInfo.FileVersion >= 8) {
	//		// crc checker for file ver >= 8 
	//		// reset crc field of header
	//		fhdr1[2] = 0;

	//		// compute crc
	//		CKDWORD gotten_crc = CKComputeDataCRC(&fhdr1, sizeof(fhdr1), 0u);
	//		parser->SetCursor(sizeof(fhdr1));
	//		gotten_crc = CKComputeDataCRC(parser->GetPtr(), sizeof(fhdr2), gotten_crc);
	//		parser->MoveCursor(sizeof(fhdr2));
	//		gotten_crc = CKComputeDataCRC(parser->GetPtr(), this->m_FileInfo.Hdr1PackSize, gotten_crc);
	//		parser->MoveCursor(this->m_FileInfo.Hdr1PackSize);
	//		gotten_crc = CKComputeDataCRC(parser->GetPtr(), this->m_FileInfo.DataPackSize, gotten_crc);
	//		parser->SetCursor(sizeof(fhdr1) + sizeof(fhdr2));

	//		if (gotten_crc != this->m_FileInfo.Crc) return CKERROR::CKERR_FILECRCERROR;

	//		// compare size to decide wheher use compress feature
	//		void* decomp_buffer = CKUnPackData(this->m_FileInfo.Hdr1UnPackSize, parser->GetPtr(), this->m_FileInfo.Hdr1PackSize);
	//		if (decomp_buffer != nullptr) {
	//			parser = new(std::nothrow) CKBufferParser(decomp_buffer, this->m_FileInfo.Hdr1UnPackSize, true);
	//			if (parser == nullptr) {
	//				delete[] decomp_buffer;
	//				return CKERROR::CKERR_OUTOFMEMORY;
	//			}
	//		}
	//	}

	//	// ========== object list read ==========
	//	// file ver >= 7 have this features
	//	if (this->m_FileInfo.FileVersion >= 7) {
	//		// apply max id saved
	//		this->m_SaveIDMax = this->m_FileInfo.MaxIDSaved;
	//		// resize
	//		this->m_FileObject.resize(this->m_FileInfo.ObjectCount);

	//		// read data
	//		for (auto& fileobj : this->m_FileObject) {
	//			// setup useless fields
	//			fileobj.ObjPtr = nullptr;
	//			fileobj.Data = nullptr;

	//			// read basic fields
	//			parser->Read(&(fileobj.Object), sizeof(CK_ID));
	//			parser->Read(&(fileobj.ObjectCid), sizeof(CK_CLASSID));
	//			parser->Read(&(fileobj.FileIndex), sizeof(CKDWORD));

	//			CKDWORD namelen;
	//			parser->Read(&namelen, sizeof(CKDWORD));
	//			if (namelen != 0) {
	//				fileobj.Name.resize(namelen);
	//				parser->Read(fileobj.Name.data(), namelen);
	//			}
	//		}
	//	}

	//	// ========== dep list read ==========
	//	// file ver >= 8 have this feature
	//	bool noDepLost = true;
	//	if (this->m_FileInfo.FileVersion >= 8) {
	//		// get size and resize
	//		CKDWORD depSize;
	//		parser->Read(&depSize, sizeof(CKDWORD));
	//		this->m_PluginDep.resize(depSize);

	//		CKDWORD guid_size;
	//		for (auto& dep : this->m_PluginDep) {
	//			// read category
	//			parser->Read(&(dep.m_PluginCategory), sizeof(CK_PLUGIN_TYPE));
	//			// get size and resize
	//			parser->Read(&guid_size, sizeof(CKDWORD));
	//			dep.m_Guids.resize(guid_size);
	//			dep.ValidGuids.resize(guid_size);
	//			// read data
	//			if (guid_size != 0) {
	//				parser->Read(dep.m_Guids.data(), sizeof(CKGUID) * guid_size);
	//			}

	//			// extra load flag
	//			if (EnumHelper::FlagEnumHas(this->m_Flags, CK_LOAD_FLAGS::CK_LOAD_CHECKDEPENDENCIES)) {
	//				for (size_t i = 0u; i < dep.m_Guids.size(); ++i) {
	//					// MARK: CKPluginManager::FindComponent
	//					bool plgEntryIsNull = true;
	//					if (plgEntryIsNull) {
	//						noDepLost = false;
	//						dep.ValidGuids[i] = false;
	//					} else {
	//						dep.ValidGuids[i] = true;
	//					}
	//				}
	//			}

	//		}
	//	}

	//	// ========== included file list read ==========
	//	// file ver >= 8 have this feature
	//	if (this->m_FileInfo.FileVersion >= 8) {
	//		// MARK: i don't knwo what is this!
	//		int32_t hasIncludedFile;
	//		parser->Read(&hasIncludedFile, sizeof(int32_t));

	//		if (hasIncludedFile > 0) {
	//			// read included file size and resize
	//			CKDWORD includedFileCount;
	//			parser->Read(&includedFileCount, sizeof(CKDWORD));
	//			this->m_IncludedFiles.resize(includedFileCount);

	//			hasIncludedFile -= 4;
	//		}

	//		// backward pos
	//		parser->SetCursor(hasIncludedFile);
	//	}

	//	// ========== read data ==========
	//	// restore old parser if needed
	//	if (parser != *ParserPtr) {
	//		delete parser;
	//		parser = *ParserPtr;
	//		parser->MoveCursor(this->m_FileInfo.Hdr1PackSize);
	//	}

	//	// MARK: dword_2405F6BC, dword_2405F6B8 set

	//	if (!(EnumHelper::FlagEnumHas(this->m_Flags, CK_LOAD_FLAGS::CK_LOAD_CHECKDEPENDENCIES) &&
	//		this->m_FileInfo.FileVersion < 8)) {
	//		// we have read all header. return result
	//		return noDepLost ? CKERROR::CKERR_OK : CKERROR::CKERR_PLUGINSMISSING;
	//	} // however, if we need check dep, and, we have not read dep list. continue read them.

	//	return CKERROR::CKERR_OK;
	//}

	//CKERROR CKFile::ReadFileData(CKBufferParser** ParserPtr) {
	//	CKBufferParser* parser = *ParserPtr;
	//	CKBufferParser* parserSrc = *ParserPtr;

	//	// ========== compress feature process ==========
	//	if (EnumHelper::FlagEnumHas(this->m_FileInfo.FileWriteMode, CK_FILE_WRITEMODE::CKFILE_CHUNKCOMPRESSED_OLD) ||
	//		EnumHelper::FlagEnumHas(this->m_FileInfo.FileWriteMode, CK_FILE_WRITEMODE::CKFILE_WHOLECOMPRESSED)) {
	//		void* decomp_buffer = CKUnPackData(this->m_FileInfo.DataUnPackSize, parser->GetPtr(), this->m_FileInfo.DataPackSize);
	//		parser->MoveCursor(this->m_FileInfo.DataPackSize);

	//		if (decomp_buffer != nullptr) {
	//			parser = new(std::nothrow) CKBufferParser(decomp_buffer, this->m_FileInfo.DataUnPackSize, true);
	//			if (parser == nullptr) {
	//				delete[] decomp_buffer;
	//				return CKERROR::CKERR_OUTOFMEMORY;
	//			}

	//			// sync to args
	//			*ParserPtr = parser;
	//		}
	//	}

	//	// ========== old file crc and obj list read ==========
	//	// only file ver < 8 run this
	//	if (this->m_FileInfo.FileVersion < 8) {
	//		// very very old flag
	//		if (this->m_FileInfo.FileVersion < 2) {
	//			;
	//			// MARK: dword_2405F6C0 setter
	//		}

	//		// check crc
	//		CKDWORD gotten_crc = CKComputeDataCRC(
	//			parser->GetPtr(),
	//			parser->GetSize() - parser->GetCursor(),
	//			0u
	//		);
	//		if (gotten_crc != this->m_FileInfo.Crc) {
	//			// MARK: report crc error
	//			return CKERROR::CKERR_FILECRCERROR;
	//		}

	//		// get save id max
	//		parser->Read(&this->m_SaveIDMax, sizeof(int32_t));
	//		// get object count and resize
	//		parser->Read(&this->m_FileInfo.ObjectCount, sizeof(CKDWORD));
	//		if (this->m_FileObject.empty()) {
	//			this->m_FileObject.resize(this->m_FileInfo.ObjectCount);
	//			// MARK: ZeroMemory removed. i think it is not necessary.
	//		}
	//	}

	//	// ========== manager read ==========
	//	// only file ver >= 6 have this
	//	if (this->m_FileInfo.FileVersion >= 6 && this->m_FileInfo.ManagerCount != 0) {
	//		this->m_ManagersData.resize(this->m_FileInfo.ManagerCount);
	//		CKDWORD stateChunkLen = 0u;
	//		bool stateChkParseSuccess = false;

	//		for (auto& mgr : this->m_ManagersData) {
	//			// read guid
	//			parser->Read(&(mgr.Manager), sizeof(CKGUID));

	//			// read statechunk len
	//			parser->Read(&stateChunkLen, sizeof(CKDWORD));
	//			// check len
	//			if (stateChunkLen == 0) {
	//				mgr.Data = nullptr;
	//				continue;
	//			}
	//			// read statechunk
	//			mgr.Data = new(std::nothrow) CKStateChunk();
	//			if (mgr.Data != nullptr) {
	//				stateChkParseSuccess = mgr.Data->ConvertFromBuffer(parser->GetPtr());
	//				if (!stateChkParseSuccess) {
	//					delete mgr.Data;
	//					mgr.Data = nullptr;
	//				}
	//			}
	//			parser->MoveCursor(stateChunkLen);
	//		}
	//	}

	//	// ========== object read ==========
	//	if (this->m_FileInfo.ObjectCount != 0) {
	//		if (this->m_FileInfo.FileVersion >= 4) {
	//			// new file reader section
	//			CKDWORD stateChunkLen = 0u;
	//			bool stateChkParseSuccess = false;
	//			for (auto& obj : this->m_FileObject) {
	//				if (this->m_FileInfo.FileVersion < 7) {
	//					// it seems that file ver < 7, ck id was stored first.
	//					parser->Read(&(obj.Object), sizeof(CK_ID));
	//				}
	//				// get statechunk len
	//				parser->Read(&stateChunkLen, sizeof(CKDWORD));

	//				// if only load behavior and current loaded obj is not behavior, give up.
	//				// >= 7 is corresponding with obj header version requirement
	//				// so don't worry about this
	//				if (EnumHelper::FlagEnumHas(this->m_Flags, CK_LOAD_FLAGS::CK_LOAD_ONLYBEHAVIORS) &&
	//					this->m_FileInfo.FileVersion >= 7 &&
	//					obj.ObjectCid != CK_CLASSID::CKCID_BEHAVIOR) {
	//					// move cursor for next one.
	//					parser->MoveCursor(stateChunkLen);
	//					continue;
	//				}

	//				// check state chunk len
	//				if (stateChunkLen == 0) {
	//					obj.Data = nullptr;
	//					continue;
	//				}
	//				// read state chunk
	//				obj.Data = new(std::nothrow) CKStateChunk();
	//				if (obj.Data != nullptr) {
	//					stateChkParseSuccess = obj.Data->ConvertFromBuffer(parser->GetPtr());
	//					if (!stateChkParseSuccess) {
	//						delete obj.Data;
	//						obj.Data = nullptr;
	//					} else {
	//						// get data size and fill some fields
	//						obj.PostPackSize = obj.Data->GetDataSize();
	//						obj.PrePackSize = obj.PostPackSize;
	//					}
	//				}
	//				parser->MoveCursor(stateChunkLen);

	//			}
	//		} else {
	//			// old file read section
	//			for (auto& obj : this->m_FileObject) {
	//				// read CK_ID
	//				parser->Read(&(obj.Object), sizeof(CK_ID));
	//				// get data len
	//				CKDWORD dataLen = 0u;
	//				parser->Read(&dataLen, sizeof(CKDWORD));

	//				// check state chunk len
	//				if (dataLen == 0) {
	//					obj.Data = nullptr;
	//					continue;
	//				}

	//				// MARK: set dword_2405F6C0
	//				
	//				// read class id
	//				CK_CLASSID clsid = CK_CLASSID::CKCID_OBJECT;
	//				parser->Read(&clsid, sizeof(CK_CLASSID));
	//				if (static_cast<uint32_t>(clsid) == UINT32_C(0)) {
	//					// invalid cid;
	//					return CKERROR::CKERR_INVALIDFILE;
	//				}

	//				// read save flags
	//				parser->Read(&obj.SaveFlags, sizeof(CK_FILE_WRITEMODE));

	//				// read statechunk len
	//				CKDWORD oldBufLen = 0u;
	//				parser->Read(&oldBufLen, sizeof(CKDWORD));

	//				// setup state chunk and order parse it
	//				obj.Data = new(std::nothrow) CKStateChunk(clsid);
	//				if (obj.Data != nullptr) {
	//					if (!obj.Data->ConvertFromOldBuffer(parser->GetPtr(), oldBufLen, dataLen, obj.SaveFlags)) {
	//						delete obj.Data;
	//						obj.Data = nullptr;
	//					}
	//				}
	//				// move to next
	//				parser->MoveCursor(oldBufLen);
	//			}

	//			// MARK: remove a weird assign: parserSrc = v46; it seems useless.
	//		}
	//	}

	//	// ========== object name get ==========
	//	// only file ver < 7 need get it from statechunk
	//	if (this->m_FileInfo.FileVersion < 7) {
	//		for (auto& obj : this->m_FileObject) {
	//			if (obj.Data != nullptr) {
	//				// TODO: CK_STATESAVE_NAME
	//				if (obj.Data->SeekIdentifier(1u)) {
	//					obj.Data->ReadString(obj.Name);
	//				}
	//			}
	//		}
	//	}

	//	// ========== included file get ==========
	//	// WARN: we use "parserSrc" to read, not "parser"!!!
	//	for (size_t i = 0; i < this->m_IncludedFiles.size(); ++i) {
	//		// get file name length and resize it
	//		CKDWORD filenamelen = 0u;
	//		parserSrc->Read(&filenamelen, sizeof(CKDWORD));
	//		std::string filename;
	//		filename.resize(filenamelen);

	//		// read filename
	//		if (filenamelen != 0) {
	//			parserSrc->Read(filename.data(), filenamelen);
	//		}

	//		// todo: construct temp folder path
	//		// and regulate file name

	//		// read file body length
	//		CKDWORD filebodylen = 0u;
	//		parserSrc->Read(&filebodylen, sizeof(CKDWORD));

	//		// todo: read file body
	//		parserSrc->MoveCursor(filebodylen);
	//	}

	//	// ========== free parser ==========
	//	if (parserSrc != parser && parserSrc != nullptr) {
	//		// WARN: we should free parserSrc! not free parser
	//		// because "parser" has synced with ParserPtr and will be freed from outside.
	//		delete parserSrc;

	//	}

	//	return CKERROR::CKERR_OK;
	//}

	//CKERROR CKFile::LoadFileData(void/*CKObjectArray list*/) {
	//	if (!this->m_Parser && !this->m_ReadFileDataDone) {
	//		return CKERROR::CKERR_INVALIDFILE;
	//	}

	//	// MARK: sub_240372EA, ctx + 193 = 1

	//	// if file data has not been read. read it
	//	CKERROR err = CKERROR::CKERR_OK;
	//	CKBufferParser** ParserPtr = &this->m_Parser;
	//	if (!this->m_ReadFileDataDone) {
	//		err = this->ReadFileData(ParserPtr);
	//	}

	//	// MARK: i assume FinishLoading do not calling mapped file anymore
	//	// free file data
	//	if (*ParserPtr != nullptr) {
	//		delete *ParserPtr;
	//		*ParserPtr = nullptr;
	//	}
	//	if (this->m_MappedFile != nullptr) {
	//		delete this->m_MappedFile;
	//		this->m_MappedFile = nullptr;
	//	}

	//	// if no error, do finish loading
	//	if (err == CKERROR::CKERR_OK) {
	//		this->FinishLoading(this->m_Flags);

	//		// MARK: dword_2405F6C0 old vt ver output
	//	}
	//	// MARK: CKContext::SetAutomaticLoadMode
	//	// MARK:CKContext::SetUserLoadCallback

	//	// MARK: sub_24037360
	//	// MARK: ctx + 193 = 0

	//	return err;
	//}

	//CKERROR CKFile::FinishLoading(/*CKObjectArray list, */CK_LOAD_FLAGS flags) {
	//	return CKERROR::CKERR_OK;
	//}

}