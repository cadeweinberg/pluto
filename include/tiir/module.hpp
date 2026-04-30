// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_MODULE_HPP
#define PLUTO_TIIR_MODULE_HPP

#include "tiir/string_interner.hpp"
#include "tiir/type_interner.hpp"
#include "tiir/literal.hpp"

namespace pluto {
class Module {
public:


private:
    StringInterner string_interner;
    TypeInterner   type_interner;
    // Map instructions to SourceLocations
    // a list of errors encountered during runtime.

};
} // namespace pluto

#endif // !PLUTO_TIIR_MODULE_HPP
