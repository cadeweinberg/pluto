# Pluto v0.1 Terminology

## Authority Boundary

This document is the source of truth for term definitions and usage rules.
It does not set scope boundaries, decision outcomes, or acceptance criteria.
It defines the meaning of status vocabulary used by planning documents.
Terms may include deferred concepts; deferment status is owned by [Charter](charter.md), [Roadmap](roadmap.md), and [Decisions](decisions.md).

## Canonical Terms

### Backend Semantics

Behavioral rules for IR, passes, and target interfaces that are independent of any high-level source language.

### TI-IR (Target-Independent IR)

The primary architecture-level IR used before target-specific lowering.
It uses assembly-like operations and explicit control/data flow constraints.

### TS-IR (Target-Specific IR)

The post-lowering IR form where target-dependent constraints are explicit.
TS-IR exists to prepare for concrete backend mapping and emission stages.

### SSA (Static Single Assignment)

A form where each value has exactly one definition and uses reference that unique definition.
Phi nodes reconcile control-flow merges where multiple reaching definitions exist.

### SSA Invariants

Rules that must always hold in TI-IR SSA form.
Examples: single definition per value, valid def-use links, dominance of defs over uses, and phi correctness at merge points.

### Verifier

A validation component that checks structural and semantic invariants and returns deterministic diagnostics.

### Verifier Pass

A pass-manager compatible pass that executes verifier checks at configured pipeline checkpoints.

### Pass

A transformation or analysis stage that consumes and produces IR state under defined contracts.

### Pass Manager

Pipeline coordinator responsible for ordering passes, handling dependencies, and enforcing invalidation semantics.

### Analysis Invalidation

Rules describing when cached analysis results become stale after a transform pass mutates IR.

### Contract

A precise specification of required preconditions, postconditions, and failure behavior between stages.

### Target Abstraction

The interface layer that represents target capabilities (register classes, calling convention constraints, instruction legality) without hardcoding one backend implementation.

### x86-64 SysV Binding

A concrete implementation of target abstraction for the x86-64 System V ABI.

### Minimal Executable Proof Slice

A narrow runnable artifact that proves end-to-end contract wiring for v0.1.
It is designed for correctness and integration validation, not performance or feature completeness.

## Status Vocabulary

### Done

Work is complete for v0.1 and accepted for its owning document domain.

### In-Progress

Work is actively being executed and is not yet accepted as complete.

### Deferred (Out-of-Scope for v0.1)

Work is intentionally postponed and excluded from v0.1 completion requirements.

### Open

Question or choice is unresolved and awaiting a recorded decision.

## Terminology Rules

1. Use TI-IR and TS-IR exactly with hyphen and capitalization as shown.
2. Use SSA invariants to mean mandatory structural validity rules, not optimization preferences.
3. Use backend semantics to distinguish from frontend language semantics.
4. Use contract when a rule has explicit preconditions, postconditions, and failure behavior.
5. Use proof slice only for the minimal v0.1 executable demonstration.
6. Use status terms (`Done`, `In-Progress`, `Deferred (Out-of-Scope for v0.1)`, `Open`) according to the definitions above.
