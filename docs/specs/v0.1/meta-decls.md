# TI-IR Metadata Declarations (v0.1 Draft)

Status: Draft
Scope: Semantics for file-scope `meta_decl` entries used by TI-IR text files.

## Grammar

```ebnf
file      = { meta_decl }, module ;
meta_decl = ident, string_literal, ";" ;
```

Interpretation:
- `ident` is the metadata key.
- `string_literal` is the metadata value.
- Metadata declarations are file-scope only and appear before `module`.

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

- The grammar intentionally allows arbitrary metadata keys via `meta_decl = ident, string_literal, ";"`.
- Parsers should preserve unknown keys.
- Semantic validation may classify unknown keys as informational, warning, or error by policy.
- Reserved keys listed above have normative meaning in v0.1.0.

## Cross-Cutting Concerns

- Linkage behavior for `import` and `export` keys is defined in [Import Export Semantics](import-export.md).

### Validation Order (Recommended)

1. Parse all `meta_decl` entries.
2. Validate required reserved keys (`format`, `version`) and their value constraints.
3. Resolve `import`/`export` semantics as specified in [Import Export Semantics](import-export.md).
4. Parse and validate module-level declarations.
