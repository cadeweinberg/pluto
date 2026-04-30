// SPDX-Identifier: GPL-3.0-or-later

#include "tiir/type.hpp"

namespace pluto {

std::unique_ptr<Type> Type::nil() {
    PrimaryType nil_type{PrimaryType::Kind::Nil};
    return std::make_unique<Type>(nil_type);
}

std::unique_ptr<Type> Type::boolean() {
    PrimaryType boolean_type{PrimaryType::Kind::Boolean};
    return std::make_unique<Type>(boolean_type);
}

std::unique_ptr<Type> Type::i8() {
    PrimaryType i8_type{PrimaryType::Kind::I8};
    return std::make_unique<Type>(i8_type);
}

std::unique_ptr<Type> Type::i16() {
    PrimaryType i16_type{PrimaryType::Kind::I16};
    return std::make_unique<Type>(i16_type);
}

std::unique_ptr<Type> Type::i32() {
    PrimaryType i32_type{PrimaryType::Kind::I32};
    return std::make_unique<Type>(i32_type);
}

std::unique_ptr<Type> Type::i64() {
    PrimaryType i64_type{PrimaryType::Kind::I64};
    return std::make_unique<Type>(i64_type);
}

std::unique_ptr<Type> Type::u8() {
    PrimaryType u8_type{PrimaryType::Kind::U8};
    return std::make_unique<Type>(u8_type);
}

std::unique_ptr<Type> Type::u16() {
    PrimaryType u16_type{PrimaryType::Kind::U16};
    return std::make_unique<Type>(u16_type);
}

std::unique_ptr<Type> Type::u32() {
    PrimaryType u32_type{PrimaryType::Kind::U32};
    return std::make_unique<Type>(u32_type);
}

std::unique_ptr<Type> Type::u64() {
    PrimaryType u64_type{PrimaryType::Kind::U64};
    return std::make_unique<Type>(u64_type);
}

std::unique_ptr<Type> Type::f32() {
    PrimaryType f32_type{PrimaryType::Kind::F32};
    return std::make_unique<Type>(f32_type);
}

std::unique_ptr<Type> Type::f64() {
    PrimaryType f64_type{PrimaryType::Kind::F64};
    return std::make_unique<Type>(f64_type);
}

std::unique_ptr<Type> Type::ptr() {
    PrimaryType ptr_type{PrimaryType::Kind::Ptr};
    return std::make_unique<Type>(ptr_type);
}

std::unique_ptr<Type> Type::tuple(std::vector<Type const *> elements) {
    TupleType tuple_type{std::move(elements)};
    return std::make_unique<Type>(std::move(tuple_type));
}

std::unique_ptr<Type> Type::array(Type const *element_type, size_t count) {
    ArrayType array_type{element_type, count};
    return std::make_unique<Type>(std::move(array_type));
}

std::unique_ptr<Type> Type::function(Type const *result, std::vector<Type const *> parameters) {
    FunctionType function_type{result, std::move(parameters)};
    return std::make_unique<Type>(std::move(function_type));
}


struct TypeEqualityVisitor {
    const Type *other;

    bool operator()(const PrimaryType &self) const {
        if (!other->is<PrimaryType>()) {
            return false;
        }
        return self.kind == other->as<PrimaryType>().kind;
    }

    bool operator()(const TupleType &self) const {
        if (!other->is<TupleType>()) {
            return false;
        }
        const auto &other_tuple = other->as<TupleType>();
        if (self.elements.size() != other_tuple.elements.size()) {
            return false;
        }
        for (size_t i = 0; i < self.elements.size(); ++i) {
            if (*self.elements[i] != *other_tuple.elements[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator()(const FunctionType &self) const {
        if (!other->is<FunctionType>()) {
            return false;
        }
        const auto &other_function = other->as<FunctionType>();
        if (*self.result != *other_function.result) {
            return false;
        }
        if (self.parameters.size() != other_function.parameters.size()) {
            return false;
        }
        for (size_t i = 0; i < self.parameters.size(); ++i) {
            if (*self.parameters[i] != *other_function.parameters[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator()(const ArrayType &self) const {
        if (!other->is<ArrayType>()) {
            return false;
        }
        const auto &other_array = other->as<ArrayType>();
        if (*self.element_type != *other_array.element_type) {
            return false;
        }
        return self.count == other_array.count;
    }
};

bool Type::operator==(const Type &other) const {
    return std::visit(TypeEqualityVisitor{&other}, m_variant);
}
} // namespace pluto
