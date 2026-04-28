# TI-IR Import Export Semantics (v0.1 Draft)

Status: Draft
Scope: Semantics for the `import` and `export` metadata keys in file-scope `meta_decl` entries.

## Grammar

```ebnf
meta_decl = ident, [ string_literal ], ";" ;
```

Interpretation:
- `import` and `export` are metadata keys under `meta_decl`.
- Their values are represented as `string_literal` payloads.

Authority boundary:
- This file is the source of truth for `import` and `export` linkage semantics.
- Key registration and general metadata mechanics are defined in [Metadata Declarations](meta-decls.md).

## Semantics

### import

- Declares an external dependency that makes imported symbols available in the current module visibility domain.

Value shape:
- Resolver-specific import descriptor encoded as a string literal.

### export

- Declares symbols intended to be visible outside the defining module visibility boundary.

Value shape:
- Export descriptor encoded as a string literal.

## Contract

- Imports are processed before binding of module declarations that depend on imported symbols.
- Import resolution policy (filesystem, package registry, virtual module map, etc.) is implementation-defined in v0.1.
- Failed import resolution is a semantic validation error.
- Export processing runs after local declarations are parsed and symbol tables are established.
- Export descriptors must resolve to symbols visible in the current module visibility domain.
- Unresolved exports are semantic validation errors.

### Ordering and Interaction

1. Parse all file-scope metadata entries.
2. Validate required metadata keys (for example `format`, `version`).
3. Resolve all imports and populate imported symbol visibility in the current module domain.
4. Parse and validate module declarations using the import-augmented visibility scope.
5. Validate and publish exports.

### Extensibility Constraints

- Additional linkage-related metadata keys may be introduced without grammar changes.
- Unknown linkage keys should be preserved by parsers and handled by policy.

## Cross-Cutting Concerns

- `meta_decl` parsing, reserved-key validation, and unknown-key policy are defined in [Metadata Declarations](meta-decls.md).
- Name resolution of imported and exported symbols is defined in [Name Semantics](names.md).
