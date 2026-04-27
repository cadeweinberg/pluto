# Roadmap

## Steps

1. Finalize v0.1 charter and terminology in top-level docs: explicitly scope Pluto to backend/compiler 
   semantics and exclude frontend/high-level language semantics. This is the baseline guardrail for all subsequent sections.

2. Define TI-IR model and storage contract: instruction/value/block/function entities, ownership/lifetime model,
   table-centric storage layout decision, mutation/access semantics, and serialization boundaries.

3. Specify SSA invariants and validation rules for TI-IR: single-definition constraints, def-use correctness, 
   dominance/phi placement requirements, type/operand consistency checks, and canonical verifier diagnostics schema.

4. Define explicit TI-IR to TS-IR contract: transformation boundary, preserved vs lowered invariants, 
   legal forms at each stage, and failure modes when lowering preconditions are not met. (Depends on steps 2 and 3.)

5. Define pass architecture with verification as first-class workflow: pass interfaces, pass manager responsibilities,
   analysis invalidation policy, verifier pass entry points, and verification checkpoints between stages. (Depends on steps 3 and 4.)

6. Define abstract backend target interface and x86-64 SysV binding: target capability traits, 
   register/calling convention abstraction points, legal instruction constraints, and mapping contract
   for an x86-64 SysV backend plug-in. (Depends on steps 4 and 5.)

7. Add minimal executable proof slice for v0.1 exit: a tiny end-to-end skeleton that loads or constructs a toy TI-IR unit, 
   runs verifier API hooks, and exercises target-interface plumbing for x86-64 SysV without requiring full codegen quality.

8. Add milestone and acceptance gating (M0-M2): M0 (IR + invariants), M1 (pipeline + contracts), M2 (target abstraction + proof slice),
   with explicit pass/fail criteria and deferred-items log.

## Step Deliverables and Discussion

### Step 1: Charter and Terminology

**Status**: Done

**Deliverables**
1. [Charter](charter.md)
2. [Terminology](terminology.md)
3. [Exit-Gate Checks](exit-gate-checks.md)

**Discussion**
This step defines v0.1 scope boundaries and canonical language used by all downstream documents.

### Step 2: TI-IR Model and Storage Contract

**Status**: Not Started

**Deliverables**
1. TI-IR entities and structural model specification.
2. Storage contract covering layout, access, mutation, and serialization boundaries.
3. [Data Oriented Design Principles](data-oriented-design-principles.md) for architectural evaluation.
4. TI-IR serialization boundary C API contract.

**Discussion**
Use this section to settle data model trade-offs before defining stricter invariants.
Apply the DOD principles document as the default rubric for storage and API decisions.

### Step 3: SSA Invariants and Validation Rules

**Status**: Not Started

**Deliverables**
1. Formal SSA invariants for TI-IR.
2. Validation rule set with deterministic diagnostic categories.

**Discussion**
Use this section to resolve ambiguity around dominance, phi legality, and type consistency checks.

### Step 4: TI-IR to TS-IR Contract

**Status**: Not Started

**Deliverables**
1. Stage boundary contract with preconditions and postconditions.
2. Failure behavior for invalid lowering inputs.

**Discussion**
This section should record which invariants are preserved vs transformed across the boundary.

### Step 5: Pass Architecture and Verification Workflow

**Status**: Not Started

**Deliverables**
1. Pass and pass-manager interface contracts.
2. Analysis invalidation policy and verifier checkpoint map.

**Discussion**
Capture pipeline-order constraints and where verification must be mandatory.

### Step 6: Target Abstraction and x86-64 SysV Binding

**Status**: Not Started

**Deliverables**
1. Abstract target capability interface.
2. x86-64 SysV binding contract aligned to that interface.

**Discussion**
Use this section to keep target-specific details isolated from target-independent contracts.

### Step 7: Minimal Executable Proof Slice

**Status**: Not Started

**Deliverables**
1. Minimal runnable slice exercising TI-IR, verifier hooks, and target-interface plumbing.
2. Proof-slice success criteria aligned with v0.1 exit checks.

**Discussion**
Record exact scope limits so this stays a contract-validation artifact, not a production prototype.

### Step 8: Milestones and Acceptance Gating

**Status**: Not Started

**Deliverables**
1. M0-M2 milestone definitions with pass/fail criteria.
2. Deferred-items log and explicit out-of-scope carryover list.

**Discussion**
Use this section to finalize objective readiness criteria for v0.1 closure.