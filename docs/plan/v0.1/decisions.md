# v0.1 Decisions

This document records decisions that are currently accepted for Pluto v0.1.

## Top-Level Decisions

1. Version label is v0.1, not v1.
2. v0.1 requires a minimal executable proof slice.
3. v0.1 is restricted to TI-IR EBNF assembly round-trip (`parse -> emit`).
4. TS-IR contract and lowering boundary are deferred beyond v0.1.
5. Backend semantics are assembly-like; high-level language semantics are excluded.
6. Implementation language baseline is C++20.
7. C API deliverable at the TI-IR serialization boundary is out of scope for v0.1.
8. Data Oriented Design principles are the default rubric for TI-IR and storage architecture decisions.

## Discussion

### Version Label

Use v0.1 consistently across planning, roadmap, and acceptance documents so scope stays clearly pre-1.0.

### Proof Slice

The proof slice exists to validate deterministic TI-IR assembly round-trip behavior. It is not intended to prove verifier, lowering, or backend readiness.

### TS-IR Contract

TS-IR is deferred beyond v0.1 and is not part of the v0.1 proof-slice contract.

### Verifier-First Depth

Verifier and pass-manager depth are deferred beyond v0.1.

### Semantics Boundary

Pluto is planning around backend and IR semantics, not source-language design.

### Implementation Language

C++20 is the implementation baseline for the core system.
This choice should not force object-heavy designs that conflict with Pluto's data-oriented architecture goals.

### C API Boundary

The C API boundary remains a future deliverable and is out of scope for v0.1.

### Data Oriented Design Rubric

DOD principles are part of the architecture baseline because TI-IR storage and traversal costs are central to the project design.
