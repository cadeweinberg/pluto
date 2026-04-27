# v0.1 Decisions

This document records decisions that are currently accepted for Pluto v0.1.

## Top-Level Decisions

1. Version label is v0.1, not v1.
2. v0.1 requires a minimal executable proof slice.
3. v0.1 includes an explicit TS-IR contract.
4. Verifier-first depth is formal rules plus pass API plus test matrix specification, without requiring a full verifier implementation in v0.1.
5. Backend semantics are assembly-like; high-level language semantics are excluded.
6. Implementation language baseline is C++20.
7. A C API is a deliverable at the TI-IR serialization boundary.
8. Data Oriented Design principles are the default rubric for TI-IR and storage architecture decisions.

## Discussion

### Version Label
Use v0.1 consistently across planning, roadmap, and acceptance documents so scope stays clearly pre-1.0.

### Proof Slice
The proof slice exists to validate contracts, verifier wiring, and target-interface integration. It is not intended to prove production readiness.

### TS-IR Contract
TS-IR remains explicitly in scope because backend abstraction and target binding need a defined lowering boundary.

### Verifier-First Depth
v0.1 requires enough verifier definition to make invariants and pass boundaries testable, but not a feature-complete implementation.

### Semantics Boundary
Pluto is planning around backend and IR semantics, not source-language design.

### Implementation Language
C++20 is the implementation baseline for the core system.
This choice should not force object-heavy designs that conflict with Pluto's data-oriented architecture goals.

### C API Boundary
The C API is intentionally constrained to the TI-IR serialization boundary so the external integration surface stays small and stable.

### Data Oriented Design Rubric
DOD principles are part of the architecture baseline because TI-IR storage and traversal costs are central to the project design.
