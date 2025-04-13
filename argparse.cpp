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

		{"-s", ARG_START_DATE},
		{"--start", ARG_START_DATE},

		{"-e", ARG_END_DATE},
		{"--end", ARG_END_DATE},

		{"-p", ARG_PERIOD},
		{"--period", ARG_PERIOD},

		{"-r", ARG_SORT},
		{"--sort", ARG_SORT}
	};

	std::map<std::string, std::string> parse_arg(int argc, char** argv) {
		std::map<std::string, std::string> res;

		for (int i = 2; i < argc; i++) {
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