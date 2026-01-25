#include "CKBitmapHandler.hpp"
#include <stb_image.h>
#include <stb_image_write.h>

namespace LibCmo::CK2::DataHandlers {

#pragma region Help Functions

	/*
	 ABGR8888 is format used by std image.
	 The data is placed in buffer with RGBA order, so the format is ABGR.
	 BGR888 also is non-alpha format used by std image.
	 The data is placed in buffer with RGB order, so the format is BGR.
	 ARGB8888 is Virtools standard.
	 The data is placed in buffer with BGRA order.
	*/

	// MARK: for std-image size, we use `n * CKSizeof(CKBYTE)` to calc offset.
	// for virtools size, we use `VxMath::VxImageDescEx::FACTOR_SIZE` and `VxMath::VxImageDescEx::PIXEL_SIZE` to calc offset.

	static void ABGRToARGB(CKDWORD count, const void* _abgr, void* _argb) {
		const CKBYTE* abgr = static_cast<const CKBYTE*>(_abgr);
		CKBYTE* argb = static_cast<CKBYTE*>(_argb);
		// copy R
		VxMath::VxCopyStructure(
			count,
			argb + (2u * VxMath::VxImageDescEx::FACTOR_SIZE),
			VxMath::VxImageDescEx::PIXEL_SIZE,
			VxMath::VxImageDescEx::FACTOR_SIZE,
			abgr + (0u * CKSizeof(CKBYTE)),
			4u * CKSizeof(CKBYTE)
		);
		// copy G
		VxMath::VxCopyStructure(
			count,
			argb + (1u * VxMath::VxImageDescEx::FACTOR_SIZE),
			VxMath::VxImageDescEx::PIXEL_SIZE,
			VxMath::VxImageDescEx::FACTOR_SIZE,
			abgr + (1u * CKSizeof(CKBYTE)),
			4u * CKSizeof(CKBYTE)
		);
		// copy B
		VxMath::VxCopyStructure(
			count,
			argb + (0u * VxMath::VxImageDescEx::FACTOR_SIZE),
			VxMath::VxImageDescEx::PIXEL_SIZE,
			VxMath::VxImageDescEx::FACTOR_SIZE,
			abgr + (2u * CKSizeof(CKBYTE)),
			4u * CKSizeof(CKBYTE)
		);
		// copy A
		VxMath::VxCopyStructure(
			count,
			argb + (3u * VxMath::VxImageDescEx::FACTOR_SIZE),
			VxMath::VxImageDescEx::PIXEL_SIZE,
			VxMath::VxImageDescEx::FACTOR_SIZE,
			abgr + (3u * CKSizeof(CKBYTE)),
			4u * CKSizeof(CKBYTE)
		);
	}

	static void ARGBToABGR(CKDWORD count, const void* _argb, void* _abgr) {
		const CKBYTE* argb = static_cast<const CKBYTE*>(_argb);
		CKBYTE* abgr = static_cast<CKBYTE*>(_abgr);
		// copy R
		VxMath::VxCopyStructure(
			count,
			abgr + (0u * CKSizeof(CKBYTE)),
			4u * CKSizeof(CKBYTE),
			VxMath::VxImageDescEx::FACTOR_SIZE,
			argb + (2u * VxMath::VxImageDescEx::FACTOR_SIZE),
			VxMath::VxImageDescEx::PIXEL_SIZE
		);
		// copy G
		VxMath::VxCopyStructure(
			count,
			abgr + (1u * CKSizeof(CKBYTE)),
			4u * CKSizeof(CKBYTE),
			VxMath::VxImageDescEx::FACTOR_SIZE,
			argb + (1u * VxMath::VxImageDescEx::FACTOR_SIZE),
			VxMath::VxImageDescEx::PIXEL_SIZE
		);
		// copy B
		VxMath::VxCopyStructure(
			count,
			abgr + (2u * CKSizeof(CKBYTE)),
			4u * CKSizeof(CKBYTE),
			VxMath::VxImageDescEx::FACTOR_SIZE,
			argb + (0u * VxMath::VxImageDescEx::FACTOR_SIZE),
			VxMath::VxImageDescEx::PIXEL_SIZE
		);
		// copy A
		VxMath::VxCopyStructure(
			count,
			abgr + (3u * CKSizeof(CKBYTE)),
			4u * CKSizeof(CKBYTE),
			VxMath::VxImageDescEx::FACTOR_SIZE,
			argb + (3u * VxMath::VxImageDescEx::FACTOR_SIZE),
			VxMath::VxImageDescEx::PIXEL_SIZE
		);
	}

