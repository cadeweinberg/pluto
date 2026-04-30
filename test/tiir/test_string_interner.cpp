// SPDX-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>

#include "tiir/string_interner.hpp"

TEST_CASE("StringInterner", "[StringInterner, ADT]") {
    pluto::StringInterner interner;
    std::vector<pluto::Index> ids;
    ids.push_back(interner.intern("hello"));
    ids.push_back(interner.intern("world"));
    ids.push_back(interner.intern("hello"));

    std::string_view str1 = interner.view(ids[0]);
    std::string_view str2 = interner.view(ids[1]);
    std::string_view str3 = interner.view(ids[2]);

    REQUIRE(str1 == "hello");
    REQUIRE(str2 == "world");
    REQUIRE(str3 == "hello");

    REQUIRE(str1 == str3);
}