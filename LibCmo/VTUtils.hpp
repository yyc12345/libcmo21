#pragma once

#if !(defined(LIBCMO_BUILD_DEBUG) ^ defined(LIBCMO_BUILD_RELEASE))
#error "You must define ONE of LIBCMO_BUILD_DEBUG and LIBCMO_BUILD_RELEASE to indicate build type!"
#endif

// https://stackoverflow.com/questions/2164827/explicitly-exporting-shared-library-functions-in-linux
// generate import export macro.
// these macro is not used by LibCmo because LibCmo is static library
// these macro may used by other project such as BMap.
#if defined(_MSC_VER)
//  Microsoft
#define LIBCMO_RAW_EXPORT __declspec(dllexport)
#define LIBCMO_RAW_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define LIBCMO_RAW_EXPORT __attribute__((visibility("default")))
#define LIBCMO_RAW_IMPORT
#elif defined(__clang__)
//  GCC
#define LIBCMO_RAW_EXPORT __attribute__((visibility("default")))
#define LIBCMO_RAW_IMPORT
#else
//  do nothing and hope for the best?
#define LIBCMO_RAW_EXPORT
#define LIBCMO_RAW_IMPORT
#pragma warning "Unknown dynamic link import/export semantics."
#endif

// choosee proper style
#if defined(LIBCMO_EXPORTING)
#define LIBCMO_NAKED_EXPORT LIBCMO_RAW_EXPORT
#else
#define LIBCMO_NAKED_EXPORT LIBCMO_RAW_IMPORT
#endif

// some work for cpp
#if defined(__cplusplus)
#define LIBCMO_EXPORT extern "C" LIBCMO_NAKED_EXPORT
#else
#define LIBCMO_EXPORT LIBCMO_NAKED_EXPORT
#endif


#if defined(_WIN32)
#define LIBCMO_OS_WIN32
// disable annoy win32 macro
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#endif

#include <cstdio>
#include <type_traits>
#include <cinttypes>
#include <cstdint>
#include <initializer_list>
#include <string>

#pragma region Batch Ctor operator= Operations

#define LIBCMO_DISABLE_COPY_MOVE(CLSNAME) \
	CLSNAME(const CLSNAME&) = delete; \
	CLSNAME(CLSNAME&&) = delete; \
    CLSNAME& operator=(const CLSNAME&) = delete; \
    CLSNAME& operator=(CLSNAME&&) = delete;

#define LIBCMO_DEFAULT_COPY_MOVE(CLSNAME) \
	CLSNAME(const CLSNAME&) = default; \
	CLSNAME(CLSNAME&&) = default; \
    CLSNAME& operator=(const CLSNAME&) = default; \
    CLSNAME& operator=(CLSNAME&&) = default;

#pragma endregion

#define LIBCMO_UNUSED [[maybe_unused]]

namespace LibCmo {

	[[noreturn]] void LibPanic(int line, const char* file, const char* errmsg);
#define LIBCMO_PANIC(msg) LibCmo::LibPanic(__LINE__, __FILE__, msg);

	void LibOrderDebugger();
#if defined(LIBCMO_BUILD_DEBUG)
	/**
	This macro only available in Debug mode.
	It will order debugger stop.
	This macro frequently used when program entering some rarely area.
	For example, in CKStateChunk::ReadObjectID, if code run into the calling of Skip, it mean that this file is pretty old and debugger should notice it.
	*/
#define LIBCMO_ORDER_DEBUGGER LibOrderDebugger();
#else
	// define a blank one
#define LIBCMO_ORDER_DEBUGGER
#endif

	namespace EnumsHelper {
		
		/**
		 * @brief Return 'e1 | e2 | ... | en'
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline TEnum Merge(std::initializer_list<TEnum> il) {
			std::underlying_type_t<TEnum> result = 0;
			for (auto it = il.begin(); it != il.end(); ++it) {
				result |= static_cast<std::underlying_type_t<TEnum>>(*it);
			}
			return static_cast<TEnum>(result);
		}

		/**
		 * @brief Return '~(e)'
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline TEnum Inv(TEnum e) {
			return static_cast<TEnum>(~(static_cast<std::underlying_type_t<TEnum>>(e)));
		}
		
		/**
		 * @brief Operate e1 &= (~e2)
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline void Rm(TEnum& e1, TEnum e2) {
			e1 = static_cast<TEnum>(static_cast<std::underlying_type_t<TEnum>>(e1) & static_cast<std::underlying_type_t<TEnum>>(Inv(e2)));
		}
		
		/**
		 * @brief Operate e1 &= e2
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline void Mask(TEnum& e1, TEnum e2) {
			e1 = static_cast<TEnum>(static_cast<std::underlying_type_t<TEnum>>(e1) & static_cast<std::underlying_type_t<TEnum>>(e2));
		}
		
		/**
		 * @brief Operate e1 |= e2
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline void Add(TEnum& e1, TEnum e2) {
			e1 = static_cast<TEnum>(static_cast<std::underlying_type_t<TEnum>>(e1) | static_cast<std::underlying_type_t<TEnum>>(e2));
		}
		
		/**
		 * @brief Return 'bool(e1 & e2)'
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline bool Has(TEnum e, TEnum probe) {
			return static_cast<bool>(static_cast<std::underlying_type_t<TEnum>>(e) & static_cast<std::underlying_type_t<TEnum>>(probe));
		}
	}

}
