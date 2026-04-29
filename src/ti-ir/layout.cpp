// SPDX-Identifier: GPL-3.0-or-later

#include "ti-ir/layout.hpp"

namespace pluto {

struct LayoutSizeVisitor {
    size_t operator()(const PrimaryLayout &layout) const {
        return layout.size;
    }
    size_t operator()(const PaddingLayout &layout) const {
        return layout.size;
    }
    size_t operator()(const TupleLayout &layout) const {
        return layout.primary.size;
    }
};

size_t Layout::size() const {
    return std::visit(LayoutSizeVisitor{}, m_variant);
}

struct LayoutAlignmentVisitor {
    size_t operator()(const PrimaryLayout &layout) const {
        return layout.alignment;
    }
    size_t operator()(const PaddingLayout &layout) const {
        return 1;
    }
    size_t operator()(const TupleLayout &layout) const {
        return layout.primary.alignment;
    }
};

size_t Layout::alignment() const {
    return std::visit(LayoutAlignmentVisitor{}, m_variant);
}

struct LayoutEqualityVisitor {
    Layout const *other;

    bool operator()(const PrimaryLayout &self) const {
        if (!other->is<PrimaryLayout>()) {
            return false;
        }
        const auto &other_primary = other->as<PrimaryLayout>();
        return self.size == other_primary.size && self.alignment == other_primary.alignment;
    }
    bool operator()(const PaddingLayout &self) const {
        if (!other->is<PaddingLayout>()) {
            return false;
        }
        const auto &other_padding = other->as<PaddingLayout>();
        return self.size == other_padding.size;
    }
    bool operator()(const TupleLayout &self) const {
        if (!other->is<TupleLayout>()) {
            return false;
        }
        const auto &other_tuple = other->as<TupleLayout>();
        if (self.primary.size != other_tuple.primary.size 
         || self.primary.alignment != other_tuple.primary.alignment) {
            return false;
        }
        if (self.elements.size() != other_tuple.elements.size()) {
            return false;
        }
        for (size_t i = 0; i < self.elements.size(); ++i) {
            if (!(*self.elements[i] == *other_tuple.elements[i])) {
                return false;
            }
        }
        return true;
    }
};

bool Layout::operator==(const Layout &other) const {
    return std::visit(LayoutEqualityVisitor{&other}, m_variant);
}
} // namespace pluto
