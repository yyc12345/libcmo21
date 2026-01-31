#include "Reporter.hpp"
#include <yycc/string/op.hpp>
#include <cstdarg>

namespace strop = yycc::string::op;

namespace BMapInspector::Reporter {

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

	ReporterDigest Reporter::GetDigest() const {
		ReporterDigest digest{.cnt_err = 0, .cnt_warn = 0, .cnt_info = 0};
		for (const auto &report : this->reports) {
			switch (report.level) {
				case Utils::ReportLevel::Error:
					++digest.cnt_err;
					break;
				case Utils::ReportLevel::Warning:
					++digest.cnt_warn;
					break;
				case Utils::ReportLevel::Info:
					++digest.cnt_info;
					break;
			}
		}
		return digest;
	}

	const std::vector<Report> &Reporter::GetReports() const {
		return this->reports;
	}

#pragma endregion

} // namespace BMapInspector::Reporter
