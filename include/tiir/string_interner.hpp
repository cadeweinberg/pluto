// SPDX_Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_STRING_INTERNER_HPP
#define PLUTO_TIIR_STRING_INTERNER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "tiir/index.hpp"

namespace pluto {
class StringInterner {
public:
    Index intern(std::string_view view);
    std::string_view view(Index index) const;

private:
    using Storage = std::vector<std::unique_ptr<std::string>>;
    Storage m_storage;
};
}


#endif // !PLUTO_TIIR_STRING_INTERNER_HPP
