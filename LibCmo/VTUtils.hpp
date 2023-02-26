#pragma once

/*
// https://stackoverflow.com/questions/2164827/explicitly-exporting-shared-library-functions-in-linux
// generate import export macro
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
#pragma warning Unknown dynamic link import/export semantics.
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
*/

#if defined(_WIN32)
#define LIBCMO_OS_WIN32
#endif

#include <cstdio>
#include <type_traits>
#include <cinttypes>
#include <cstdint>
#include <cstdarg>

namespace LibCmo {

    namespace EnumsHelper {
        template<typename TEnum>
        inline TEnum FlagEnumAdd(TEnum e, ...) {
            TEnum result = e;
            va_list argptr;
            va_start(argptr, e);
            result = static_cast<TEnum>(static_cast<std::underlying_type_t<TEnum>>(result) | static_cast<std::underlying_type_t<TEnum>>(va_arg(argptr, TEnum)));
            va_end(argptr);
            return result;
        }

        template<typename TEnum>
        inline bool FlagEnumHas(TEnum e, TEnum probe) {
            return static_cast<bool>(static_cast<std::underlying_type_t<TEnum>>(e) & static_cast<std::underlying_type_t<TEnum>>(probe));
        }
    }

	namespace StreamHelper {

		void CopyStream(const void* src, FILE* dest, size_t len);
		void CopyStream(FILE* src, void* dest, size_t len);

	}
}
