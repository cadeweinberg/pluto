// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_INDEX_HPP
#define PLUTO_TIIR_INDEX_HPP

#include <cstdint>
#include <limits>

namespace pluto {
struct Index {
    uint32_t value;

    static Index invalid() {
        return Index{std::numeric_limits<uint32_t>::max()};
    }

    bool isValid() const noexcept {
        return value != invalid().value;
    }

    bool operator==(const Index &other) const {
        return value == other.value;
    }
    bool operator!=(const Index &other) const {
        return value != other.value;
    }
    bool operator<(const Index &other) const {
        return value < other.value;
    }
    bool operator<=(const Index &other) const {
        return value <= other.value;
    }
    bool operator>(const Index &other) const {
        return value > other.value;
    }
    bool operator>=(const Index &other) const {
        return value >= other.value;
    }
};

struct Slice {
    Index start;
    Index count;
};
} // namespace pluto

#endif // !PLUTO_TIIR_INDEX_HPP
