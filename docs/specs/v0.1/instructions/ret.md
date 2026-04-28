# TI-IR RET Instruction Semantics (v0.1 Draft)

Status: Draft
Scope: Semantics and contract for the `ret` instruction in TI-IR v0.1.

## Grammar

```ebnf
inst_decl = [ result_pattern, ":", type_expr, "=" ], opcode, operand_list, ";" ;
```

`ret` is represented as an `inst_decl` whose `opcode` is `ret`.

## Semantics

- `ret` terminates execution of the current function and returns control to the caller.
- `ret` is the only terminator opcode in v0.1.
- `ret` carries the returned value in its operand list.

## Contract

- `ret` must not bind a result pattern.
- `ret` must appear as the final instruction in a basic block.
- `ret` return operand types must match the function return `type_expr`.
- In v0.1 function forms, `ret` must provide exactly one return operand.

## Cross-Cutting Concerns

- Generic instruction syntax and instruction-wide rules are defined in [Instruction Semantics](../instructions.md).
- Block termination and placement constraints are defined in [Basic Block Semantics](../basic-blocks.md).
- Function return type semantics are defined in [Function Semantics](../functions.md).
