#pragma once

#include <map>
#include <string>

namespace expensetracker {

	constexpr std::string ARG_ID = "id";
	constexpr std::string ARG_DESCRIPTION = "description";
	constexpr std::string ARG_CATEGORY = "category";
	constexpr std::string ARG_AMOUNT = "amount";
	constexpr std::string ARG_MONTH = "month";
	constexpr std::string ARG_YEAR = "year";
	constexpr std::string ARG_SORT = "sort";


	std::map<std::string, std::string> parse_arg(int argc, char** argv);
}