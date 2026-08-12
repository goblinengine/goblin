# ADR Index

Accepted and proposed Architecture Decision Records for Goblin Engine (the Godot fork).

## When To Use An ADR vs An RFC

Use an **ADR** when:
- a decision is stable enough to guide multiple implementations
- the decision is expensive to reverse later
- the decision is no longer mainly exploratory

Use an **RFC** (see [../proposal/](../proposal/)) when:
- the design is still exploratory
- the implementation shape is not yet proven enough to freeze
- multiple credible options still need evaluation

Promote an RFC to an ADR when direction and boundary are both stable enough to freeze.

## Accepted ADRs

1. [0001-source-override-architecture.md](0001-source-override-architecture.md)
   Locks the three override mechanisms: whole-module replacement, single-file core swap, and builder monkey-patching. All changes live in `modules/goblin/`.

2. [0002-stable-release-tracking.md](0002-stable-release-tracking.md)
   Locks tracking of official Godot stable release tags instead of `master`, with rebase only at release boundaries.

3. [0003-module-trim-evidence-standard.md](0003-module-trim-evidence-standard.md)
   Locks the evidence standard required before any module is disabled, and the fixed trim list.

## Proposed ADRs (not yet accepted)

4. `0004-gdscript-union-types` — union types in the GDScript fork.
5. `0005-gdscript-safe-navigation-null-coalescing` — `?.` and `??` operators.
6. `0006-runtime-lightmap-api-surface` — promote `LightmapBaker` to public API.
7. `0007-branding-compile-time-overrides` — replace retry loops with compile-time overrides.

## Not Yet ADRs

The following remain at RFC level intentionally:
- exact struct memory layout and copy semantics
- typed dictionary syntax and runtime representation
- light-probe grid data format and sampling model
- which GDScript performance optimizations (opcode fusing, inline caching, PIC) to port and in what order
