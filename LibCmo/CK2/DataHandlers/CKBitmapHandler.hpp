#pragma once

#include "../../VTAll.hpp"

namespace LibCmo::CK2::DataHandlers {

	/**
	 * The interface about processing bitmap data between raw data and specific data.
	 * This interface will be capable to converting specific bitmap data into raw ARGB8888 raw data,
	 * or convert them back.
	*/
	class CKBitmapHandler {
	public:
		CKBitmapHandler() {}
		virtual ~CKBitmapHandler() {}
		LIBCMO_DISABLE_COPY_MOVE(CKBitmapHandler);

		/**
		 * @brief Returns the current default bitmap options.
		 * @return Current default bitmap options
		*/
		virtual const CKBitmapProperties& GetBitmapDefaultProperties() = 0;

		/**
		@brief Loads a bitmap file.
		@return Returns true if successful.
		@param u8filename[in] The file ready to read.
		@param read_image[out] The pointer point to existed image desc which describe this image HW.
		@see ReadMemory
		*/
		virtual bool ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) = 0;
		/**
		@brief Loads a bitmap file stored in memory.
		@return Returns true if successful.
		@param memory[in] The pointer to memory.
		@param size[in] The size of memory.
		@param read_image[out] The pointer point to existed image desc which describe this image HW.
		@see ReadFile
		*/
		virtual bool ReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) = 0;
		/**
		@brief Saves a image to a file 
		@return Returns true if successful.
		@param u8filename[in] The file ready to write.
		@param write_image[in] The image will be written in file.
		@param codec_param[in] The written image format parameter.
		@see SaveMemory
		*/
		virtual bool SaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) = 0;
		/**
		@brief Saves an image into a memory block.
		@return Returns the number of written bytes if successful, 0 otherwise.
		@param memory[in] The memory where the image will be written. nullptr if you need a dry run to get how many bytes you need to allocate.
		@param write_image[in] The image will be written in file.
		@param codec_param[in] The written image format parameter.
		@remark
			+ You should call this method first to get how much bytes will be written.
			+ Then you allocate a proper buffer.
			+ Finally, call this method with your allocated buffer to get the result.
		@see SaveFile
		*/
		virtual CKDWORD SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) = 0;

	};

	class CKBitmapBMPHandler : public CKBitmapHandler {
	public:
		CKBitmapBMPHandler();
		virtual ~CKBitmapBMPHandler();
		LIBCMO_DISABLE_COPY_MOVE(CKBitmapBMPHandler);

		virtual const CKBitmapProperties& GetBitmapDefaultProperties() override;
		virtual bool ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) override;
		virtual bool ReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) override;
		virtual bool SaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) override;
		virtual CKDWORD SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) override;

	protected:
		CKBitmapProperties c_DefaultProp;
	};
	
	class CKBitmapTGAHandler : public CKBitmapHandler {
	public:
		CKBitmapTGAHandler();
		virtual ~CKBitmapTGAHandler();
		LIBCMO_DISABLE_COPY_MOVE(CKBitmapTGAHandler);

		virtual const CKBitmapProperties& GetBitmapDefaultProperties() override;
		virtual bool ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) override;
		virtual bool ReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) override;
		virtual bool SaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) override;
		virtual CKDWORD SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) override;

	protected:
		CKBitmapProperties c_DefaultProp;
	};

}
