// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_FRONTEND_SOURCE_LOCATION_HPP
#define PLUTO_FRONTEND_SOURCE_LOCATION_HPP

#include <cstdint>

namespace pluto {
struct SourceLocation {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;

    SourceLocation()
        : first_line(1)
        , first_column(1)
        , last_line(1)
        , last_column(1) 
    {}

    SourceLocation(uint32_t first_line, uint32_t first_column, uint32_t last_line, uint32_t last_column)
        : first_line(first_line)
        , first_column(first_column)
        , last_line(last_line)
        , last_column(last_column) 
    {}

};
} // namespace pluto

#endif // !PLUTO_FRONTEND_SOURCE_LOCATION_HPP