	static void ARGBToBGR(CKDWORD count, const void* _argb, void* _bgr) {
		const CKBYTE* argb = static_cast<const CKBYTE*>(_argb);
		CKBYTE* bgr = static_cast<CKBYTE*>(_bgr);
		// copy R
		VxMath::VxCopyStructure(
			count,
			bgr + (0u * CKSizeof(CKBYTE)),
			3u * CKSizeof(CKBYTE),
			VxMath::VxImageDescEx::FACTOR_SIZE,
			argb + (2u * VxMath::VxImageDescEx::FACTOR_SIZE),
			VxMath::VxImageDescEx::PIXEL_SIZE
		);
		// copy G
		VxMath::VxCopyStructure(
			count,
			bgr + (1u * CKSizeof(CKBYTE)),
			3u * CKSizeof(CKBYTE),
			VxMath::VxImageDescEx::FACTOR_SIZE,
			argb + (1u * VxMath::VxImageDescEx::FACTOR_SIZE),
			VxMath::VxImageDescEx::PIXEL_SIZE
		);
		// copy B
		VxMath::VxCopyStructure(
			count,
			bgr + (2u * CKSizeof(CKBYTE)),
			3u * CKSizeof(CKBYTE),
			VxMath::VxImageDescEx::FACTOR_SIZE,
			argb + (0u * VxMath::VxImageDescEx::FACTOR_SIZE),
			VxMath::VxImageDescEx::PIXEL_SIZE
		);
		// skip A factor
	}

