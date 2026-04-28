# Data Oriented Design Principles (v0.1)

This document defines how Pluto evaluates architecture decisions during v0.1, especially for TI-IR model/storage and pass data flow.

## Scope

These principles apply to:

1. TI-IR in-memory representation.
2. TI-IR mutation/access APIs.
3. Verifier and pass-manager data paths.
4. TI-IR serialization boundary design.

## Core Principles

1. Data layout first
Choose memory layout based on dominant access patterns and iteration cost before API ergonomics.

2. Prefer contiguous hot paths
Favor contiguous storage for frequently traversed data and avoid pointer-heavy graphs on critical paths.

3. Separate hot and cold data
Keep frequently accessed fields compact and isolate metadata or infrequently used fields.

4. Stable identities over raw pointers
Use stable IDs/handles for cross-table references to support compaction, serialization, and validation.

5. Explicit ownership and mutation
Define mutation points and ownership clearly so pass invalidation and verifier behavior remain deterministic.

6. Batch-friendly operations
Prefer APIs that allow batched queries/updates over one-object-at-a-time patterns in hot pipelines.

7. Serialization mirrors storage contracts
TI-IR serialization should preserve stable identity and structural invariants with deterministic ordering.

8. Measure before locking design
When design options are unclear, capture simple measurements and expected complexity trade-offs before finalizing.

## Decision Rubric

Use this checklist for architecture decisions:

1. What access pattern is dominant (scan, random lookup, grouped traversal)?
2. What data is hot vs cold?
3. What mutation frequency is expected?
4. How are references represented across tables (ID scheme)?
5. What invariants must remain checkable by the verifier?
6. How does serialization preserve identity and deterministic ordering?
7. What invalidation behavior follows from this design?
8. What is the fallback if this design underperforms?

## v0.1 Constraints

1. Keep design choices simple and explicit.
2. Avoid premature optimization that increases conceptual complexity without measurable benefit.
3. Record unresolved alternatives as deferred items linked to roadmap milestones.
