# expense-tracker: A CLI tool for managing your expenses
https://roadmap.sh/projects/expense-tracker
## Code
CMake and C++23 standard.
## Building
On root, run the following commands:
- `cmake -S . -B build/`
- `cmake --build build/`

The executable should be in build/expense-tracker.exe.

## Preview
```
expense-tracker | github.com/bramar2/expense-tracker
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
usage: expense-tracker summary --start 2024-08 --end 2024-10
```
![image](https://github.com/user-attachments/assets/6dc87873-70b6-42cf-9f92-b2de598437ea)
