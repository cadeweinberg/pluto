// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TI_IR_LAYOUT_INTERNER_HPP
#define PLUTO_TI_IR_LAYOUT_INTERNER_HPP

#include <vector>
#include <memory>

#include "ti-ir/layout.hpp"

namespace pluto {
struct Type;
class Module;

class LayoutInterner {
    struct List {
        struct Element {
            struct Type const *type;
            std::unique_ptr<Layout> layout;
        };

        std::vector<std::unique_ptr<Element>> list;
    };

public:
    LayoutInterner(class Module &module);

    Layout const *layout_of(struct Type const *type);

private:
    std::unique_ptr<Layout> nil_layout;
    std::unique_ptr<Layout> i8_layout;
    std::unique_ptr<Layout> i16_layout;
    std::unique_ptr<Layout> i32_layout;
    std::unique_ptr<Layout> i64_layout;
    std::unique_ptr<Layout> u8_layout;
    std::unique_ptr<Layout> u16_layout;
    std::unique_ptr<Layout> u32_layout;
    std::unique_ptr<Layout> u64_layout;
    std::unique_ptr<Layout> f32_layout;
    std::unique_ptr<Layout> f64_layout;
    std::unique_ptr<Layout> boolean_layout;
    std::unique_ptr<Layout> ptr_layout;
    List tuple_layouts;
    List array_layouts;
};
} // namespace pluto

#endif // !PLUTO_TI_IR_LAYOUT_INTERNER_HPP