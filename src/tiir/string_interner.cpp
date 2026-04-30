// SPDX_Identifier: GPL-3.0-or-later

#include <algorithm>
#include <cassert>
#include <limits>

#include "tiir/string_interner.hpp"

namespace pluto {
Index StringInterner::intern(std::string_view view) {
    auto it = std::lower_bound(m_storage.begin(), m_storage.end(), view, 
    [](const std::unique_ptr<std::string> &entry, const std::string_view &v) {
        return *entry < v;
    });

    if (it == m_storage.end() || **it != view) {
        auto id = m_storage.size();
        assert(id < std::numeric_limits<uint32_t>::max() 
            && "StringInterner capacity exceeded");
        m_storage.emplace(it, std::make_unique<std::string>(view));
        return Index{static_cast<uint32_t>(id)};
    }

    size_t value = std::distance(m_storage.begin(), it);
    assert(value < std::numeric_limits<uint32_t>::max()
        && "StringInterner capacity exceeded");
    return Index{static_cast<uint32_t>(value)};
}

std::string_view StringInterner::view(Index id) const {
    assert(id.value < m_storage.size() && "Invalid StringId");
    return *m_storage[id.value];
}
} // namespace pluto
