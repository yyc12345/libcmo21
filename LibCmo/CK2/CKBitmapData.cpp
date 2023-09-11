#include "CKBitmapData.hpp"
#include "CKContext.hpp"
#include "CKStateChunk.hpp"
#include "DataHandlers/CKBitmapHandler.hpp"
#include "MgrImpls/CKPathManager.hpp"
#include <memory>

namespace LibCmo::CK2 {

#pragma region Assist RW Functions

	bool CKBitmapData::ReadSpecificFormatBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot) {
		return false;
	}

	bool CKBitmapData::ReadRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot) {
		return false;
	}

	bool CKBitmapData::ReadOldRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot) {
		return false;
	}

	void CKBitmapData::WriteSpecificFormatBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* slot) {

	}

	void CKBitmapData::WriteRawBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* slot) {
		
	}

#pragma endregion

#pragma region Core Read / Write

	bool CKBitmapData::ReadFromChunk(CKStateChunk* chunk, CKFileVisitor* file, const CKBitmapDataReadIdentifiers& identifiers) {
		XContainer::XBitArray notReadSlot;

		// check 3 types enbedded image
		if (chunk->SeekIdentifierDword(identifiers.m_SpecificFormat)) {
			// specific format
			CKDWORD slotcount, width, height, bpp;
			chunk->ReadStruct(slotcount);
			chunk->ReadStruct(width);
			chunk->ReadStruct(height);
			chunk->ReadStruct(bpp);

			SetSlotCount(slotcount);
			notReadSlot.resize(slotcount, false);

			if (width > 0 && height > 0) {
				for (CKDWORD i = 0; i < slotcount; ++i) {
					CreateImage(width, height, i);
					if (ReadSpecificFormatBitmap(chunk, GetImageDesc(i))) {
						notReadSlot[i] = true;
					} else {
						ReleaseImage(i);
					}
				}
			}

		} else if (chunk->SeekIdentifierDword(identifiers.m_RawData)) {
			// raw data
			CKDWORD slotcount;
			chunk->ReadStruct(slotcount);
			
			SetSlotCount(slotcount);
			notReadSlot.resize(slotcount, false);

			for (CKDWORD i = 0; i < slotcount; ++i) {
				if (ReadRawBitmap(chunk, GetImageDesc(i))) {
					notReadSlot[i] = true;
				} else {
					ReleaseImage(i);
				}
			}

		} else if (chunk->SeekIdentifierDword(identifiers.m_OldRawData)) {
			// raw data (old format)
			CKDWORD slotcount;
			chunk->ReadStruct(slotcount);
			
			SetSlotCount(slotcount);
			notReadSlot.resize(slotcount, false);

			for (CKDWORD i = 0; i < slotcount; ++i) {
				if (ReadOldRawBitmap(chunk, GetImageDesc(i))) {
					notReadSlot[i] = true;
				} else {
					ReleaseImage(i);
				}
			}

		}

		// file name section
		if (chunk->SeekIdentifierDword(identifiers.m_FileNames)) {
			// read slot count
			CKDWORD slotcount;
			chunk->ReadStruct(slotcount);
			SetSlotCount(slotcount);

			// read string in detail
			// and try load not loaded image.
			for (CKDWORD i = 0; i < slotcount; ++i) {
				std::string filename;
				chunk->ReadString(filename);
				if (filename.empty()) return;

				// set 
				SetSlotFileName(i, filename.c_str());

				// if not loaded, and suc in resolve file path.
				// load it
				bool isNotLoaded = i >= notReadSlot.size() || notReadSlot[i];
				if (isNotLoaded && m_Context->GetPathManager()->ResolveFileName(filename)) {
					LoadImage(filename.c_str(), i);
				}
			}
		}

		// movie info
		if (chunk->SeekIdentifierDword(identifiers.m_MovieFileName)) {
			// MARK: movie is not implemented here.
		}
		
	}

	bool CKBitmapData::DumpToChunk(CKStateChunk* chunk, CKFileVisitor* file, const CKBitmapDataWriteIdentifiers& identifiers) {
		return false;
	}

#pragma endregion

