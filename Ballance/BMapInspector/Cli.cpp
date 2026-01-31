#include "Cli.hpp"
#include <yycc.hpp>
#include <yycc/carton/clap.hpp>
#include <filesystem>

namespace clap = yycc::carton::clap;

namespace BMapInspector::Cli {

#pragma region Custom Validators

	struct ReportLevelValidator {
		using ReturnType = Utils::ReportLevel;
		std::optional<ReturnType> validate(const std::u8string_view& sv) const { return Utils::ParseReportLevel(sv); }
	};

	struct MapFileValidator {
		using ReturnType = std::u8string;
		std::optional<ReturnType> validate(const std::u8string_view& sv) const {
			std::filesystem::path p(sv);
			if (std::filesystem::is_regular_file(p)) return std::u8string(sv);
			else return std::nullopt;
		}
	};

	struct BlcDirValidator {
		using ReturnType = std::u8string;
		std::optional<ReturnType> validate(const std::u8string_view& sv) const {
			std::filesystem::path p(sv);
			auto tdb = p / u8"Database.tdb";
			if (std::filesystem::is_directory(p) && std::filesystem::is_regular_file(tdb)) return std::u8string(sv);
			else return std::nullopt;
		}
	};

	struct EncNameValidator {
		using ReturnType = std::u8string;
		std::optional<ReturnType> validate(const std::u8string_view& sv) const { 
			// TODO: use checker for checking this name first.
			return std::u8string(sv);
		}
	};

#pragma endregion

	Result<std::optional<Args>> parse() {
		// Create options
		clap::option::OptionCollection opt_collection;
		auto opt_file = opt_collection.add_option(
		    clap::option::Option(u8"i", u8"file", u8"FILE", u8R"(The path to map file loaded by this program.
This field is required.)"));
		auto opt_ballance = opt_collection.add_option(
		    clap::option::Option(u8"b", u8"ballance", u8"DIR", u8R"(The path to your Ballance root directory for finding resources.
This field is required.)"));
		auto opt_encoding = opt_collection.add_option(
		    clap::option::Option(u8"e", u8"encoding", u8"ENC", u8R"(The encoding used when loading this map file.
Frequently used encodings are "cp1252" and "gbk".
Default value is "cp1252".)"));
		auto opt_level = opt_collection.add_option(
		    clap::option::Option(u8"l", u8"level", u8"LEVEL", u8R"(Set the filter level for checker output.
Available levels are "error", "warning" and "info".
Default value is "info".)"));
		auto opt_version = opt_collection.add_option(
		    clap::option::Option(u8"v", u8"version", std::nullopt, u8"Print version of this program."));
		auto opt_help = opt_collection.add_option(clap::option::Option(u8"h", u8"help", std::nullopt, u8"Print this page."));
		// Create variables
		clap::variable::VariableCollection var_collection;
		// Create manifest
		clap::summary::Summary summary(u8"" BMAPINSP_NAME, u8"yyc12345", u8"Universal", u8"" BMAPINSP_DESC);
		// Create application
		clap::application::Application app(std::move(summary), std::move(opt_collection), std::move(var_collection));
		// Create manual
		clap::manual::Manual manual(app);

		// Create parser and parse command line arguments
		auto rv_parser = clap::parser::Parser::from_system(app);
		if (!rv_parser.has_value()) return std::unexpected(Error::BadParse);
		auto& parser = rv_parser.value();

		// Check version and help first
		if (auto help_flag = parser.get_flag_option(opt_help); help_flag.has_value() && help_flag.value()) {
			manual.print_help();
			return std::nullopt;
		}
		if (auto version_flag = parser.get_flag_option(opt_version); version_flag.has_value() && version_flag.value()) {
			manual.print_version();
			return std::nullopt;
		}

		// Check other args
		std::u8string file_rv;
		if (parser.has_option(opt_file)) {
			auto file_value = parser.get_value_option<MapFileValidator>(opt_file);
			if (!file_value.has_value()) return std::unexpected(Error::BadFile);
			file_rv = std::move(file_value.value());
		} else {
			return std::unexpected(Error::NoFile);
		}
		std::u8string ballance_rv;
		if (parser.has_option(opt_ballance)) {
			auto ballance_value = parser.get_value_option<BlcDirValidator>(opt_ballance);
			if (!ballance_value.has_value()) return std::unexpected(Error::BadBallance);
			ballance_rv = std::move(ballance_value.value());
		} else {
			return std::unexpected(Error::NoBallance);
		}
		std::u8string encoding_rv;
		if (parser.has_option(opt_encoding)) {
			auto encoding_value = parser.get_value_option<EncNameValidator>(opt_encoding);
			if (!encoding_value.has_value()) return std::unexpected(Error::BadEncoding);
			encoding_rv = std::move(encoding_value.value());
		} else {
			encoding_rv = u8"cp1252";
		}
		Utils::ReportLevel level_rv;
		if (parser.has_option(opt_level)) {
			auto level_value = parser.get_value_option<ReportLevelValidator>(opt_level);
			if (!level_value.has_value()) return std::unexpected(Error::BadLevel);
			level_rv = std::move(level_value.value());
		} else {
			level_rv = Utils::ReportLevel::Info;
		}

		// Return result
		return Args{
		    .level = level_rv,
		    .file_path = file_rv,
		    .encoding = encoding_rv,
		    .ballance_path = ballance_rv,
		};
	}

} // namespace BMapInspector::Cli
