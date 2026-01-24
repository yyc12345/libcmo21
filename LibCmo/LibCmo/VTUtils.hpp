#pragma once

// Check LibCmo build type first.
#if !(defined(LIBCMO_BUILD_DEBUG) ^ defined(LIBCMO_BUILD_RELEASE))
#error "You must define ONE of LIBCMO_BUILD_DEBUG and LIBCMO_BUILD_RELEASE to indicate build type!"
#endif

// Include YYCC helper library and check its version
#include <YYCCommonplace.hpp>
#if YYCC_VERCMP_NE(YYCC_VER_MAJOR, YYCC_VER_MINOR, YYCC_VER_PATCH, 1, 3, 0)
#error "YYCC library version is not matched with our expected version. Please check your library configuration."
#endif

// Header for this namespace implementation
#include <string>

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
		[[nodiscard]] virtual const char* what() const noexcept override { return message.c_str(); }
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
		[[nodiscard]] virtual const char* what() const noexcept override { return message.c_str(); }
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
		[[nodiscard]] virtual const char* what() const noexcept override { return message.c_str(); }
	private:
		std::string message;
	};

#pragma endregion

}
