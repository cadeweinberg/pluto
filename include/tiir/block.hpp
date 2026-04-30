// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_BLOCK_HPP
#define PLUTO_TIIR_BLOCK_HPP

#include "tiir/index.hpp"

namespace pluto {
struct Block {
    Index name;
    Slice instructions;
};
};


#endif // !PLUTO_TIIR_BLOCK_HPP