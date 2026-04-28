// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_FRONTEND_FLAT_MAP_HPP
#define PLUTO_FRONTEND_FLAT_MAP_HPP

#include <algorithm>
#include <cstdint>
#include <vector>
#include <utility>

namespace pluto {
template <typename Key, typename Value, class Compare = std::less<Key>>
class FlatMap {
public:
    using Entry = std::pair<Key, Value>;
    using Storage = std::vector<Entry>;

    Entry &at(const Key &key) {
        auto it = std::lower_bound(m_storage.begin(), m_storage.end(), key, [&](const Entry &entry, const Key &k) {
            return Compare{}(entry.first, k);
        });
        if (it == m_storage.end() || it->first != key) {
            return *m_storage.emplace(it, key, Value{});
        }

        return *it;
    }

private:
    Storage m_storage;
};
} // namespace pluto

#endif // !PLUTO_FRONTEND_FLAT_MAP_HPP