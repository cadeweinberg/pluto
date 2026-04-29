// SPDX-Identifier: GPL-3.0-or-later

#include "ti-ir/type_interner.hpp"
namespace pluto {
TypeInterner::TypeInterner()
{}

uint32_t TypeInterner::intern(std::unique_ptr<Type> type) const {
    for (const auto &entry : storage) {
        if (*entry.second == *type) {
            return entry.first;
        }
    }

    size_t index = storage.size();
    assert(index < UINT32_MAX);
    storage.emplace_back(static_cast<uint32_t>(index), std::move(type));
    return static_cast<uint32_t>(index);
}

uint32_t TypeInterner::nil() const {
    return intern(Type::nil());
}

uint32_t TypeInterner::boolean() const {
    return intern(Type::boolean());
}

uint32_t TypeInterner::i8() const {
    return intern(Type::i8());
}

uint32_t TypeInterner::i16() const {
    return intern(Type::i16());
}

uint32_t TypeInterner::i32() const {
    return intern(Type::i32());
}

uint32_t TypeInterner::i64() const {
    return intern(Type::i64());
}

uint32_t TypeInterner::u8() const {
    return intern(Type::u8());
}

uint32_t TypeInterner::u16() const {
    return intern(Type::u16());
}

uint32_t TypeInterner::u32() const {
    return intern(Type::u32());
}

uint32_t TypeInterner::u64() const {
    return intern(Type::u64());
}

uint32_t TypeInterner::f32() const {
    return intern(Type::f32());
}

uint32_t TypeInterner::f64() const {
    return intern(Type::f64());
}

uint32_t TypeInterner::ptr() const {
    return intern(Type::ptr());
}

uint32_t TypeInterner::tuple(std::vector<Type const *> elements) {
    TupleType tuple_type{std::move(elements)};
    return intern(std::make_unique<Type>(tuple_type));
}

uint32_t TypeInterner::array(Type const *element_type, size_t count) {
    ArrayType array_type{element_type, count};
    return intern(std::make_unique<Type>(array_type));
}

uint32_t TypeInterner::function(Type const *result, std::vector<Type const *> parameters) {
    FunctionType function_type{result, std::move(parameters)};
    return intern(std::make_unique<Type>(function_type));
}

} // namespace pluto
