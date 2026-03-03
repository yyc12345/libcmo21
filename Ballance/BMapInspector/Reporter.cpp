#include "Reporter.hpp"
#include <yycc/string/op.hpp>
#include <cstdarg>
#include <stdexcept>

using BMapInspector::Utils::ReportLevel;
namespace strop = yycc::string::op;

namespace BMapInspector::Reporter {

#pragma region Reporter

	Reporter::Reporter() : current_rule(std::nullopt), reports() {}

	Reporter::~Reporter() {}

	void Reporter::EnterRule(const std::u8string_view &rule) {
		if (this->current_rule.has_value()) throw std::logic_error("can not enter rule multiple times");
		else this->current_rule = std::u8string(rule);
	}

	void Reporter::LeaveRule() {
		if (this->current_rule.has_value()) this->current_rule = std::nullopt;
		else throw std::logic_error("can not leave rule without any existing rule");
	}

	void Reporter::AddReport(ReportLevel level, const std::u8string_view &content) {
		if (this->current_rule.has_value()) {
			this->reports.emplace_back(Report{
			    .level = level,
			    .rule = std::u8string(this->current_rule.value()),
			    .content = std::u8string(content),
			});
		} else {
			throw std::logic_error("can not add report without any rule scope");
		}
	}

	void Reporter::WriteInfo(const std::u8string_view &content) {
		this->AddReport(ReportLevel::Info, content);
	}

	void Reporter::FormatInfo(const char8_t *fmt, ...) {
		va_list argptr;
		va_start(argptr, fmt);
		this->WriteInfo(strop::vprintf(fmt, argptr));
		va_end(argptr);
	}

	void Reporter::WriteWarning(const std::u8string_view &content) {
		this->AddReport(ReportLevel::Warning, content);
	}
	void Reporter::FormatWarning(const char8_t *fmt, ...) {
		va_list argptr;
		va_start(argptr, fmt);
		this->WriteWarning(strop::vprintf(fmt, argptr));
		va_end(argptr);
	}

	void Reporter::WriteError(const std::u8string_view &content) {
		this->AddReport(ReportLevel::Error, content);
	}

	void Reporter::FormatError(const char8_t *fmt, ...) {
		va_list argptr;
		va_start(argptr, fmt);
		this->WriteError(strop::vprintf(fmt, argptr));
		va_end(argptr);
	}

	ReporterDigest Reporter::GetDigest() const {
		ReporterDigest digest{.cnt_err = 0, .cnt_warn = 0, .cnt_info = 0};
		for (const auto &report : this->reports) {
			switch (report.level) {
				case ReportLevel::Error:
					++digest.cnt_err;
					break;
				case ReportLevel::Warning:
					++digest.cnt_warn;
					break;
				case ReportLevel::Info:
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