	static bool StbReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) {
		if (u8filename == nullptr || read_image == nullptr) return false;
		FILE* fs = YYCC::IOHelper::UTF8FOpen(u8filename, u8"rb");
		if (fs == nullptr) return false;

		// read data
		int x, y, channels_in_file;
		stbi_uc* data = stbi_load_from_file(fs, &x, &y, &channels_in_file, 4);	// 4 == RGBA8888
		std::fclose(fs);
		if (data == nullptr) return false;

		// create read image
		read_image->CreateImage(static_cast<CKDWORD>(x), static_cast<CKDWORD>(y));

		// copy data
		ABGRToARGB(read_image->GetPixelCount(), data, read_image->GetMutableImage());

		// clear data
		stbi_image_free(data);
		return true;
	}
	static bool StbReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) {
		// same like ReadFile
		if (memory == nullptr || read_image == nullptr) return false;

		// read data
		int x, y, channels_in_file;
		stbi_uc* data = stbi_load_from_memory(
			static_cast<const stbi_uc*>(memory),
			static_cast<int>(size),
			&x, &y, &channels_in_file, 4	// 4 == RGBA8888
		);
		if (data == nullptr) return false;

		// create read image
		read_image->CreateImage(static_cast<CKDWORD>(x), static_cast<CKDWORD>(y));

		// copy data
		ABGRToARGB(read_image->GetPixelCount(), data, read_image->GetMutableImage());

		// clear data
		stbi_image_free(data);
		return true;
	}

	struct FileSaveContext {
		FileSaveContext(FILE* fs) :
			m_Fs(fs), m_Counter(0) {}
		FILE* m_Fs;
		size_t m_Counter;
	};
	static void FileWriteFunction(void* context, void* data, int size) {
		FileSaveContext* ctx = static_cast<FileSaveContext*>(context);
		if (ctx->m_Fs != nullptr) {
			std::fwrite(data, sizeof(CKBYTE), size, ctx->m_Fs);
		}
		ctx->m_Counter += size;
	}
	struct MemorySaveContext {
		MemorySaveContext(void* mem) :
			m_Mem(mem), m_Counter(0) {}
		void* m_Mem;
		size_t m_Counter;
	};
	static void MemoryWriteFunction(void* context, void* data, int size) {
		MemorySaveContext* ctx = static_cast<MemorySaveContext*>(context);
		if (ctx->m_Mem != nullptr) {
			std::memcpy(ctx->m_Mem, data, size);
			ctx->m_Mem = static_cast<CKBYTE*>(ctx->m_Mem) + size;
		}
		ctx->m_Counter += size;
	}

	using SaveOperation = std::function<int(stbi_write_func*, void*, int, int, int, const void*)>;
	static bool StbSaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, bool save_alpha, SaveOperation oper) {
		if (u8filename == nullptr || write_image == nullptr) return false;
		if (!write_image->IsValid()) return false;
		FILE* fs = YYCC::IOHelper::UTF8FOpen(u8filename, u8"wb");
		if (fs == nullptr) return false;

		// allocate buffer and convert data from ARGB to RGBA or RGB
		CKBYTE* data = nullptr;
		int channel_count = 0;
		if (save_alpha) {
			// save with alpha
			data = new CKBYTE[CKSizeof(CKBYTE) * 4u * write_image->GetWidth() * write_image->GetHeight()];
			ARGBToABGR(write_image->GetPixelCount(), write_image->GetImage(), data);
			channel_count = 4;
		} else {
			// save without alpha
			data = new CKBYTE[CKSizeof(CKBYTE) * 3u * write_image->GetWidth() * write_image->GetHeight()];
			ARGBToBGR(write_image->GetPixelCount(), write_image->GetImage(), data);
			channel_count = 3;
		}

		// write data
		FileSaveContext* ctx = new FileSaveContext(fs);
		int ret = oper(
			&FileWriteFunction, ctx,
			static_cast<int>(write_image->GetWidth()), static_cast<int>(write_image->GetHeight()),
			channel_count, data	// 4 == RGBA8888
		);

		// free data
		delete ctx;
		delete[] data;
		std::fclose(fs);

		// ret is 0 mean failed.
		return ret != 0;
	}
	static CKDWORD StbSaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, bool save_alpha, SaveOperation oper) {
		if (write_image == nullptr) return 0;
		if (!write_image->IsValid()) return 0;

		// allocate buffer and convert data from ARGB to RGBA or RGB
		CKBYTE* data = nullptr;
		int channel_count = 0;
		if (save_alpha) {
			// save with alpha
			data = new CKBYTE[CKSizeof(CKBYTE) * 4u * write_image->GetWidth() * write_image->GetHeight()];
			ARGBToABGR(write_image->GetPixelCount(), write_image->GetImage(), data);
			channel_count = 4;
		} else {
			// save without alpha
			data = new CKBYTE[CKSizeof(CKBYTE) * 3u * write_image->GetWidth() * write_image->GetHeight()];
			ARGBToBGR(write_image->GetPixelCount(), write_image->GetImage(), data);
			channel_count = 3;
		}

		// write data
		MemorySaveContext* ctx = new MemorySaveContext(memory);
		int ret = oper(
			&MemoryWriteFunction, ctx,
			static_cast<int>(write_image->GetWidth()), static_cast<int>(write_image->GetHeight()),
			channel_count, data	// 4 == RGBA8888
		);

		// free data
		CKDWORD expected = static_cast<CKDWORD>(ctx->m_Counter);
		delete ctx;
		delete[] data;

		// ret is 0 mean failed. return zero size.
		if (ret == 0) return 0;
		else return expected;
	}

#pragma endregion

