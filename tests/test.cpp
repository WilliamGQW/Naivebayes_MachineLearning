#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;

// Combine i, j, f, clazz to a string, as a key passed into a map
string generate_str(int i, int j, int f, int clazz);

string generate_str(int i, int j, int f, int clazz) {
    stringstream ss;
    ss << i << "_";
    ss << j << "_";
    ss << f << "_";
    ss << clazz;

    return ss.str();
}

TEST_CASE("Test for the generate_str function") {
    int i = 0;
    int j = 1;
    int f = 2;
    int clazz = 3;
    REQUIRE(generate_str(i, j, f, clazz) == "0_1_2_3");
}

TEST_CASE("More Test for the generate_str function") {
    int i = 11;
    int j = 0;
    int f = 2;
    int clazz = 5;
    REQUIRE(generate_str(i, j, f, clazz) == "11_0_2_5");
}

TEST_CASE("Another test for the generate_str function") {
    int i = 12;
    int j = 50;
    int f = 1;
    int clazz = 2;
    REQUIRE(generate_str(i, j, f, clazz) == "12_50_1_2");
}

TEST_CASE("Another test for the generate_str function which includes double and should PASS") {
    double i = 1.1;
    int j = 1;
    int f = 2;
    int clazz = 3;
    REQUIRE(generate_str(i, j, f, clazz) == "1_1_2_3");
}

TEST_CASE("Another test for the generate_str function which includes double and should FAIL") {
    double i = 1.1;
    int j = 1;
    int f = 2;
    int clazz = 3;
    REQUIRE(generate_str(i, j, f, clazz) == "1.1_1_2_3");
}