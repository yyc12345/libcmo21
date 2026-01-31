#include "Utils.hpp"
#include <yycc.hpp>
#include <yycc/cenum.hpp>

namespace BMapInspector::Utils {

	std::optional<ReportLevel> ParseReportLevel(const std::u8string_view &value) {
		if (value == u8"error") return ReportLevel::Error;
		else if (value == u8"warning") return ReportLevel::Warning;
		else if (value == u8"info") return ReportLevel::Info;
		return std::nullopt;
	}

	bool FilterReportLevel(ReportLevel check, ReportLevel filter) {
		auto num_check = yycc::cenum::integer(check);
		auto num_filter = yycc::cenum::integer(filter);
		return num_check <= num_filter;
	}

} // namespace BMapInspector::Utils
