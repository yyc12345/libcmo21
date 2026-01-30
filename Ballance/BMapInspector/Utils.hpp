#pragma once
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace BMapInspector::Utils {

	enum class ReportKind { Error, Warning, Info };

	struct Report {
		ReportKind kind;       ///< The kind of this report.
		std::u8string rule;    ///< The name of rule adding this report.
		std::u8string content; ///< The content of this report.
	};

	class Reporter {
	public:
		Reporter();
		~Reporter();
		YYCC_DEFAULT_COPY_MOVE(Reporter)

	private:
		void AddReport(ReportKind kind, const std::u8string_view& rule, const std::u8string_view& content);

	public:
		void WriteInfo(const std::u8string_view& rule, const std::u8string_view& content);
		void FormatInfo(const std::u8string_view& rule, const char8_t* fmt, ...);
		void WriteWarning(const std::u8string_view& rule, const std::u8string_view& content);
		void FormatWarning(const std::u8string_view& rule, const char8_t* fmt, ...);
		void WriteError(const std::u8string_view& rule, const std::u8string_view& content);
		void FormatError(const std::u8string_view& rule, const char8_t* fmt, ...);

	public:
		void PrintConclusion() const;
		void PrintDetails() const;

	private:
		std::vector<Report> reports;
	};

} // namespace BMapInspector::Utils
