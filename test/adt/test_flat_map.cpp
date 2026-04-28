// SPDX-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "adt/flat_map.hpp"

TEST_CASE("FlatMap", "[FlatMap, ADT]") {
    pluto::FlatMap<int, std::string> map;

    std::vector<std::pair<int, std::string>> entries;

    for (int i = 0; i < 256; ++i) {
        entries.emplace_back(i, std::to_string(i));
    }

    for (const auto &[key, value] : entries) {
        auto &entry = map.at(key);
        entry.second = value;
    }

    for (const auto &[key, value] : entries) {
        REQUIRE(map.at(key).second == value);
    }

    // BENCHMARK("FlatMap.at(key)") {
    //     return map.at(64).second;
    // };
}

