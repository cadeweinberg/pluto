# TI-IR Module Semantics (v0.1 Draft)

Status: Draft
Scope: Semantics for module structure and module-scope declarations in TI-IR text files.

## Grammar

```ebnf
module      = "module", ident, "{" { module_decl } "}" ;
module_decl = type_decl | const_decl | function_decl ;
```

## Semantics

### Module Form

- `module` introduces a named declaration scope.
- The declaration block inside `{ ... }` is the module section.

### Module Declarations

- `module_decl` lists declarations that create symbols in module scope.

Allowed declarations:
- `type_decl`
- `const_decl`
- `function_decl`

## Contract

- Exactly one module form appears per file in v0.1.
- Module name must be a valid `ident`.
- Only `module_decl` entries are allowed inside the module section.
- Metadata declarations are file-scope only and must appear before `module`.
- Declarations are ordered and preserved in source order.
- Duplicate symbol names in the same module scope are invalid unless explicitly allowed by a future overloading rule.
- Unqualified references inside the module resolve against local/module scope first.
- Qualified names (for example `math.add`) may reference symbols outside the current module, subject to import and linker policy.

## Cross-Cutting Concerns

- Module scope is the base namespace for unqualified global symbols.
- Local function/block/value scopes shadow module-level names by normal lexical scoping rules.
- Final stable internal IDs are assigned after parse and symbol resolution.
- Detailed name syntax and resolution rules are defined in [Name Semantics](names.md).
