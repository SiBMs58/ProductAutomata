cmake_minimum_required(VERSION 3.25)
project(Productautomaat)

set(CMAKE_CXX_STANDARD 17)

add_executable(Productautomaat main.cpp DFA.cpp json.hpp)
