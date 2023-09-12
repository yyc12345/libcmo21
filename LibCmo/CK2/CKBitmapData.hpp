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
			m_ImageData(), m_FileName() {}
		~CKBitmapSlot() {}
		LIBCMO_DEFAULT_COPY_MOVE(CKBitmapSlot);

		VxMath::VxImageDescEx m_ImageData;
		XContainer::XString m_FileName;
	};

	class CKBitmapData {
	public:
		CKBitmapData(CKContext* ctx);
		~CKBitmapData();
		LIBCMO_DISABLE_COPY_MOVE(CKBitmapData);

		static bool ReadSpecificFormatBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot);
		static bool ReadRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot);
		static bool ReadOldRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot);
		static void WriteSpecificFormatBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* slot);
		static void WriteRawBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* slot);

		bool ReadFromChunk(CKStateChunk* chunk, CKFileVisitor* file, const CKBitmapDataReadIdentifiers& identifiers);
		bool DumpToChunk(CKStateChunk* chunk, CKFileVisitor* file, const CKBitmapDataWriteIdentifiers& identifiers);

		void SetSlotCount(CKDWORD count);
		CKDWORD GetSlotCount();
		void SetCurrentSlot(CKDWORD slot);
		CKDWORD GetCurrentSlot();

		void CreateImage(CKDWORD Width, CKDWORD Height, CKDWORD Slot);
		bool LoadImage(CKSTRING filename, CKDWORD slot);
		bool SaveImage(CKSTRING filename, CKDWORD slot, bool isForceThisFmt = false);
		VxMath::VxImageDescEx* GetImageDesc(CKDWORD slot);
		void ReleaseImage(CKDWORD slot);

		void SetSlotFileName(CKDWORD slot, CKSTRING filename);
		CKSTRING GetSlotFileName(CKDWORD slot);

		const CKBitmapProperties& GetSaveFormat();
		void SetSaveFormat(const CKBitmapProperties& props);
		CK_TEXTURE_SAVEOPTIONS GetSaveOptions();
		void SetSaveOptions(CK_TEXTURE_SAVEOPTIONS opts);

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
		Summary: Sets pick threshold value.
		Arguments:
			pt: Pick threshold value to be set.
		Remarks:
			+ The pick threshold is used when picking object with 
			transparent textures.
			+ It is the minimum value for alpha component
			below which picking is not valid.So this value is supposed to be in the range 0..255
			and the default value 0 means the picking is always valid.
			+ But if a value >0 is used and the texture use transparency (some pixels of the bitmap will have
			alpha component of 0) an object will not be picked on its transparent part.

		See Also: CKRenderContext::Pick
		*/
		void SetPickThreshold(CKDWORD threshold);
		/**
		Summary: Gets pick threshold value.
		*/
		CKDWORD GetPickThreshold();

	protected:
		CKContext* m_Context;
		XContainer::XArray<CKBitmapSlot> m_Slots;
		CKDWORD m_CurrentSlot;
		CKDWORD m_PickThreshold;
		CK_BITMAPDATA_FLAGS m_BitmapFlags;
		CKDWORD m_TransColor;

		CKBitmapProperties m_SaveProperties;
		CK_TEXTURE_SAVEOPTIONS m_SaveOptions;
	};

}