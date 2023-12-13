cmake_minimum_required(VERSION 3.26)
project(lab3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(lab3 src/main.cpp)
set_target_properties(lab3 PROPERTIES WIN32_EXECUTABLE true)