#pragma region CKBitmapBMPHandler

	static const CKBitmapProperties g_BMPProperties(CKGUID(0xBCA97223u, 0x48578BCAu), u8"Bmp");

	CKBitmapBMPHandler::CKBitmapBMPHandler() :
		CKBitmapHandler() {}

	CKBitmapBMPHandler::~CKBitmapBMPHandler() {}

	const CKBitmapProperties& CKBitmapBMPHandler::GetBitmapDefaultProperties() {
		return g_BMPProperties;
	}

	bool CKBitmapBMPHandler::ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) {
		return StbReadFile(u8filename, read_image);
	}

	bool CKBitmapBMPHandler::ReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) {
		return StbReadMemory(memory, size, read_image);
	}

	// MARK: when stb-image writing bmp file with alpha channel, it will create a very rare bmp file supporting alpha channel.
	// this format is not supported by virtools and will result blank image.
	// so we create an alpha option to forcely change channel count to 3 (RGB) then the bmp writer can work normally.

	bool CKBitmapBMPHandler::SaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveFile(u8filename, write_image, false,	// bmp do not support alpha
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_bmp_to_func(func, context, w, h, comp, data);
			});
	}

	CKDWORD CKBitmapBMPHandler::SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveMemory(memory, write_image, false,	// bmp do not support alpha
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_bmp_to_func(func, context, w, h, comp, data);
			});
	}

	bool CKBitmapBMPHandler::CanSaveAlpha() {
		return false;
	}

#pragma endregion

#pragma region CKBitmapTGAHandler

	static const CKBitmapProperties g_TGAProperties(CKGUID(0x585C7216u, 0x33302657u), u8"Tga");

	CKBitmapTGAHandler::CKBitmapTGAHandler() :
		CKBitmapHandler() {}

	CKBitmapTGAHandler::~CKBitmapTGAHandler() {}

	const CKBitmapProperties& CKBitmapTGAHandler::GetBitmapDefaultProperties() {
		return g_TGAProperties;
	}

	bool CKBitmapTGAHandler::ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) {
		return StbReadFile(u8filename, read_image);
	}

	bool CKBitmapTGAHandler::ReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) {
		return StbReadMemory(memory, size, read_image);
	}

	bool CKBitmapTGAHandler::SaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveFile(u8filename, write_image, false,	// tga support alpha
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_tga_to_func(func, context, w, h, comp, data);
			});
	}

	CKDWORD CKBitmapTGAHandler::SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveMemory(memory, write_image, false,	// tga support alpha
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_tga_to_func(func, context, w, h, comp, data);
			});
	}

	bool CKBitmapTGAHandler::CanSaveAlpha() {
		return true;
	}

#pragma endregion
	
#pragma region CKBitmapJPGHandler

	// MARK: this GUID is gotten from Virtools 3.5 Plugins.
	static const CKBitmapProperties g_JPGProperties(CKGUID(0x4AE51AC4u, 0x04587D76u), u8"jpg");
	// MARK: this quality is gotten from default value of virtools.
	constexpr int g_JPGDefaultQuality = 75;

	CKBitmapJPGHandler::CKBitmapJPGHandler() :
		CKBitmapHandler() {}

	CKBitmapJPGHandler::~CKBitmapJPGHandler() {}

	const CKBitmapProperties& CKBitmapJPGHandler::GetBitmapDefaultProperties() {
		return g_JPGProperties;
	}

	bool CKBitmapJPGHandler::ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) {
		return StbReadFile(u8filename, read_image);
	}

	bool CKBitmapJPGHandler::ReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) {
		return StbReadMemory(memory, size, read_image);
	}

	bool CKBitmapJPGHandler::SaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveFile(u8filename, write_image, false,	// jpg do not support alpha
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_jpg_to_func(func, context, w, h, comp, data, g_JPGDefaultQuality);
			});
	}

	CKDWORD CKBitmapJPGHandler::SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveMemory(memory, write_image, false,	// jpg do not support alpha
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_jpg_to_func(func, context, w, h, comp, data, g_JPGDefaultQuality);
			});
	}

	bool CKBitmapJPGHandler::CanSaveAlpha() {
		return false;
	}

#pragma endregion
	
