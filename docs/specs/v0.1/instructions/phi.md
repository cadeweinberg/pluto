# TI-IR PHI Instruction Semantics (v0.1 Draft)

Status: Reserved for future revision
Scope: Placeholder for `phi` instruction semantics; `phi` is out of scope for TI-IR v0.1.

## Grammar

```ebnf
inst_decl = [ result_pattern, ":", type_expr, "=" ], opcode, operand_list, ";" ;
```

`phi` has no distinct top-level grammar production in v0.1; when introduced in a future revision, it will be represented as an `inst_decl` whose `opcode` is `phi`.

## Semantics

- `phi` semantics are intentionally unspecified for v0.1.
- Control-flow join and SSA merge behavior will be specified when branching opcodes are introduced.

## Contract

- `phi` is not a valid opcode in v0.1 programs.
- Any use of `phi` in v0.1 input is a semantic validation error.

## Cross-Cutting Concerns

- Control-flow edges and block predecessor relationships are defined in [Basic Block Semantics](../basic-blocks.md).
- Generic instruction syntax and instruction-wide contract are defined in [Instruction Semantics](../instructions.md).
- Name resolution for incoming arm values is defined in [Name Semantics](../names.md).
