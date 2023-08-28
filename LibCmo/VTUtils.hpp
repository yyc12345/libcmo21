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

namespace LibCmo {

	[[noreturn]] void LibPanic(int line, const char* file, const char* errmsg);
#define LIBPANIC(msg) LibCmo::LibPanic(__LINE__, __FILE__, msg);

	namespace TypeHelper {

		/**
		 * @brief MKString is a compatible type.
		 * In some original Virtools case, we need CKSTRING to hold string.
		 * But CKSTRING is just a pointer and it is very cause memory leak.
		 * So I invent this. It like CKSTRING but will free memory automatically.
		 * And it is safe in CKSTRING copying.
		 * It operate like std::string. Have a function called c_str() to create usable CKSTRING.
		 * We call it as MKString. (memory-safe CKSTRING.) :pu: =.=
		*/
		class MKString {
		public:
			MKString() : m_HasStr(false), m_Str() {}
			~MKString() {}

			MKString(const char* cstr) : m_HasStr(cstr != nullptr), m_Str(m_HasStr ? cstr : "") {}
			MKString& operator=(const char* cstr) {
				m_HasStr = cstr != nullptr;
				m_Str = m_HasStr ? cstr : "";
				return *this;
			}
			MKString(const std::string& cstr) : m_HasStr(true), m_Str(cstr) {}
			MKString& operator=(const std::string& cstr) {
				m_HasStr = true;
				m_Str = cstr;
				return *this;
			}

			MKString(const MKString& rhs) : m_HasStr(rhs.m_HasStr), m_Str(rhs.m_Str) {}
			MKString(MKString&& rhs) noexcept : m_HasStr(rhs.m_HasStr), m_Str(std::move(rhs.m_Str)) {
				rhs.m_HasStr = false;
			}
			MKString& operator=(const MKString& rhs) {
				m_HasStr = rhs.m_HasStr;
				m_Str = rhs.m_Str;
				return *this;
			}
			MKString& operator=(MKString&& rhs) noexcept {
				m_HasStr = rhs.m_HasStr;
				m_Str = std::move(rhs.m_Str);
				rhs.m_HasStr = false;
				return *this;
			}

			const char* c_str() const {
				return m_HasStr ? m_Str.c_str() : nullptr;
			}
			const std::string& string() const {
				return m_Str;
			}
			const size_t size() const {
				return m_HasStr ? m_Str.size() : 0u;
			}

		private:
			bool m_HasStr;
			std::string m_Str;
		};

	}

	namespace EnumsHelper {
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline TEnum Merge(std::initializer_list<TEnum> il) {
			std::underlying_type_t<TEnum> result = 0;
			for (auto it = il.begin(); it != il.end(); ++it) {
				result |= static_cast<std::underlying_type_t<TEnum>>(*it);
			}
			return static_cast<TEnum>(result);
		}

		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline TEnum Inv(TEnum e) {
			return static_cast<TEnum>(~(static_cast<std::underlying_type_t<TEnum>>(e)));
		}

		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline void Rm(TEnum& e1, TEnum e2) {
			e1 = static_cast<TEnum>(static_cast<std::underlying_type_t<TEnum>>(e1) & static_cast<std::underlying_type_t<TEnum>>(Inv(e2)));
		}

		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline void Add(TEnum& e1, TEnum e2) {
			e1 = static_cast<TEnum>(static_cast<std::underlying_type_t<TEnum>>(e1) | static_cast<std::underlying_type_t<TEnum>>(e2));
		}

		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline bool Has(TEnum e, TEnum probe) {
			return static_cast<bool>(static_cast<std::underlying_type_t<TEnum>>(e) & static_cast<std::underlying_type_t<TEnum>>(probe));
		}
	}

	//namespace StreamHelper {

	//	void CopyStream(const void* src, FILE* dest, size_t len);
	//	void CopyStream(FILE* src, void* dest, size_t len);

	//}
}
