# TI-IR Instruction Semantics (v0.1 Draft)

Status: Draft
Scope: Semantics for instruction and terminator forms in TI-IR text files.

## Grammar

```ebnf
inst_decl       = [ result_pattern, ":", type_expr, "=" ], opcode, operand_list, ";" ;
```

## Semantics

### Instruction Declarations

- `inst_decl` describes value-producing or effectful operations with optional result binding and explicit type annotation when a result is bound.

### Opcode Set (v0.1)

- The minimal opcode set for v0.1 is: `add`, `sub`, `mul`, `div`, `mod`, `ret`.
- `ret` is the only terminator opcode in v0.1.
- More complex control-flow and interprocedural opcodes are deferred to later versions.

### Result Patterns

- Result patterns specify how an instruction's value is bound after execution.
- In the v0.1 minimal opcode set, result patterns are used as single-name bindings for arithmetic instructions.
- Tuple result patterns are grammar-level surface forms reserved for future opcode extensions.

### Operands

- Operands are value-supplying terms in instructions and terminators. Operands parse uniformly as either names or literals and are unified during semantic analysis.

### Terminators

- Terminators are `inst_decl` entries whose opcode belongs to the terminator opcode class.
- In v0.1, this class contains only `ret`.

## Contract

- `:` is reserved for type annotation and binds the annotated `type_expr` to the result binding/pattern.
- **Named result patterns**: A single name binds the instruction result as a local SSA value:
  ```
  x: i32 = add a, b;    (* result bound to name "x" *)
  ```
  The name `x` becomes available for use in subsequent instructions and must satisfy SSA domination constraints.
- **Tuple result patterns**: Reserved for future opcodes that produce tuple values; no opcode in the v0.1 minimal set supports tuple-producing results.
- **Tuple destructuring as surface form**: Reserved for future lowering behavior once tuple-producing opcodes are in scope.
- **Opcode allow-list**: Only `add`, `sub`, `mul`, `div`, `mod`, and `ret` are valid in v0.1.
- **Out-of-scope opcodes**: `br`, `phi`, `call`, and related control-flow/interprocedural opcodes are out of scope for v0.1.
- **Name operands**: Resolved according to name resolution rules defined in [Name Semantics](names.md).
  - A function parameter.
  - An instruction result with SSA domination constraint.
  - An unnamed temporary defined earlier in the function.
  - A constant or function visible in the enclosing module visibility domain.
- **Literal operands**: Treated as immediate values with inferred or contextually determined types (see [Literal Semantics](literals.md)).
- Opcode and operand type compatibility is checked during semantic validation; operand types must match the opcode's declared operand type signature.
- Terminators never bind result values.
- Terminators must appear as the final instruction in a basic block.
- Terminator operands follow the same resolution rules as instruction operands (see Operand Semantics above).

## Cross-Cutting Concerns

- Basic block placement and terminator requirements are defined in [Basic Block Semantics](basic-blocks.md).
- Type annotations and expression-level type rules are defined in [Type Semantics](types.md).
- Name syntax and name resolution are defined in [Name Semantics](names.md).

### Opcode-Specific Specifications (v0.1)

- `add`: [ADD Instruction Semantics](instructions/add.md)
- `sub`: [SUB Instruction Semantics](instructions/sub.md)
- `mul`: [MUL Instruction Semantics](instructions/mul.md)
- `div`: [DIV Instruction Semantics](instructions/div.md)
- `mod`: [MOD Instruction Semantics](instructions/mod.md)
- `ret`: [RET Instruction Semantics](instructions/ret.md)
