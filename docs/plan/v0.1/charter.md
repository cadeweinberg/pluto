# Pluto v0.1 Charter

## Purpose
Pluto is a target-independent backend framework for code generation, optimization, and analysis.
v0.1 establishes the minimum specification and executable slice needed to prove TI-IR assembly round-trip behavior.

## v0.1 Objective
Deliver a deterministic TI-IR EBNF assembly round-trip slice (`parse -> emit`).
The baseline is assembly-like TI-IR text semantics, not high-level language semantics.

## Implementation Baseline
1. Core implementation language: C++20.
2. C API boundary is deferred and out of scope for v0.1.

## In Scope
1. TI-IR model and storage contracts.
2. TI-IR EBNF grammar specification.
3. TI-IR domain semantics specifications.
4. Minimal executable proof slice for deterministic TI-IR assembly round-trip (`parse -> emit`).
5. M0 acceptance criteria for the round-trip slice and deferred-items tracking.
6. Data Oriented Design (DOD) principles used to evaluate architecture decisions.

## Out of Scope
1. Frontend parser design and high-level language semantics.
2. SSA invariants, dominance/phi legality, and verifier-pass workflow.
3. TI-IR to TS-IR lowering contracts.
4. Pass-manager architecture and analysis invalidation workflow.
5. Backend target abstraction and x86-64 SysV binding.
6. C API deliverable at the TI-IR serialization boundary.
7. Full object emission/linker parity.

## Release Form
v0.1 is specification-first and includes one minimal executable proof slice.
The proof slice demonstrates deterministic TI-IR assembly round-trip, not full verifier or backend readiness.

## Exit Criteria
1. Grammar, domain semantics, and storage-contract docs are complete and internally consistent.
2. Scope boundaries are explicitly documented and enforced.
3. Minimal proof slice runs and demonstrates deterministic TI-IR assembly round-trip (`parse -> emit`).
4. Deferred work is tracked outside v0.1 scope.

## Audience
1. Implementers building TI-IR text parser/emitter infrastructure for v0.1.
2. Reviewers validating scope discipline, grammar completeness, and round-trip determinism.
