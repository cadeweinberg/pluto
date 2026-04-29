// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TS_IR_TARGET_HPP
#define PLUTO_TS_IR_TARGET_HPP

#include <cstdint>
#include <cstddef>

namespace pluto {
struct TypePrimary;

class Target {
public:
    virtual size_t size_of(TypePrimary const &type) const;
    virtual size_t align_of(TypePrimary const &type) const;

};
} // namespace pluto

#endif // !PLUTO_TS_IR_TARGET_HPP