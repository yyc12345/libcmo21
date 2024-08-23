#include "CKTexture.hpp"
#include "../CKStateChunk.hpp"
#include "../CKContext.hpp"
#include "../MgrImpls/CKPathManager.hpp"

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
#pragma pack(4)
	struct FakeBitmapProperties {
		CKINT m_Size;
		struct {
			// fake CKGUID
			CKDWORD d1, d2;
		}m_ReaderGuid;
		struct {
			// fake CKFileExtension
			char m_Data[4];
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
#pragma pack()

	CKTexture::CKTexture(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKBeObject(ctx, ckid, name),
		m_ImageHost(ctx),
		m_VideoFormat(VxMath::VX_PIXELFORMAT::_16_ARGB1555), m_UseMipMap(false), m_MipmapImages() {}

	CKTexture::~CKTexture() {}

	bool CKTexture::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKBeObject::Save(chunk, file, flags);
		if (!suc) return false;

		// save base image
		suc = m_ImageHost.DumpToChunk(chunk, file, CKBitmapDataWriteIdentifiers {
			.m_SpecificFormat = static_cast<CKDWORD>(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXREADER),
			.m_RawData = static_cast<CKDWORD>(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXCOMPRESSED),
			.m_FileNames = static_cast<CKDWORD>(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXFILENAMES),
			.m_MovieFileName = static_cast<CKDWORD>(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXAVIFILENAME)
		});
		if (!suc) return false;

		// write main properties
		{
			// write ident
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_OLDTEXONLY);

			// prepare mix data. see Read for more info about the struct of this mix data
			CKDWORD mixdata = 0;
			// save options
			mixdata |= static_cast<CKDWORD>(m_ImageHost.GetSaveOptions()) & 0xFF;
			mixdata <<= 8;
			// mix flags
			CKDWORD mixflags = 0;
			if (m_ImageHost.IsTransparent()) {
				mixflags |= 0x1;
			}
			if (m_VideoFormat != VxMath::VX_PIXELFORMAT::UNKNOWN_PF) {
				mixflags |= 0x2;
			}
			if (m_ImageHost.IsCubeMap()) {
				mixflags |= 0x4;
			}
			mixdata |= mixflags & 0xFF;
			mixdata <<= 8;
			// mipmap
			mixdata |= (IsUseMipmap() ? 0xFF : 0);

			// write mix data
			chunk->WriteStruct(mixdata);

			// transparent color
			chunk->WriteStruct(m_ImageHost.GetTransparentColor());
			// current slot
			if (m_ImageHost.GetSlotCount() > 1) {
				chunk->WriteStruct(m_ImageHost.GetCurrentSlot());
			}
			// video fmt
			if (m_VideoFormat != VxMath::VX_PIXELFORMAT::UNKNOWN_PF) {
				chunk->WriteStruct(m_VideoFormat);
			}

		}

		// mipmap
		if (GetMipmapLevel() != 0) {
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_USERMIPMAP);

			// write mipmap level
			chunk->WriteStruct(GetMipmapLevel());

			// write data
			for (auto& level : m_MipmapImages) {
				if (level.IsValid()) {
					// do upside down and write
					VxMath::VxImageDescEx upsidedown(level.GetWidth(), level.GetHeight());
					VxMath::VxDoBlitUpsideDown(&level, &upsidedown);
					CKBitmapData::WriteRawBitmap(chunk, &upsidedown);
				} else {
					// write it directly
					CKBitmapData::WriteRawBitmap(chunk, &level);
				}
			}
		}

		// pick threshold
		if (m_ImageHost.GetPickThreshold() != 0) {
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_PICKTHRESHOLD);
			chunk->WriteStruct(m_ImageHost.GetPickThreshold());
		}

		// bitmap properties
		{
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXSAVEFORMAT);

			// prepare a fake one
			FakeBitmapProperties props;
			const CKBitmapProperties& realprops = m_ImageHost.GetSaveFormat();

			// setup fake self
			props.m_Size = CKSizeof(props);
			props.m_Data = 0;

			// setup fake VxImageDescEx
			props.m_Format.Size = CKSizeof(props.m_Format);
			props.m_Format.Flags = static_cast<CKDWORD>(VxMath::VX_PIXELFORMAT::_32_ARGB8888);
			props.m_Format.Width = m_ImageHost.GetWidth();
			props.m_Format.Height = m_ImageHost.GetHeight();
			props.m_Format.BytesPerLine = VxMath::VxImageDescEx::PixelSize * props.m_Format.Width;
			props.m_Format.BitsPerPixel = 32;

			props.m_Format.RedMask = 0x00FF0000;
			props.m_Format.GreenMask = 0x0000FF00;
			props.m_Format.BlueMask = 0x000000FF;
			props.m_Format.AlphaMask = 0xFF000000;

			props.m_Format.BytesPerColorEntry = 0;
			props.m_Format.ColorMapEntries = 0;

			props.m_Format.ColorMap = 0;
			props.m_Format.Image = 0;

			// setup ext and guid
			props.m_ReaderGuid.d1 = realprops.m_ReaderGuid.d1;
			props.m_ReaderGuid.d2 = realprops.m_ReaderGuid.d2;
			std::string ext;
			m_Context->GetOrdinaryString(realprops.m_Ext.GetExt(), ext);
			std::memcpy(
				props.m_Ext.m_Data,
				ext.c_str(),
				std::min(CKSizeof(props.m_Ext.m_Data) - CKDWORD_C(1), static_cast<CKDWORD>(ext.size()))
			);

			// write fake one
			chunk->WriteBuffer(&props, CKSizeof(props));
		}

		chunk->SetClassId(CK_CLASSID::CKCID_TEXTURE);
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
			// MARK: there is a patch for virtools 2.1 implement.
			// CK_STATESAVE_TEXONLY is noy valid in 2.1 but we add it for cpmpatibility reason.
			if (chunk->SeekIdentifierAndReturnSize(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_OLDTEXONLY, &fmtbytesize) ||
				chunk->SeekIdentifierAndReturnSize(CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXONLY, &fmtbytesize)) {
				// for mid data:
				// HIGH >>> 0xFF (blank) 0xFF (save options) 0xFF (transparent + movie info + video fmt) 0xFF (mip map) <<< LOW
				// for mixed flags:
				// HIGH >>> 1(blank) 1(cubemap) 1(has video fmt) 1(is transparent)
				CKDWORD mixdata;
				chunk->ReadStruct(mixdata);
				// set mipmap
				UseMipmap(mixdata & 0xFF);
				mixdata >>= 8;
				// mix flags
				CKDWORD mixflags = mixdata & 0xFF;
				mixdata >>= 8;
				m_ImageHost.SetTransparent(mixflags & 0x1);
				bool hasVideoFmt = mixflags & 0x2;
				m_ImageHost.SetCubeMap(mixflags & 0x4);
				// save options
				m_ImageHost.SetSaveOptions(static_cast<CK_TEXTURE_SAVEOPTIONS>(mixdata & 0xFF));
				mixdata >>= 8;
				
				// set current slot, transparent color, and video format.
				CKDWORD currentSlot, transColor;
				fmtbytesize -= CKSizeof(CKDWORD);
				switch (fmtbytesize) {
					case (3 * CKSizeof(CKDWORD)):
						chunk->ReadStruct(transColor);
						m_ImageHost.SetTransparentColor(transColor);
						chunk->ReadStruct(currentSlot);
						m_ImageHost.SetCurrentSlot(currentSlot);
						chunk->ReadStruct(m_VideoFormat);
						break;
					case (2 * CKSizeof(CKDWORD)):
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
					case (CKSizeof(CKDWORD)):
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
				SetMipmapLevel(mipmapCount);

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
					FakeBitmapProperties* props = static_cast<FakeBitmapProperties*>(buf.get());

					// get utf8 extension
					XContainer::XString ext;
					m_Context->GetUTF8String(props->m_Ext.m_Data, ext);

					// get my bitmap prop
					CKBitmapProperties myprops(
						CKGUID(props->m_ReaderGuid.d1, props->m_ReaderGuid.d2),
						ext.c_str()
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

#pragma region Visitor

	CKBitmapData& CKTexture::GetUnderlyingData() {
		return m_ImageHost;
	}

	bool CKTexture::LoadImage(CKSTRING filename, CKDWORD slot) {
		// check file name
		if (filename == nullptr) return false;
		// check slot
		if (slot >= m_ImageHost.GetSlotCount()) return false;

		// resolve file name first
		XContainer::XString filepath;
		XContainer::NSXString::FromCKSTRING(filepath, filename);
		if (!m_Context->GetPathManager()->ResolveFileName(filepath)) return false;

		// try loading image
		if (!m_ImageHost.LoadImage(XContainer::NSXString::ToCKSTRING(filepath), slot)) return false;

		// sync file name
		return m_ImageHost.SetSlotFileName(slot, XContainer::NSXString::ToCKSTRING(filepath));
	}

	bool CKTexture::IsUseMipmap() const {
		return m_UseMipMap;
	}

	void CKTexture::UseMipmap(bool isUse) {
		m_UseMipMap = isUse;

		if (!m_UseMipMap) {
			m_MipmapImages.clear();
		}
	}

	CKDWORD CKTexture::GetMipmapLevel() const {
		return static_cast<CKDWORD>(m_MipmapImages.size());
	}

	void CKTexture::SetMipmapLevel(CKDWORD level) {
		m_MipmapImages.resize(level);
	}

	VxMath::VxImageDescEx* CKTexture::GetMipmapLevelData(CKDWORD level) {
		if (!m_UseMipMap || level >= m_MipmapImages.size()) return nullptr;
		return &m_MipmapImages[level];
	}

	VxMath::VX_PIXELFORMAT CKTexture::GetVideoFormat() const {
		return m_VideoFormat;
	}

	void CKTexture::SetVideoFormat(VxMath::VX_PIXELFORMAT fmt) {
		m_VideoFormat = fmt;
	}

#pragma endregion


}
