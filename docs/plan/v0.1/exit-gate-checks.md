# v0.1 Exit-Gate Checks

These checks formalize the minimum bar required to clear the Pluto v0.1 exit gate.

## Authority Boundary

This document is the source of truth for v0.1 pass/fail gate criteria and required evidence.
Scope definitions belong in [Charter](charter.md).
Step sequencing and status belong in [Roadmap](roadmap.md).
Decision rationale belongs in [Decisions](decisions.md).

## Required Checks

1. Scope check
Every v0.1 in-scope artifact has a named section in docs and at least one acceptance criterion.

2. Boundary check
Frontend and high-level language semantics remain explicitly out of scope in all v0.1 planning docs.

3. Contract check
TI-IR EBNF grammar and domain semantics are internally consistent across docs and aligned to the locked v0.1 opcode/scope set.

4. Proof-slice check
Minimal executable proof-slice requirements are testable and explicit, including deterministic TI-IR assembly round-trip (`parse -> emit`).

5. Exit-gate check
v0.1 completion requires docs completeness plus the agreed minimal executable proof slice, with deferred work tracked outside v0.1.

## Evidence Mapping

1. Scope boundaries and objective: [v0.1 Charter](charter.md)
2. Canonical terminology: [v0.1 Terminology](terminology.md)
3. Sequencing and status: [v0.1 Roadmap](roadmap.md)
4. Accepted rationale-bearing choices: [v0.1 Decisions](decisions.md)
5. Architecture evaluation rubric: [Data Oriented Design Principles](data-oriented-design-principles.md)
6. Planning umbrella and ownership map: [Plan](../../plan.md)
