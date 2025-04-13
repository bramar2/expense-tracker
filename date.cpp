#include "date.hpp"
#include "argparse.hpp"

#include <iostream>
#include <charconv>
#include <stdexcept>

namespace expensetracker {
	std::optional<Date> parse_date(const std::string& dateStr) {
		static constexpr uint32_t MAX_YEAR = 9999;
		static constexpr uint32_t MONTHS[13] {
			0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
		};

		if (dateStr.size() > 11) {
			return std::nullopt;
		}

		size_t first = dateStr.find('-');
		if (first == std::string::npos) {
			return std::nullopt;
		}
		size_t second = dateStr.find('-');
		if (second == std::string::npos) second = dateStr.size();
		try {
			Date result;
			const char* data = dateStr.data();

			if (auto res = std::from_chars(data, data + first, result[0]); res.ec != std::errc {}) {
				return std::nullopt;
			}

			if (auto res = std::from_chars(data + first + 1, data + second, result[1]); res.ec != std::errc {}) {
				return std::nullopt;
			}


			if (! (
				result[0] <= MAX_YEAR &&
				1 <= result[1] && result[1] <= std::size(MONTHS))) {
				return std::nullopt;
			}

			if (second != dateStr.size())  {
				if (auto res = std::from_chars(data + second + 1, data + dateStr.size(), result[2]); res.ec != std::errc {}) {
					return std::nullopt;
				}


				uint32_t dayCnt = MONTHS[result[1]];
				// if leap year, add 1
				if (result[1] == 2 && result[0] % 4 == 0 && (result[0] % 100 != 0 || result[0] % 400 == 0)) {
					dayCnt++;
				}

				if (! (1 <= result[2] && result[2] <= dayCnt)) {
					return std::nullopt;
				}
			} else result[2] = 0;


			return result;
		} catch (const std::invalid_argument& exc) {
			return std::nullopt;
		}
	}
	bool parse_period_args(const std::map<std::string, std::string>& args, Date& start_result, Date& end_result) {
		bool has_period = args.contains(ARG_PERIOD);
		bool has_start = args.contains(ARG_START_DATE);
		bool has_end = args.contains(ARG_END_DATE);
		if (has_period && (has_start || has_end)) {
			std::cerr << "error: cannot use period and start-end at the same time\n";
			return false;
		}
		if ((has_start && !has_end) || (has_end && !has_start)) {
			std::cerr << "error: start and end args have to come together\n";
			return false;
		}
		
		if (has_period) {
			if (std::optional<Date> date = parse_date(args.at(ARG_PERIOD)); date.has_value()) {
				start_result = std::move(date.value());
				end_result = start_result;
				if (end_result[2] == 0) end_result[2] = 32;
			} else {
				std::cerr << "error: invalid period date arg\n";
				return false;
			}
		} else if (has_start) {
			if (std::optional<Date> date = parse_date(args.at(ARG_START_DATE)); date.has_value()) {
				start_result = std::move(date.value());
			} else {
				std::cerr << "error: invalid start date arg\n";
				return false;
			}
			if (std::optional<Date> date = parse_date(args.at(ARG_END_DATE)); date.has_value()) {
				end_result = std::move(date.value());
				if (end_result[2] == 0) end_result[2] = 32;
			} else {
				std::cerr << "error: invalid end date arg\n";
				return false;
			}

			if (!date_cmp_less_equal(start_result, end_result)) {
				std::cerr << "error: start date has to come before end date\n";
				return false;
			}
		} else {
			start_result = {0, 1, 1};
			end_result = {9999, 12, 31};
		}
		return true;
	}
	bool date_cmp_less(const Date& a, const Date& b) {
		return (a[0] < b[0]) || (a[0] == b[0] &&
			 (a[1] < b[1]) || (a[1] == b[1] &&
			 (a[2] < b[2])));
	}
	bool date_cmp_less_equal(const Date& a, const Date& b) {
		return (a[0] < b[0]) || (a[0] == b[0] &&
			 (a[1] < b[1]) || (a[1] == b[1] &&
			 (a[2] <= b[2])));
	}
}