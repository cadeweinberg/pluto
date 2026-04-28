# v0.1 Decisions

This document records preliminary decisions currently accepted for Pluto v0.1 based on recent planning/spec commits.
It is a working decision log and may be tightened into a final ADR-style set before v0.1 closure.

## Authority Boundary

This document is the source of truth for accepted decision records and rationale.
It should not restate broad scope baseline text owned by [Charter](charter.md) except as needed inside specific decision records.
Open unresolved questions belong in [Further Considerations](further-considerations.md).

## Preliminary Accepted Decisions

### D-001: Scope Lock to EBNF Round-Trip Slice

Decision:

- v0.1 is scoped to deterministic TI-IR text grammar parse/emit round-trip behavior.

Implications:

- v0.1 artifacts optimize for grammar + semantics clarity, not full compiler pipeline completeness.
- Exit criteria must validate deterministic round-trip and scope discipline.

### D-002: Minimal Opcode Surface for v0.1

Decision:

- Accepted opcode set for v0.1 is `add`, `sub`, `mul`, `div`, `mod`, `ret`.

Implications:

- `ret` is the only in-scope terminator opcode.
- Control-flow/interprocedural opcodes (`br`, `phi`, `call`, etc.) are explicitly out of scope for v0.1.

### D-003: Module Concept Reduced to Symbol Visibility Boundary

Decision:

- Module semantics in v0.1 define symbol visibility boundaries only.
- Module semantics do not own type/function/constant behavior contracts.

Implications:

- Declaration-specific semantics remain in domain docs (`types.md`, `constants.md`, `functions.md`).
- Import/export rules govern cross-boundary symbol visibility.

### D-004: No Qualified Name Syntax in v0.1

Decision:

- Name references are unqualified in v0.1 text syntax.
- Dotted qualification syntax is invalid.

Implications:

- Name grammar is restricted to `ident` and `@N` temporary forms.
- Cross-module visibility is policy/metadata-driven rather than text-level qualified addressing.

### D-005: Reused Metadata Form with Optional Value

Decision:

- Metadata entries use `meta_decl = ident, [ string_literal ], ";"`.
- Omitted value denotes flag-style metadata.

Implications:

- File metadata can represent both key-value and key-only flags.
- Metadata parser path is shared between file-level and declaration-attached metadata.

### D-006: Declaration-Attached Metadata for Type/Const/Fn

Decision:

- v0.1 supports declaration-attached metadata lists for `type_decl`, `const_decl`, and `function_decl`.
- Attachment form is `decl_meta_list` and applies to exactly one following declaration.

Implications:

- Per-declaration annotations are available without adding new declaration kinds.
- Key collisions inside one attachment list are validation errors unless a key rule defines merge behavior.

### D-007: Single Source of Truth by Spec Domain

Decision:

- Grammar and semantic ownership is split across domain documents with cross-cutting links.

Implications:

- Grammar remains centralized in TI-IR grammar doc.
- Domain semantics remain localized (module, names, import/export, instructions, etc.) to reduce drift.

## Discussion

The notes below summarize recurring rationale themes across decisions.

### Version Label

Use v0.1 consistently across planning, roadmap, and acceptance documents so scope stays clearly pre-1.0.

### Proof Slice

The proof slice validates deterministic TI-IR text round-trip behavior. It does not prove verifier, lowering, or backend readiness.

### TS-IR Contract

TS-IR is deferred beyond v0.1 and is not part of the v0.1 proof-slice contract.

### Verifier-First Depth

Verifier and pass-manager depth are deferred beyond v0.1.

### Semantics Boundary

Pluto is planning around backend and IR semantics, not source-language design.

### Module and Name Model

Modules in v0.1 are visibility boundaries, not namespace-qualified naming roots.
Name syntax is intentionally unqualified (`ident` or `@N`) for the round-trip slice.

### Metadata Model

Metadata is intentionally uniform and reusable:
- file-level metadata and declaration-attached metadata use the same `meta_decl` shape,
- value payload is optional,
- declaration attachment is limited to type/const/function in v0.1.

### Implementation Language

C++20 is the implementation baseline for the core system.
This choice should not force object-heavy designs that conflict with Pluto's data-oriented architecture goals.

### C API Boundary

The C API boundary remains a future deliverable and is out of scope for v0.1.

### Data Oriented Design Rubric

DOD principles are part of the architecture baseline because TI-IR storage and traversal costs are central to the project design.

## Outstanding Clarifications (Preliminary)

1. Define canonical reserved keys for declaration-attached metadata in v0.1 (if any).
2. Decide whether duplicate metadata keys may be tolerated for specific keys via explicit merge semantics.
3. Confirm whether declaration metadata ordering must be preserved exactly in emitter round-trip output.
