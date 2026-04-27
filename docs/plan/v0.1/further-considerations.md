# v0.1 Further Considerations

This document tracks open planning questions that are not yet fully locked but still influence v0.1 execution.

## Top-Level List

1. Decide whether the proof slice should be CLI-driven or library-test-driven as the canonical demonstration path.
2. Decide whether TI-IR serialization in v0.1 should standardize one format only (text or binary) or define both with one marked experimental.
3. Decide whether TS-IR in v0.1 is strictly a contract/spec section or also includes a small canonical example corpus.

## Discussion

### Proof Slice Delivery Path
Current direction: test-library driven.
Next action: choose the test library and define the minimal harness shape.

### TI-IR Serialization Format
Current direction: text-first, assembly-like, with binary marked experimental.
Next action: define whether binary is merely deferred or explicitly present as experimental in v0.1 docs.

### TS-IR Canonical Example Corpus
Current direction: include a small canonical example corpus.
Next action: decide whether the corpus is purely documentary, serialized fixtures, or both.
