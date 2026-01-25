#include "CKBitmapData.hpp"
#include "CKContext.hpp"
#include "CKStateChunk.hpp"
#include "CKFile.hpp"
#include "DataHandlers/CKBitmapHandler.hpp"
#include "MgrImpls/CKPathManager.hpp"
#include <yycc/cenum.hpp>
#include <memory>

namespace LibCmo::CK2 {

#pragma region Assist RW Functions

	constexpr CKDWORD c_SpecificFmtHasTransparent = 2;
	constexpr CKDWORD c_SpecificFmtNoTransparent = 1;

	bool CKBitmapData::ReadSpecificFormatBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot) {
		// read transparent prop
		CKDWORD transprop;
		chk->ReadStruct(transprop);

		// get ext and guid to find correct guid
		CKCHAR filerawext[4];
		CKGUID fileguid;
		chk->ReadAndFillBuffer(filerawext, CKSizeof(filerawext));
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
			auto imgdata = chk->LockReadBufferWrapper(imgbytesize);
			if (imgdata == nullptr) {
				return false;
			}

			// parse image
			VxMath::VxImageDescEx cache;
			if (!reader->ReadMemory(imgdata.get(), imgbytesize, &cache)) {
				return false;
			}
			// unlock buffer
			imgdata.reset();

			// post proc image (copy to slot)
			VxMath::VxDoBlit(&cache, slot);

			// proc image alpha
			if (transprop == c_SpecificFmtHasTransparent) {
				CKDWORD alphacount;
				chk->ReadStruct(alphacount);
				if (alphacount == 1) {
					CKDWORD globalalpha;
					chk->ReadStruct(globalalpha);
					VxMath::VxDoAlphaBlit(slot, static_cast<CKBYTE>(globalalpha));
				} else {
					auto alphabuf = chk->ReadBufferWrapper();
					VxMath::VxDoAlphaBlit(slot, static_cast<const CKBYTE*>(alphabuf.get()));
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
		CKStateChunk::Buffer_t redBuffer, greenBuffer, blueBuffer, alphaBuffer;
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
			blueBuffer = chk->ReadBufferWrapper();
			greenBuffer = chk->ReadBufferWrapper();
			redBuffer = chk->ReadBufferWrapper();
		}
		alphaBuffer = chk->ReadBufferWrapper();

		// write into file
		if (redBuffer != nullptr && greenBuffer != nullptr && blueBuffer != nullptr) {
			// create image
			slot->CreateImage(width, height);
			// get essential data
			CKDWORD pixelcount = slot->GetPixelCount();
			CKBYTE* dst = slot->GetMutableImage(),
				* redSrc = static_cast<CKBYTE*>(redBuffer.get()),
				* greenSrc = static_cast<CKBYTE*>(greenBuffer.get()),
				* blueSrc = static_cast<CKBYTE*>(blueBuffer.get()),
				* alphaSrc = static_cast<CKBYTE*>(alphaBuffer.get());
			for (CKDWORD p = 0; p < pixelcount; ++p) {
				*(dst++) = *(blueSrc++);
				*(dst++) = *(greenSrc++);
				*(dst++) = *(redSrc++);
				// if no alpha data, set to 0xFF
				*(dst++) = (alphaBuffer != nullptr ? (*(alphaSrc++)) : 0xFFu);
			}
		}

		return true;
	}

	bool CKBitmapData::ReadOldRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot) {
		// MARK: not supported because all of my work do not involve this function.
		return false;
	}

	void CKBitmapData::WriteSpecificFormatBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* slot, const CKBitmapProperties* savefmt) {
		// check image validation
		if (slot->IsValid()) {
			// get reader
			auto reader = DataHandlers::CKBitmapHandler::GetBitmapHandlerWrapper(savefmt->m_Ext, savefmt->m_ReaderGuid);
			if (reader != nullptr) {

				// prepare file data
				CKDWORD expectedSize = reader->SaveMemory(nullptr, slot, *savefmt);
				std::unique_ptr<CKBYTE[]> filebuf(new CKBYTE[expectedSize]);
				reader->SaveMemory(filebuf.get(), slot, *savefmt);

				// in original Virtools design, only save alpha data when raw data can not represent alpha data
				bool canSaveAlpha = reader->CanSaveAlpha();

				// write basic data
				// whether has transparent
				chk->WriteStruct(canSaveAlpha ? c_SpecificFmtNoTransparent : c_SpecificFmtHasTransparent);
				// write ext and guid
				chk->WriteBufferNoSize(savefmt->m_Ext.GetExt(), savefmt->m_Ext.GetSize());
				chk->WriteStruct(savefmt->m_ReaderGuid);
				
				// write file data len and self
				chk->WriteStruct(expectedSize);
				chk->WriteBufferNoSize(filebuf.get(), expectedSize);
				// free filebuf
				filebuf.reset();

				// write alpha if necessary
				if (!canSaveAlpha) {
					// prepare alpha list
					CKDWORD pixelCount = slot->GetPixelCount();
					std::unique_ptr<CKBYTE[]> alphabuf(new CKBYTE[pixelCount * VxMath::VxImageDescEx::FACTOR_SIZE]);
					VxMath::VxCopyStructure(
						pixelCount,
						alphabuf.get(),
						VxMath::VxImageDescEx::FACTOR_SIZE,
						VxMath::VxImageDescEx::FACTOR_SIZE,
						slot->GetImage() + 3 * VxMath::VxImageDescEx::FACTOR_SIZE,	// move to A factor
						VxMath::VxImageDescEx::PIXEL_SIZE
					);

					// check whether alpha are the same value
					bool isSameAlpha = true;
					CKDWORD sameAlpha = 0;
					for (CKDWORD i = 0; i < pixelCount; ++i) {
						if (i == 0) {
							sameAlpha = static_cast<CKDWORD>(alphabuf[i * VxMath::VxImageDescEx::FACTOR_SIZE]);
						} else {
							if (sameAlpha != static_cast<CKDWORD>(alphabuf[i * VxMath::VxImageDescEx::FACTOR_SIZE])) {
								isSameAlpha = false;
								break;
							}
						}
					}

					// write alpha count
					// MARK: originally, the written value is how many different alpha value in this image.
					// so obviously its range is from 1 - 255 and 1 mean all alpha are the same value.
					// thus we write 2 here because Virtools do not depend this value in reader, 
					// we only just let virtools to know there is a alpha list.
					chk->WriteStruct(isSameAlpha ? 1 : 2);
					if (isSameAlpha) {
						chk->WriteStruct(sameAlpha);
					} else {
						chk->WriteBuffer(alphabuf.get(), pixelCount * VxMath::VxImageDescEx::FACTOR_SIZE);
					}

					// free alphabuf
					alphabuf.reset();

				}

				// free reader
				reader.reset();

				// explicitly return to skip fallback
				return;
			}
		} 

		// fallback
		// if image is invalid, or get reader failed.
		// write a zero
		chk->WriteStruct(0);

	}

	void CKBitmapData::WriteRawBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* slot) {
		// check image validation
		if (slot->IsValid()) {
			// write basic data
			chk->WriteStruct(32);	// write bytePerPixel. always is 32 bpp.
			chk->WriteStruct(slot->GetWidth());
			chk->WriteStruct(slot->GetHeight());
			// alpha, red, green, blue mask, we always use ARGB8888 fmt.
			chk->WriteStruct(0xFF000000);
			chk->WriteStruct(0x00FF0000);
			chk->WriteStruct(0x0000FF00);
			chk->WriteStruct(0x000000FF);

			// write bufopt
			// write 0 to indicate there is no shitty jpeg compression
			// see ReadRawBitmap for more info.
			chk->WriteStruct(0);

			// MARK: originally there is a bunch of code to convert non-ARGB data into ARGB fmt.
			// but in my project design, all data is ARGB fmt, so we can simply write them.
			// all convertion code removed.

			// create 4 channel buf
			// we always write alpha channel data.
			CKDWORD pixelCount = slot->GetPixelCount();
			CKDWORD bufSize = pixelCount * VxMath::VxImageDescEx::FACTOR_SIZE;
			std::unique_ptr<CKBYTE[]> redbuf(new CKBYTE[bufSize]);
			std::unique_ptr<CKBYTE[]> greenbuf(new CKBYTE[bufSize]);
			std::unique_ptr<CKBYTE[]> bluebuf(new CKBYTE[bufSize]);
			std::unique_ptr<CKBYTE[]> alphabuf(new CKBYTE[bufSize]);

			// copy channel data
			// copy a
			VxMath::VxCopyStructure(
				pixelCount,
				alphabuf.get(),
				VxMath::VxImageDescEx::FACTOR_SIZE,
				VxMath::VxImageDescEx::FACTOR_SIZE,
				slot->GetImage() + (3 * VxMath::VxImageDescEx::FACTOR_SIZE),
				VxMath::VxImageDescEx::PIXEL_SIZE
			);
			// copy r
			VxMath::VxCopyStructure(
				pixelCount,
				redbuf.get(),
				VxMath::VxImageDescEx::FACTOR_SIZE,
				VxMath::VxImageDescEx::FACTOR_SIZE,
				slot->GetImage() + (2 * VxMath::VxImageDescEx::FACTOR_SIZE),
				VxMath::VxImageDescEx::PIXEL_SIZE
			);
			// copy g
			VxMath::VxCopyStructure(
				pixelCount,
				greenbuf.get(),
				VxMath::VxImageDescEx::FACTOR_SIZE,
				VxMath::VxImageDescEx::FACTOR_SIZE,
				slot->GetImage() + (1 * VxMath::VxImageDescEx::FACTOR_SIZE),
				VxMath::VxImageDescEx::PIXEL_SIZE
			);
			// copy b
			VxMath::VxCopyStructure(
				pixelCount,
				bluebuf.get(),
				VxMath::VxImageDescEx::FACTOR_SIZE,
				VxMath::VxImageDescEx::FACTOR_SIZE,
				slot->GetImage() + (0 * VxMath::VxImageDescEx::FACTOR_SIZE),
				VxMath::VxImageDescEx::PIXEL_SIZE
			);

			// write 4 buf
			chk->WriteBuffer(bluebuf.get(), bufSize);
			chk->WriteBuffer(greenbuf.get(), bufSize);
			chk->WriteBuffer(redbuf.get(), bufSize);
			chk->WriteBuffer(alphabuf.get(), bufSize);

			// free 4 buf
			redbuf.reset();
			greenbuf.reset();
			bluebuf.reset();
			alphabuf.reset();
			
			// explicitly return to skip fallback
			return;
		}

		// fallback
		// if image is invalid, write a zero
		chk->WriteStruct(0);
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
						XContainer::NSXBitArray::Set(hasReadSlot, i);
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
					XContainer::NSXBitArray::Set(hasReadSlot, i);

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
					XContainer::NSXBitArray::Set(hasReadSlot, i);
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
				XContainer::XString filename;
				chunk->ReadString(filename);
				if (filename.empty()) continue;
				
				bool isNotLoaded = (i >= hasReadSlot.size()) || (!XContainer::NSXBitArray::IsSet(hasReadSlot, i));
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
		// MARK: movie is not implemented here.
		
		return true;
	}

	bool CKBitmapData::DumpToChunk(CKStateChunk* chunk, CKFileVisitor* file, const CKBitmapDataWriteIdentifiers& identifiers) {
		// MARK: movie is not supported in this project in plan.
		
		// resolve save fmt and save opt
		CK_TEXTURE_SAVEOPTIONS saveopt = GetSaveOptions();
		if (saveopt == CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_USEGLOBAL) {
			saveopt = m_Context->GetGlobalImagesSaveOptions();
		}
		CKBitmapProperties savefmt(GetSaveFormat());
		if (GetSaveOptions() == CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_USEGLOBAL) {
			savefmt = m_Context->GetGlobalImagesSaveFormat();
		}

		// filter external / original files
		// the slot can not fulfill extrnal saving requirement will be written in raw data
		CKDWORD slotcount = GetSlotCount();
		XContainer::XBitArray validExternalSavingSlot;
		if (saveopt == CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_EXTERNAL) {
			for (CKDWORD i = 0; i < slotcount; ++i) {
				if (GetSlotFileName(i) == nullptr) {
					saveopt = CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_RAWDATA;
				} else {
					XContainer::NSXBitArray::Set(validExternalSavingSlot, i);
				}
			}
		}
		if (saveopt == CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_INCLUDEORIGINALFILE) {
			for (CKDWORD i = 0; i < slotcount; ++i) {
				if (file->AddSavedFile(GetSlotFileName(i))) {
					XContainer::NSXBitArray::Set(validExternalSavingSlot, i);
				} else {
					saveopt = CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_RAWDATA;
				}
			}
		}

		// save data
		if (saveopt == CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_RAWDATA) {
			// save as raw data
			chunk->WriteIdentifierDword(identifiers.m_RawData);
			chunk->WriteStruct(slotcount);

			VxMath::VxImageDescEx invalidDesc;
			for (CKDWORD i = 0; i < slotcount; ++i) {
				VxMath::VxImageDescEx* thisimg = GetImageDesc(i);
				if (XContainer::NSXBitArray::IsSet(validExternalSavingSlot, i) || !thisimg->IsValid()) {
					// if this slot can save as external, pass a invalid desc to writer
					// or image is invalid, simply write it as invalid one.
					WriteRawBitmap(chunk, &invalidDesc);
				} else {
					// otherwise, pass the real slot data
					// do upside down first as reader done
					VxMath::VxImageDescEx upsidedown(thisimg->GetWidth(), thisimg->GetHeight());
					VxMath::VxDoBlitUpsideDown(thisimg, &upsidedown);
					WriteRawBitmap(chunk, &upsidedown);
				}
			}

		}
		if (saveopt == CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_IMAGEFORMAT) {
			// save as special format
			chunk->WriteIdentifierDword(identifiers.m_SpecificFormat);
			chunk->WriteStruct(slotcount);

			// write width, height and bpp
			chunk->WriteStruct(GetWidth());
			chunk->WriteStruct(GetHeight());
			chunk->WriteStruct(32);

			// write slot one by one
			for (CKDWORD i = 0; i < slotcount; ++i) {
				WriteSpecificFormatBitmap(chunk, GetImageDesc(i), &savefmt);
			}

		}

		// write filename
		{
			chunk->WriteIdentifierDword(identifiers.m_FileNames);
			chunk->WriteStruct(slotcount);

			XContainer::XString filename;
			for (CKDWORD i = 0; i < slotcount; ++i) {
				XContainer::NSXString::FromCKSTRING(filename, GetSlotFileName(i));
				m_Context->GetPathManager()->GetFileName(filename);
				chunk->WriteString(filename);
			}
		}

		return true;
	}

