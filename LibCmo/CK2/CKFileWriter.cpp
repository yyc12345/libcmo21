#include "CKFile.hpp"
#include "CKContext.hpp"
#include "CKStateChunk.hpp"
#include "ObjImpls/CKObject.hpp"
#include "MgrImpls/CKBaseManager.hpp"
#include "MgrImpls/CKPathManager.hpp"
#include "../VxMath/VxMemoryMappedFile.hpp"
#include <memory>

namespace LibCmo::CK2 {

	CKERROR CKFileWriter::Save(CKSTRING u8_filename) {
		// check document status
		if (this->m_Done) return CKERROR::CKERR_CANCELLED;

		// encoding conv helper
		XContainer::XString name_conv;

		// try detect filename legality
		CKERROR err = PrepareFile(u8_filename);
		if (err != CKERROR::CKERR_OK) return err;

		// ========== Prepare Stage ==========
		// todo: add TOBEDELETED flag for all Referenced objects's m_ObjectFlags

		// MARK: ignore the notification to all CKBehavior based objects.

		// ========== StateChunk convertion ==========

		// iterate all objects and transform it into CKStateChunk
		// MARK: Drop the support of collecting the sum of InterfaceChunk's size.
		// because it is useless.
		for (auto& obj : m_FileObjects) {
			// if there is a chunk, skip
			if (obj.Data != nullptr) continue;

			obj.Data = new CKStateChunk(&m_Visitor, m_Ctx);
			obj.Data->StartWrite();
			bool suc = obj.ObjPtr->Save(obj.Data, &m_Visitor, obj.SaveFlags);
			obj.Data->StopWrite();
			if (!suc) {
				// fail to parse
				delete obj.Data;
				obj.Data = nullptr;
			}
		}

		// iterate manager
		// if copied from reader. skip
		if (!m_IsCopyFromReader) {
			CKINT mgrcount = m_Ctx->GetManagerCount();
			CKINT availablemgr = 0;

			// place manager
			// if no data, skip it
			m_ManagersData.resize(mgrcount);
			for (CKINT i = 0; i < mgrcount; ++i) {
				MgrImpls::CKBaseManager* mgr = m_Ctx->GetManager(i);

				m_ManagersData[availablemgr].Manager = mgr->GetGuid();

				m_ManagersData[availablemgr].Data = new CKStateChunk(&m_Visitor, m_Ctx);
				m_ManagersData[availablemgr].Data->StartWrite();
				bool suc = mgr->SaveData(m_ManagersData[availablemgr].Data, &m_Visitor);
				m_ManagersData[availablemgr].Data->StopWrite();
				if (!suc) {
					delete m_ManagersData[availablemgr].Data;
					m_ManagersData[availablemgr].Data = nullptr;
				} else {
					++availablemgr;
				}
			}
			// resize to the new size which erase all skipped manager
			m_ManagersData.resize(availablemgr);

		}

		// if copied from reader, skip plugin dep
		if (!m_IsCopyFromReader) {
			// todo: finish plugin dep filling
		}

		// MARK: skip include file filling.
		// we order user manually fill it.

		// ========== Size Calc ==========
		// iterate 3 list to get each parts' size
		CKDWORD sumDataObjSize = 0,
			sumHdrObjSize = 0;
		for (auto& obj : m_FileObjects) {
			// += 4DWORD(ObjId, ObjCid, FileIndex, NameLen)
			sumHdrObjSize += 4 * CKSizeof(CKDWORD);
			if (XContainer::NSXString::ToCKSTRING(obj.Name) != nullptr) {
				// += Name size
				m_Ctx->GetNativeString(obj.Name, name_conv);
				sumHdrObjSize += static_cast<CKDWORD>(name_conv.size());
			}

			if (obj.Data == nullptr) {
				obj.PackSize = 0;
			} else {
				obj.PackSize = obj.Data->ConvertToBuffer(nullptr);
			}
			// += chunk size + chunk
			sumDataObjSize += obj.PackSize + CKSizeof(CKDWORD);
		}

		CKDWORD sumDataMgrSize = 0;
		for (auto& mgr : m_ManagersData) {
			CKDWORD chunksize;
			if (mgr.Data == nullptr) {
				chunksize = 0;
			} else {
				chunksize = mgr.Data->ConvertToBuffer(nullptr);
			}
			// += GUID(2 DWORD) + chunk size + chunk
			sumDataMgrSize += chunksize + 3 * CKSizeof(CKDWORD);
		}

		// += Plugin Dep list size
		CKDWORD sumHdrPlgSize = CKSizeof(CKDWORD);
		for (auto& plg : m_PluginsDep) {
			// += GUID list + (dep category + GUID list size)
			sumHdrPlgSize +=
				CKSizeof(CKGUID) * static_cast<CKDWORD>(plg.m_Guids.size())
				+ 2 * CKSizeof(CKDWORD);
		}

		CKDWORD sumHdrIncludedFiles = CKSizeof(CKINT) + CKSizeof(CKDWORD);

		// calc the whole size
		CKDWORD sumHdrSize = sumHdrObjSize + sumHdrPlgSize + sumHdrIncludedFiles;
		CKDWORD sumDataSize = sumDataObjSize + sumDataMgrSize;

		// compute file index for all object
		if (!m_FileObjects.empty()) {
			// set base for first one
			m_FileObjects[0].FileIndex = sumHdrSize + sumDataMgrSize + CKSizeof(CKRawFileInfo);
			// calc the remains
			for (size_t i = 1; i < m_FileObjects.size(); ++i) {
				// prev obj PackSize + prev obj FileIndex + prev obj chunk size
				m_FileObjects[i].FileIndex = m_FileObjects[i - 1].FileIndex + m_FileObjects[i - 1].PackSize + CKSizeof(CKDWORD);
			}
		}

		// ========== Construct File Header ==========
		CK_FILE_WRITEMODE fileWriteMode = m_Ctx->GetFileWriteMode();

		CKRawFileInfo rawHeader;
		std::memcpy(&rawHeader.NeMo, CKNEMOFI, sizeof(CKRawFileInfo::NeMo));
		rawHeader.Crc = 0;
		rawHeader.Zero = 0;
		rawHeader.CKVersion = CKVERSION;
		rawHeader.FileVersion = 8;
		rawHeader.FileWriteMode = static_cast<CKDWORD>(fileWriteMode);
		rawHeader.ObjectCount = static_cast<CKDWORD>(m_FileObjects.size());
		rawHeader.ManagerCount = static_cast<CKDWORD>(m_ManagersData.size());
		rawHeader.Hdr1UnPackSize = sumHdrSize;
		rawHeader.DataUnPackSize = sumDataSize;
		rawHeader.Hdr1PackSize = sumHdrSize;
		rawHeader.DataPackSize = sumDataSize;
		rawHeader.ProductVersion = DEVVERSION;
		rawHeader.ProductBuild = DEVBUILD;
		rawHeader.MaxIDSaved = m_SaveIDMax;
		// crc will filled later

		// ========== Write header ==========
		// create a buffer
		std::unique_ptr<CKBufferParser> hdrparser(new CKBufferParser(sumHdrSize));

		// write obj
		for (auto& obj : m_FileObjects) {

			// todo: remove TOBEDELETED for referenced objects' m_ObjectFlags

			hdrparser->Write(&obj.ObjectId);
			hdrparser->Write(&obj.ObjectCid);
			hdrparser->Write(&obj.FileIndex);

			if (XContainer::NSXString::ToCKSTRING(obj.Name) != nullptr) {
				m_Ctx->GetNativeString(obj.Name, name_conv);
				CKDWORD namelen = static_cast<CKDWORD>(name_conv.size());
				hdrparser->Write(&namelen);
				hdrparser->Write(name_conv.data(), namelen);
			}
		}

		// write plugin dep
		{
			CKDWORD depsize = static_cast<CKDWORD>(m_PluginsDep.size());
			hdrparser->Write(&depsize);

			for (auto& dep : m_PluginsDep) {
				hdrparser->Write(&dep.m_PluginCategory, sizeof(CK_PLUGIN_TYPE));

				CKDWORD guidsize = static_cast<CKDWORD>(dep.m_Guids.size());
				hdrparser->Write(&guidsize);

				hdrparser->Write(dep.m_Guids.data(), sizeof(CKGUID) * guidsize);
			}
		}

		// write included file
		{
			CKDWORD cache = CKSizeof(CKDWORD);
			hdrparser->Write(&cache);

			cache = static_cast<CKDWORD>(m_IncludedFiles.size());
			hdrparser->Write(&cache);
		}

		// compress header if needed
		if (EnumsHelper::Has(fileWriteMode, CK_FILE_WRITEMODE::CKFILE_CHUNKCOMPRESSED_OLD) ||
			EnumsHelper::Has(fileWriteMode, CK_FILE_WRITEMODE::CKFILE_WHOLECOMPRESSED)) {

			CKDWORD comp_buf_size = 0;
			void* comp_buffer = CKPackData(hdrparser->GetBase(), hdrparser->GetSize(), comp_buf_size, m_Ctx->GetCompressionLevel());
			if (comp_buffer != nullptr) {
				hdrparser = std::unique_ptr<CKBufferParser>(new CKBufferParser(comp_buffer, comp_buf_size, true));
				rawHeader.Hdr1PackSize = comp_buf_size;
			} else {
				// fallback
				rawHeader.Hdr1PackSize = rawHeader.Hdr1UnPackSize;
			}
		}

		// ========== Write data ==========
		// create a buffer
		std::unique_ptr<CKBufferParser> datparser(new CKBufferParser(sumDataSize));

		// write manager
		for (auto& mgr : m_ManagersData) {
			datparser->Write(&mgr.Manager);

			CKDWORD writtenSize = 0;
			if (mgr.Data != nullptr) {
				writtenSize = mgr.Data->ConvertToBuffer(datparser->GetMutablePtr(CKSizeof(CKDWORD)));
				delete mgr.Data;
				mgr.Data = nullptr;
			}

			datparser->Write(&writtenSize);
			datparser->MoveCursor(writtenSize);
		}

		// write object
		for (auto& obj : m_FileObjects) {
			datparser->Write(&obj.PackSize);

			if (obj.Data != nullptr) {
				obj.Data->ConvertToBuffer(datparser->GetMutablePtr());
				delete obj.Data;
				obj.Data = nullptr;
			}

			datparser->MoveCursor(obj.PackSize);
		}

		// compress header if needed
		if (EnumsHelper::Has(fileWriteMode, CK_FILE_WRITEMODE::CKFILE_CHUNKCOMPRESSED_OLD) ||
			EnumsHelper::Has(fileWriteMode, CK_FILE_WRITEMODE::CKFILE_WHOLECOMPRESSED)) {

			CKDWORD comp_buf_size = 0;
			void* comp_buffer = CKPackData(datparser->GetBase(), datparser->GetSize(), comp_buf_size, m_Ctx->GetCompressionLevel());
			if (comp_buffer != nullptr) {
				datparser = std::unique_ptr<CKBufferParser>(new CKBufferParser(comp_buffer, comp_buf_size, true));
				rawHeader.DataPackSize = comp_buf_size;
			} else {
				// fallback
				rawHeader.DataPackSize = rawHeader.DataUnPackSize;
			}
		}

		// ========== Construct File Info ==========
		// compute crc
		CKDWORD computedcrc = CKComputeDataCRC(&rawHeader, CKSizeof(CKRawFileInfo), 0u);
		computedcrc = CKComputeDataCRC(hdrparser->GetBase(), hdrparser->GetSize(), computedcrc);
		computedcrc = CKComputeDataCRC(datparser->GetBase(), datparser->GetSize(), computedcrc);

		// copy to file info
		this->m_FileInfo.ProductVersion = rawHeader.ProductVersion;
		this->m_FileInfo.ProductBuild = rawHeader.ProductBuild;
		this->m_FileInfo.FileWriteMode = static_cast<CK_FILE_WRITEMODE>(rawHeader.FileWriteMode);
		this->m_FileInfo.CKVersion = rawHeader.CKVersion;
		this->m_FileInfo.FileVersion = rawHeader.FileVersion;
		this->m_FileInfo.Hdr1PackSize = rawHeader.Hdr1PackSize;
		this->m_FileInfo.Hdr1UnPackSize = rawHeader.Hdr1UnPackSize;
		this->m_FileInfo.DataPackSize = rawHeader.DataPackSize;
		this->m_FileInfo.DataUnPackSize = rawHeader.DataUnPackSize;
		this->m_FileInfo.ManagerCount = rawHeader.ManagerCount;
		this->m_FileInfo.ObjectCount = rawHeader.ObjectCount;
		this->m_FileInfo.MaxIDSaved = rawHeader.MaxIDSaved;
		// fill file size and crc
		this->m_FileInfo.FileSize = CKSizeof(CKRawFileInfo) + rawHeader.DataPackSize + rawHeader.Hdr1PackSize;
		this->m_FileInfo.Crc = computedcrc;
		rawHeader.Crc = computedcrc;

		// ========== Open File & Write Essential Data ==========
		// open file and test
		FILE* fs = EncodingHelper::U8FOpen(u8_filename, "wb");
		if (fs == nullptr) return CKERROR::CKERR_CANTWRITETOFILE;
		// write small header + header + data
		std::fwrite(&rawHeader, sizeof(CKRawFileInfo), 1, fs);
		std::fwrite(hdrparser->GetBase(), sizeof(CKBYTE), hdrparser->GetSize(), fs);
		std::fwrite(datparser->GetBase(), sizeof(CKBYTE), datparser->GetSize(), fs);
		// free buffer
		hdrparser.reset();
		datparser.reset();

		// ========== Included Files ==========
		for (auto& fentry : m_IncludedFiles) {
			// write filename
			m_Ctx->GetNativeString(fentry, name_conv);
			CKDWORD filenamelen = static_cast<CKDWORD>(name_conv.size());
			std::fwrite(&filenamelen, sizeof(CKDWORD), 1, fs);
			std::fwrite(name_conv.data(), sizeof(CKBYTE), filenamelen, fs);

			// try mapping file.
			XContainer::XString tempfilename = m_Ctx->GetPathManager()->GetTempFilePath(fentry.c_str());
			std::unique_ptr<VxMath::VxMemoryMappedFile> mappedFile(new VxMath::VxMemoryMappedFile(tempfilename.c_str()));
			if (mappedFile->IsValid()) {
				// write file length
				CKDWORD filebodylen = mappedFile->GetFileSize();
				std::fwrite(&filebodylen, sizeof(CKDWORD), 1, fs);

				// write file body
				std::fwrite(mappedFile->GetBase(), sizeof(CKBYTE), filebodylen, fs);
			} else {
				// write zero file length
				CKDWORD filebodylen = 0;
				std::fwrite(&filebodylen, sizeof(CKDWORD), 1, fs);

				// report error
				m_Ctx->OutputToConsoleEx("Fail to open temp file: %s", tempfilename.c_str());
			}

			// release mapped file
			mappedFile.reset();

		}

		// close file
		std::fclose(fs);

		return CKERROR::CKERR_OK;
	}

	CKERROR CKFileWriter::PrepareFile(CKSTRING filename) {
		// check nullptr
		if (filename == nullptr) return CKERROR::CKERR_INVALIDFILE;

		// try open file to check whether we can write it.
		CKERROR err;
		FILE* tryfile = EncodingHelper::U8FOpen(filename, "ab");
		if (tryfile == nullptr) {
			err = CKERROR::CKERR_CANTWRITETOFILE;
		} else {
			err = CKERROR::CKERR_OK;
			std::fclose(tryfile);
		}

		return err;
	}

}