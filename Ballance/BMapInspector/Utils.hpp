#pragma once
#include <cstdint>
#include <optional>
#include <string_view>

namespace BMapInspector::Utils {

#define PRIuSIZET "zu"

#define BMAPINSP_NAME "Ballance Map Inspector"
#define BMAPINSP_DESC "The inspector for checking whether your Ballance custom map can be loaded without any issues."

	enum class ReportLevel : std::uint32_t {
		Error = 0,
		Warning = 1,
		Info = 2,
	};

	std::optional<ReportLevel> ParseReportLevel(const std::u8string_view& value);

	/**
	 * @brief Check whether given report level can pass given filter.
	 * @param[in] check The level for checking whether it can pass filter.
	 * @param[in] filter The level of filter.
	 * @return True if is can pass, otherwise false.
	 */
	bool FilterReportLevel(ReportLevel check, ReportLevel filter);

} // namespace BMapInspector::Utils
