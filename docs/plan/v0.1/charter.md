# Pluto v0.1 Charter

## Authority Boundary

This document is the source of truth for v0.1 objective, scope boundaries, implementation baseline, release form, and audience.
Step sequencing belongs in [Roadmap](roadmap.md).
Accepted rationale-bearing choices belong in [Decisions](decisions.md).
Exit pass/fail checks belong in [Exit-Gate Checks](exit-gate-checks.md).

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
4. TI-IR Parser Specification
5. Minimal executable proof slice for deterministic TI-IR assembly round-trip (`parse -> emit`).
6. M0 acceptance criteria for the round-trip slice and deferred-items tracking.
7. Data Oriented Design (DOD) principles used to evaluate architecture decisions.

## Out of Scope

1. SSA invariants, dominance/phi legality, and verifier-pass workflow.
2. TI-IR to TS-IR lowering contracts.
3. Pass-manager architecture and analysis invalidation workflow.
4. Backend target abstraction and x86-64 SysV binding.
5. C API deliverable at the TI-IR serialization boundary.
6. Full object emission/linker parity.

## Release Form

v0.1 is specification-first and includes one minimal executable proof slice.
The proof slice demonstrates deterministic TI-IR assembly round-trip, not full verifier or backend readiness.

## Alignment Notes

1. Scope details here are normative; roadmap items must not expand scope beyond this charter.
2. Decision IDs in [Decisions](decisions.md) should reference and preserve these scope boundaries.
3. Exit checks in [Exit-Gate Checks](exit-gate-checks.md) should validate this charter directly.

## Exit Criteria

1. Grammar, domain semantics, and storage-contract docs are complete and internally consistent.
2. Scope boundaries are explicitly documented and enforced.
3. Minimal proof slice runs and demonstrates deterministic TI-IR assembly round-trip (`parse -> emit`).
4. Deferred work is tracked outside v0.1 scope.

## Audience

1. Implementers building TI-IR text parser/emitter infrastructure for v0.1.
2. Reviewers validating scope discipline, grammar completeness, and round-trip determinism.
