// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_FRONTEND_TOKEN_HPP
#define PLUTO_FRONTEND_TOKEN_HPP

#include <cstdint>

namespace pluto {
enum class Token : uint16_t {
    // Control 
    EndOfFile,

    // Keywords
    Def,
    Type,
    Ptr,

    // Symbols
    ParenBegin,
    ParenEnd,
    BraceBegin,
    BraceEnd,
    BracketBegin,
    BracketEnd,
    Equal,
    Colon,
    Semicolon,
    Comma,

    // Types
    TypeI8,
    TypeI16,
    TypeI32,
    TypeI64,
    TypeU8,
    TypeU16,
    TypeU32,
    TypeU64,
    TypeF32,
    TypeF64,
    TypeBool,
    TypeNil,

    // Literals
    LiteralInteger,
    LiteralFloat,
    LiteralString,
    LiteralTrue,
    LiteralFalse,
    Identifier,

};
}

#endif // !PLUTO_FRONTEND_TOKEN_HPP
