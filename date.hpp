#pragma once

#include <cstdint>
#include <array>
#include <optional>
#include <string>
#include <map>

namespace expensetracker {
	using Date = std::array<uint32_t, 3>;
	std::optional<Date> parse_date(const std::string& dateStr);
	bool parse_period_args(const std::map<std::string, std::string>& args, Date& start_result, Date& end_result);
	bool date_cmp_less(const Date& a, const Date& b);
	bool date_cmp_less_equal(const Date& a, const Date& b);
}