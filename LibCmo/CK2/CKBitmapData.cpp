#include "CKBitmapData.hpp"
#include "CKContext.hpp"
#include "CKStateChunk.hpp"
#include "DataHandlers/CKBitmapHandler.hpp"
#include "MgrImpls/CKPathManager.hpp"
#include <memory>

namespace LibCmo::CK2 {

#pragma region Assist RW Functions

	bool CKBitmapData::ReadSpecificFormatBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot) {
		// read transparent prop
		CKDWORD transprop;
		chk->ReadStruct(transprop);

		// get ext and guid to find correct guid
		CKCHAR filerawext[4];
		CKGUID fileguid;
		chk->ReadNoSizeBuffer(CKSizeof(filerawext), filerawext);
		chk->ReadStruct(fileguid);
		CKFileExtension fileext(filerawext);
		auto reader = DataHandlers::CKBitmapHandler::GetBitmapHandlerWrapper(fileext, fileguid);
		if (reader == nullptr) {
			return false;
		}

		// read image size
		CKDWORD imgbytesize;
		chk->ReadStruct(imgbytesize);
		if (imgbytesize != 0) {
			// get image data ptr
			const void* imgdata = nullptr;
			if (!chk->ReadDryBuffer(&imgdata, imgbytesize)) {
				return false;
			}

			// parse image
			VxMath::VxImageDescEx cache;
			if (!reader->ReadMemory(imgdata, imgbytesize, &cache)) {
				return false;
			}

			// post proc image (copy to slot)
			VxMath::VxDoBlit(&cache, slot);

			// proc image alpha
			if (transprop == 2) {
				CKDWORD alphacount;
				chk->ReadStruct(alphacount);
				if (alphacount == 1) {
					CKDWORD globalalpha;
					chk->ReadStruct(globalalpha);
					VxMath::VxDoAlphaBlit(slot, static_cast<CKBYTE>(globalalpha));
				} else {
					CKStateChunk::TBuffer alphabuf;
					CKDWORD buflen;
					chk->ReadBufferWrapper(&alphabuf, &buflen);
					VxMath::VxDoAlphaBlit(slot, reinterpret_cast<CKBYTE*>(alphabuf.get()));
				}
			}

		}

		return true;
	}

	bool CKBitmapData::ReadRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot) {
		CKDWORD bytePerPixel, width, height, redMask, greenMask, blueMask, alphaMask;
		chk->ReadStruct(bytePerPixel);	// not used
		if (bytePerPixel == 0) return false;

		chk->ReadStruct(width);
		chk->ReadStruct(height);
		chk->ReadStruct(alphaMask);
		chk->ReadStruct(redMask);
		chk->ReadStruct(greenMask);
		chk->ReadStruct(blueMask);

		// read RGBA buffer
		CKStateChunk::TBuffer redBuffer, greenBuffer, blueBuffer, alphaBuffer;
		CKDWORD bufsize;
		CKDWORD bufopt;
		chk->ReadStruct(bufopt);
		bufopt &= 0xFu;
		if (bufopt != 0) {
			// MARK: not supported CCompressionTools::jpegDecode()
			// There are some shitty jpeg decode function.
			// I do not want to touch them because all of my work do not related to them
			// so return false simply
			return false;
		} else {
			chk->ReadBufferWrapper(&redBuffer, &bufsize);
			chk->ReadBufferWrapper(&greenBuffer, &bufsize);
			chk->ReadBufferWrapper(&blueBuffer, &bufsize);
		}
		chk->ReadBufferWrapper(&alphaBuffer, &bufsize);

		// write into file
		if (redBuffer != nullptr && greenBuffer != nullptr && blueBuffer != nullptr) {
			// create image
			slot->CreateImage(width, height);
			// get essential data
			CKDWORD pixelcount = slot->GetPixelCount();
			CKBYTE* dst = slot->GetMutableImage(),
				* redSrc = reinterpret_cast<CKBYTE*>(redBuffer.get()),
				* greenSrc = reinterpret_cast<CKBYTE*>(greenBuffer.get()),
				* blueSrc = reinterpret_cast<CKBYTE*>(blueBuffer.get()),
				* alphaSrc = reinterpret_cast<CKBYTE*>(alphaBuffer.get());
			for (CKDWORD p = 0; p < pixelcount; ++p) {
				// if no alpha data, set to 0xFF
				*(dst++) = (alphaBuffer != nullptr ? (*(alphaSrc++)) : 0xFFu);
				*(dst++) = *(redSrc++);
				*(dst++) = *(greenSrc++);
				*(dst++) = *(blueSrc++);
			}
		}

		return true;
	}

	bool CKBitmapData::ReadOldRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot) {
		// MARK: not supported because all of my work do not involve this function.
		return false;
	}

	void CKBitmapData::WriteSpecificFormatBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* slot) {

	}

	void CKBitmapData::WriteRawBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* slot) {
		
	}

#pragma endregion

