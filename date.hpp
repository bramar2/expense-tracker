#pragma once

#include <cstdint>
#include <array>
#include <optional>
#include <string>

namespace expensetracker {
	using Date = std::array<uint32_t, 3>;
	std::optional<Date> parse_date(const std::string& dateStr);
}