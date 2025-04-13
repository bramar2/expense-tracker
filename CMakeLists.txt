cmake_minimum_required(VERSION 3.27)
project(expense-tracker)

set(CMAKE_CXX_STANDARD 23)

add_executable(expense-tracker program.cpp expense.cpp expense.hpp argparse.cpp argparse.hpp date.cpp date.hpp)