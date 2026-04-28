// SPDX_Identifier: GPL-3.0-or-later

#include <cassert>
#include <limits>

#include "adt/string_interner.hpp"
namespace pluto {
StringInterner::StringId StringInterner::intern(std::string_view view) {
    auto it = std::lower_bound(m_storage.begin(), m_storage.end(), view, 
    [](const std::unique_ptr<std::string> &entry, const std::string_view &v) {
        return *entry < v;
    });

    if (it == m_storage.end() || **it != view) {
        auto id = static_cast<StringId>(m_storage.size());
        assert(id < std::numeric_limits<StringId>::max() 
            && "StringInterner capacity exceeded");
        m_storage.emplace(it, std::make_unique<std::string>(view));
        return id;
    }

    return static_cast<StringId>(std::distance(m_storage.begin(), it));
}

std::string_view StringInterner::view(StringId id) const {
    assert(id < m_storage.size() && "Invalid StringId");
    return *m_storage[id];
}
} // namespace pluto
