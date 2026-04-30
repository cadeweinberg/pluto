// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_OPCODE_HPP
#define PLUTO_TIIR_OPCODE_HPP

#include <cstdint>

namespace pluto {
enum class Opcode : uint16_t {
    // Arithmetic
    Add,
    Addi,
    Sub,
    Subi,
    Mul,
    Muli,
    Div,
    Divi,
    Rem,
    Remi,

    // Control Flow
    Ret, // No Operands
};
} // namespace pluto

/*
    Lets have a discussion about the set of operands which form 
    a base on which we can design a high level language.

    Arithmetic operations: add, sub, mul, div, rem.
        Using these 5 operations, we can derive all other arithmetic operations, 
        such as negation, increment, decrement, etc. and more complex operations 
        such as exponentiation, logarithm, etc. can be implemented using 
        these basic operations.

    Bitwise operations: and, or, xor, not, shl, shra, shrl.
        These operations are essential for low level programming and can be used 
        to implement a wide range of functionality, such as bit manipulation, 
        masking, etc.

    Comparison operations: eq, neq, lt, lte, gt, gte.
        These operations are essential for control flow and can be used to implement 
        if statements, loops, etc. They are also used in user expressions, to compute 
        boolean values, and not necessarily tied to control flow.

    Control flow operations: ret, call, b, switch
        These operations are essential for implementing function calls and control flow 
        structures such as if statements and loops.

    Memory operations: load, store, gep, cmpxchg, atomicrmw
        These operations are essential for moving data between memory and registers.
        Memory on the stack, memory in global read/write and read only segments, and memory in the heap.

    Casting operations: trunc, zext, sext, fptrunc, fpext, fptoui, fptosi, uitofp, sitofp
        These operations are essential for converting between different types.
        signed and unsigned integers, floating point numbers, etc.

    We need to have a selection of types available in the IR, 
    such as i8, i16, i32, i64, u8, u16, u32, u64, f32, f64, bool, nil.
*/

#endif // !PLUTO_TIIR_OPCODE_HPP