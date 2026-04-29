// SPDX-Identifier: GPL-3.0-or-later

#include "ti-ir/type_interner.hpp"
#include "ti-ir/module.hpp"

namespace pluto {
TypeInterner::TypeInterner(class Module &module)
    : nil_type(Type::nil(module))
    , i8_type(Type::i8(module))
    , i16_type(Type::i16(module))
    , i32_type(Type::i32(module))
    , i64_type(Type::i64(module))
    , u8_type(Type::u8(module))
    , u16_type(Type::u16(module))
    , u32_type(Type::u32(module))
    , u64_type(Type::u64(module))
    , f32_type(Type::f32(module))
    , f64_type(Type::f64(module))
    , boolean_type(Type::boolean(module))
    , module(&module)
{}

Type const *TypeInterner::tuple(std::vector<Type const *> elements) {
    // #NOTE: We duplicate the work of computing the layout here,
    // if the type is already interned. We have to some up with a 
    // proxy type which doesn't have a layout, check for equality,
    // and only on a new insert do we compute the layout and create the full type.
    return tuple_types.intern(Type::tuple(elements, *module));
}

Type const *TypeInterner::array(Type const *element_type, size_t count) {
    return array_types.intern(Type::array(element_type, count, *module));
}

Type const *TypeInterner::function(Type const *result, std::vector<Type const *> parameters) {
    return function_types.intern(Type::function(result, std::move(parameters), *module));
}

} // namespace pluto
