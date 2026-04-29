// SPDX_Identifier: GPL-3.0-or-later

#ifndef PLUTO_TI_IR_STRING_INTERNER_HPP
#define PLUTO_TI_IR_STRING_INTERNER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "ti-ir/index.hpp"

namespace pluto {
class StringInterner {
public:
    Index intern(std::string_view view);
    std::string_view view(Index index) const;

private:
    // #NOTE: We need a stable uint32_t ID for each unique string,
    // so we use a vector of unique_ptr to strings. 
    // The index in the vector serves as the StringId.
    // This could be a slowdown point in the program, so we'll 
    // keep an eye on it and optimize if necessary.
    using Storage = std::vector<std::unique_ptr<std::string>>;

    Storage m_storage;
};
}


#endif // !PLUTO_TI_IR_STRING_INTERNER_HPP