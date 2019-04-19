cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(DataStructure)

set(CMAKE_CXX_STANDARD 17)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -Wconversion -pedantic -Werror=return-type")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

set(SRC_INCLUDE src src/ThreadPool.h)
add_executable(DataStructure main.cpp ${SRC_INCLUDE})

set(CMAKE_CXX_FLAGS "-O3")