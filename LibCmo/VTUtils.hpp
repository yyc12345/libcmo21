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
#include <initializer_list>

#pragma region Enum Helper

#define LIBCMO_BITFLAG_OPERATORS_BITWISE(OP, ENUM_TYPE) \
    constexpr ENUM_TYPE operator OP(ENUM_TYPE lhs, ENUM_TYPE rhs) noexcept { \
        typedef std::underlying_type_t<ENUM_TYPE> underlying; \
        return static_cast<ENUM_TYPE>(static_cast<underlying>(lhs) OP static_cast<underlying>(rhs)); \
    } \
    constexpr ENUM_TYPE& operator OP ## = (ENUM_TYPE& lhs, ENUM_TYPE rhs) noexcept { \
        return (lhs = lhs OP rhs); \
    }

#define LIBCMO_BITFLAG_OPERATORS_BOOLEAN(OP, ENUM_TYPE) \
    constexpr bool operator OP(ENUM_TYPE lhs, std::underlying_type_t<ENUM_TYPE> rhs) noexcept { \
        return static_cast<std::underlying_type_t<ENUM_TYPE>>(lhs) OP rhs; \
    } \
    constexpr bool operator OP(std::underlying_type_t<ENUM_TYPE> lhs, ENUM_TYPE rhs) noexcept { \
        return lhs OP static_cast<std::underlying_type_t<ENUM_TYPE>>(rhs); \
    }

#define LIBCMO_BITFLAG_OPERATORS(ENUM_TYPE) \
    LIBCMO_BITFLAG_OPERATORS_BITWISE(|, ENUM_TYPE) \
    LIBCMO_BITFLAG_OPERATORS_BITWISE(&, ENUM_TYPE) \
    LIBCMO_BITFLAG_OPERATORS_BITWISE(^, ENUM_TYPE) \
    LIBCMO_BITFLAG_OPERATORS_BOOLEAN(==, ENUM_TYPE) \
    LIBCMO_BITFLAG_OPERATORS_BOOLEAN(!=, ENUM_TYPE) \
    LIBCMO_BITFLAG_OPERATORS_BOOLEAN(<, ENUM_TYPE) \
    LIBCMO_BITFLAG_OPERATORS_BOOLEAN(>, ENUM_TYPE) \
    LIBCMO_BITFLAG_OPERATORS_BOOLEAN(>=, ENUM_TYPE) \
    LIBCMO_BITFLAG_OPERATORS_BOOLEAN(<=, ENUM_TYPE) \
    constexpr ENUM_TYPE operator~(ENUM_TYPE e) noexcept { \
        return static_cast<ENUM_TYPE>(~static_cast<std::underlying_type_t<ENUM_TYPE>>(e)); \
    } \
    constexpr bool operator!(ENUM_TYPE e) noexcept { \
        return static_cast<bool>(static_cast<std::underlying_type_t<ENUM_TYPE>>(e)); \
    }

#pragma endregion

namespace LibCmo {

	namespace StreamHelper {

		void CopyStream(const void* src, FILE* dest, size_t len);
		void CopyStream(FILE* src, void* dest, size_t len);

	}
}
