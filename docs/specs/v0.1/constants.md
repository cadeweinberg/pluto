# TI-IR Constant Semantics (v0.1 Draft)

Status: Draft
Scope: Semantics for constant declarations in TI-IR text files.

## Grammar

```ebnf
const_decl = "const", ident, ":", type_expr, "=", literal, ";" ;
```

## Semantics

- `const` introduces a constant symbol in the enclosing module visibility domain with explicit type and literal initializer.

## Contract

- Constant names must be valid `ident` values.
- Constants require explicit type annotations.
- Initializers are literal-only in v0.1.
- The initializer literal must type-check against the declared `type_expr`.

## Cross-Cutting Concerns

- `type_expr` semantics are defined in [Type Semantics](types.md).
- Literal syntax and literal typing behavior are defined in [Literal Semantics](literals.md).
- Module visibility boundaries are defined in [Module Semantics](modules.md).
