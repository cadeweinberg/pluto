// SPDX-License-Identifier: GPL-3.0-or-later

//! This module provides the definition of the 
//! internal memory representation of the intermediate
//! representation fot the target independent assembly.

// We are going to use fixed length instructions.
// each instruction can have a maximum of 3 operands.
// each operand is a maximum of 32 bits long.
//  any operand which needs to be longer will be implemented 
//  as a index into an array, which will be stored either in
//  a translation unit context or a function context as appropriate
// the opcode to the instruction will encode how to interpret 
// the operand fields, and this will flatten what would otherwise 
// be a nesting of switch statements into a single (admittedly large)
// switch statement over all possible instructions. 
// This switch is unavoidable as far as I can tell, the flattening
// is my attempt to make it as performant as possible. As you can 
// clearly see, switching over the opcode, and then over each of 
// the kinds of operand requires 4 switches per instruction. by 
// lifting that information from a operand kind field into the opcode
// we remove 3 switches from the interpretation of instructions. which 
// should have knock on effects throughout the compiler. as anywhere 
// we need to consider what kind of instruction we are dealing with,
// we will need a switch statement.
// 
// at 1 opcode field, 3 operand fields and 32 bit operands, we have an 
// instruction which takes up 16 bytes. This is well under a single 
// cache line. in fact on my machine with a 64 byte cache line we can 
// fit 4 instructions into a single cache line. This should also have 
// a positive impact on performance. 
//
// I am considering more ways to improve performance.
// specifically I am trying to fit instructions into a struct of arrays approach
// though i am unsure if that would be worth the hassle. SOA really shines if we 
// have algorithms which only need to consider one peice of data within our struct,
// accross all of the structs defining the current state of the program. such as 
// just the physics data of each object in a scene, the SOA approach lets this physics 
// process single out just the relevant data for it's work.
// wheras with the compiler, our algorithms operate on the sequence of instructions.
// Optimizations, Code Generation, and serialization all iterate over the instructions.
// they don't iterate over just the constants used by the instructions. So while we do 
// offload the constant into a separate array, considering only the constants is not 
// really something that is necessary for the correct operation of the compiler.
// I think the best we can do is optimize the size and data present within each instruction.
// so we can process these as efficiently as is reasonable. I think the biggest win 
// performance wise is going to show up in our removal of the Abstract Syntax Tree.
// and with it any pointer chasing that would normally occur. These incur cache misses, which 
// I hypothesize is a source of a bottleneck in the speed of compilation.
// by making each instruction smaller, and by allowing the most commonly used data arrays 
// (local registers, constants, names) to be loaded alongside the sequence of instructions
// because they too are small and do not contain pointers (as much as possible) the combined 
// effect is that more of a translation unit will be able to fit into cache at a time, which 
// will have the biggest impact on the overall performance of the compiler.
//

// we will have registers, immediates, values, and labels.
// registers will refer to the virtual registers in Static Single Assignment form.
//  these can be lowered into actual registers or stack slots depending.
// immediates are just that, immediate values in the instruction.
//  the type associated with the instruction will define how to interpret the type of 
//  the immediate
// values are a superset of immediates, strictly speaking immediates are values which are 
//  small enough to fit within an operand, so we do just that to save on time and space.
//  wheras values are for data which is too large to fit within 32 bits, yet we need to use 
//  as the operand to an instruction anyways. examples include constant values, and function 
//  arguments.
// labels are for referencing symbols defined in the current module or function as an operand 
//  to an instruction. examples include referencing a global variable or constant, referencing 
//  a function. and referencing a local variable or constant within the current scope.

