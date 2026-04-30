// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_INSTRUCTION_HPP
#define PLUTO_TIIR_INSTRUCTION_HPP

#include <cstring>

#include "tiir/opcode.hpp"

namespace pluto {
/// An instruction is a single operation in the IR.
/// In order to reduce the amount of dispatch required 
/// to process an instruction, we need to know the layout 
/// and types of an instruction's operands directly from 
/// the opcode.
struct Instruction {
    Opcode opcode;
    uint8_t data[16];

    struct R {
        Opcode opcode;
        uint32_t type;
        uint32_t dest;
        uint32_t left;
        uint32_t right;
    };

    struct I {
        Opcode opcode;
        uint32_t type;
        uint32_t dest;
        uint32_t left;
        uint32_t imm;
    };

    template <class T>
    T as() const = delete;
};

template <>
inline Instruction::R Instruction::as<Instruction::R>() const {
    Instruction::R result;
    std::memcpy(&result, this, sizeof(Instruction::R));
    return result;
}

template <>
inline Instruction::I Instruction::as<Instruction::I>() const {
    Instruction::I result;
    std::memcpy(&result, this, sizeof(Instruction::I));
    return result;  
}

} // namespace pluto

#endif // !PLUTO_TIIR_INSTRUCTION_HPP