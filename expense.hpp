#pragma once

#include <cstdint>
#include <string>
#include <ostream>

namespace expensetracker {
	class Expense {
	public:
		uint64_t id;
		uint64_t amount;

		std::string description;
		std::string category;
		std::string date; // YYYY-MM-DD format
		

		void write(std::ostream& out) const;
		static std::optional<Expense> read(std::string& row);
	};
}