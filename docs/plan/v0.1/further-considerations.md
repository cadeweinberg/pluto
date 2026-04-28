# v0.1 Further Considerations

This document tracks open planning questions that are not yet fully locked but still influence v0.1 execution.

## Authority Boundary

This document is the source of truth for unresolved/open planning items only.
Accepted outcomes should be moved into [Decisions](decisions.md) and removed from here.
Normative scope and gates should not be defined here.

## Top-Level List

1. Decide whether the proof slice should be CLI-driven or library-test-driven as the canonical demonstration path.
2. Decide the minimum canonical fixture corpus required to prove deterministic TI-IR round-trip behavior.
3. Decide whether emitter normalization rules (whitespace/order/style) must be strict or policy-driven for v0.1 acceptance.

## Promotion Rule

When an item is resolved:
1. Record the accepted choice in [Decisions](decisions.md).
2. Update any impacted scope/step/gate docs.
3. Remove or mark the item resolved in this document.

## Discussion

### Proof Slice Delivery Path

Current direction: test-library driven.
Next action: choose the test library and define the minimal harness shape.

### Round-Trip Fixture Corpus

Current direction: keep fixtures small and focused on accepted v0.1 opcode and form coverage.
Next action: lock a minimal corpus list and expected emitted outputs.

### Emitter Normalization Rules

Current direction: deterministic output is required, but formatting strictness is still open.
Next action: define canonicalization requirements for v0.1 exit-gate checks.
