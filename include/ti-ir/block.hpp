// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TI_IR_BLOCK_HPP
#define PLUTO_TI_IR_BLOCK_HPP

#include "ti-ir/index.hpp"

namespace pluto {
struct Block {
    Index name;
    Slice instructions;
};
};


#endif // !PLUTO_TI_IR_BLOCK_HPP