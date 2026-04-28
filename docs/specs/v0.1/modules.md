# TI-IR Module Semantics (v0.1 Draft)

Status: Draft
Scope: Semantics for module symbol visibility boundaries in TI-IR text files.

## Grammar

```ebnf
module      = "module", ident, "{" { module_decl } "}" ;
module_decl = type_decl | const_decl | function_decl ;
```

## Semantics

### Module Form

- `module` introduces a named symbol visibility boundary for top-level declarations in the file.
- The module name is a visibility label used by name resolution and linkage metadata.

### Module Declarations

- `module_decl` is a syntactic container for top-level declarations.
- Module semantics do not redefine declaration behavior for `type_decl`, `const_decl`, or `function_decl`; those are specified by their own semantic documents.

Allowed declarations:
- `type_decl`
- `const_decl`
- `function_decl`

## Contract

- Exactly one module form appears per file in v0.1.
- Module name must be a valid `ident`.
- Only `module_decl` entries are allowed inside the module section.
- Metadata declarations are file-scope only and must appear before `module`.
- Top-level symbols are visible within the defining module unless restricted by future policy.
- Cross-module symbol visibility is controlled by linkage metadata (`import` and `export`).
- Module semantics are limited to visibility boundaries and do not introduce additional typing or execution behavior.

## Cross-Cutting Concerns

- Declaration shape and validation are defined in [Type Semantics](types.md), [Constant Semantics](constants.md), and [Function Semantics](functions.md).
- Name syntax and resolution order are defined in [Name Semantics](names.md).
- Linkage of visible symbols across compilation units is defined in [Import Export Semantics](import-export.md).
