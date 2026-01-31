#include "Utils.hpp"
#include "Cli.hpp"
#include "Reporter.hpp"
#include "Ruleset.hpp"
#include <VTAll.hpp>
#include <yycc.hpp>
#include <yycc/carton/termcolor.hpp>
#include <yycc/string/op.hpp>
#include <yycc/patch/stream.hpp>
#include <iostream>
#include <optional>

using namespace yycc::patch::stream;
namespace strop = yycc::string::op;
namespace termcolor = yycc::carton::termcolor;
using termcolor::Color;

static void PrintSplash() {
	std::cout << termcolor::colored(u8"" BMAPINSP_NAME, Color::LightYellow)
	          << " (based on LibCmo " LIBCMO_VER_STR ") built at " __DATE__ " " __TIME__ << std::endl
	          << u8"" BMAPINSP_DESC << std::endl;
}

static std::optional<BMapInspector::Cli::Args> AcceptArgs() {
	auto request = BMapInspector::Cli::parse();
	if (request.has_value()) {
		return request.value();
	} else {
		using BMapInspector::Cli::Error;

		std::u8string err_words;
		switch (request.error()) {
			case Error::BadParse:
				err_words = u8"Can not parse given command line argument.";
				break;
			case Error::NoFile:
				err_words = u8"You must specify a file for checking.";
				break;
			case Error::BadFile:
				err_words = u8"Your specified file is invalid.";
				break;
			case Error::NoBallance:
				err_words = u8"You must specify Ballance root directory for finding resources.";
				break;
			case Error::BadBallance:
				err_words = u8"Your specified Ballance root directory is invalid.";
				break;
			case Error::BadEncoding:
				err_words = u8"Your specified encoding name is invalid.";
				break;
			case Error::BadLevel:
				err_words = u8"Your specified report level filter name is invalid.";
				break;
			default:
				err_words = u8"Unknown error.";
				break;
		}

		termcolor::cprintln(err_words, Color::Red);
		termcolor::cprintln(u8"Please browse manual or use -h switch to see help first.", Color::Red);
		return std::nullopt;
	}
}

static std::optional<BMapInspector::Ruleset::RuleContext> LoadVirtools(BMapInspector::Cli::Args& args) {
	return std::nullopt;
}

static void CheckRules(BMapInspector::Ruleset::RuleContext& ctx) {
	// Create reporter
	BMapInspector::Reporter::Reporter reporter;

	// Get rule collection
	BMapInspector::Ruleset::RuleCollection rule_collection;
	// Show rule infos
	std::cout << strop::printf(u8"Total %" PRIuSIZET " rule(s) are loaded.", rule_collection.GetRuleCount()) << std::endl
	          << u8"Check may take few minutes. Please do not close this console..." << std::endl;

	// Check rules one by one
	for (auto* rule : rule_collection.GetRules()) {
		rule->Check(reporter, ctx);
	}

	// Show report conclusion
	auto digest = reporter.GetDigest();
	termcolor::cprintln(strop::printf(u8"Total %" PRIuSIZET " error(s), %" PRIuSIZET " warning(s) and %" PRIuSIZET " info(s).",
	                                  digest.cnt_err,
	                                  digest.cnt_warn,
	                                  digest.cnt_info),
	                    Color::LightYellow);
	// Print report in detail
	using BMapInspector::Utils::ReportLevel;
	for (const auto& report : reporter.GetReports()) {
		switch (report.level) {
			case ReportLevel::Error:
				termcolor::cprintln(strop::printf(u8"[ERROR] [RULE: %s] %s", report.rule.c_str(), report.content.c_str()), Color::Red);
				break;
			case ReportLevel::Warning:
				termcolor::cprintln(strop::printf(u8"[WARNING] [RULE: %s] %s", report.rule.c_str(), report.content.c_str()), Color::Yellow);
				break;
			case ReportLevel::Info:
				termcolor::cprintln(strop::printf(u8"[INFO] [RULE: %s] %s", report.rule.c_str(), report.content.c_str()), Color::White);
				break;
		}
	}
}

int main(int argc, char* argv[]) {
	auto args = AcceptArgs();
	if (args.has_value()) {
		PrintSplash();
		std::cout << std::endl;

		auto ctx = LoadVirtools(args.value());
		if (ctx.has_value()) {
			CheckRules(ctx.value());
		}
	}
	return 0;
}