#pragma region Core Read / Write

	bool CKBitmapData::ReadFromChunk(CKStateChunk* chunk, CKFileVisitor* file, const CKBitmapDataReadIdentifiers& identifiers) {
		XContainer::XBitArray hasReadSlot;

		// check 3 types enbedded image
		// MARK: i think there is a potential vulnerable issue.
		// if a slot failed, all following slot will read data from a wrong position.
		// thus the program will crash or allocated massive garbage data.
		if (chunk->SeekIdentifierDword(identifiers.m_SpecificFormat)) {
			// specific format
			CKDWORD slotcount, width, height, bpp;
			chunk->ReadStruct(slotcount);
			chunk->ReadStruct(width);
			chunk->ReadStruct(height);
			chunk->ReadStruct(bpp);

			SetSlotCount(slotcount);
			hasReadSlot.resize(slotcount, false);

			// the height and width is written outside of specific format
			// so we create image first for it.
			// and let reader to read data.
			// and free image if is is failed.
			if (width > 0 && height > 0) {
				for (CKDWORD i = 0; i < slotcount; ++i) {
					CreateImage(width, height, i);
					if (ReadSpecificFormatBitmap(chunk, GetImageDesc(i))) {
						hasReadSlot[i] = true;
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
			hasReadSlot.resize(slotcount, false);

			// the height and width is read by raw data function self.
			// so we pass a cache variable to reader and do some modification
			// if it is success.
			for (CKDWORD i = 0; i < slotcount; ++i) {
				VxMath::VxImageDescEx rawcache;
				if (ReadRawBitmap(chunk, &rawcache)) {
					hasReadSlot[i] = true;

					// do upside down blit
					CreateImage(rawcache.GetWidth(), rawcache.GetHeight(), i);
					VxMath::VxDoBlitUpsideDown(&rawcache, GetImageDesc(i));
				}
			}

		} else if (chunk->SeekIdentifierDword(identifiers.m_OldRawData)) {
			// raw data (old format)
			CKDWORD slotcount;
			chunk->ReadStruct(slotcount);
			
			SetSlotCount(slotcount);
			hasReadSlot.resize(slotcount, false);

			// MARK: a rough implement because we do not support this identifier
			for (CKDWORD i = 0; i < slotcount; ++i) {
				if (ReadOldRawBitmap(chunk, GetImageDesc(i))) {
					hasReadSlot[i] = true;
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
				if (filename.empty()) continue;
				
				bool isNotLoaded = i >= hasReadSlot.size() || (!hasReadSlot[i]);
				if (isNotLoaded) {
					// if this image is not loaded.
					// try resolve its file name and load it.
					// and set resolved filename for it.
					if (m_Context->GetPathManager()->ResolveFileName(filename)) {
						if (LoadImage(filename.c_str(), i)) {
							SetSlotFileName(i, filename.c_str());
						}
					}
				} else {
					// otherwise, set filename simply
					SetSlotFileName(i, filename.c_str());
				}

			}
		}

		// movie info
		if (chunk->SeekIdentifierDword(identifiers.m_MovieFileName)) {
			// MARK: movie is not implemented here.
		}
		
		return true;
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
		auto reader = DataHandlers::CKBitmapHandler::GetBitmapHandlerWrapper(CKFileExtension(ext.c_str()), CKGUID());
		if (reader == nullptr) return false;

		// get desc and read data
		if (!reader->ReadFile(filename, GetImageDesc(slot))) {
			return false;
		}

		return true;
	}

	bool CKBitmapData::SaveImage(CKSTRING filename, CKDWORD slot, bool isForceThisFmt) {
		if (filename == nullptr) return false;
		if (slot >= m_Slots.size()) return false;

		// prepare save format
		CKBitmapProperties savefmt;
		if (isForceThisFmt) {
			savefmt = this->m_SaveProperties;
		} else {
			std::string ext(filename);
			m_Context->GetPathManager()->GetExtension(ext);
			if (ext.empty()) {
				// fallback to this fmt
				savefmt = this->m_SaveProperties;
			} else {
				savefmt.m_Ext.SetExt(ext.c_str());
			}
		}

		// get reader by format
		auto reader = DataHandlers::CKBitmapHandler::GetBitmapHandlerWrapper(savefmt.m_Ext, savefmt.m_ReaderGuid);
		if (reader == nullptr) return false;

		// save file
		if (!reader->SaveFile(filename, GetImageDesc(slot), savefmt)) {
			return false;
		}

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
		if (Transparency) {
			EnumsHelper::Add(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_TRANSPARENT);
		} else {
			EnumsHelper::Rm(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_TRANSPARENT);
		}
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

	void CKBitmapData::SetPickThreshold(CKDWORD threshold) {
		m_PickThreshold = threshold;
	}

	CKDWORD CKBitmapData::GetPickThreshold() {
		return m_PickThreshold;
	}
	
#pragma endregion

	CKBitmapData::CKBitmapData(CKContext* ctx) :
		m_Context(ctx),
		m_Slots(), m_CurrentSlot(0),
		m_PickThreshold(0), m_BitmapFlags(CK_BITMAPDATA_FLAGS::CKBITMAPDATA_INVALID), m_TransColor(0),
		m_SaveProperties(ctx->GetGlobalImagesSaveFormat()), m_SaveOptions(CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_USEGLOBAL) {}

	CKBitmapData::~CKBitmapData() {}

}
