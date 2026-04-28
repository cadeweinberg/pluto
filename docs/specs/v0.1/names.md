# TI-IR Name Semantics (v0.1 Draft)

Status: Draft
Scope: Syntax and semantic resolution rules for names in TI-IR text files.

## Grammar

```ebnf
name           = ident | "@", integral_literal ;
ident          = letter, { letter | digit | "_" } ;
```

## Semantics

- Names are symbolic references to declarations and SSA values.
- Source names remain symbolic in text; internal stable IDs are compiler artifacts and are not part of the surface syntax.

### Unqualified Names

Resolution order:
1. Local SSA scope (parameters and instruction results).
2. Enclosing module visibility domain (types, constants, functions).
3. Imported symbols that are visible in the current module domain, if applicable.

### Unnamed Temporaries

- `@N` names denote unnamed function-local SSA values.

## Contract

### Resolution Constraints

- Resolution is lexical and deterministic.
- If multiple symbols in the same namespace would match, resolution fails with a semantic validation error.
- Name references are unqualified in v0.1; dotted qualification syntax is invalid.
- Unknown or non-visible symbol names are semantic validation errors.

### Unnamed Temporary Constraints

- `N` is a non-negative decimal integer.
- Unnamed temporaries are scoped to their defining function.
- Equality is syntactic: two different `@N` spellings denote different SSA bindings.

### Lexical Constraints

- `ident` begins with a letter and continues with letters, digits, or `_`.
- Name matching is case-sensitive.

## Cross-Cutting Concerns

- Instruction operand and result-pattern behavior is defined in [Instruction Semantics](instructions.md).
- Module visibility boundaries are defined in [Module Semantics](modules.md).
- Name use in type positions is constrained by [Type Semantics](types.md).
