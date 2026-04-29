// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TI_IR_TYPE_INTERNER_HPP
#define PLUTO_TI_IR_TYPE_INTERNER_HPP

#include <vector>
#include <memory>

#include "ti-ir/type.hpp"

namespace pluto {
class TypeInterner {
    struct List {
        std::vector<std::unique_ptr<Type>> types;

        Type const *intern(std::unique_ptr<Type> type) {
            for (const auto &t : types) {
                if (*t == *type) {
                    return t.get();
                }
            }
            types.push_back(std::move(type));
            return types.back().get();
        }
    };

    std::unique_ptr<Type> nil_type;
    std::unique_ptr<Type> i8_type;
    std::unique_ptr<Type> i16_type;
    std::unique_ptr<Type> i32_type;
    std::unique_ptr<Type> i64_type;
    std::unique_ptr<Type> u8_type;
    std::unique_ptr<Type> u16_type;
    std::unique_ptr<Type> u32_type;
    std::unique_ptr<Type> u64_type;
    std::unique_ptr<Type> f32_type;
    std::unique_ptr<Type> f64_type;
    std::unique_ptr<Type> boolean_type;
    List tuple_types;
    List array_types;
    List function_types;
    class Module *module;

public:
    TypeInterner(class Module &module);

    Type const *nil() const { return nil_type.get(); }
    Type const *i8() const { return i8_type.get(); }
    Type const *i16() const { return i16_type.get(); }
    Type const *i32() const { return i32_type.get(); }
    Type const *i64() const { return i64_type.get(); }
    Type const *u8() const { return u8_type.get(); }
    Type const *u16() const { return u16_type.get(); }
    Type const *u32() const { return u32_type.get(); }
    Type const *u64() const { return u64_type.get(); }
    Type const *f32() const { return f32_type.get(); }
    Type const *f64() const { return f64_type.get(); }
    Type const *boolean() const { return boolean_type.get(); }
    Type const *tuple(std::vector<Type const *> elements);
    Type const *array(Type const *element_type, size_t count);
    Type const *function(Type const *result, std::vector<Type const *> parameters);
};
} // namespace pluto

#endif // !PLUTO_TI_IR_TYPE_INTERNER_HPP