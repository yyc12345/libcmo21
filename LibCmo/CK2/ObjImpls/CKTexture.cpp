#include "CKTexture.hpp"
#include "../CKStateChunk.hpp"

namespace LibCmo::CK2::ObjImpls {

	/**
	 * @brief A fake struct define.
	 * This define is served for a buffer read / write in CKTexture.
	 * Because Virtools directly write a raw struct into file,
	 * and our defines are different with Virtools.
	 * So we need create a fake struct.
	 * @remark
	 * All pointers should translate to DWORD(32 bit) for platform independent.
	 * Otherwise this struct may be corrupted in x64 platform because pointer is QWORD in x64.
	*/
	struct FakeBitmapProperties {
		CKINT m_Size;
		struct {
			// fake CKGUID
			CKDWORD d1, d2;
		}m_ReaderGuid;
		struct {
			// fake CKFileExtension
			CKCHAR m_Data[4];
		}m_Ext;
		struct {
			// fake VxImageDescEx
			CKINT Size; ///< Size of the structure
			CKDWORD Flags; ///< Reserved for special formats (such as compressed ) 0 otherwise

			CKINT Width; ///< Width in pixel of the image
			CKINT Height; ///< Height in pixel of the image
			union {
				CKINT BytesPerLine; ///< Pitch (width in bytes) of the image
				CKINT TotalImageSize; ///< For compressed image (DXT1...) the total size of the image
			};
			CKINT BitsPerPixel; ///< Number of bits per pixel
			union {
				CKDWORD RedMask; ///< Mask for Red component
				CKDWORD BumpDuMask; ///< Mask for Bump Du component
			};
			union {
				CKDWORD GreenMask; ///< Mask for Green component	
				CKDWORD BumpDvMask; ///< Mask for Bump Dv component
			};
			union {
				CKDWORD BlueMask; ///< Mask for Blue component
				CKDWORD BumpLumMask; ///< Mask for Luminance component

			};
			CKDWORD AlphaMask; ///< Mask for Alpha component

			CKWORD BytesPerColorEntry; ///< ColorMap Stride
			CKWORD ColorMapEntries; ///< If other than 0 image is palletized

			/*CKBYTE**/CKPTR ColorMap; ///< Palette colors
			/*CKBYTE**/CKPTR Image; ///< Image
		}m_Format;
		/*void**/CKPTR m_Data;
	};

	CKTexture::CKTexture(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKBeObject(ctx, ckid, name),
		m_ImageHost(ctx),
		m_VideoFormat(VxMath::VX_PIXELFORMAT::_16_ARGB1555), m_UseMipMap(false), m_MipmapImages() {}

	CKTexture::~CKTexture() {}

	bool CKTexture::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKBeObject::Save(chunk, file, flags);
		if (!suc) return false;

		return true;
	}

	bool CKTexture::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CKBeObject::Load(chunk, file);
		if (!suc) return false;

		// read base image
		suc = m_ImageHost.ReadFromChunk(chunk, file, CKBitmapDataReadIdentifiers {
			.m_SpecificFormat = static_cast<CKDWORD>(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXREADER),
			.m_RawData = static_cast<CKDWORD>(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXCOMPRESSED),
			.m_OldRawData = static_cast<CKDWORD>(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXBITMAPS),
			.m_FileNames = static_cast<CKDWORD>(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXFILENAMES),
			.m_MovieFileName = static_cast<CKDWORD>(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXAVIFILENAME)
		});
		if (!suc) return false;

		if (chunk->GetDataVersion() < CK_STATECHUNK_DATAVERSION::CHUNK_MAJORCHANGE_VERSION) {
			// MARK: old data process. i don't want to process it anymore.
			// thus return false directly.
			return false;
		} else {
			CKDWORD fmtbytesize;
			if (chunk->SeekIdentifierAndReturnSize(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_OLDTEXONLY, &fmtbytesize)) {
				// 0xFF (blank) 0xFF (save options) 0xFF (transparent + movie info + video fmt) 0xFF (mip map)
				CKDWORD mixdata;
				chunk->ReadStruct(mixdata);
				m_UseMipMap = (mixdata & 0xFF);
				m_ImageHost.SetSaveOptions(static_cast<CK_TEXTURE_SAVEOPTIONS>((mixdata & 0xFF0000) >> 16));
				
				mixdata = mixdata & 0xFF00 >> 8;
				m_ImageHost.SetTransparent(mixdata & 0x1);
				bool hasVideoFmt = mixdata & 0x2;
				// MARK: I ignore 0x4 in there because it involve video.

				// set current slot, transparent color, and video format.
				CKDWORD currentSlot, transColor;
				fmtbytesize -= CKSizeof(CKDWORD);
				switch (fmtbytesize) {
					case (3 * sizeof(CKDWORD)):
						chunk->ReadStruct(transColor);
						m_ImageHost.SetTransparentColor(transColor);
						chunk->ReadStruct(currentSlot);
						m_ImageHost.SetCurrentSlot(currentSlot);
						chunk->ReadStruct(m_VideoFormat);
						break;
					case (2 * sizeof(CKDWORD)):
						if (m_ImageHost.GetSlotCount() <= 1 || !hasVideoFmt) {
							chunk->ReadStruct(transColor);
							m_ImageHost.SetTransparentColor(transColor);
						}
						if (m_ImageHost.GetSlotCount() > 1) {
							chunk->ReadStruct(currentSlot);
							m_ImageHost.SetCurrentSlot(currentSlot);
						}
						if (hasVideoFmt) {
							chunk->ReadStruct(m_VideoFormat);
						}
						break;
					case (sizeof(CKDWORD)):
						if (hasVideoFmt) {
							chunk->ReadStruct(m_VideoFormat);
						} else if (m_ImageHost.GetSlotCount() <= 1) {
							chunk->ReadStruct(transColor);
							m_ImageHost.SetTransparentColor(transColor);
						} else {
							chunk->ReadStruct(currentSlot);
							m_ImageHost.SetCurrentSlot(currentSlot);
						}
						break;
				}

			}

			// read mipmap
			if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_USERMIPMAP)) {
				CKDWORD mipmapCount;
				chunk->ReadStruct(mipmapCount);
				m_MipmapImages.resize(mipmapCount);

				for (CKDWORD i = 0; i < mipmapCount; ++i) {
					VxMath::VxImageDescEx cache;
					if (CKBitmapData::ReadRawBitmap(chunk, &cache)) {
						VxMath::VxDoBlitUpsideDown(&cache, &m_MipmapImages[i]);
					}
				}

			}

			// pick threshold
			if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_PICKTHRESHOLD)) {
				CKDWORD threshold;
				chunk->ReadStruct(threshold);
				m_ImageHost.SetPickThreshold(threshold);
			}

			// save properties
			if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXSAVEFORMAT)) {
				auto buf = chunk->ReadBufferWrapper();
				if (buf != nullptr) {
					FakeBitmapProperties* props = reinterpret_cast<FakeBitmapProperties*>(buf.get());

					CKBitmapProperties myprops(
						CKGUID(props->m_ReaderGuid.d1, props->m_ReaderGuid.d2),
						props->m_Ext.m_Data
					);
					m_ImageHost.SetSaveFormat(myprops);
				}
			}
			
		}

		// correct video format
		if (m_VideoFormat > VxMath::VX_PIXELFORMAT::_32_X8L8V8U8) {
			m_VideoFormat = VxMath::VX_PIXELFORMAT::_16_ARGB1555;
		}

		return true;
	}

}
