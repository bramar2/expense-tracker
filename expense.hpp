#pragma once

#include <cstdint>
#include <string>
#include <ostream>

namespace expensetracker {
	class Expense {
	public:
		uint32_t id;

		std::string description;
		std::string category;
		uint32_t amount;
		uint32_t year;
		uint8_t month;
		uint8_t day;
		

		void write(std::ostream& out);
		static std::optional<Expense> read(std::string& row);
	};
}