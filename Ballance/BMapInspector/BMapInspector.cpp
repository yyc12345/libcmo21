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

static void PrintSplash() {
	std::cout << termcolor::colored(u8"" BMAPINSP_NAME, termcolor::Color::LightYellow)
	          << " (based on LibCmo " LIBCMO_VER_STR ") built at " __DATE__ " " __TIME__ << std::endl
	          << u8"" BMAPINSP_DESC << std::endl;
}

static std::optional<BMapInspector::Cli::Args> AcceptArgs() {
	auto request = BMapInspector::Cli::parse();
	if (request.has_value()) {
		return request.value();
	} else {
		using BMapInspector::Cli::Error;

		std::u8string bad_words;
		switch (request.error()) {
			case Error::BadParse:
			    bad_words = u8"Can not parse given command line argument."; break;
			    case Error::NoFile:
				bad_words = u8"You must specify a file for checking.";
				break;
			case Error::BadFile:
				bad_words = u8"Your specified file is invalid.";
				break;
			case Error::NoBallance:
				bad_words = u8"You must specify Ballance root directory for finding resources.";
				break;
			case Error::BadBallance:
				bad_words = u8"Your specified Ballance root directory is invalid.";
				break;
			case Error::BadEncoding:
				bad_words = u8"Your specified encoding name is invalid.";
				break;
			case Error::BadLevel:
				bad_words = u8"Your specified report level filter name is invalid.";
				break;
			default:
				bad_words = u8"Unknown error.";
				break;
		}

		termcolor::cprintln(bad_words, termcolor::Color::Red);
		termcolor::cprintln(u8"Please browse manual or use -h switch to see help first.", termcolor::Color::Red);
		return std::nullopt;
	}
}

static void LoadVirtools() {}

static void CheckRules() {
	// Create reporter
	BMapInspector::Reporter reporter;

	// Get rule collection
	BMapInspector::Ruleset::RuleCollection rule_collection;
	// Show rule infos
	std::cout << strop::printf(u8"Total %" PRIuSIZET " rule(s) are loaded.", rule_collection.GetRuleCount()) << std::endl
	          << u8"Check may take few minutes. Please do not close this console..." << std::endl;

	// Show report conclusion
	reporter.PrintConclusion();
	// Print report in detail
	reporter.PrintReport();
}

int main(int argc, char *argv[]) {
	auto args = AcceptArgs();
	if (args.has_value()) {
		PrintSplash();
		std::cout << std::endl;

		CheckRules();
	}
	return 0;
}
