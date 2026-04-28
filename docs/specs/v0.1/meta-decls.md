# TI-IR Metadata Declarations (v0.1 Draft)

Status: Draft
Scope: Semantics for `meta_decl` entries at file scope and declaration-attached metadata lists in TI-IR text files.

## Grammar

```ebnf
file      = { meta_decl }, module ;
meta_decl = ident, [ literal ], ";" ;
```

Interpretation:
- `ident` is the metadata key.
- `literal` is an optional metadata value payload.
- If the value is omitted, the entry is a flag-style metadata declaration.
- Metadata declarations may appear at file scope and attached to `type_decl`, `let_decl`, and `function_decl`.

## Semantics

### format

- Declares the file format discriminator.

Notes:
- Intended as textual header magic.
- Binary representations should use the same discriminator concept.

### version

- Declares which TI-IR specification version this file targets.

Notes:
- The meaning of the version string is defined as in [SemVer 2.0](https://semver.org/)
- Unknown or unsupported versions should fail semantic validation.
- Future versions are intended to support more Reserved Keys, 
  Grammar Changes are not expected unless there is a very good reason.

### import

- Declares that import linkage metadata is present for this file.

Notes:
- The authoritative import semantics are specified in [Import Export Semantics](import-export.md).

### export

- Declares that export linkage metadata is present for this file.

Notes:
- The authoritative export semantics are specified in [Import Export Semantics](import-export.md).

## Contract

### format

- Value should be `"ti-ir"`.

### version

- Value should be "integer_literal, [ ".", integer_literal, [ ".", integer_literal ] ]".

### import

- Value is a string-literal payload interpreted by import/export linkage rules.

### export

- Value is a string-literal payload interpreted by import/export linkage rules.
- Unknown or unsupported versions should fail semantic validation.

### Extensibility Constraints

- The grammar intentionally allows arbitrary metadata keys via `meta_decl = ident, [ string_literal ], ";"`.
- Parsers should preserve unknown keys.
- Semantic validation may classify unknown keys as informational, warning, or error by policy.
- Reserved keys listed above have normative meaning in v0.1.0.

### Declaration-Attached Metadata

- `decl_meta_list` applies metadata to exactly one following declaration.
- `decl_meta_list` supports only `type_decl`, `let_decl`, and `function_decl` in v0.1.
- Declaration metadata keys are independent from file-level reserved keys unless a key specification states otherwise.
- Key collisions inside a single `decl_meta_list` are semantic validation errors unless key-specific rules define merge behavior.

## Cross-Cutting Concerns

- Linkage behavior for `import` and `export` keys is defined in [Import Export Semantics](import-export.md).

### Validation Order (Recommended)

1. Parse all `meta_decl` entries.
2. Validate required reserved keys (`format`, `version`) and their value constraints.
3. Resolve `import`/`export` semantics as specified in [Import Export Semantics](import-export.md).
4. Parse and validate declarations in the current module visibility domain.
