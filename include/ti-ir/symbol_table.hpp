// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TI_IR_SYMBOL_TABLE_HPP
#define PLUTO_TI_IR_SYMBOL_TABLE_HPP

#include "ti-ir/module.hpp"
#include "ti-ir/function.hpp"
#include "ti-ir/literal.hpp"
#include "ti-ir/type.hpp"

namespace pluto {
class SymbolTable {
public:
    struct Symbol {
        enum class Kind {
            Let,
            Type,
            Function,
        };
        Kind kind;
        
    };

private:
};
}

#endif // !PLUTO_TI_IR_SYMBOL_TABLE_HPP