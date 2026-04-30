// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_TYPE_HPP
#define PLUTO_TIIR_TYPE_HPP

#include <memory>
#include <vector>
#include <variant>

namespace pluto {
struct Type;
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

public:
    Type(PrimaryType primary) : m_variant(std::move(primary)) {}
    Type(TupleType tuple) : m_variant(std::move(tuple)) {}
    Type(ArrayType array) : m_variant(std::move(array)) {}
    Type(FunctionType function) : m_variant(std::move(function)) {}

    static std::unique_ptr<Type> nil();
    static std::unique_ptr<Type> boolean();
    static std::unique_ptr<Type> i8();
    static std::unique_ptr<Type> i16();
    static std::unique_ptr<Type> i32();
    static std::unique_ptr<Type> i64();
    static std::unique_ptr<Type> u8();
    static std::unique_ptr<Type> u16();
    static std::unique_ptr<Type> u32();
    static std::unique_ptr<Type> u64();
    static std::unique_ptr<Type> f32();
    static std::unique_ptr<Type> f64();
    static std::unique_ptr<Type> ptr();
    static std::unique_ptr<Type> tuple(std::vector<Type const *> elements);
    static std::unique_ptr<Type> array(Type const *element_type, size_t count);
    static std::unique_ptr<Type> function(Type const *result, std::vector<Type const *> parameters);

    Variant const &variant() const { return m_variant; }

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

#endif // !PLUTO_TIIR_TYPE_HPP