#pragma endregion

#pragma region Slot Functions

	void CKBitmapData::SetSlotCount(CKDWORD count) {
		m_Slots.resize(count);

		if (count == 0) {
			yycc::cenum::add(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_INVALID);
		}
	}

	CKDWORD CKBitmapData::GetSlotCount() const {
		return static_cast<CKDWORD>(m_Slots.size());
	}

	void CKBitmapData::SetCurrentSlot(CKDWORD slot) {
		if (slot >= m_Slots.size()) return;

		m_CurrentSlot = slot;

		// NOTE: idk what the fuck this is. just interpter the IDA decompiled code.
		if (yycc::cenum::has(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_CUBEMAP)) {
			yycc::cenum::add(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_FORCERESTORE);
		}
	}

	CKDWORD CKBitmapData::GetCurrentSlot() const {
		return m_CurrentSlot;
	}

	bool CKBitmapData::CreateImage(CKDWORD Width, CKDWORD Height, CKDWORD Slot) {
		if (Slot >= m_Slots.size()) return false;

		CKBitmapSlot& slotdata = m_Slots[Slot];
		slotdata.m_ImageData.CreateImage(Width, Height);
		VxMath::VxDoAlphaBlit(&slotdata.m_ImageData, 0xFFu);
		return true;
	}

	bool CKBitmapData::LoadImage(CKSTRING filename, CKDWORD slot) {
		if (filename == nullptr) return false;
		if (slot >= m_Slots.size()) return false;

		// get extension of file. then get corresponding reader
		XContainer::XString ext(filename);
		m_Context->GetPathManager()->GetExtension(ext);
		auto reader = DataHandlers::CKBitmapHandler::GetBitmapHandlerWrapper(
			CKFileExtension(ext.c_str()),
			CKGUID()
		);
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
			XContainer::XString ext(filename);
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

	bool CKBitmapData::SetSlotFileName(CKDWORD slot, CKSTRING filename) {
		if (slot >= m_Slots.size()) return false;
		if (filename == nullptr) return false;
		m_Slots[slot].m_FileName = filename;
		return true;
	}

	CKSTRING CKBitmapData::GetSlotFileName(CKDWORD slot) const {
		if (slot >= m_Slots.size()) return nullptr;

		// return nullptr if corresponding filename is empty to indicate there is no binding filename
		return XContainer::NSXString::ToCKSTRING(m_Slots[slot].m_FileName);
	}

	CKDWORD CKBitmapData::GetWidth() const {
		for (auto& slot : m_Slots) {
			if (slot.m_ImageData.IsValid()) {
				return slot.m_ImageData.GetWidth();
			}
		}
		return 0;
	}

	CKDWORD CKBitmapData::GetHeight() const {
		for (auto& slot : m_Slots) {
			if (slot.m_ImageData.IsValid()) {
				return slot.m_ImageData.GetHeight();
			}
		}
		return 0;
	}

#pragma endregion

#pragma region Not important variable visitor

	CK_BITMAPDATA_FLAGS CKBitmapData::GetBitmapFlags() const {
		return m_BitmapFlags;
	}

	void CKBitmapData::SetCubeMap(bool is_cube) {
		// MARK: originally we should resize solot to 6 exactly.
		// but we decide split the flag settings and slot. 
		// User should set slot count manually.
		if (is_cube) {
			yycc::cenum::add(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_CUBEMAP);
		} else {
			yycc::cenum::remove(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_CUBEMAP);
		}
	}

	bool CKBitmapData::IsCubeMap() const {
		return yycc::cenum::has(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_CUBEMAP);
	}

	const CKBitmapProperties& CKBitmapData::GetSaveFormat() const {
		return m_SaveProperties;
	}

	void CKBitmapData::SetSaveFormat(const CKBitmapProperties& props) {
		m_SaveProperties = props;
	}

	CK_TEXTURE_SAVEOPTIONS CKBitmapData::GetSaveOptions() const {
		return m_SaveOptions;
	}

	void CKBitmapData::SetSaveOptions(CK_TEXTURE_SAVEOPTIONS opts) {
		m_SaveOptions = opts;
	}

	void CKBitmapData::SetTransparent(bool Transparency) {
		if (Transparency) {
			yycc::cenum::add(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_TRANSPARENT);
		} else {
			yycc::cenum::remove(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_TRANSPARENT);
		}
	}

	bool CKBitmapData::IsTransparent() const {
		return yycc::cenum::has(m_BitmapFlags, CK_BITMAPDATA_FLAGS::CKBITMAPDATA_TRANSPARENT);
	}

	void CKBitmapData::SetTransparentColor(CKDWORD col) {
		// MARK: originally, we should set CK_BITMAPDATA_FLAGS::CKBITMAPDATA_TRANSPARENT for m_BitmapFlags.
		// but in CKTexture::Read(), I don't know why Virtools use this fucking design. Virtools split the enable and color data self,
		// and always write Transparent Color no matter whether Transparent enabled.
		// so i split transparent enable function and transparent color. User should manually enable transparent if they needed. 
		// Just set transparent color will not enable transparent automatically.
		m_TransColor = col;
	}

	CKDWORD CKBitmapData::GetTransparentColor() const {
		return m_TransColor;
	}

	void CKBitmapData::SetPickThreshold(CKDWORD threshold) {
		m_PickThreshold = threshold;
	}

	CKDWORD CKBitmapData::GetPickThreshold() const {
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
