cmake_minimum_required(VERSION 3.5)
project(naivebayes)

set(CMAKE_CXX_STANDARD 14)

add_executable(naivebayes main.cpp main.h)
add_executable(Unitests tests/catch.hpp tests/test.cpp)
