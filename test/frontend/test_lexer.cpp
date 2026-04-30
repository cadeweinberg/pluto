// SPDX-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>

#include "frontend/lexer.hpp"

#include <vector>

TEST_CASE("Lexer keywords", "[Lexer, Frontend]") {
    std::vector<std::pair<std::string_view, pluto::Token>> cases = {
        {"def", pluto::Token::Def},
        {"ptr", pluto::Token::Ptr},
    };

    for (const auto& [input, expected] : cases) {
        pluto::Lexer lexer(input);
        REQUIRE(lexer.next() == expected);
    }
}

TEST_CASE("Lexer symbols", "[Lexer, Frontend]") {
    std::vector<std::pair<std::string_view, pluto::Token>> cases = {
        {"(", pluto::Token::ParenBegin},
        {")", pluto::Token::ParenEnd},
        {"{", pluto::Token::BraceBegin},
        {"}", pluto::Token::BraceEnd},
        {"=", pluto::Token::Equal},
        {":", pluto::Token::Colon},
        {";", pluto::Token::Semicolon},
        {",", pluto::Token::Comma},
    };

    for (const auto& [input, expected] : cases) {
        pluto::Lexer lexer(input);
        REQUIRE(lexer.next() == expected);
    }
}

TEST_CASE("Lexer types", "[Lexer, Frontend]") {
    std::vector<std::pair<std::string_view, pluto::Token>> cases = {
        {"i8", pluto::Token::TypeI8},
        {"i16", pluto::Token::TypeI16},
        {"i32", pluto::Token::TypeI32},
        {"i64", pluto::Token::TypeI64},
        {"u8", pluto::Token::TypeU8},
        {"u16", pluto::Token::TypeU16},
        {"u32", pluto::Token::TypeU32},
        {"u64", pluto::Token::TypeU64},
        {"f32", pluto::Token::TypeF32},
        {"f64", pluto::Token::TypeF64},
        {"bool", pluto::Token::TypeBool},
        {"nil", pluto::Token::TypeNil},
    };

    for (const auto& [input, expected] : cases) {
        pluto::Lexer lexer(input);
        REQUIRE(lexer.next() == expected);
    }
}

TEST_CASE("Lexer literals", "[Lexer, Frontend]") {
    std::vector<std::pair<std::string_view, pluto::Token>> cases = {
        {"123", pluto::Token::LiteralInteger},
        {"3.14", pluto::Token::LiteralFloat},
        {"\"hello\"", pluto::Token::LiteralString},
        {"true", pluto::Token::LiteralTrue},
        {"false", pluto::Token::LiteralFalse},
    };

    for (const auto& [input, expected] : cases) {
        pluto::Lexer lexer(input);
        REQUIRE(lexer.next() == expected);
    }
}

TEST_CASE("Lexer identifiers", "[Lexer, Frontend]") {
    std::vector<std::string_view> cases = {
        "!foo",
        "@bar",
        "%my_variable",
        "@Test123",
    };

    for (const auto& input : cases) {
        pluto::Lexer lexer(input);
        REQUIRE(lexer.next() == pluto::Token::Identifier);
    }
}