#pragma region CKBitmapPNGHandler

	// MARK: this GUID is gotten from Virtools 3.5 Plugins.
	static const CKBitmapProperties g_PNGProperties(CKGUID(0x02D45C7Bu, 0x4AAC16ECu), u8"png");
	// MARK: this is compress level gotten from default value of virtools.
	constexpr int g_PNGDefaultCompressLevel = 3;
	
	/**
	 * @brief A helper function to get stride parameter passed to png writer.
	 * @param width[in] The width given by general stb writer wrapper.
	 * @param comp[in] The comp given by general stb writer wrapper.
	 * @return The stride data passed to real stb writer.
	*/
	static int StbPngStrideGetter(int width, int comp) {
		return width * comp;
	}

	CKBitmapPNGHandler::CKBitmapPNGHandler() :
		CKBitmapHandler() {}

	CKBitmapPNGHandler::~CKBitmapPNGHandler() {}

	const CKBitmapProperties& CKBitmapPNGHandler::GetBitmapDefaultProperties() {
		return g_PNGProperties;
	}

	bool CKBitmapPNGHandler::ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) {
		return StbReadFile(u8filename, read_image);
	}

	bool CKBitmapPNGHandler::ReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) {
		return StbReadMemory(memory, size, read_image);
	}

	bool CKBitmapPNGHandler::SaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveFile(u8filename, write_image, false,	// png support alpha
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				// set default compress level
				stbi_write_png_compression_level = g_PNGDefaultCompressLevel;
				// write data
				return stbi_write_png_to_func(func, context, w, h, comp, data, StbPngStrideGetter(w, comp));
			});
	}

	CKDWORD CKBitmapPNGHandler::SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveMemory(memory, write_image, false,	// png support alpha
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				stbi_write_png_compression_level = g_PNGDefaultCompressLevel;
				return stbi_write_png_to_func(func, context, w, h, comp, data, StbPngStrideGetter(w, comp));
			});
	}

	bool CKBitmapPNGHandler::CanSaveAlpha() {
		return true;
	}

#pragma endregion

#pragma region General Getter Freer

	static CKBitmapHandler* FindHandlerByExt(const CKFileExtension& ext) {
		if (ext == g_BMPProperties.m_Ext) return new CKBitmapBMPHandler();
		if (ext == g_TGAProperties.m_Ext) return new CKBitmapTGAHandler();
		if (ext == g_JPGProperties.m_Ext) return new CKBitmapJPGHandler();
		if (ext == g_PNGProperties.m_Ext) return new CKBitmapPNGHandler();
		return nullptr;
	}

	static CKBitmapHandler* FindHandlerByGuid(const CKGUID& guid) {
		if (guid == g_BMPProperties.m_ReaderGuid) return new CKBitmapBMPHandler();
		if (guid == g_TGAProperties.m_ReaderGuid) return new CKBitmapTGAHandler();
		if (guid == g_JPGProperties.m_ReaderGuid) return new CKBitmapJPGHandler();
		if (guid == g_PNGProperties.m_ReaderGuid) return new CKBitmapPNGHandler();
		return nullptr;
	}

	CKBitmapHandler* CKBitmapHandler::GetBitmapHandler(const CKFileExtension& ext, const CKGUID& guid) {
		CKBitmapHandler* handler = nullptr;

		// check ext first
		handler = FindHandlerByExt(ext);
		if (handler != nullptr) return handler;

		// check guid
		handler = FindHandlerByGuid(guid);
		if (handler != nullptr) return handler;

		return nullptr;
	}

	CKBitmapHandlerWrapper_t CKBitmapHandler::GetBitmapHandlerWrapper(const CKFileExtension& ext, const CKGUID& guid) {
		return CKBitmapHandlerWrapper_t(GetBitmapHandler(ext, guid));
	}

	void CKBitmapHandlerDeleter::operator()(CKBitmapHandler* handler) {
		CKBitmapHandler::ReleaseBitmapHandler(handler);
	}

	void CKBitmapHandler::ReleaseBitmapHandler(CKBitmapHandler* handler) {
		if (handler != nullptr) delete handler;
	}

#pragma endregion

}
