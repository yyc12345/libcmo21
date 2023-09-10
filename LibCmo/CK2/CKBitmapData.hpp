#pragma once

#include "../VTAll.hpp"

namespace LibCmo::CK2 {

	struct CKBitmapDataReadIdentifiers {
		CKDWORD m_SpecificFormat;	/**< CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXREADER(0x100000) in default. */
		CKDWORD m_RawData;	/**< CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXCOMPRESSED(0x20000) in default. */
		CKDWORD m_OldRawData;	/**< CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXBITMAPS(0x4000) in default. */

		CKDWORD m_FileNames;	/**< CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXFILENAMES(0x10000) in default. */
		CKDWORD m_MovieFileName;	/**< CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXAVIFILENAME(0x1000) in default. */
	};

	struct CKBitmapDataWriteIdentifiers {
		CKDWORD m_SpecificFormat;	/**< CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXREADER(0x100000) in default. */
		CKDWORD m_RawData;	/**< CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXCOMPRESSED(0x20000) in default. */

		CKDWORD m_FileNames;	/**< CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXFILENAMES(0x10000) in default. */
		CKDWORD m_MovieFileName;	/**< CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXAVIFILENAME(0x1000) in default. */
	};

	class CKBitmapSlot {
	public:
		CKBitmapSlot() :
			m_ImageData(nullptr), m_FileName() {}
		~CKBitmapSlot() {
			FreeImage();
		}

		void CreateImage(CKDWORD Width, CKDWORD Height) {
			FreeImage();
			m_ImageData = new VxMath::VxImageDescEx(Width, Height);
		}
		void FreeImage() {
			if (m_ImageData != nullptr) {
				delete m_ImageData;
				m_ImageData = nullptr;
			}
		}

		VxMath::VxImageDescEx* m_ImageData;
		XContainer::XString m_FileName;
	};

	class CKBitmapData {
	public:
		CKBitmapData(CKContext* ctx);
		~CKBitmapData();
		LIBCMO_DISABLE_COPY_MOVE(CKBitmapData);

		static bool ReadSpecificFormatBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* desc);
		static bool ReadRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* desc);
		static bool ReadOldRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* desc);
		static void WriteSpecificFormatBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* desc);
		static void WriteRawBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* desc);

		bool ReadFromChunk(CKStateChunk* chunk, CKFileVisitor* file, const CKBitmapDataReadIdentifiers& identifiers);
		bool DumpToChunk(CKStateChunk* chunk, CKFileVisitor* file, const CKBitmapDataWriteIdentifiers& identifiers);

		void SetSlotCount(CKDWORD count);
		CKDWORD GetSlotCount();
		void SetCurrentSlot(CKDWORD slot);
		CKDWORD GetCurrentSlot();

		void CreateImage(CKDWORD Width, CKDWORD Height, CKDWORD Slot);
		bool LoadImage(CKSTRING filename, CKDWORD slot);
		bool SaveImage(CKSTRING filename, CKDWORD slot);
		VxMath::VxImageDescEx* GetImageDesc(CKDWORD slot);
		void ReleaseImage(CKDWORD slot);

		void SetSlotFileName(CKDWORD slot, CKSTRING filename);
		CKSTRING GetSlotFileName(CKDWORD slot);

		const CKBitmapProperties& GetSaveProperties();
		void SetSaveProperties(const CKBitmapProperties& props);
		CK_TEXTURE_SAVEOPTIONS GetSaveOptions();
		void SetSaveOptions(CK_TEXTURE_SAVEOPTIONS opts);
		VxMath::VX_PIXELFORMAT GetDesiredVideoFormat();
		void SetDesiredVideoFormat(VxMath::VX_PIXELFORMAT fmt);

		/**
		Summary: Enables or disables the color key transparency.

		Arguments:
			Transparency: TRUE activates transparency, FALSE disables it.
		Remarks:
			+ 0x00000000 (black) is the default transparent color.
			+ Setting on the transparency and a transparent color automatically 
			updates the alpha channel so that pixel with the transparent color have 
			a 0 alpha value.
		See also: IsTransparent,SetTranparentColor
		*/
		void SetTransparent(bool Transparency);
		/**
		Summary: Returns whether color keyed transparency is enabled.

		Return Value:
			TRUE if successful.
		Arguments:
			Transparency: TRUE activates transparency, FALSE disables it.
		Return Value:
			TRUE if color keying is enabled.
		See also: IsTransparent
		*/
		bool IsTransparent();
		/**
		Summary: Returns the transparent color.
		Return Value:
			Color: A 32 bit ARGB transparent color.
		Remarks:
			+ 0x00000000 (black) is the default transparent color.

		See also: SetTranparentColor,SetTransparent
		*/
		CKDWORD GetTransparentColor();
		/**
		Summary: Sets the transparent color.
		Arguments:
			Color: A 32 bit ARGB transparent color.
		Remarks:
		+ 0x00000000 (black) is the default transparent color.
		+ Setting on the transparency and a transparent color automatically 
		updates the alpha channel so that pixel with the transparent color have 
		a 0 alpha value.

		See also: GetTranparentColor,SetTransparent
		*/
		void SetTransparentColor(CKDWORD col);

	protected:
		CKContext* m_Context;
		XContainer::XArray<CKBitmapSlot> m_Slots;
		CKDWORD m_CurrentSlot;
		CKINT m_PickThreshold;
		CKDWORD m_BitmapFlags;
		CKDWORD m_TransColor;

		CKBitmapProperties m_SaveProperties;
		CK_TEXTURE_SAVEOPTIONS m_SaveOptions;
		VxMath::VX_PIXELFORMAT m_DesiredVideoFormat;
	};

}