const Opcode = enum(u32) {
    // the return instruction is used to return 
    // control back to the enclosing scope. and 
    // it can pass along a single value as it does 
    // so. This value may originate in a register,
    // in an immediate, a value, or a label.
    ret_r,
    ret_i,
    ret_v,
    ret_l,

    // call,

    // b, beq, bneq, blt, blte, bgt, bgte,

    // eq, neq, lt, lte, gt, gte

    // and, or, xor, not

    // the negate instruction takes two operands, one the 
    // source, and one the destination. The source may be 
    // a register, immediate, value, or label. the destination 
    // may be a register or label
    neg_rr,
    neg_ri,
    neg_rv,
    neg_rl,
    neg_lr,
    neg_li,
    neg_lv,
    neg_ll,

    // the add instruction takes three operands, two sources
    // and one destination. a source can be a register, a
    // immediate, a value, or a label. whereas a destination 
    // may be a register, a or a label. Values only exist as 
    // compile time structures, so it doesn't make sense to 
    // assign to them, and immediates are immediate to the 
    // instruction the appear in, it also doesn't make sense 
    // to assign to them.
    // the add instruction traps on overflow
    add_rrr,
    add_rri,
    add_rrv,
    add_rrl,
    add_rir,
    add_rii,
    add_riv,
    add_ril,
    add_rvr,
    add_rvi,
    add_rvv,
    add_rvl,
    add_rlr,
    add_rli,
    add_rlv,
    add_rll,
    add_lrr,
    add_lri,
    add_lrv,
    add_lrl,
    add_lir,
    add_lii,
    add_liv,
    add_lil,
    add_lvr,
    add_lvi,
    add_lvv,
    add_lvl,
    add_llr,
    add_lli,
    add_llv,
    add_lll,

    // the subtract instruction takes three operands, two 
    // sources and one destination. The source may be a 
    // register, immediate, value, or label. and the 
    // destination may only be a register or label.
    // The subtract instruction traps on overflow
    sub_rrr,
    sub_rri,
    sub_rrv,
    sub_rrl,
    sub_rir,
    sub_rii,
    sub_riv,
    sub_ril,
    sub_rvr,
    sub_rvi,
    sub_rvv,
    sub_rvl,
    sub_rlr,
    sub_rli,
    sub_rlv,
    sub_rll,
    sub_lrr,
    sub_lri,
    sub_lrv,
    sub_lrl,
    sub_lir,
    sub_lii,
    sub_liv,
    sub_lil,
    sub_lvr,
    sub_lvi,
    sub_lvv,
    sub_lvl,
    sub_llr,
    sub_lli,
    sub_llv,
    sub_lll,

    // the multiply instruction takes three operands, two
    // sources and one destination, the source may be a 
    // register, immediate, value, or label. and the destination
    // may only be a register or label.
    // the multiply instruction traps on overflow
    mul_rrr,
    mul_rri,
    mul_rrv,
    mul_rrl,
    mul_rir,
    mul_rii,
    mul_riv,
    mul_ril,
    mul_rvr,
    mul_rvi,
    mul_rvv,
    mul_rvl,
    mul_rlr,
    mul_rli,
    mul_rlv,
    mul_rll,
    mul_lrr,
    mul_lri,
    mul_lrv,
    mul_lrl,
    mul_lir,
    mul_lii,
    mul_liv,
    mul_lil,
    mul_lvr,
    mul_lvi,
    mul_lvv,
    mul_lvl,
    mul_llr,
    mul_lli,
    mul_llv,
    mul_lll,

    // the divide instruction takes three operands, two, sources
    // one, the destination. the sources may be register,
    // immediates, value, or label, and the destination may 
    // only be a register or label.
    // the divide instruction traps on divide by zero or on overflow.
    div_rrr,
    div_rri,
    div_rrv,
    div_rrl,
    div_rir,
    div_rii,
    div_riv,
    div_ril,
    div_rvr,
    div_rvi,
    div_rvv,
    div_rvl,
    div_rlr,
    div_rli,
    div_rlv,
    div_rll,
    div_lrr,
    div_lri,
    div_lrv,
    div_lrl,
    div_lir,
    div_lii,
    div_liv,
    div_lil,
    div_lvr,
    div_lvi,
    div_lvv,
    div_lvl,
    div_llr,
    div_lli,
    div_llv,
    div_lll,

    // The modulus operation takes three operands,
    // two are sources, and one is the destination.
    // the sources may be register, immediate, value,
    // or label, while the destination may be register
    // or label.
    mod_rrr,
    mod_rri,
    mod_rrv,
    mod_rrl,
    mod_rir,
    mod_rii,
    mod_riv,
    mod_ril,
    mod_rvr,
    mod_rvi,
    mod_rvv,
    mod_rvl,
    mod_rlr,
    mod_rli,
    mod_rlv,
    mod_rll,
    mod_lrr,
    mod_lri,
    mod_lrv,
    mod_lrl,
    mod_lir,
    mod_lii,
    mod_liv,
    mod_lil,
    mod_lvr,
    mod_lvi,
    mod_lvv,
    mod_lvl,
    mod_llr,
    mod_lli,
    mod_llv,
    mod_lll,

    // saturating and overflow arithmetic
};

const OperandKind = enum {
    register,
    immediate,
    value,
    label,
};

