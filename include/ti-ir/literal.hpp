// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TI_IR_LITERAL_HPP
#define PLUTO_TI_IR_LITERAL_HPP

#include "ti-ir/index.hpp"
#include "ti-ir/operand.hpp"

namespace pluto {
struct Literal {
    enum class Kind : uint8_t {
        I8,
        I16,
        I32,
        I64,
        U8,
        U16,
        U32,
        U64,
        F32,
        F64,
        String,
        Boolean,
        Nil,
    };

    // Payload index into a kind-specific table below.
    // For Nil, payload is ignored.
    // For String, payload is an index into the module's string interner.
    // For I8, I16, I32, U8, U16, U32, payload the literal value itself.
    // For I64 and U64, payload is an index into the module's i64_literals or u64_literals table.
    // For Float, payload is an index into the module's literal tables.
    // For Boolean, payload is 0 for false and 1 for true.
    Kind  kind;
    Index data;
};
} // namespace pluto

#endif // !PLUTO_TI_IR_LITERAL_HPP
