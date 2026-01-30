#include "Utils.hpp"
#include "Ruleset.hpp"
#include <VTAll.hpp>
#include <yycc.hpp>
#include <yycc/carton/termcolor.hpp>
#include <yycc/string/op.hpp>
#include <yycc/patch/stream.hpp>
#include <iostream>

using namespace yycc::patch::stream;
namespace strop = yycc::string::op;
namespace termcolor = yycc::carton::termcolor;

int main(int argc, char *argv[]) {

    // Show splash
	std::cout << termcolor::colored(u8"Ballance Map Inspector", termcolor::Color::LightYellow)
	          << " (based on LibCmo " LIBCMO_VER_STR ") built at " __DATE__ " " __TIME__ << std::endl
	          << u8"The inspector for checking whether your Ballance custom map can be loaded without any issues." << std::endl
	          << std::endl;

	// Create reporter
	BMapInspector::Utils::Reporter reporter;

	// Get rule collection
	BMapInspector::Ruleset::RuleCollection rule_collection;
	// Show rule infos
	std::cout << strop::printf(u8"Total %" PRIuSIZET " rule(s) are loaded.", rule_collection.GetRuleCount()) << std::endl
	          << u8"Check may take few minutes. Please do not close this console..." << std::endl
	          << std::endl;
	
	// Show report conclusion
	reporter.PrintConclusion();
	// Print report in detail
	reporter.PrintReport();

    return 0;
}