#pragma region Misc Functions

	void CKBitmapData::SetSlotCount(CKDWORD count) {
		m_Slots.resize(count);

		if (count == 0) {
			EnumsHelper::Add(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_INVALID);
		}
	}

	CKDWORD CKBitmapData::GetSlotCount() {
		return static_cast<CKDWORD>(m_Slots.size());
	}

	void CKBitmapData::SetCurrentSlot(CKDWORD slot) {
		if (slot >= m_Slots.size()) return;

		m_CurrentSlot = slot;

		// NOTE: idk what the fuck this is. just interpter the IDA decompiled code.
		if (EnumsHelper::Has(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_CUBEMAP)) {
			EnumsHelper::Add(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_FORCERESTORE);
		}
	}

	CKDWORD CKBitmapData::GetCurrentSlot() {
		return m_CurrentSlot;
	}

	void CKBitmapData::CreateImage(CKDWORD Width, CKDWORD Height, CKDWORD Slot) {
		if (Slot >= m_Slots.size()) return;

		CKBitmapSlot& slotdata = m_Slots[Slot];
		slotdata.m_ImageData.CreateImage(Width, Height);
		VxMath::VxDoAlphaBlit(&slotdata.m_ImageData, 0xFFu);
	}

	bool CKBitmapData::LoadImage(CKSTRING filename, CKDWORD slot) {
		if (filename == nullptr) return false;
		if (slot >= m_Slots.size()) return false;

		// get extension of file. then get corresponding reader
		std::string ext(filename);
		m_Context->GetPathManager()->GetExtension(ext);
		std::unique_ptr<DataHandlers::CKBitmapHandler, DataHandlers::CKBitmapHandlerDeleter> reader(
			DataHandlers::CKBitmapHandler::GetBitmapHandler(CKFileExtension(ext.c_str()), CKGUID())
		);
		if (reader == nullptr) return false;

		// get desc
		VxMath::VxImageDescEx* desc = GetImageDesc(slot);
		if (desc == nullptr) return false;

		// read data
		if (!reader->ReadFile(filename, desc)) {
			return false;
		}

		// free reader
		reader.reset();
		return true;
	}

	bool CKBitmapData::SaveImage(CKSTRING filename, CKDWORD slot) {
		if (filename == nullptr) return false;
		if (slot >= m_Slots.size()) return false;

		return true;
	}

	VxMath::VxImageDescEx* CKBitmapData::GetImageDesc(CKDWORD slot) {
		if (slot >= m_Slots.size()) return nullptr;
		return &m_Slots[slot].m_ImageData;
	}

	void CKBitmapData::ReleaseImage(CKDWORD slot) {
		if (slot >= m_Slots.size()) return;
		m_Slots[slot].m_ImageData.FreeImage();
	}

	void CKBitmapData::SetSlotFileName(CKDWORD slot, CKSTRING filename) {
		if (slot >= m_Slots.size()) return;
		if (filename == nullptr) return;
		m_Slots[slot].m_FileName = filename;
	}

	CKSTRING CKBitmapData::GetSlotFileName(CKDWORD slot) {
		if (slot >= m_Slots.size()) return nullptr;
		return m_Slots[slot].m_FileName.c_str();
	}

	const CKBitmapProperties& CKBitmapData::GetSaveFormat() {
		return m_SaveProperties;
	}

	void CKBitmapData::SetSaveFormat(const CKBitmapProperties& props) {
		m_SaveProperties = props;
	}

	CK_TEXTURE_SAVEOPTIONS CKBitmapData::GetSaveOptions() {
		return m_SaveOptions;
	}

	void CKBitmapData::SetSaveOptions(CK_TEXTURE_SAVEOPTIONS opts) {
		m_SaveOptions = opts;
	}

	void CKBitmapData::SetTransparent(bool Transparency) {
		EnumsHelper::Add(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_TRANSPARENT);
	}

	bool CKBitmapData::IsTransparent() {
		return EnumsHelper::Has(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_TRANSPARENT);
	}

	void CKBitmapData::SetTransparentColor(CKDWORD col) {
		SetTransparent(true);
		m_TransColor = col;
	}

	CKDWORD CKBitmapData::GetTransparentColor() {
		return m_TransColor;
	}
	
#pragma endregion

	CKBitmapData::CKBitmapData(CKContext* ctx) :
		m_Context(ctx),
		m_Slots(), m_CurrentSlot(0),
		m_PickThreshold(0), m_BitmapFlags(CK_BITMAPDATA_FLAGS::CKBITMAPDATA_INVALID), m_TransColor(0),
		m_SaveProperties(ctx->GetGlobalImagesSaveFormat()), m_SaveOptions(CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_USEGLOBAL) {}

	CKBitmapData::~CKBitmapData() {}

}
