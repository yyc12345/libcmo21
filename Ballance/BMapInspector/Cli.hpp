#pragma once
#include "Utils.hpp"
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <string>
#include <optional>
#include <expected>

namespace BMapInspector::Cli {

	struct Args {
		Utils::ReportLevel level;    ///< The filter level.
		std::u8string file_path;     ///< The path to loaded map file.
		std::u8string encoding;      ///< The encoding used when loading map file.
		std::u8string ballance_path; ///< The path to Ballance root directory for loading resources.
	};

	enum class Error {
		BadParse,    ///< Error occurs when executing parser.
		NoFile,      ///< User do not specify file path for loading.
		BadFile,     ///< User specified file path is bad.
		NoBallance,  ///< User do not specify Ballance directory for loading.
		BadBallance, ///< User specified Ballance directory is bad.
		BadEncoding, ///< User given encoding value is bad.
		BadLevel,    ///< User given level name is bad.
	};

	template<typename T>
	using Result = std::expected<T, Error>;

	Result<std::optional<Args>> parse();

} // namespace BMapInspector::Cli
