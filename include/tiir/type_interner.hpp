// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_TYPE_INTERNER_HPP
#define PLUTO_TIIR_TYPE_INTERNER_HPP

#include <cassert>
#include <vector>
#include <memory>

#include "tiir/type.hpp"

namespace pluto {
class TypeInterner {
    using Element = std::pair<uint32_t, std::unique_ptr<Type>>;
    using Storage = std::vector<Element>;
    mutable Storage storage;

    uint32_t intern(std::unique_ptr<Type>type) const;

public:
    TypeInterner();

    Type const *at(uint32_t index) const {
        assert(index < storage.size());
        return storage[index].second.get();
    }

    uint32_t nil() const;
    uint32_t boolean() const;
    uint32_t i8() const;
    uint32_t i16() const;
    uint32_t i32() const;
    uint32_t i64() const;
    uint32_t u8() const;
    uint32_t u16() const;
    uint32_t u32() const;
    uint32_t u64() const;
    uint32_t f32() const;
    uint32_t f64() const;
    uint32_t ptr() const;
    uint32_t tuple(std::vector<Type const *> elements);
    uint32_t array(Type const *element_type, size_t count);
    uint32_t function(Type const *result, std::vector<Type const *> parameters);
};
} // namespace pluto

#endif // !PLUTO_TIIR_TYPE_INTERNER_HPP