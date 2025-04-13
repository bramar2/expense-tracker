#include "argparse.hpp"
#include "expense.hpp"
#include "date.hpp"

#include <array>
#include <chrono>
#include <functional>
#include <vector>
#include <map>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>


using expensetracker::Expense, expensetracker::Date;



constexpr char HELP[] = R"(expense-tracker | github.com/bramar2/expense-tracker
usage: expense-tracker add --description "Lunch" --amount 20 --category "Food"
usage: expense-tracker edit --id 1 --description "Dinner" --amount 300
usage: expense-tracker delete --id 1
usage: expense-tracker view --id 1
usage: expense-tracker list
usage: expense-tracker list --period 2024-08 --sort (id_asc | id_desc | amount_asc | amount_desc | date_asc | date_desc)
usage: expense-tracker list --start 2024-08 --end 2024-10 --category "Food"
usage: expense-tracker summary
usage: expense-tracker summary --category "Food"
usage: expense-tracker summary --period 2024-08
usage: expense-tracker summary --start 2024-08 --end 2024-10)";

constexpr char CSV_FILE[] = "expenses.csv";
constexpr char CSV_HEADER[] = "Id,Amount,Description,Category,Date";
const std::map<std::string, std::function<bool(const Expense&, const Expense&)>> SORTINGS
{
	{"id_asc", [](const Expense& a, const Expense& b) -> bool { return a.id < b.id; }},
	{"id_desc", [](const Expense& a, const Expense& b) -> bool { return a.id > b.id; }},
	{"amount_asc", [](const Expense& a, const Expense& b) -> bool { return a.amount < b.amount; }},
	{"amount_desc", [](const Expense& a, const Expense& b) -> bool { return a.amount > b.amount; }},
	{"date_asc", [](const Expense& a, const Expense& b) -> bool { return expensetracker::date_cmp_less(expensetracker::parse_date(a.date).value(), expensetracker::parse_date(b.date).value()); }},
	{"date_desc", [](const Expense& a, const Expense& b) -> bool { return !expensetracker::date_cmp_less_equal(expensetracker::parse_date(a.date).value(), expensetracker::parse_date(b.date).value()); }},
};

void write_to_csv(const std::vector<Expense>& data) {
	std::ofstream out(CSV_FILE);
	out << CSV_HEADER << '\n';
	for (const Expense& expense : data) {
		expense.write(out);
		out << '\n';
	}
}


void read_csv(std::vector<Expense>& data) {
	if (!std::filesystem::exists(CSV_FILE)) {
		write_to_csv({});
		return;
	}

	std::ifstream in(CSV_FILE);
	std::string row;

	std::getline(in, row);
	while (std::getline(in, row)) {
		if (std::optional<Expense> opt = Expense::read(row); opt.has_value()) {
			data.push_back(std::move(opt.value()));
		}
	}
}
void print_data(const std::vector<Expense>& data) {
	size_t maxId = 2, maxAmount = 6, maxDateLength = 4, maxCatLength = 8, maxDescLength = std::size("Description");

	for (const Expense& exp : data) {
		maxId = std::max(maxId, std::to_string(exp.id).size());
		maxAmount = std::max(maxAmount, std::to_string(exp.amount).size());
		maxDateLength = std::max(maxDateLength, exp.date.size());
		maxCatLength = std::max(maxDateLength, exp.category.size());
		maxDescLength = std::max(maxDateLength, exp.description.size());
	}

	if (!data.empty()) {
		std::cout << "# (" << data.size() << " entries)\n";
	}

	std::cout << "#";

	std::cout << " " << std::left << std::setw(maxId) << "ID";
	std::cout << " " << std::left << std::setw(maxAmount) << "Amount";
	std::cout << " " << std::left << std::setw(maxDateLength) << "Date";
	std::cout << " " << std::left << std::setw(maxCatLength) << "Category";
	std::cout << " " << std::left << std::setw(maxDescLength) << "Description";
	std::cout << '\n';

	if (data.empty()) {
		std::cout << "# No data.\n";
	}

	for (const Expense& exp : data) {
		std::cout << "#";
		std::cout << " " << std::left << std::setw(maxId) << exp.id;
		std::cout << " " << std::left << std::setw(maxAmount) << exp.amount;
		std::cout << " " << std::left << std::setw(maxDateLength) << exp.date;
		std::cout << " " << std::left << std::setw(maxCatLength) << exp.category;
		std::cout << " " << std::left << std::setw(maxDescLength) << exp.description;
		std::cout << '\n';
	}

}

