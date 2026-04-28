# Roadmap

## Authority Boundary

This document is the source of truth for step sequencing, per-step status, and deliverables.
Scope boundaries are owned by [Charter](charter.md).
Accepted choice rationale is owned by [Decisions](decisions.md).
Pass/fail gate criteria are owned by [Exit-Gate Checks](exit-gate-checks.md).

## Steps

Scope reference:
- v0.1 scope is defined in [Charter](charter.md).
- Deferrals and refinements are tracked in [Decisions](decisions.md).

1. Finalize v0.1 charter and terminology in top-level docs: explicitly scope Pluto to backend/compiler 
   semantics and exclude frontend/high-level language semantics. This is the baseline guardrail for all subsequent sections.

2. Define TI-IR model and storage contract: instruction/value/block/function entities, ownership/lifetime model,
   table-centric storage layout decision, and mutation/access semantics. (C serialization API is out of scope for v0.1.)

3. Specify SSA invariants and validation rules for TI-IR. **Deferred**

4. Define explicit TI-IR to TS-IR contract. **Deferred**

5. Define pass architecture with verification as first-class workflow. **Deferred**

6. Define abstract backend target interface and x86-64 SysV binding. **Deferred**

7. Add minimal executable proof slice for v0.1 exit: parse TI-IR assembly text using the v0.1 EBNF grammar and emit TI-IR assembly text back out,
   with deterministic round-trip behavior for the accepted v0.1 forms.

8. Add milestone and acceptance gating for v0.1: M0 (EBNF round-trip slice), with explicit pass/fail criteria and deferred-items log.

## Step Deliverables and Discussion

### Step 1: Charter and Terminology

Status:

Done

Deliverables:

1. [Charter](charter.md)
2. [Terminology](terminology.md)
3. [Exit-Gate Checks](exit-gate-checks.md)
4. [Plan Overview](../../plan.md) ownership/deconfliction map

Discussion:

This step defines v0.1 scope boundaries and canonical language used by all downstream documents.

### Step 2: TI-IR Model and Storage Contract

Status:

In-Progress

Deliverables:

1. TI-IR entities and structural model specification. **Status**: In-Progress
2. Storage contract covering layout, access, mutation, and TI-IR text assembly serialization boundaries. **Status**: In-Progress
3. TI-IR EBNF grammar specification. **Status**: In-Progress
4. Domain semantics specifications for each TI-IR language domain. **Status**: In-Progress
5. [Data Oriented Design Principles](data-oriented-design-principles.md) for architectural evaluation. **Status**: Done
6. TI-IR serialization boundary C API contract. **Status**: Deferred

Discussion:

Use this section to settle data model trade-offs for the v0.1 round-trip slice.
Apply the DOD principles document as the default rubric for storage and API decisions.

### Step 3: SSA Invariants and Validation Rules

Status:

Deferred

Deliverables:

1. Formal SSA invariants for TI-IR.
2. Validation rule set with deterministic diagnostic categories.

Discussion:
Use this section to resolve ambiguity around dominance, phi legality, and type consistency checks.

### Step 4: TI-IR to TS-IR Contract

Status:

Deferred

Deliverables:

1. Stage boundary contract with preconditions and postconditions.
2. Failure behavior for invalid lowering inputs.

Discussion:
This section should record which invariants are preserved vs transformed across the boundary.

### Step 5: Pass Architecture and Verification Workflow

Status:

Deferred

Deliverables:

1. Pass and pass-manager interface contracts.
2. Analysis invalidation policy and verifier checkpoint map.

Discussion:
Capture pipeline-order constraints and where verification must be mandatory.

### Step 6: Target Abstraction and x86-64 SysV Binding

Status:

Deferred

Deliverables:

1. Abstract target capability interface.
2. x86-64 SysV binding contract aligned to that interface.

Discussion:
Use this section to keep target-specific details isolated from target-independent contracts.

### Step 7: Minimal Executable Proof Slice

Status:

In-Progress

Deliverables:

1. Minimal runnable TI-IR assembly parser for the v0.1 EBNF grammar.
2. Deterministic TI-IR assembly emitter for accepted v0.1 forms.
3. Round-trip proof tests (`parse -> emit`) aligned with v0.1 exit checks.

Discussion:
Record exact scope limits so this remains an assembly round-trip artifact, not a full verifier or backend prototype.

### Step 8: Milestones and Acceptance Gating

Status:

In-Progress

Deliverables:

1. M0 definition for EBNF round-trip proof slice with pass/fail criteria.
2. Deferred-items log and explicit out-of-scope carryover list.

Discussion:
Use this section to finalize objective readiness criteria for v0.1 closure.

## Cross-Doc Consistency Rules

1. Step statuses are updated only in this roadmap.
2. Status labels used here follow [Status Vocabulary](terminology.md).
3. If a step implies a scope change, update [Charter](charter.md) and record the decision in [Decisions](decisions.md).
4. Exit-gate wording here should summarize only; normative checks live in [Exit-Gate Checks](exit-gate-checks.md).