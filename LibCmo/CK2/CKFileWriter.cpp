#include "CKFile.hpp"
#include "CKContext.hpp"
#include "CKStateChunk.hpp"
#include "ObjImpls/CKObject.hpp"
#include "MgrImpls/CKBaseManager.hpp"
#include "../VxMath/VxMemoryMappedFile.hpp"
#include <memory>

namespace LibCmo::CK2 {

	CKERROR CKFileWriter::Save(CKSTRING u8_filename) {
		// check document status
		if (this->m_Done) CKERROR::CKERR_CANCELLED;

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
			// += 4DWORD(ObjId, ObjCid, FileIndex, NameLen) + Name size
			sumHdrObjSize += 4 * sizeof(CKDWORD) + obj.Name.size();

			if (obj.Data == nullptr) {
				obj.PackSize = 0;
			} else {
				obj.PackSize = obj.Data->ConvertToBuffer(nullptr);
			}
			// += chunk size + chunk
			sumDataObjSize += obj.PackSize + sizeof(CKDWORD);
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
			sumDataMgrSize += chunksize + 3 * sizeof(CKDWORD);
		}

		// += Plugin Dep list size
		CKDWORD sumHdrPlgSize = sizeof(CKDWORD);
		for (auto& plg : m_PluginsDep) {
			// += GUID list + (dep category + GUID list size)
			sumHdrPlgSize += sizeof(CKGUID) * plg.m_Guids.size() + 2 * sizeof(CKDWORD);
		}

		CKDWORD sumHdrIncludedFiles = sizeof(int32_t) + sizeof(CKDWORD);

		// calc the whole size
		CKDWORD sumHdrSize = sumHdrObjSize + sumHdrPlgSize + sumHdrIncludedFiles;
		CKDWORD sumDataSize = sumDataObjSize + sumDataMgrSize;

		// compute file index for all object
		if (!m_FileObjects.empty()) {
			// set base for first one
			m_FileObjects[0].FileIndex = sumHdrSize + sumDataMgrSize + sizeof(CKRawFileInfo);
			// calc the remains
			for (size_t i = 1; i < m_FileObjects.size(); ++i) {
				// prev obj PackSize + prev obj FileIndex + prev obj chunk size
				m_FileObjects[i].FileIndex = m_FileObjects[i - 1].FileIndex +  m_FileObjects[i - 1].PackSize + sizeof(CKDWORD);
			}
		}

		// ========== Construct File Header ==========
		CKRawFileInfo rawHeader;
		std::memcpy(&rawHeader.NeMo, CKNEMOFI, sizeof(CKRawFileInfo::NeMo));
		rawHeader.Crc = 0;
		rawHeader.Zero = 0;
		rawHeader.CKVersion = CKVERSION;
		rawHeader.FileVersion = 8;
		rawHeader.FileWriteMode = static_cast<CKDWORD>(m_Ctx->GetFileWriteMode());
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

		// create a encoding conversion helper string
		std::string name_conv;

		// ========== Writing header ==========
		// create a buffer
		std::unique_ptr<CKBufferParser> hdrparser(new CKBufferParser(sumHdrSize));

		// write obj
		for (auto& obj : m_FileObjects) {

			// todo: remove TOBEDELETED for referenced objects' m_ObjectFlags

			hdrparser->Write(&obj.ObjectId, sizeof(CK_ID));
			hdrparser->Write(&obj.ObjectCid, sizeof(CK_CLASSID));
			hdrparser->Write(&obj.FileIndex, sizeof(CKDWORD));

			if (obj.Name.c_str() != nullptr) {
				m_Ctx->GetNativeString(obj.Name.string(), name_conv);
				CKDWORD namelen = static_cast<CKDWORD>(name_conv.size());
				hdrparser->Write(&namelen, sizeof(CKDWORD));
				hdrparser->Write(name_conv.data(), namelen);
			}
		}

		// write plugin dep
		{
			CKDWORD depsize = static_cast<CKDWORD>(m_PluginsDep.size());
			hdrparser->Write(&depsize, sizeof(CKDWORD));

			for (auto& dep : m_PluginsDep) {
				hdrparser->Write(&dep.m_PluginCategory, sizeof(CK_PLUGIN_TYPE));

				CKDWORD guidsize = static_cast<CKDWORD>(dep.m_Guids.size());
				hdrparser->Write(&guidsize, sizeof(CKDWORD));

				hdrparser->Write(dep.m_Guids.data(), sizeof(CKGUID) * guidsize);
			}
		}

	}

	CKERROR CKFileWriter::PrepareFile(CKSTRING filename) {
		// check nullptr
		if (filename == nullptr) return CKERROR::CKERR_INVALIDFILE;
		
		// try open file to check whether we can write it.
		CKERROR err;
		FILE* tryfile = m_Ctx->OpenFile(filename, "ab");
		if (tryfile == nullptr) {
			err = CKERROR::CKERR_CANTWRITETOFILE;
		} else {
			err = CKERROR::CKERR_OK;
			std::fclose(tryfile);
		}

		return err;
	}

}