#include "expense.hpp"

namespace expensetracker {

	void writestr(std::ostream& out, const std::string& str) {
		if(str.find(',') == std::string::npos && str.find('"') == std::string::npos) {
			out << str;
		}else {
			out << '"';
			for (char c : str) {
				if (c == '"') out << R"("")";
				else out << c;
			}
			out << '"';
		}
	}
	void Expense::write(std::ostream& out) const {
		out << id << ",";
		out << amount << ",";
		writestr(out, description); out << ",";
		writestr(out, category); out << ",";
		writestr(out, date);
	}


	bool readstr(const std::string& row, size_t& start, std::string& res) {
		if (start >= row.size()) {
			return false;
		}
		if (row[start] != '"') {
			size_t comma = row.find(',', start);
			if (comma == std::string::npos) {
				comma = row.size();
			}
			res = row.substr(start, comma - start);
			start = comma + 1;
			return true;
		} else {
			size_t size = row.size();
			for (; start < size; start++) {
				if (row[start] == '"') {
					if (start + 1 == size || row[start + 1] == ',') {
						return true;
					}
					if (row[start + 1] == '"') {
						res.push_back('"');
						start++;
					} else {
						return false;
					}
				} else {
					res.push_back(row[start]);
				}
			}
			return false;
		}
	}

	std::optional<Expense> Expense::read(std::string& row) {
		Expense exp;

		size_t start = 0;

		if (size_t comma = row.find(',', start); comma != std::string::npos &&
			std::from_chars(row.c_str() + start, row.c_str() + comma, exp.id).ec == std::errc{}) {
			start = comma + 1;
		} else return std::nullopt;

		if (size_t comma = row.find(',', start); comma != std::string::npos &&
			std::from_chars(row.c_str() + start, row.c_str() + comma, exp.amount).ec == std::errc{}) {
			start = comma + 1;
		} else return std::nullopt;

		if (!readstr(row, start, exp.description)) {
			return std::nullopt;
		}

		if (!readstr(row, start, exp.category)) {
			return std::nullopt;
		}

		if (!readstr(row, start, exp.date)) {
			return std::nullopt;
		}

		return exp;
	}
}