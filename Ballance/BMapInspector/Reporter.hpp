#pragma once
#include "Utils.hpp"
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace BMapInspector::Reporter {

	struct Report {
		Utils::ReportLevel level; ///< The level of this report.
		std::u8string rule;       ///< The name of rule adding this report.
		std::u8string content;    ///< The content of this report.
	};

	struct ReporterDigest {
		size_t cnt_err;  ///< The count of error report.
		size_t cnt_warn; ///< The count of warning report.
		size_t cnt_info; ///< The count of info report.
	};

	class Reporter {
	public:
		Reporter();
		~Reporter();
		YYCC_DEFAULT_COPY_MOVE(Reporter)

	private:
		void AddReport(Utils::ReportLevel level, const std::u8string_view& rule, const std::u8string_view& content);

	public:
		void WriteInfo(const std::u8string_view& rule, const std::u8string_view& content);
		void FormatInfo(const std::u8string_view& rule, const char8_t* fmt, ...);
		void WriteWarning(const std::u8string_view& rule, const std::u8string_view& content);
		void FormatWarning(const std::u8string_view& rule, const char8_t* fmt, ...);
		void WriteError(const std::u8string_view& rule, const std::u8string_view& content);
		void FormatError(const std::u8string_view& rule, const char8_t* fmt, ...);

	public:
		ReporterDigest GetDigest() const;
		const std::vector<Report>& GetReports() const;

	private:
		std::vector<Report> reports;
	};

} // namespace BMapInspector
