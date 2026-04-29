// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TI_IR_LAYOUT_HPP
#define PLUTO_TI_IR_LAYOUT_HPP

#include <vector>
#include <variant>

namespace pluto {
struct PrimaryLayout {
    size_t size;
    size_t alignment;
};

struct PaddingLayout {
    size_t size;
};

struct TupleLayout {
    PrimaryLayout primary;
    std::vector<Layout const *> elements;
};

class Layout {
    using Variant = std::variant<PrimaryLayout, PaddingLayout, TupleLayout>;

    Variant m_variant;

public:
    Layout(PrimaryLayout primary) : m_variant(primary) {}
    Layout(PaddingLayout padding) : m_variant(padding) {}
    Layout(TupleLayout tuple) : m_variant(std::move(tuple)) {}

    static std::unique_ptr<Layout> primary(size_t size, size_t alignment) {
        return std::make_unique<Layout>(PrimaryLayout{size, alignment});
    }

    static std::unique_ptr<Layout> padding(size_t size) {
        return std::make_unique<Layout>(PaddingLayout{size});
    }

    static std::unique_ptr<Layout> tuple(size_t size, size_t alignment, std::vector<Layout const *> elements) {
        return std::make_unique<Layout>(TupleLayout{PrimaryLayout{size, alignment}, std::move(elements)});
    }

    size_t size() const;
    size_t alignment() const;

    template <class T>
    constexpr bool is() const {
        return std::holds_alternative<T>(m_variant);
    }

    template <class T>
    constexpr T const &as() const {
        return std::get<T>(m_variant);
    }

    bool operator==(const Layout &other) const;
    bool operator!=(const Layout &other) const {
        return !(*this == other);
    }
};


} // namespace pluto

#endif // !PLUTO_TI_IR_LAYOUT_HPP