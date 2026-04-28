# TI-IR Function Semantics (v0.1 Draft)

Status: Draft
Scope: Semantics for function declarations and signatures in TI-IR text files.

## Grammar

```ebnf
function_decl  = "fn", ident, function_sig, "{", { block_decl }, "}" ;
function_sig   = "(", [ param_list ], ")", "->", type_expr ;
param_list     = param, { ",", param } ;
param          = ident, ":", type_expr ;
```

## Semantics

### Function Declaration

- `fn` introduces a callable symbol in the enclosing module visibility domain.

### Signature and Return Model

- Function signatures model one return slot of type `type_expr`.
- Multi-value returns are modeled as tuple-typed single returns (for example `-> (i32, i32)`).

## Contract

- Function names must be valid `ident` values.
- Function bodies are brace-delimited and contain one or more `block_decl` entries.
- Function declarations are ordered and participate in normal module-visibility symbol resolution.
- Parameters are explicitly typed via `ident ":" type_expr`.
- Function return type is exactly one `type_expr`.
- Multi-value returns are represented as tuple-typed single returns (for example `-> (i32, i32)`).

## Cross-Cutting Concerns

- `block_decl` semantics govern intra-function control flow and terminator requirements.
- `type_expr` semantics are defined in [Type Semantics](types.md).
- Symbol visibility boundaries are defined in [Module Semantics](modules.md).
