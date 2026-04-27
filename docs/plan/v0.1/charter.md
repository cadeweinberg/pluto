# Pluto v0.1 Charter

## Purpose
Pluto is a target-independent backend framework for code generation, optimization, and analysis.
v0.1 establishes the architecture and contracts needed to implement the backend safely and consistently.

## v0.1 Objective
Define a stable, implementation-ready foundation for backend semantics and interfaces.
The baseline is assembly-like IR semantics, not high-level language semantics.

## In Scope
1. TI-IR model and storage contracts.
2. SSA invariants and validation rules.
3. TI-IR to TS-IR boundary contract.
4. Pass architecture with verifier-first workflow.
5. Abstract backend target interface.
6. x86-64 SysV binding points against that interface.
7. Minimal executable proof slice that validates the wiring and contracts.
8. M0-M2 milestones and acceptance criteria.

## Out of Scope
1. Frontend parser design and high-level language semantics.
2. Broad optimization catalog beyond what is needed to define contracts.
3. Advanced register allocation strategies (for example graph coloring).
4. Full object emission/linker parity.
5. Multi-target delivery beyond x86-64 SysV.

## Release Form
v0.1 is specification-first and includes one minimal executable proof slice.
The proof slice demonstrates contract viability, not production-quality code generation.

## Exit Criteria
1. Contract and invariant docs are complete and internally consistent.
2. Scope boundaries are explicitly documented and enforced.
3. Minimal proof slice runs and demonstrates verifier and target-interface integration.
4. Deferred work is tracked outside v0.1 scope.

## Audience
1. Backend implementers building TI-IR, TS-IR, and pass infrastructure.
2. Reviewers validating architectural consistency and scope discipline.
