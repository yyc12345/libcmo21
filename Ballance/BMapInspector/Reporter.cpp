#include "Reporter.hpp"
#include <yycc/carton/termcolor.hpp>
#include <yycc/string/op.hpp>
#include <yycc/patch/stream.hpp>
#include <iostream>
#include <cstdarg>

using namespace yycc::patch::stream;
namespace strop = yycc::string::op;
namespace termcolor = yycc::carton::termcolor;

namespace BMapInspector {

#pragma region Reporter

	Reporter::Reporter() {}

	Reporter::~Reporter() {}

	void Reporter::AddReport(Utils::ReportLevel level, const std::u8string_view &rule, const std::u8string_view &content) {
		this->reports.emplace_back(Report{
		    .level = level,
		    .rule = std::u8string(rule),
		    .content = std::u8string(content),
		});
	}

	void Reporter::WriteInfo(const std::u8string_view &rule, const std::u8string_view &content) {
		this->AddReport(Utils::ReportLevel::Info, rule, content);
	}

	void Reporter::FormatInfo(const std::u8string_view &rule, const char8_t *fmt, ...) {
		va_list argptr;
		va_start(argptr, fmt);
		this->WriteInfo(rule, strop::vprintf(fmt, argptr));
		va_end(argptr);
	}

	void Reporter::WriteWarning(const std::u8string_view &rule, const std::u8string_view &content) {
		this->AddReport(Utils::ReportLevel::Warning, rule, content);
	}
	void Reporter::FormatWarning(const std::u8string_view &rule, const char8_t *fmt, ...) {
		va_list argptr;
		va_start(argptr, fmt);
		this->WriteWarning(rule, strop::vprintf(fmt, argptr));
		va_end(argptr);
	}

	void Reporter::WriteError(const std::u8string_view &rule, const std::u8string_view &content) {
		this->AddReport(Utils::ReportLevel::Error, rule, content);
	}

	void Reporter::FormatError(const std::u8string_view &rule, const char8_t *fmt, ...) {
		va_list argptr;
		va_start(argptr, fmt);
		this->WriteError(rule, strop::vprintf(fmt, argptr));
		va_end(argptr);
	}

	void Reporter::PrintConclusion() const {
		// Conclude count
		size_t cnt_err = 0, cnt_warn = 0, cnt_info = 0;
		for (const auto &report : this->reports) {
			switch (report.level) {
				case Utils::ReportLevel::Error:
					++cnt_err;
					break;
				case Utils::ReportLevel::Warning:
					++cnt_warn;
					break;
				case Utils::ReportLevel::Info:
					++cnt_info;
					break;
			}
		}

		// Show in console
		termcolor::cprintln(strop::printf(u8"Total %" PRIuSIZET " error(s), %" PRIuSIZET " warning(s) and %" PRIuSIZET " info(s).",
		                                  cnt_err,
		                                  cnt_warn,
		                                  cnt_info),
		                    termcolor::Color::LightBlue);
	}

	void Reporter::PrintReport() const {
		// Print all entries by different color
		for (const auto &report : this->reports) {
			switch (report.level) {
				case Utils::ReportLevel::Error:
					termcolor::cprintln(strop::printf(u8"[ERROR] [RULE: %s] %s", report.rule.c_str(), report.content.c_str()),
					                    termcolor::Color::LightRed);
					break;
				case Utils::ReportLevel::Warning:
					termcolor::cprintln(strop::printf(u8"[WARNING] [RULE: %s] %s", report.rule.c_str(), report.content.c_str()),
					                    termcolor::Color::LightYellow);
					break;
				case Utils::ReportLevel::Info:
					termcolor::cprintln(strop::printf(u8"[INFO] [RULE: %s] %s", report.rule.c_str(), report.content.c_str()),
					                    termcolor::Color::Default);
					break;
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Utils
