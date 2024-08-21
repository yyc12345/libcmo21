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

/**
 * @brief The core namespace of LibCmo project.
 * @details All functions and classes involving LibCmo are located in this namespace.
*/
namespace LibCmo {

#pragma region LibCmo Exceptions

	/**
	 * @brief The exception thrown when library entering unreachable scope.
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
	 * @brief The exception thrown when library entering logic error.
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
	 * @brief The exception thrown when library entering runtime error.
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

	/**
	 * @brief The namespace for convenient C++ enum class logic operations.
	 * @details
	 * C++ enum class statement is a modern way to declare enum in C++.
	 * But it lack essential logic operations which is commonly used by programmer.
	 * So we create this helper to resolve this issue.
	*/
	namespace EnumsHelper {
		
		/**
		 * @brief Merge given enum flags like performing <TT>e1 | e2 | ... | en</TT>
		 * @tparam TEnum Enum type for processing.
		 * @param[in] il The list of enum flags to be merged.
		 * @return The merged enum flag.
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		constexpr TEnum Merge(std::initializer_list<TEnum> il) {
			using ut = std::underlying_type_t<TEnum>;
			ut result = 0;
			for (auto it = il.begin(); it != il.end(); ++it) {
				result |= static_cast<ut>(*it);
			}
			return static_cast<TEnum>(result);
		}

		/**
		 * @brief Reverse given enum flags like performing <TT>~(e)</TT>
		 * @tparam TEnum Enum type for processing.
		 * @param[in] e The list of enum flags to be inversed.
		 * @return The inversed enum flag.
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		constexpr TEnum Inv(TEnum e) {
			using ut = std::underlying_type_t<TEnum>;
			return static_cast<TEnum>(~(static_cast<ut>(e)));
		}
		
		/**
		 * @brief Remove specified enum flags from given enum flags like performing <TT>e1 &= (~e2)</TT>
		 * @tparam TEnum Enum type for processing.
		 * @param[in,out] e1 The enum flags to be processed.
		 * @param[in] e2 The enum flag to be removed.
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		constexpr void Rm(TEnum& e1, TEnum e2) {
			using ut = std::underlying_type_t<TEnum>;
			e1 = static_cast<TEnum>(static_cast<ut>(e1) & static_cast<ut>(Inv(e2)));
		}

		/**
		 * @brief Use specified enum flags to mask given enum flags like performing <TT>e1 &= e2</TT>
		 * @tparam TEnum Enum type for processing.
		 * @param[in,out] e1 The enum flags to be masked.
		 * @param[in] e2 The mask enum flag.
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		constexpr void Mask(TEnum& e1, TEnum e2) {
			using ut = std::underlying_type_t<TEnum>;
			e1 = static_cast<TEnum>(static_cast<ut>(e1) & static_cast<ut>(e2));
		}

		/**
		 * @brief Add specified enum flags to given enum flags like performing <TT>e1 |= e2</TT>
		 * @tparam TEnum Enum type for processing.
		 * @param[in,out] e1 The enum flags to be processed.
		 * @param[in] e2 The enum flag to be added.
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		constexpr void Add(TEnum& e1, TEnum e2) {
			using ut = std::underlying_type_t<TEnum>;
			e1 = static_cast<TEnum>(static_cast<ut>(e1) | static_cast<ut>(e2));
		}

		/**
		 * @brief Check whether given enum flags has specified enum flag like performing <TT>bool(e & probe)</TT>
		 * @tparam TEnum Enum type for processing.
		 * @param[in] e1 The enum flags to be checked.
		 * @param[in] e2 The enum flag for checking.
		 * @return True if it has, otherwise false.
		*/
		template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, int> = 0>
		constexpr bool Has(TEnum e1, TEnum e2) {
			using ut = std::underlying_type_t<TEnum>;
			return static_cast<bool>(static_cast<ut>(e1) & static_cast<ut>(e2));
		}
	}

}