const Operand = struct {
    kind: OperandKind,
    data: u32,

    pub fn init(kind: OperandKind, data: u32) Operand {
        return Operand {
            .kind = kind,
            .data = data,
        };
    }

    pub fn register(data: u32) Operand {
        return Operand.init(OperandKind.register, data);
    }

    pub fn immediate(data: u32) Operand {
        return Operand.init(OperandKind.immediate, data);
    }

    pub fn value(data: u32) Operand {
        return Operand.init(OperandKind.value, data);
    }

    pub fn label(data: u32) Operand {
        return Operand.init(OperandKind.label, data);
    }

    pub fn is(self: Operand, OperandKind kind) bool {
        return self.kind == kind;
    }
};

const InstructionError = error {
    InvalidDestination,
};

const Instruction = struct {
    opcode: Opcode,
    _type:  u32,
    a:      u32,
    b:      u32,
    c:      u32,

    pub fn init(opcode: Opcode, _type: u32, a: u32, b: u32, c: u32) Instruction {
        return Instruction {
            .opcode = opcode,
            ._type  = _type,
            .a      = a,
            .b      = b,
            .c      = c,
        };
    }

    pub fn ret(_type: u32, result: Operand) Instruction {
        return switch (result.kind) {
            OperandKind.register  => Instruction.init(Opcode.reg_r, _type, result.data, 0, 0),
            OperandKind.immediate => Instruction.init(Opcode.reg_i, _type, result.data, 0, 0),
            OperandKind.value     => Instruction.init(Opcode.reg_v, _type, result.data, 0, 0),
            OperandKind.label     => Instruction.init(Opcode.reg_l, _type, result.data, 0, 0),
        }
    }

    pub fn neg(_type: u32, dst: Operand, src: Operand) InstructionError!Instruction {
        return switch (dst.kind) {
            OperandKind.register => switch (src.kind) {
                OperandKind.register  => Instruction.init(Opcode.neg_rr, _type, dst.data, src.data, 0),
                OperandKind.immediate => Instruction.init(Opcode.neg_ri, _type, dst.data, src.data, 0),
                OperandKind.value     => Instruction.init(Opcode.neg_rv, _type, dst.data, src.data, 0),
                OperandKind.label     => Instruction.init(Opcode.neg_rl, _type, dst.data, src.data, 0),
            },

            OperandKind.immediate => InstructionError.InvalidDestination,
            OperandKind.value     => InstructionError.InvalidDestination,

            OperandKind.label => switch (src.kind) {
                OperandKind.register  => Instruction.init(Opcode.neg_lr, _type, dst.data, src.data, 0),
                OperandKind.immediate => Instruction.init(Opcode.neg_li, _type, dst.data, src.data, 0),
                OperandKind.value     => Instruction.init(Opcode.neg_lv, _type, dst.data, src.data, 0),
                OperandKind.label     => Instruction.init(Opcode.neg_ll, _type, dst.data, src.data, 0),
            },
        }
    }

    pub fn add(_type: u32, dst: Operand, lhs: Operand, rhs: Operand) InstructionError!Instruction {
        return switch (dst.kind) {
            OperandKind.register => switch (lhs.kind) {
                OperandKind.register  => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.add_rrr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.add_rri, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.add_rrv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.add_rrl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.immediate => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.add_rir, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.add_rii, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.add_riv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.add_ril, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.value     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.add_rvr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.add_rvi, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.add_rvv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.add_rvl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.label     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.add_rlr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.add_rli, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.add_rlv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.add_rll, _type, dst.data, lhs.data, rhs.data),
                },
            },

            OperandKind.immediate => InstructionError.InvalidDestination,
            OperandKind.value     => InstructionError.InvalidDestination,

            OperandKind.label     => switch (lhs.kind) {
                OperandKind.register => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.add_lrr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.add_lri, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.add_lrv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.add_lrl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.immediate => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.add_lir, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.add_lii, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.add_liv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.add_lil, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.value     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.add_lvr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.add_lvi, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.add_lvv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.add_lvl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.label     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.add_llr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.add_lli, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.add_llv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.add_lll, _type, dst.data, lhs.data, rhs.data),
                },
            },
        }
    }

    pub fn sub(_type: u32, dst: Operand, lhs: Operand, rhs: Operand) InstructionError!Instruction {
        return switch (dst.kind) {
            OperandKind.register => switch (lhs.kind) {
                OperandKind.register  => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.sub_rrr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.sub_rri, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.sub_rrv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.sub_rrl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.immediate => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.sub_rir, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.sub_rii, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.sub_riv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.sub_ril, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.value     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.sub_rvr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.sub_rvi, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.sub_rvv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.sub_rvl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.label     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.sub_rlr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.sub_rli, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.sub_rlv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.sub_rll, _type, dst.data, lhs.data, rhs.data),
                },
            },

            OperandKind.immediate => InstructionError.InvalidDestination,
            OperandKind.value     => InstructionError.InvalidDestination,

            OperandKind.label     => switch (lhs.kind) {
                OperandKind.register => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.sub_lrr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.sub_lri, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.sub_lrv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.sub_lrl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.immediate => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.sub_lir, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.sub_lii, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.sub_liv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.sub_lil, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.value     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.sub_lvr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.sub_lvi, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.sub_lvv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.sub_lvl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.label     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.sub_llr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.sub_lli, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.sub_llv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.sub_lll, _type, dst.data, lhs.data, rhs.data),
                },
            },
        }
    }

    pub fn mul(_type: u32, dst: Operand, lhs: Operand, rhs: Operand) InstructionError!Instruction {
        return switch (dst.kind) {
            OperandKind.register => switch (lhs.kind) {
                OperandKind.register  => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mul_rrr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mul_rri, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mul_rrv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mul_rrl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.immediate => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mul_rir, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mul_rii, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mul_riv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mul_ril, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.value     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mul_rvr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mul_rvi, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mul_rvv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mul_rvl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.label     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mul_rlr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mul_rli, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mul_rlv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mul_rll, _type, dst.data, lhs.data, rhs.data),
                },
            },

            OperandKind.immediate => InstructionError.InvalidDestination,
            OperandKind.value     => InstructionError.InvalidDestination,

            OperandKind.label     => switch (lhs.kind) {
                OperandKind.register => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mul_lrr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mul_lri, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mul_lrv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mul_lrl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.immediate => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mul_lir, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mul_lii, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mul_liv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mul_lil, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.value     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mul_lvr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mul_lvi, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mul_lvv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mul_lvl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.label     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mul_llr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mul_lli, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mul_llv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mul_lll, _type, dst.data, lhs.data, rhs.data),
                },
            },
        }
    }

    pub fn div(_type: u32, dst: Operand, lhs: Operand, rhs: Operand) InstructionError!Instruction {
        return switch (dst.kind) {
            OperandKind.register => switch (lhs.kind) {
                OperandKind.register  => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.div_rrr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.div_rri, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.div_rrv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.div_rrl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.immediate => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.div_rir, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.div_rii, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.div_riv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.div_ril, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.value     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.div_rvr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.div_rvi, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.div_rvv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.div_rvl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.label     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.div_rlr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.div_rli, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.div_rlv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.div_rll, _type, dst.data, lhs.data, rhs.data),
                },
            },

            OperandKind.immediate => InstructionError.InvalidDestination,
            OperandKind.value     => InstructionError.InvalidDestination,

            OperandKind.label     => switch (lhs.kind) {
                OperandKind.register => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.div_lrr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.div_lri, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.div_lrv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.div_lrl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.immediate => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.div_lir, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.div_lii, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.div_liv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.div_lil, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.value     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.div_lvr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.div_lvi, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.div_lvv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.div_lvl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.label     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.div_llr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.div_lli, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.div_llv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.div_lll, _type, dst.data, lhs.data, rhs.data),
                },
            },
        }
    }

    pub fn mod(_type: u32, dst: Operand, lhs: Operand, rhs: Operand) InstructionError!Instruction {
        return switch (dst.kind) {
            OperandKind.register => switch (lhs.kind) {
                OperandKind.register  => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mod_rrr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mod_rri, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mod_rrv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mod_rrl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.immediate => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mod_rir, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mod_rii, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mod_riv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mod_ril, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.value     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mod_rvr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mod_rvi, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mod_rvv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mod_rvl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.label     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mod_rlr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mod_rli, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mod_rlv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mod_rll, _type, dst.data, lhs.data, rhs.data),
                },
            },

            OperandKind.immediate => InstructionError.InvalidDestination,
            OperandKind.value     => InstructionError.InvalidDestination,

            OperandKind.label     => switch (lhs.kind) {
                OperandKind.register => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mod_lrr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mod_lri, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mod_lrv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mod_lrl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.immediate => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mod_lir, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mod_lii, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mod_liv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mod_lil, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.value     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mod_lvr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mod_lvi, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mod_lvv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mod_lvl, _type, dst.data, lhs.data, rhs.data),
                },

                OperandKind.label     => switch (rhs.kind) {
                    OperandKind.register  => Instruction.init(Opcode.mod_llr, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.immediate => Instruction.init(Opcode.mod_lli, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.value     => Instruction.init(Opcode.mod_llv, _type, dst.data, lhs.data, rhs.data),
                    OperandKind.label     => Instruction.init(Opcode.mod_lll, _type, dst.data, lhs.data, rhs.data),
                },
            },
        }
    }
};

