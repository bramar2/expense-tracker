#pragma once

#include <cstdint>
#include <array>
#include <optional>
#include <string>

namespace expensetracker {
	std::optional<std::array<uint32_t, 3>> parse_date(const std::string& dateStr);
}