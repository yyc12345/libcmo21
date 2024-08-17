#pragma once

#include "../VTInternal.hpp"

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
		YYCC_DEF_CLS_COPY_MOVE(CKBitmapSlot);

		VxMath::VxImageDescEx m_ImageData;
		XContainer::XString m_FileName;
	};

	class CKBitmapData {
	public:
		CKBitmapData(CKContext* ctx);
		~CKBitmapData();
		YYCC_DEL_CLS_COPY_MOVE(CKBitmapData);

#pragma region RW Funcs

		static bool ReadSpecificFormatBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot);
		static bool ReadRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot);
		static bool ReadOldRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* slot);
		static void WriteSpecificFormatBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* slot, const CKBitmapProperties* savefmt);
		static void WriteRawBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* slot);

		bool ReadFromChunk(CKStateChunk* chunk, CKFileVisitor* file, const CKBitmapDataReadIdentifiers& identifiers);
		bool DumpToChunk(CKStateChunk* chunk, CKFileVisitor* file, const CKBitmapDataWriteIdentifiers& identifiers);

#pragma endregion

#pragma region Slot funcs

		void SetSlotCount(CKDWORD count);
		CKDWORD GetSlotCount() const;
		void SetCurrentSlot(CKDWORD slot);
		CKDWORD GetCurrentSlot() const;

		/**
		 * @brief Create a black image with full alpha in specified slot.
		 * @param Width[in] Image width
		 * @param Height[in] Image height
		 * @param Slot[in] The slot placing image.
		 * @return True if creating success.
		*/
		bool CreateImage(CKDWORD Width, CKDWORD Height, CKDWORD Slot);
		/**
		 * @brief Load image into specified slot.
		 * @param filename[in] The file name of loading image.
		 * @param slot[in] The slot placing loaded image.
		 * @return True if load successfully.
		*/
		bool LoadImage(CKSTRING filename, CKDWORD slot);
		/**
		 * @brief Save image for specified slot.
		 * @param filename[in] The file name of saving image.
		 * @param slot[in] The slot will be saved.
		 * @param isForceThisFmt[in] True to use this class specified format to save image. Otherwise use the format evaluated by the image file name.
		 * @return True if success.
		*/
		bool SaveImage(CKSTRING filename, CKDWORD slot, bool isForceThisFmt = false);
		/**
		 * @brief Get specified slot image descriptor.
		 * @param slot[in] The slot to get.
		 * @return The descriptor. nullptr if failed.
		*/
		VxMath::VxImageDescEx* GetImageDesc(CKDWORD slot);
		/**
		 * @brief Release specified slot image.
		 * @param slot[in] The slot to free.
		*/
		void ReleaseImage(CKDWORD slot);

		/**
		 * @brief Set associated file name for specified slot.
		 * @param slot[in] The slot to set.
		 * @param filename[in] The associated file name.
		*/
		bool SetSlotFileName(CKDWORD slot, CKSTRING filename);
		/**
		 * @brief Get associated file name for specified slot.
		 * @param slot[in] The slot to get.
		 * @return The file name. nullptr if failed.
		*/
		CKSTRING GetSlotFileName(CKDWORD slot) const;

		/**
		 * @brief Get first valid image's width.
		 * @return 0 if no valid image.
		*/
		CKDWORD GetWidth() const;
		/**
		 * @brief Get first valid image's height.
		 * @return 0 if no valid image
		*/
		CKDWORD GetHeight() const;

#pragma endregion

#pragma region Not important funcs

		CK_BITMAPDATA_FLAGS GetBitmapFlags() const;

		void SetCubeMap(bool is_cube);
		bool IsCubeMap() const;
		
		const CKBitmapProperties& GetSaveFormat() const;
		void SetSaveFormat(const CKBitmapProperties& props);
		CK_TEXTURE_SAVEOPTIONS GetSaveOptions() const;
		void SetSaveOptions(CK_TEXTURE_SAVEOPTIONS opts);
		/**
		@brief Enables or disables the color key transparency.
		@param Transparency[in] TRUE activates transparency, FALSE disables it.
		@remark
			+ 0x00000000 (black) is the default transparent color.
			+ Setting on the transparency and a transparent color automatically 
			updates the alpha channel so that pixel with the transparent color have 
			a 0 alpha value.
		@see IsTransparent, SetTranparentColor
		*/
		void SetTransparent(bool Transparency);
		/**
		@brief Returns whether color keyed transparency is enabled.
		@return TRUE if color keying is enabled.
		@see SetTransparent
		*/
		bool IsTransparent() const;
		/**
		@brief Sets the transparent color.
		@param Color[in] A 32 bit ARGB transparent color.
		@remark
		+ 0x00000000 (black) is the default transparent color.
		+ Setting on the transparency and a transparent color automatically 
		updates the alpha channel so that pixel with the transparent color have 
		a 0 alpha value.
		@see GetTranparentColor, SetTransparent
		*/
		void SetTransparentColor(CKDWORD col);
		/**
		@brief Returns the transparent color.
		@return A 32 bit ARGB transparent color.
		@remark
			+ 0x00000000 (black) is the default transparent color.
		@see SetTranparentColor, SetTransparent
		*/
		CKDWORD GetTransparentColor() const;
		/**
		@brief Sets pick threshold value.
		@param pt[in] Pick threshold value to be set.
		@remark
			+ The pick threshold is used when picking object with 
			transparent textures.
			+ It is the minimum value for alpha component
			below which picking is not valid.So this value is supposed to be in the range 0..255
			and the default value 0 means the picking is always valid.
			+ But if a value >0 is used and the texture use transparency (some pixels of the bitmap will have
			alpha component of 0) an object will not be picked on its transparent part.
		@see CKRenderContext::Pick
		*/
		void SetPickThreshold(CKDWORD threshold);
		/**
		@brief Gets pick threshold value.
		@return Pick threshold value
		*/
		CKDWORD GetPickThreshold() const;

#pragma endregion

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
