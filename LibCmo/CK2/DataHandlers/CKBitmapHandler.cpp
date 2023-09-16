#include "CKBitmapHandler.hpp"

namespace LibCmo::CK2::DataHandlers {

#pragma region Help Functions

	/*
	 ABGR8888 is format used by std image.
	 The data is placed in buffer with RGBA order, so the format is ABGR.
	 ARGB8888 is Virtools standard.
	 The data is placed in buffer with BGRA order.
	*/

	static void ABGRToARGB(CKDWORD count, const void* _abgr, void* _argb) {
		const char* abgr = reinterpret_cast<const char*>(_abgr);
		char* argb = reinterpret_cast<char*>(_argb);
		// copy R
		VxMath::VxCopyStructure(
			count,
			argb + (2u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize,
			VxMath::VxImageDescEx::ColorFactorSize,
			abgr + (0u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize
		);
		// copy G
		VxMath::VxCopyStructure(
			count,
			argb + (1u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize,
			VxMath::VxImageDescEx::ColorFactorSize,
			abgr + (1u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize
		);
		// copy B
		VxMath::VxCopyStructure(
			count,
			argb + (0u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize,
			VxMath::VxImageDescEx::ColorFactorSize,
			abgr + (2u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize
		);
		// copy A
		VxMath::VxCopyStructure(
			count,
			argb + (3u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize,
			VxMath::VxImageDescEx::ColorFactorSize,
			abgr + (3u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize
		);
	}

	static void ARGBToABGR(CKDWORD count, const void* _argb, void* _abgr) {
		const char* argb = reinterpret_cast<const char*>(_argb);
		char* abgr = reinterpret_cast<char*>(_abgr);
		// copy R
		VxMath::VxCopyStructure(
			count,
			abgr + (0u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize,
			VxMath::VxImageDescEx::ColorFactorSize,
			argb + (2u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize
		);
		// copy G
		VxMath::VxCopyStructure(
			count,
			abgr + (1u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize,
			VxMath::VxImageDescEx::ColorFactorSize,
			argb + (1u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize
		);
		// copy B
		VxMath::VxCopyStructure(
			count,
			abgr + (2u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize,
			VxMath::VxImageDescEx::ColorFactorSize,
			argb + (0u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize
		);
		// copy A
		VxMath::VxCopyStructure(
			count,
			abgr + (3u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize,
			VxMath::VxImageDescEx::ColorFactorSize,
			argb + (3u * VxMath::VxImageDescEx::ColorFactorSize),
			VxMath::VxImageDescEx::PixelSize
		);
	}

	static bool StbReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) {
		if (u8filename == nullptr || read_image == nullptr) return false;
		FILE* fs = EncodingHelper::U8FOpen(u8filename, "rb");
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
			reinterpret_cast<const stbi_uc*>(memory),
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
		FileSaveContext* ctx = reinterpret_cast<FileSaveContext*>(context);
		if (ctx->m_Fs != nullptr) {
			std::fwrite(data, sizeof(char), size, ctx->m_Fs);
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
		MemorySaveContext* ctx = reinterpret_cast<MemorySaveContext*>(context);
		if (ctx->m_Mem != nullptr) {
			std::memcpy(ctx->m_Mem, data, size);
			ctx->m_Mem = reinterpret_cast<char*>(ctx->m_Mem) + size;
		}
		ctx->m_Counter += size;
	}

	using SaveOperation = std::function<int(stbi_write_func*, void*, int, int, int, const void*)>;
	static bool StbSaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, SaveOperation oper) {
		if (u8filename == nullptr || write_image == nullptr) return false;
		if (!write_image->IsValid()) return false;
		FILE* fs = EncodingHelper::U8FOpen(u8filename, "wb");
		if (fs == nullptr) return false;

		// allocate buffer and convert data from ARGB to RGBA
		CKBYTE* data = new CKBYTE[write_image->GetImageSize()];
		ARGBToABGR(write_image->GetPixelCount(), write_image->GetImage(), data);

		// write data
		FileSaveContext* ctx = new FileSaveContext(fs);
		int ret = oper(
			&FileWriteFunction, ctx,
			static_cast<int>(write_image->GetWidth()), static_cast<int>(write_image->GetHeight()),
			4, data	// 4 == RGBA8888
		);

		// free data
		delete ctx;
		delete[] data;
		std::fclose(fs);

		return ret == 0;
	}
	static CKDWORD StbSaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, SaveOperation oper) {
		if (write_image == nullptr) return 0;
		if (!write_image->IsValid()) return 0;

		// allocate buffer and convert data from ARGB to RGBA
		CKBYTE* data = new CKBYTE[write_image->GetImageSize()];
		ARGBToABGR(write_image->GetPixelCount(), write_image->GetImage(), data);

		// write data
		MemorySaveContext* ctx = new MemorySaveContext(memory);
		int ret = oper(
			&MemoryWriteFunction, ctx,
			static_cast<int>(write_image->GetWidth()), static_cast<int>(write_image->GetHeight()),
			4, data	// 4 == RGBA8888
		);

		// free data
		CKDWORD expected = static_cast<CKDWORD>(ctx->m_Counter);
		delete ctx;
		delete[] data;

		if (ret == 0) return 0;
		else return expected;
	}
	
#pragma endregion

#pragma region CKBitmapBMPHandler

	static const CKBitmapProperties g_BMPProperties(CKGUID(0xBCA97223u, 0x48578BCAu), "Bmp");

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

	bool CKBitmapBMPHandler::SaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveFile(u8filename, write_image,
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_bmp_to_func(func, context, w, h, comp, data);
			});
	}

	CKDWORD CKBitmapBMPHandler::SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveMemory(memory, write_image,
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_bmp_to_func(func, context, w, h, comp, data);
			});
	}

#pragma endregion

#pragma region CKBitmapTGAHandler
	
	static const CKBitmapProperties g_TGAProperties(CKGUID(0x585C7216u,  0x33302657u), "Tga");

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
		return StbSaveFile(u8filename, write_image,
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_tga_to_func(func, context, w, h, comp, data);
			});
	}

	CKDWORD CKBitmapTGAHandler::SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveMemory(memory, write_image,
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_tga_to_func(func, context, w, h, comp, data);
			});
	}

#pragma endregion
	
#pragma region General Getter Freer

	static CKBitmapHandler* FindHandlerByExt(const CKFileExtension& ext) {
		if (ext == g_BMPProperties.m_Ext) return new CKBitmapBMPHandler();
		if (ext == g_TGAProperties.m_Ext) return new CKBitmapTGAHandler();
		return nullptr;
	}

	static CKBitmapHandler* FindHandlerByGuid(const CKGUID& guid) {
		if (guid == g_BMPProperties.m_ReaderGuid) return new CKBitmapBMPHandler();
		if (guid == g_TGAProperties.m_ReaderGuid) return new CKBitmapTGAHandler();
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

	std::unique_ptr<CKBitmapHandler, CKBitmapHandlerDeleter> CKBitmapHandler::GetBitmapHandlerWrapper(const CKFileExtension& ext, const CKGUID& guid) {
		return std::unique_ptr<CKBitmapHandler, CKBitmapHandlerDeleter>(GetBitmapHandler(ext, guid));
	}

	void CKBitmapHandlerDeleter::operator()(CKBitmapHandler* handler) {
		CKBitmapHandler::ReleaseBitmapHandler(handler);
	}

	void CKBitmapHandler::ReleaseBitmapHandler(CKBitmapHandler* handler) {
		if (handler != nullptr) delete handler;
	}

#pragma endregion

}
