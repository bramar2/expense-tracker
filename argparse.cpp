#include "argparse.hpp"
#include <stdexcept>

namespace expensetracker {


	const std::map<std::string, std::string> ARG_MAP {
		{"-i", ARG_ID},
		{"-id", ARG_ID},
		{"--id", ARG_ID},

		{"-d", ARG_DESCRIPTION},
		{"--desc", ARG_DESCRIPTION},
		{"--description", ARG_DESCRIPTION},

		{"-c", ARG_CATEGORY},
		{"--cat", ARG_CATEGORY},
		{"--category", ARG_CATEGORY},

		{"-a", ARG_AMOUNT},
		{"--amount", ARG_AMOUNT},

		{"-m", ARG_MONTH},
		{"--month", ARG_MONTH},

		{"-y", ARG_YEAR},
		{"--year", ARG_YEAR}
	};

	std::map<std::string, std::string> parse_arg(int argc, char** argv) {
		std::map<std::string, std::string> res;

		for (int i = 1; i < argc; i++) {
			std::string arg(argv[i]);
			auto it = ARG_MAP.find(arg);
			if (it == ARG_MAP.end()) {
				throw std::invalid_argument("unknown argument (at pos " + std::to_string(i) + "): " + arg);
			}

			if (res.contains(it->second)) {
				throw std::invalid_argument("duplicate argument (at pos " + std::to_string(i) + "): " + it->second);
			}

			if (i + 1 == argc) {
				throw std::invalid_argument("missing argument value (at pos " + std::to_string(i) + "): " + arg);
			}

			i++;
			res[it->second] = std::string(argv[i]);
		}

		return res;
	}
}