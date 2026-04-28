# TI-IR MUL Instruction Semantics (v0.1 Draft)

Status: Draft
Scope: Semantics and contract for the `mul` instruction in TI-IR v0.1.

## Grammar

```ebnf
inst_decl = [ result_pattern, ":", type_expr, "=" ], opcode, operand_list, ";" ;
```

`mul` is represented as an `inst_decl` whose `opcode` is `mul`.

## Semantics

- `mul` computes the product of two operands.
- The result is a single SSA value bound by the instruction result pattern.

## Contract

- `mul` must have exactly two operands.
- Both operands must type-check as the same arithmetic type.
- The result type annotation must match the operand type.
- `mul` must bind a result value (it is not a terminator).

## Cross-Cutting Concerns

- Generic instruction syntax and instruction-wide rules are defined in [Instruction Semantics](../instructions.md).
- Name resolution for operands is defined in [Name Semantics](../names.md).
- Primitive type availability is defined in [Type Semantics](../types.md).
