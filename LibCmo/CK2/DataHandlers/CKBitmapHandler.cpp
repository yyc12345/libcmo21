#include "CKBitmapHandler.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"

namespace LibCmo::CK2::DataHandlers {

	static void RGBAToARGB(CK2::CKDWORD count, const void* _rgba, void* _argb) {
		const char* rgba = reinterpret_cast<const char*>(_rgba);
		char* argb = reinterpret_cast<char*>(_argb);
		// copy RGB
		VxMath::VxCopyStructure(
			count,
			argb + sizeof(uint8_t),
			4 * sizeof(uint8_t),
			3 * sizeof(uint8_t),
			rgba,
			4 * sizeof(uint8_t)
		);
		// copy A
		VxMath::VxCopyStructure(
			count,
			argb,
			4 * sizeof(uint8_t),
			sizeof(uint8_t),
			rgba + (3 * sizeof(uint8_t)),
			4 * sizeof(uint8_t)
		);
	}

	static void ARGBToRGBA(CK2::CKDWORD count, const void* _argb, void* _rgba) {
		const char* argb = reinterpret_cast<const char*>(_argb);
		char* rgba = reinterpret_cast<char*>(_rgba);
		// copy RGB
		VxMath::VxCopyStructure(
			count,
			rgba,
			4 * sizeof(uint8_t),
			3 * sizeof(uint8_t),
			argb + sizeof(uint8_t),
			4 * sizeof(uint8_t)
		);
		// copy A
		VxMath::VxCopyStructure(
			count,
			rgba + (3 * sizeof(uint8_t)),
			4 * sizeof(uint8_t),
			sizeof(uint8_t),
			argb,
			4 * sizeof(uint8_t)
		);
	}

	static void PostRead(stbi_uc* data, int x, int y, VxMath::VxImageDescEx* read_image) {
		// scale image if need
		if (x != read_image->GetWidth() || y != read_image->GetHeight()) {
			// alloc
			CKBYTE* newdata = new CKBYTE[read_image->GetImageSize()];
			// resize
			stbir_resize(
				data, x, y, 0,
				newdata, static_cast<int>(read_image->GetWidth()), static_cast<int>(read_image->GetHeight()), 0,
				STBIR_TYPE_UINT8, 4, STBIR_ALPHA_CHANNEL_NONE, 0,	// no alpha channel, mean we treat alpha channel as a normal color factor.
				STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
				STBIR_FILTER_BOX, STBIR_FILTER_BOX,
				STBIR_COLORSPACE_SRGB, nullptr
			);
			// copy data
			RGBAToARGB(read_image->GetPixelCount(), newdata, read_image->GetImage());
			// free
			delete[] newdata;
		} else {
			// copy data RGBA -> ARGB
			RGBAToARGB(read_image->GetPixelCount(), data, read_image->GetImage());
		}
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

		// scale image if need
		PostRead(data, x, y, read_image);

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

		// scale image if need
		PostRead(data, x, y, read_image);

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
		FILE* fs = EncodingHelper::U8FOpen(u8filename, "wb");
		if (fs == nullptr) return;

		// allocate buffer and convert data from ARGB to RGBA
		VxMath::VxImageDescEx* wi = const_cast<VxMath::VxImageDescEx*>(write_image);
		CKBYTE* data = new CKBYTE[write_image->GetImageSize()];
		ARGBToRGBA(write_image->GetPixelCount(), wi->GetImage(), data);

		// write data
		FileSaveContext* ctx = new FileSaveContext(fs);
		int ret = oper(
			&FileWriteFunction, ctx,
			static_cast<int>(wi->GetWidth()), static_cast<int>(wi->GetHeight()),
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

		// allocate buffer and convert data from ARGB to RGBA
		VxMath::VxImageDescEx* wi = const_cast<VxMath::VxImageDescEx*>(write_image);
		CKBYTE* data = new CKBYTE[write_image->GetImageSize()];
		ARGBToRGBA(write_image->GetPixelCount(), wi->GetImage(), data);

		// write data
		MemorySaveContext* ctx = new MemorySaveContext(memory);
		int ret = oper(
			&FileWriteFunction, ctx,
			static_cast<int>(wi->GetWidth()), static_cast<int>(wi->GetHeight()),
			4, data	// 4 == RGBA8888
		);

		// free data
		CKDWORD expected = static_cast<CKDWORD>(ctx->m_Counter);
		delete ctx;
		delete[] data;

		if (ret == 0) return 0;
		else return expected;
	}

#pragma region CKBitmapBMPHandler

	CKBitmapBMPHandler::CKBitmapBMPHandler() :
		CKBitmapHandler(), c_DefaultProp() {
		c_DefaultProp.SetExt("Tga");
		c_DefaultProp.m_ReaderGuid.d1 = 0xBCA97223u;
		c_DefaultProp.m_ReaderGuid.d2 = 0x48578BCAu;
	}

	CKBitmapBMPHandler::~CKBitmapBMPHandler() {}

	const CKBitmapProperties& CKBitmapBMPHandler::GetBitmapDefaultProperties() {
		return c_DefaultProp;
	}

	bool CKBitmapBMPHandler::ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) {
		StbReadFile(u8filename, read_image);
	}

	bool CKBitmapBMPHandler::ReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) {
		StbReadMemory(memory, size, read_image);
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

	CKBitmapTGAHandler::CKBitmapTGAHandler() :
		CKBitmapHandler(), c_DefaultProp() {
		c_DefaultProp.SetExt("Tga");
		c_DefaultProp.m_ReaderGuid.d1 = 0x585C7216u;
		c_DefaultProp.m_ReaderGuid.d2 = 0x33302657u;
	}

	CKBitmapTGAHandler::~CKBitmapTGAHandler() {}

	const CKBitmapProperties& CKBitmapTGAHandler::GetBitmapDefaultProperties() {
		return c_DefaultProp;
	}

	bool CKBitmapTGAHandler::ReadFile(CKSTRING u8filename, VxMath::VxImageDescEx* read_image) {
		StbReadFile(u8filename, read_image);
	}

	bool CKBitmapTGAHandler::ReadMemory(const void* memory, CKDWORD size, VxMath::VxImageDescEx* read_image) {
		StbReadMemory(memory, size, read_image);
	}

	bool CKBitmapTGAHandler::SaveFile(CKSTRING u8filename, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveFile(u8filename, write_image,
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_bmp_to_func(func, context, w, h, comp, data);
			});
	}

	CKDWORD CKBitmapTGAHandler::SaveMemory(void* memory, const VxMath::VxImageDescEx* write_image, const CKBitmapProperties& codec_param) {
		return StbSaveMemory(memory, write_image,
			[&codec_param](stbi_write_func* func, void* context, int w, int h, int comp, const void* data) -> int {
				return stbi_write_bmp_to_func(func, context, w, h, comp, data);
			});
	}

#pragma endregion


}
