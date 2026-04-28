# TI-IR Basic Block Semantics (v0.1 Draft)

Status: Draft
Scope: Semantics for basic-block structure within function bodies.

## Grammar

```ebnf
block_decl      = ident, ":", { inst_decl } ;
```

## Semantics

- A basic block is a labeled, single-entry sequence of instructions terminated by exactly one terminator instruction.

### Control-Flow Model

- Block labels are symbols used by control-flow instructions to identify jump targets.
- In v0.1, `ret` is the only terminator opcode; block termination exits the enclosing function.
- Branching and merge-based control flow are deferred to later versions.

## Contract

- A block begins at its `ident ":"` label.
- A block extends through its instruction sequence and ends at its required terminator instruction.
- Every block must contain exactly one terminator and it must be last.
- No non-terminator instruction may appear after a terminator in the same block.
- Terminator opcode validity is checked during semantic validation.

## Cross-Cutting Concerns

- Function-body structure and visibility are defined in [Function Semantics](functions.md).
- Instruction syntax and opcode-level typing are defined in [Instruction Semantics](instructions.md).
