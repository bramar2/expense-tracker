#include "date.hpp"

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
	bool date_cmp_less(const Date& a, const Date& b) {
		return (a[0] < b[0]) || (a[0] == b[0] &&
			 (a[1] < b[1]) || (a[1] == b[1] &&
			 (a[2] < b[2])));
	}
}