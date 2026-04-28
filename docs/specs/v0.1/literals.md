# TI-IR Literal Semantics (v0.1 Draft)

Status: Draft
Scope: Semantics for literal forms in TI-IR text files.

## Grammar

```ebnf
literal          = tuple_literal | scalar_literal ;
tuple_literal    = "(", [ operand, { ",", operand }, ] ")" ;
scalar_literal   = integral_literal | string_literal | "true" | "false" ;
integral_literal = digit, { digit } ;
```

## Semantics

- Literals are immediate values embedded directly in TI-IR source.

### Scalar Literals

- Integral literals represent base-10 integer values.
- String literals represent immutable string constants.
- `true` and `false` represent boolean literals.

### Tuple Literals

- Tuple literals represent ordered, fixed-arity value aggregates.
- Tuple literal element types are validated against the expected type at the use site.
- Nested tuple literals are permitted where the expected type is compatible.

## Contract

- Tuple literals are valid in operand and const-initializer positions.
- `()` denotes the nil literal.
- Single-value parenthesization is not a tuple literal of arity 1.

## Cross-Cutting Concerns

- Operands and opcode-level operand checks are defined in [Instruction Semantics](instructions.md).
- Constant initializer constraints are defined in [Constant Semantics](constants.md).
- Type compatibility checks are defined in [Type Semantics](types.md).
