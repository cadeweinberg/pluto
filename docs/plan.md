# Plan: Confirm Pluto v0.1 Baseline Scope

Define and lock Pluto v0.1 as a spec-first architecture release with one minimal executable proof slice.
v0.1 will establish assembly-like backend semantics (not high-level language semantics), 
TI-IR + TS-IR contracts, SSA invariants/validation rules, pass architecture with verifier-first design,
and an abstract target interface that can host x86-64 SysV.

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

## Relevant files

1. [v0.1 Charter](plan/v0.1/charter.md) — v0.1 charter, scope boundaries, and exit framing.
2. [v0.1 Terminology](plan/v0.1/terminology.md) — canonical terminology and usage rules.
3. [v0.1 Roadmap](plan/v0.1/roadmap.md) — ordered execution plan for v0.1 work.
4. [v0.1 Exit-Gate Checks](plan/v0.1/exit-gate-checks.md) — formal checks required to clear the v0.1 exit gate.

## Checks

Checks moved to [v0.1 Exit-Gate Checks](plan/v0.1/exit-gate-checks.md) to formalize exit-gate requirements.

## Decisions

1. Version label is v0.1 (not v1).
2. v0.1 requires a minimal executable proof slice.
3. v0.1 includes an explicit TS-IR contract.
4. Verifier-first depth is formal rules + pass API + test matrix specification (no full verifier implementation requirement in v0.1 scope docs).
5. Backend semantics are assembly-like; high-level language semantics are excluded.

## Further Considerations

1. Decide whether the proof slice should be CLI-driven or library-test-driven as the canonical demonstration path.
Current direction: test-library driven. Next action: choose the test library.
2. Decide whether TI-IR serialization in v0.1 should standardize one format only (text or binary) or define both with one marked experimental.
Current direction: text-first (assembly-like), with binary marked experimental.
3. Decide whether TS-IR in v0.1 is strictly a contract/spec section or also includes a small canonical example corpus.
Current direction: include a small canonical example corpus.

