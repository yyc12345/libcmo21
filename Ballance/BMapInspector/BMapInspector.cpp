#include "Utils.hpp"
#include <VTAll.hpp>
#include <yycc.hpp>
#include <yycc/carton/termcolor.hpp>
#include <yycc/patch/stream.hpp>
#include <iostream>

using namespace yycc::patch::stream;
namespace termcolor = yycc::carton::termcolor;

int main(int argc, char *argv[]) {

    // Show splash
	std::cout << termcolor::colored(u8"Ballance Map Inspector", termcolor::Color::LightYellow)
	          << " (based on LibCmo " LIBCMO_VER_STR ") built at " __DATE__ " " __TIME__ << std::endl
	          << u8"The inspector for checking whether your Ballance custom map can be loaded without any issues." << std::endl;

	// Create reporter
	BMapInspector::Utils::Reporter reporter;

	// Show report
	reporter.PrintConclusion();
	reporter.PrintDetails();

    return 0;
}
