#pragma once

// Check LibCmo build type first.
#if !(defined(LIBCMO_BUILD_DEBUG) ^ defined(LIBCMO_BUILD_RELEASE))
#error "You must define ONE of LIBCMO_BUILD_DEBUG and LIBCMO_BUILD_RELEASE to indicate build type!"
#endif

// Include YYCC helper library
#include <YYCCommonplace.hpp>

// Header for this namespace implementation
#include <string>
#include <type_traits>
#include <initializer_list>

namespace LibCmo {

#pragma region LibCmo Exceptions

	/**
	 * @brief The exception raised when library entering unreachable scope.
	 * @details This exception usually used in \c switch syntax.
	 * It means that program enter the scope which it definitely can not enter.
	*/
	class UnreachableException : public std::exception {
	public:
		UnreachableException(const char* msg) : message(msg ? msg : "") {}
		UnreachableException(const UnreachableException& rhs) : message(rhs.message) {}
		virtual ~UnreachableException() {}
		[[nodiscard]] virtual const char* what() const override { return message.c_str(); }
	private:
		std::string message;
	};

	/**
	 * @brief The exception raised when library entering logic error.
	 * @details In theory, this exception can be found by reading code.
	 * It usually caused by programmer use functions in a wrong way.
	 * For example, pass invalid argument to function and etc.
	*/
	class LogicException : public std::exception {
	public:
		LogicException(const char* msg) : message(msg ? msg : "") {}
		LogicException(const LogicException& rhs) : message(rhs.message) {}
		virtual ~LogicException() {}
		[[nodiscard]] virtual const char* what() const override { return message.c_str(); }
	private:
		std::string message;
	};
	
	/**
	 * @brief The exception raised when library entering runtime error.
	 * @details In theory, this exception can not be found by reading code.
	 * It may caused by user input or anything else.
	*/
	class RuntimeException : public std::exception {
	public:
		RuntimeException(const char* msg) : message(msg ? msg : "") {}
		RuntimeException(const RuntimeException& rhs) : message(rhs.message) {}
		virtual ~RuntimeException() {}
		[[nodiscard]] virtual const char* what() const override { return message.c_str(); }
	private:
		std::string message;
	};

#pragma endregion

	namespace EnumsHelper {
		
		/**
		 * @brief Return 'e1 | e2 | ... | en'
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline TEnum Merge(std::initializer_list<TEnum> il) {
			using ut = std::underlying_type_t<TEnum>;
			ut result = 0;
			for (auto it = il.begin(); it != il.end(); ++it) {
				result |= static_cast<ut>(*it);
			}
			return static_cast<TEnum>(result);
		}

		/**
		 * @brief Return '~(e)'
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline TEnum Inv(TEnum e) {
			using ut = std::underlying_type_t<TEnum>;
			return static_cast<TEnum>(~(static_cast<ut>(e)));
		}
		
		/**
		 * @brief Operate e1 &= (~e2)
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline void Rm(TEnum& e1, TEnum e2) {
			using ut = std::underlying_type_t<TEnum>;
			e1 = static_cast<TEnum>(static_cast<ut>(e1) & static_cast<ut>(Inv(e2)));
		}
		
		/**
		 * @brief Operate e1 &= e2
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline void Mask(TEnum& e1, TEnum e2) {
			using ut = std::underlying_type_t<TEnum>;
			e1 = static_cast<TEnum>(static_cast<ut>(e1) & static_cast<ut>(e2));
		}
		
		/**
		 * @brief Operate e1 |= e2
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline void Add(TEnum& e1, TEnum e2) {
			using ut = std::underlying_type_t<TEnum>;
			e1 = static_cast<TEnum>(static_cast<ut>(e1) | static_cast<ut>(e2));
		}
		
		/**
		 * @brief Return 'bool(e1 & e2)'
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		inline bool Has(TEnum e, TEnum probe) {
			using ut = std::underlying_type_t<TEnum>;
			return static_cast<bool>(static_cast<ut>(e) & static_cast<ut>(probe));
		}
	}

}
