#pragma once

#include "../../VTAll.hpp"
#include <memory>
#include <functional>

namespace LibCmo::CK2::DataHandlers {

	class CKBitmapHandler;
	/**
	 * @brief An assist class which can applied to std::unique_ptr as a custom deleter
	 * to make sure the CKBitmapHandler* can be free correctly.
	*/
	struct CKBitmapHandlerDeleter {
		CKBitmapHandlerDeleter() = default;
		CKBitmapHandlerDeleter(const CKBitmapHandlerDeleter&) noexcept {}
		void operator()(CKBitmapHandler* handler);
	};

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
		 * @brief General CKBitmapHandler getter.
		 * @param ext[in] The file extention help finding corresponding bitmap handler.
		 * @param guid[in] The GUID of bitmap handler used in fall back finding.
		 * @remark 
		 *		+ Returns a reader capable of reading file with the given extension ext
		 *		+ It may be several plugins that support the same extension, in which case a preferedGUID identifying the reader that should be returned can be given (optionnal).
		 * @return The pointer to CKBitmapHandler. nullptr if fail to find.
		*/
		static CKBitmapHandler* GetBitmapHandler(const CKFileExtension& ext, const CKGUID& guid);
		/**
		 * @brief A auto free wrapper for GetBitmapHandler
		*/
		static std::unique_ptr<CKBitmapHandler, CKBitmapHandlerDeleter> GetBitmapHandlerWrapper(const CKFileExtension& ext, const CKGUID& guid);
		/**
		 * @brief General CKBitmapHandler disposer
		 * @param handler[in] The handler need to be free.
		*/
		static void ReleaseBitmapHandler(CKBitmapHandler* handler);

		/**
		@brief Loads a bitmap file.
		@return Returns true if successful.
		@param u8filename[in] The file ready to read.
		@param read_image[out] The pointer point to a blank image desc to receive read image.
		@see ReadMemory
		*/
		virtual bool ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) = 0;
		/**
		@brief Loads a bitmap file stored in memory.
		@return Returns true if successful.
		@param memory[in] The pointer to memory.
		@param size[in] The size of memory.
		@param read_image[out] The pointer point to a blank image desc to receive read image.
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

		static const CKBitmapProperties& GetBitmapDefaultProperties();

		virtual bool ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) override;
		virtual bool ReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) override;
		virtual bool SaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) override;
		virtual CKDWORD SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) override;

	};
	
	class CKBitmapTGAHandler : public CKBitmapHandler {
	public:
		CKBitmapTGAHandler();
		virtual ~CKBitmapTGAHandler();
		LIBCMO_DISABLE_COPY_MOVE(CKBitmapTGAHandler);

		static const CKBitmapProperties& GetBitmapDefaultProperties();

		virtual bool ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) override;
		virtual bool ReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) override;
		virtual bool SaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) override;
		virtual CKDWORD SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) override;

	};

}
