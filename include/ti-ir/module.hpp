// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TI_IR_MODULE_HPP
#define PLUTO_TI_IR_MODULE_HPP

#include "ti-ir/string_interner.hpp"
#include "ti-ir/type_interner.hpp"
#include "ti-ir/layout_interner.hpp"
#include "ti-ir/literal.hpp"

namespace pluto {
class Module {
public:
    Layout const *layout(const PrimaryType &type) const;
    Layout const *layout(const TupleType &type) const;
    Layout const *layout(const ArrayType &type) const;
    Layout const *layout(const FunctionType &type) const;
    Layout const *layout(const Type &type) const;

private:
    StringInterner string_interner;
    TypeInterner   type_interner;
    LayoutInterner layout_interner;
    // Map instructions to SourceLocations
    // a list of errors encountered during runtime.

};
} // namespace pluto

#endif // !PLUTO_TI_IR_MODULE_HPP
