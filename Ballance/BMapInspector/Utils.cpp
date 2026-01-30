#include "Utils.hpp"
#include <yycc/carton/termcolor.hpp>
#include <yycc/string/op.hpp>
#include <yycc/patch/stream.hpp>
#include <iostream>
#include <cstdarg>

using namespace yycc::patch::stream;
namespace strop = yycc::string::op;
namespace termcolor = yycc::carton::termcolor;

namespace BMapInspector::Utils {

#pragma region Reporter

	Reporter::Reporter() {}

	Reporter::~Reporter() {}

	void Reporter::AddReport(ReportKind kind, const std::u8string_view &rule, const std::u8string_view &content) {
		this->reports.emplace_back(Report{
		    .kind = kind,
		    .rule = std::u8string(rule),
		    .content = std::u8string(content),
		});
	}

	void Reporter::WriteInfo(const std::u8string_view &rule, const std::u8string_view &content) {
		this->AddReport(ReportKind::Info, rule, content);
	}

	void Reporter::FormatInfo(const std::u8string_view &rule, const char8_t *fmt, ...) {
		va_list argptr;
		va_start(argptr, fmt);
		this->WriteInfo(rule, strop::vprintf(fmt, argptr));
		va_end(argptr);
	}

	void Reporter::WriteWarning(const std::u8string_view &rule, const std::u8string_view &content) {
		this->AddReport(ReportKind::Warning, rule, content);
	}
	void Reporter::FormatWarning(const std::u8string_view &rule, const char8_t *fmt, ...) {
		va_list argptr;
		va_start(argptr, fmt);
		this->WriteWarning(rule, strop::vprintf(fmt, argptr));
		va_end(argptr);
	}

	void Reporter::WriteError(const std::u8string_view &rule, const std::u8string_view &content) {
		this->AddReport(ReportKind::Error, rule, content);
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
			switch (report.kind) {
				case ReportKind::Error:
					++cnt_err;
					break;
				case ReportKind::Warning:
					++cnt_warn;
					break;
				case ReportKind::Info:
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
			switch (report.kind) {
				case ReportKind::Error:
					termcolor::cprintln(strop::printf(u8"[ERROR] [RULE: %s] %s", report.rule.c_str(), report.content.c_str()),
					                    termcolor::Color::LightRed);
					break;
				case ReportKind::Warning:
					termcolor::cprintln(strop::printf(u8"[WARNING] [RULE: %s] %s", report.rule.c_str(), report.content.c_str()),
					                    termcolor::Color::LightYellow);
					break;
				case ReportKind::Info:
					termcolor::cprintln(strop::printf(u8"[INFO] [RULE: %s] %s", report.rule.c_str(), report.content.c_str()),
					                    termcolor::Color::Default);
					break;
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Utils
