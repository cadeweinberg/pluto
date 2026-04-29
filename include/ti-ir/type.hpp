// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TI_IR_TYPE_HPP
#define PLUTO_TI_IR_TYPE_HPP

#include <vector>
#include <variant>

#include "ti-ir/layout.hpp"

namespace pluto {
struct Type;
class Module;

struct PrimaryType {
    enum class Kind {
        Nil,
        Boolean,
        I8,
        I16,
        I32,
        I64,
        U8,
        U16,
        U32,
        U64,
        F32,
        F64,
        Ptr,
    } kind;

    PrimaryType() : kind(Kind::Nil) {}
    PrimaryType(Kind kind) : kind(kind) {}
};

struct TupleType {
    std::vector<Type const *> elements;
};

struct ArrayType {
    Type const *element_type;
    size_t count;
};

struct FunctionType {
    Type const *result;
    std::vector<Type const *> parameters;
};

class Type {
    using Variant = std::variant<PrimaryType, TupleType, ArrayType, FunctionType>;

    Variant       m_variant;
    Layout const *m_layout; // size and alignment.

public:
    Type(PrimaryType primary, Layout const *layout) : m_variant(primary), m_layout(layout) {}
    Type(TupleType tuple, Layout const *layout) : m_variant(std::move(tuple)), m_layout(layout) {}
    Type(ArrayType array, Layout const *layout) : m_variant(std::move(array)), m_layout(layout) {}
    Type(FunctionType function, Layout const *layout) : m_variant(std::move(function)), m_layout(layout) {}

    static std::unique_ptr<Type> nil(const Module &module);
    static std::unique_ptr<Type> boolean(const Module &module);
    static std::unique_ptr<Type> i8(const Module &module);
    static std::unique_ptr<Type> i16(const Module &module);
    static std::unique_ptr<Type> i32(const Module &module);
    static std::unique_ptr<Type> i64(const Module &module);
    static std::unique_ptr<Type> u8(const Module &module);
    static std::unique_ptr<Type> u16(const Module &module);
    static std::unique_ptr<Type> u32(const Module &module);
    static std::unique_ptr<Type> u64(const Module &module);
    static std::unique_ptr<Type> f32(const Module &module);
    static std::unique_ptr<Type> f64(const Module &module);
    static std::unique_ptr<Type> ptr(const Module &module);
    static std::unique_ptr<Type> tuple(std::vector<Type const *> elements, const Module &module);
    static std::unique_ptr<Type> array(Type const *element_type, size_t count, const Module &module);
    static std::unique_ptr<Type> function(Type const *result, std::vector<Type const *> parameters,
         const Module &module);

    Layout const *layout() const {
        return m_layout;
    }

    size_t size() const { return m_layout->size();}
    size_t alignment() const { return m_layout->alignment(); }

    template <class T>
    constexpr bool is() const {
        return std::holds_alternative<T>(m_variant);
    }

    template <class T>
    constexpr T const &as() const {
        return std::get<T>(m_variant);
    }

    bool operator==(const Type &other) const;
    bool operator!=(const Type &other) const {
        return !(*this == other);
    }
};
} // namespace pluto

#endif // !PLUTO_TI_IR_TYPE_HPP