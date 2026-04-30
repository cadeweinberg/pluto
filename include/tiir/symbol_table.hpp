// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_SYMBOL_TABLE_HPP
#define PLUTO_TIIR_SYMBOL_TABLE_HPP

#include <variant>

#include "adt/flat_map.hpp"
#include "tiir/function.hpp"
#include "tiir/literal.hpp"
#include "tiir/type.hpp"

namespace pluto {
/**
 * @brief The Symbol Table manages Bindings of names at the Module Scope.
 * The module is concerned with Functions, Types, and Constants.
 * Each of these is represented as a Name paired with A Type and Value.
 */
class SymbolTable {
public:
    struct Symbol {
        uint32_t name;
	uint32_t type;
	uint32_t value;
    };

    Symbol &resolve(uint32_t name) {
	return m_table.at(name).second;
    }

private:
    FlatMap<uint32_t, Symbol> m_table;
};
}

#endif // !PLUTO_TIIR_SYMBOL_TABLE_HPP
