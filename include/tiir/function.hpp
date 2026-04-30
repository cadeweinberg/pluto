// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_FUNCTION_HPP
#define PLUTO_TIIR_FUNCTION_HPP

#include <vector>

#include "tiir/block.hpp"
#include "tiir/instruction.hpp"
#include "tiir/operand.hpp"
#include "tiir/ssa.hpp"
#include "tiir/tuple.hpp"

namespace pluto {
struct Function {
  struct Parameter {
    Index name;
    Index type;
  };

  Index name;
  Index type;
  std::vector<Parameter> parameters;
  std::vector<SSA> locals;
  std::vector<Block> blocks;
  std::vector<Instruction> instructions;
  std::vector<Tuple> tuples;
  std::vector<Operand> tuple_operands;
};
} // namespace pluto

#endif // !PLUTO_TIIR_FUNCTION_HPP