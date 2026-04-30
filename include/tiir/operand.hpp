// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_OPERAND_HPP
#define PLUTO_TIIR_OPERAND_HPP

#include <cstdint>

#include "tiir/index.hpp"

namespace pluto {
struct Operand {
    enum class Kind : uint8_t {
        SSA,
        Type,
        Name,
        Literal,
    };
    Kind kind;
    Index index;
};
} // namespace pluto

#endif // !PLUTO_TIIR_OPERAND_HPP