#include "Cli.hpp"
#include <yycc.hpp>
#include <yycc/carton/clap.hpp>

namespace clap = yycc::carton::clap;

namespace BMapInspector::Cli {

#pragma region Request

	Request Request::FromHelpRequest() {
		return Request(RequestKind::Help, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
	}

	Request Request::FromVersionRequest() {
		return Request(RequestKind::Version, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
	}

	Request Request::FromWorkRequest(Utils::ReportLevel level,
	                                 const std::u8string_view& file_path,
	                                 const std::u8string_view& encoding,
	                                 const std::u8string_view& ballance_path) {
		return Request(RequestKind::Work, level, file_path, encoding, ballance_path);
	}

	Request::Request(RequestKind kind,
	                 std::optional<Utils::ReportLevel> level,
	                 std::optional<std::u8string_view> file_path,
	                 std::optional<std::u8string_view> encoding,
	                 std::optional<std::u8string_view> ballance_path) :
	    kind(kind), level(level), file_path(file_path), encoding(encoding), ballance_path(ballance_path) {}

	Request::~Request() {}

	RequestKind Request::GetRequestKind() const {
		return this->kind;
	}

	Utils::ReportLevel Request::GetLevel() const {
		if (this->level.has_value()) return this->level.value();
		else throw std::logic_error("can not visit this property in current kind");
	}

	std::u8string_view Request::GetFilePath() const {
		if (this->file_path.has_value()) return this->file_path.value();
		else throw std::logic_error("can not visit this property in current kind");
	}

	std::u8string_view Request::GetEncoding() const {
		if (this->encoding.has_value()) return this->encoding.value();
		else throw std::logic_error("can not visit this property in current kind");
	}

	std::u8string_view Request::GetBallancePath() const {
		if (this->ballance_path.has_value()) return this->ballance_path.value();
		else throw std::logic_error("can not visit this property in current kind");
	}

#pragma endregion

#pragma region Custom Validators

	struct ReportLevelValidator {
		using ReturnType = Utils::ReportLevel;
		std::optional<ReturnType> validate(const std::u8string_view& sv) const { return Utils::ParseReportLevel(sv); }
	};

#pragma endregion


	Result<Request> parse() {
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
Frequently used encoding is "cp1252" and "gbk".
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

		// Create parser and parse command line arguments
		auto rv_parser = clap::parser::Parser::from_system(app);
		if (!rv_parser.has_value()) return std::unexpected(Error::BadParse);
		auto& parser = rv_parser.value();

		// Check version and help first
		if (auto help_flag = parser.get_flag_option(opt_help); help_flag.has_value()) {
			return Request::FromHelpRequest();
		}
		if (auto version_flag = parser.get_flag_option(opt_version); version_flag.has_value()) {
			return Request::FromVersionRequest();
		}

		// Check other args
		std::u8string file_rv;
		if (parser.has_option(opt_file)) {
			auto file_value = parser.get_value_option<clap::validator::StringValidator>(opt_file);
			if (!file_value.has_value()) return std::unexpected(Error::BadFile);
			file_rv = std::move(file_value.value());
		} else {
			return std::unexpected(Error::NoFile);
		}
		std::u8string ballance_rv;
		if (parser.has_option(opt_ballance)) {
			auto ballance_value = parser.get_value_option<clap::validator::StringValidator>(opt_ballance);
			if (!ballance_value.has_value()) return std::unexpected(Error::BadBallance);
			ballance_rv = std::move(ballance_value.value());
		} else {
			return std::unexpected(Error::NoBallance);
		}
		std::u8string encoding_rv;
		if (parser.has_option(opt_encoding)) {
			auto encoding_value = parser.get_value_option<clap::validator::StringValidator>(opt_encoding);
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
		return Request::FromWorkRequest(level_rv, file_rv, encoding_rv, ballance_rv);
	}

} // namespace BMapInspector::Cli