bool stringEqualIgnoreCase(const std::string& a, const std::string& b) {
	if (a.size() != b.size()) return false;
	for (size_t idx = 0; idx < a.size(); idx++) {
		if (a[idx] == b[idx]) continue;
		if (std::isalpha(a[idx]) && std::tolower(a[idx]) == std::tolower(b[idx])) continue;
		return false;
	}
	return true;
}

int cmd_add(std::map<std::string, std::string>& args, std::vector<Expense>& data, bool& updated) {
	if (!args.contains(expensetracker::ARG_DESCRIPTION) || !args.contains(expensetracker::ARG_AMOUNT)) {
		std::cerr << "usage: expense-tracker add --description \"Lunch\" --amount 20 --category \"Food\"\n";
		return 10;
	}
	Expense exp;

	uint64_t maxId = 0;
	for (const Expense& exp : data) {
		maxId = std::max(maxId, exp.id);
	}

	exp.id = maxId + 1;
	exp.description = args[expensetracker::ARG_DESCRIPTION];
	exp.amount = std::stoull(args[expensetracker::ARG_AMOUNT]);
	if (args.contains(expensetracker::ARG_CATEGORY)) {
		exp.category = args[expensetracker::ARG_CATEGORY];
	}

	std::chrono::time_point now {std::chrono::system_clock::now()};
	std::stringstream ymdStream; 
	ymdStream << std::chrono::year_month_day {std::chrono::floor<std::chrono::days>(now)};
	exp.date = ymdStream.str();

	data.push_back(exp);

	std::cout << "# Expense added successfully (ID: " << exp.id << ")\n";
	updated = true;
	return 0;
}
int cmd_edit(std::map<std::string, std::string>& args, std::vector<Expense>& data, bool& updated) {
	if (!args.contains(expensetracker::ARG_ID)) {
		std::cerr << "usage: expense-tracker edit --id 1 --description \"Dinner\" --amount 300\n";
		return 10;
	}

	uint64_t inputId = std::stoull(args[expensetracker::ARG_ID]);
	bool found = false;
	for (Expense& exp : data) {
		if(exp.id == inputId) {
			found = true;
			if (args.contains(expensetracker::ARG_DESCRIPTION)) {
				exp.description = args[expensetracker::ARG_DESCRIPTION];
			}

			if (args.contains(expensetracker::ARG_CATEGORY)) {
				exp.category = args[expensetracker::ARG_CATEGORY];
			}

			if (args.contains(expensetracker::ARG_AMOUNT)) {
				exp.amount = std::stoull(args[expensetracker::ARG_AMOUNT]);
			}
		}
	}
	if (!found) {
		std::cerr << "error: no expense found with ID " << inputId << '\n';
		return 10;
	}
	std::cout << "# Expense updated successfully\n";
	updated = true;
	return 0;
}
int cmd_delete(std::map<std::string, std::string>& args, std::vector<Expense>& data, bool& updated) {
	if (!args.contains(expensetracker::ARG_ID)) {
		std::cerr << "usage: expense-tracker delete --id 1\n";
		return 10;
	}

	uint64_t inputId = std::stoull(args[expensetracker::ARG_ID]);
	bool found = false;
	std::vector<Expense> newData;
	for (const Expense& exp : data) {
		if (exp.id != inputId) {
			newData.push_back(exp);
		} else {
			found = true;
		}
	}
	if (!found) {
		std::cerr << "error: no expense found with ID " << inputId << '\n';
		return 10;
	}
	data = newData;
	updated = true;
	std::cout << "# Expense deleted successfully\n";
	return 0;
}
int cmd_view(std::map<std::string, std::string>& args, std::vector<Expense>& data, bool& updated) {
	if (!args.contains(expensetracker::ARG_ID)) {
		std::cerr << "usage: expense-tracker view --id 1\n";
		return 10;
	}

	uint64_t inputId = std::stoull(args[expensetracker::ARG_ID]);
	Expense* exp = nullptr;

	for (Expense& e : data) {
		if (e.id == inputId) {
			exp = &e;
			break;
		}
	}
	if (exp == nullptr) {
		std::cerr << "error: no expense found with ID " << inputId << '\n';
		return 10;
	}
	print_data({*exp});
	return 0;
}
int cmd_list(std::map<std::string, std::string>& args, std::vector<Expense>& data, bool& updated) {
	Date start_date, end_date;
	if (!expensetracker::parse_period_args(args, start_date, end_date))
		return 10;

	std::string sort_type = args.emplace(expensetracker::ARG_SORT, "id_asc").first->second;
	for (char& c : sort_type) {
		if ('A' <= c && c <= 'Z') c += 32;
	}
	if (!SORTINGS.contains(sort_type)) {
		std::cerr << "error: unknown sorting (" << sort_type << ")\n";
		return 10;	
	}
	std::optional<std::string> category, searchQuery;
	if (args.contains(expensetracker::ARG_CATEGORY)) {
		category = args[expensetracker::ARG_CATEGORY];
	}
	if (args.contains(expensetracker::ARG_SEARCH)) {
		searchQuery = args[expensetracker::ARG_SEARCH];
		for (char& c : searchQuery.value()) {
			if (std::islower(c)) c -= 32;
		}
	}


	std::vector<Expense> filtered_data;
	for (const Expense& exp : data) {
		std::optional<Date> dateOpt = expensetracker::parse_date(exp.date);
		if (!dateOpt.has_value()) continue;
		if (!(expensetracker::date_cmp_less_equal(start_date, dateOpt.value()) &&
			expensetracker::date_cmp_less_equal(dateOpt.value(), end_date))) {
			continue;
		}
		if (category.has_value() && !stringEqualIgnoreCase(exp.category, category.value())) {
			continue;
		}
		if (searchQuery.has_value()) {
			std::string description = exp.description;
			for (char& c : description) {
				if (std::islower(c)) c -= 32;
			}
			if (description.find(searchQuery.value()) == std::string::npos) {
				continue;
			}
		}

		filtered_data.push_back(exp);
	}
	std::sort(filtered_data.begin(), filtered_data.end(), SORTINGS.at(sort_type));

	print_data(filtered_data);
	return 0;
}
int cmd_summary(std::map<std::string, std::string>& args, std::vector<Expense>& data, bool& updated) {
	Date start_date, end_date;
	if (!expensetracker::parse_period_args(args, start_date, end_date))
		return 10;

	std::optional<std::string> category;
	if (args.contains(expensetracker::ARG_CATEGORY)) {
		category = args[expensetracker::ARG_CATEGORY];
	}

	std::vector<Expense> filtered_data;
	for (const Expense& exp : data) {
		std::optional<Date> dateOpt = expensetracker::parse_date(exp.date);
		if (!dateOpt.has_value()) continue;
		if (!(expensetracker::date_cmp_less_equal(start_date, dateOpt.value()) &&
			expensetracker::date_cmp_less_equal(dateOpt.value(), end_date))) {
			continue;
		}
		if (category.has_value() && !stringEqualIgnoreCase(exp.category, category.value())) {
			continue;
		}
		filtered_data.push_back(exp);
	}

	long double total = 0;
	uint64_t min = 0, max = 0;
	if (!filtered_data.empty()) {
		min = max = filtered_data[0].amount;
	}
	for (const Expense& exp : filtered_data) {
		total += exp.amount;
		min = std::min(min, exp.amount);
		max = std::max(min, exp.amount);
	}
	long double avg = (filtered_data.empty() ? 0 : 1.0 * total / filtered_data.size());


	std::cout << "# Summary (" << filtered_data.size() << " entries)\n";
	std::cout << "# Minimum: " << min << '\n';
	std::cout << "# Maximum: " << max << '\n';
	std::cout << "# Average/exp: " << std::fixed << avg << '\n';
	std::cout << "# Total: " << std::fixed << total << '\n';
	return 0;
}

int main(int argc, char** argv) {
	if (argc == 1) {
		std::cout << HELP << '\n';
		return 1;
	}

	std::vector<Expense> data;
	read_csv(data);

	bool updated = false;
	int status = 11;
	try {
		std::string command(argv[1]);
		std::map<std::string, std::string> args = expensetracker::parse_arg(argc, argv);
		if (command == "add") {
			status = cmd_add(args, data, updated);
		} else if (command == "edit") {
			status = cmd_edit(args, data, updated);
		} else if (command == "delete") {
			status = cmd_delete(args, data, updated);
		} else if (command == "view") {
			status = cmd_view(args, data, updated);
		} else if (command == "list") {
			status = cmd_list(args, data, updated);
		} else if (command == "summary") {
			status = cmd_summary(args, data, updated);
		} else {
			std::cerr << "unknown command: " << command << '\n';
			return 2;
		}
	} catch (const std::invalid_argument& err) {
		std::cerr << "error: " << err.what() << '\n';
		return 4;
	}
	if (updated) {
		write_to_csv(data);
	}
	return status;
}