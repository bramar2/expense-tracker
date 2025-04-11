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
		std::string date; // YYYY-MM-DD format
		

		void write(std::ostream& out);
		static std::optional<Expense> read(std::string& row);
	};
}