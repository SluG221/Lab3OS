cmake_minimum_required(VERSION 3.26)
project(lab3)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

add_test(NAME MyTests COMMAND MyTests)

add_executable(lab3 src/main.cpp)
