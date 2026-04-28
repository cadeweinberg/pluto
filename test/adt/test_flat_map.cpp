// SPDX-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>

#include "adt/flat_map.hpp"

TEST_CASE("FlatMap can insert and retrieve values", "[FlatMap]") {
    pluto::FlatMap<int, std::string> map;

    std::vector<std::pair<int, std::string>> entries = {
        {1, "one"},
        {2, "two"},
        {3, "three"},
        {4, "four"},
        {5, "five"},
        {6, "six"},
        {7, "seven"},
        {8, "eight"},
        {9, "nine"},
        {10, "ten"},
    };

    for (const auto &[key, value] : entries) {
        auto &entry = map.at(key);
        entry.second = value;
    }

    for (const auto &[key, value] : entries) {
        REQUIRE(map.at(key).second == value);
    }
}

