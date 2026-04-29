// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TI_IR_FUNCTION_HPP
#define PLUTO_TI_IR_FUNCTION_HPP

#include <vector>

#include "ti-ir/block.hpp"
#include "ti-ir/instruction.hpp"
#include "ti-ir/operand.hpp"
#include "ti-ir/ssa.hpp"
#include "ti-ir/tuple.hpp"

namespace pluto {
struct Function {
  struct Parameter {
    Index name;
    Index type;
  };

  Index name;
  Index return_type;
  std::vector<Parameter> parameters;
  std::vector<SSA> locals;
  std::vector<Block> blocks;
  std::vector<Instruction> instructions;
  std::vector<Tuple> tuples;
  std::vector<Operand> tuple_operands;
};
} // namespace pluto

#endif // !PLUTO_TI_IR_FUNCTION_HPP