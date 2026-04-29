// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TI_IR_OPERAND_HPP
#define PLUTO_TI_IR_OPERAND_HPP

#include <cstdint>

#include "ti-ir/index.hpp"

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

#endif // !PLUTO_TI_IR_OPERAND_HPP