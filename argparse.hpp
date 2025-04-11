#pragma once

#include <map>
#include <string>

namespace expensetracker {

	constexpr std::string ARG_ID = "id";
	constexpr std::string ARG_DESCRIPTION = "description";
	constexpr std::string ARG_CATEGORY = "category";
	constexpr std::string ARG_AMOUNT = "amount";
	constexpr std::string ARG_START_DATE = "start-date";
	constexpr std::string ARG_END_DATE = "end-date";
	constexpr std::string ARG_PERIOD = "period";
	constexpr std::string ARG_SORT = "sort";


	std::map<std::string, std::string> parse_arg(int argc, char** argv);
}