# Backlog

Single source of truth for planned, in-progress, and proposed work on Goblin Engine. Update this file whenever work is planned, started, or completed. Do not let a task live only in a prompt or chat.

Status legend: `todo` (planned), `doing` (in progress), `done` (complete), `blocked` (waiting on something).

Priorities: `P0` (critical), `P1` (high), `P2` (medium), `P3` (low).

---

## 0. Documentation & Governance

| ID | Item | Status | Priority | ADR/RFC | Notes |
|----|------|--------|----------|---------|-------|
| D-01 | Rewrite `INDEX.md` (module root) to reflect the fork, not branding-only | done | P0 | — | Rewritten as fork-focused index |
| D-02 | Rewrite `STRUCTURE.md` to document actual override mechanisms | done | P0 | — | Rewritten to document the three override mechanisms |
| D-03 | Update `GOBLIN_FORK_PLAN.md` §4 to match implemented override mechanisms | done | P0 | — | Now documents `GOBLIN_MODULE_OVERRIDES` + `goblin_add_library()` |
| D-04 | Write `gdscript_features.md` documenting fork language additions | done | P0 | — | Union types, @private, String ctors documented |
| D-05 | Create ADRs for accepted decisions | done | P0 | — | 0001-0003 accepted, 0004-0007 proposed |
| D-06 | Keep `LIGHTMAP_INVESTIGATION.md` as reference for lightmap core changes | done | P3 | — | Decision: it informs C-01/C-02, so it stays in the fork |
| D-07 | Verify editor texture import works with compression modules disabled | todo | P1 | 0003 | `basis_universal`/`ktx`/`astcenc`/`etcpak` trimmed unconditionally — needs editor build test |

---

## 1. GDScript Language Features

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| G-01 | Union types (`int \| String`, `Dictionary \| null`) | done | P1 | — | 0004 | In fork; regression tests added (union dedup/collapse, null typing) |
| G-02 | `@private` annotation | done | P2 | — | — | Enforced for vars/funcs/consts/inner classes; same-script access policy; `@export` conflict error; regression tests added. Subclass name reuse deliberately NOT supported (costs an O(n) scan on instance creation + sparse member indices) |
| G-16 | Regression tests for G-01..G-03 (`private_member_access`, `null_type_assignment`, `null_null_union`, etc.) | done | P1 | — | — | Added to mirror `tests/scripts/`; `.out` files written by hand — verify with `--gdscript-generate-tests` on a `tests=yes` build |
| G-03 | String constructors (`String(int)`, `String(float)`, `String(bool)`) | done | P3 | — | — | Already in fork, via `core/variant` override |
| G-04 | Safe navigation `?.` | todo | P1 | 2-3d | — | Replace hundreds of `if x != null:` guards |
| G-05 | Null coalescing `??` | todo | P1 | 2-3d | — | Pairs with G-04 |
| G-06 | `swap(a, b)` built-in | todo | P3 | 1h | — | Already in gdscript2; trivial port |
| G-07 | Structs / value types | todo | P1 | 4-6w | — | Biggest gap. Dict-heavy entity model, 60+ `duplicate(true)`. De-risk with 50 parser-only test cases first |
| G-08 | Typed dictionaries `Dictionary[K, V]` | todo | P1 | 1-2w | — | Kills ~30 `typeof()`+`as` checks in navigation |
| G-09 | Built-in `PriorityQueue` | todo | P2 | 2-3d | — | Navigation Dijkstra is O(N²) with no heap |
| G-10 | Inline caching (property access) | todo | P2 | 2-3d | — | From gdscript2; faster physics/AI hot paths |
| G-11 | Opcode fusing | todo | P2 | 3-5d | — | From gdscript2 `opcode_fusing`; fused array/dict/iterate |
| G-12 | Blocks / stack-bound callables | todo | P3 | 2-3w | — | Kills 33+ `sort_custom` lambda allocations |
| G-13 | `yield` generators | todo | P3 | 3-4w | — | Lazy iteration without intermediate arrays |
| G-14 | Generics + `typeinfo` | todo | P3 | 4-6w | — | Typed containers without boxing; long-term |
| G-15 | Named args, destructuring | todo | P3 | — | — | Readability only |

---

## 2. Core Engine Changes

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| C-01 | Fix LightmapGI frustum culling (#71585) | todo | P0 | 1-2d | — | Critical bug; runtime lightmap pipeline partially works around it |
| C-02 | Runtime LightmapBaker as public API | todo | P1 | 2-3d | 0006 | Eliminates `ClassDB.class_exists("LightmapBaker")` guard |
| C-03 | MIDI in `AudioStreamPlayer3D` | todo | P1 | 1-2d | — | 3D spatialized MIDI needs manual node construction today |
| C-04 | `Vector3i` keys for AStar3D | todo | P2 | 1-2d | — | Kills `"%d\|%d\|%d"` string keys in nav hot path |
| C-05 | Generic spatial field / probe grid (light + audio + effects) | todo | P2 | 2-3w | — | Replaces viewport-based `LightSensor`; one field infrastructure, many consumers (see M-07) |

---

## 3. Modules & Build

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| B-01 | Module trim (30 modules disabled) | done | P1 | — | 0003 | Implemented in `config.py` |
| B-02 | Whole-module override mechanism | done | P1 | — | 0001 | `GOBLIN_MODULE_OVERRIDES` in `SCsub` |
| B-03 | Single-file core override mechanism | done | P1 | — | 0001 | `goblin_add_library()` in `config.py` |
| B-04 | Retry-loop replacement (compile-time overrides for `editor_about.cpp`) | todo | P1 | 1-2d | 0007 | Remove the 120-attempt SceneTree polling |
| B-05 | Add `--max-drift=1 --implicit-deps-unchanged` to default build | todo | P3 | — | — | Faster incremental builds |
| B-06 | Platform driver trim decision | todo | P3 | — | — | Keep all platforms; decide on per-platform audio drivers |

---

## 4. Migrate From The From-Scratch Engine

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| M-02 | Script module tier system (Stable/Tooling/Expert) | todo | P3 | — | — | Governs which internals GDScript exposes |
| M-03 | Basis-frame transform convention | todo | P3 | — | — | Eliminates Euler/axis-order ambiguity |
| M-04 | Component family contracts | todo | P3 | — | — | Basis for struct-based ECS |
| M-05 | Portals & mirrors as **custom nodes** (`PortalSurface3D`/`MirrorSurface3D`), not first-class core | todo | P3 | — | — | `SubViewport` + teleport + portal-aware queries |
| M-06 | Retro-native rendering as **editor-provided** nodes/plugins (palette, dither, color cycling) | todo | P3 | — | — | Not core renderer changes |
| M-07 | Generic spatial field system (light + audio + effects probe grid) | todo | P2 | — | — | The ambient-probe field generalized; drives light/audio/music/effects |
| M-08 | Native stealth shadow value (Thief light gem) | todo | P2 | — | — | Gameplay readout on top of M-07 field; GDR-009a |
| M-09 | Hitscan surface metadata contract | todo | P3 | — | — | Raycast → surface class + object ID + impact UV |
| M-10 | Kinematic brush movers (doors/lifts/crushers) | todo | P3 | — | — | Generalizes DB `Moving` via `AnimatableBody3D` |
| M-11 | Lightstyle channels + retro surface-class lighting | todo | P3 | — | — | Style-channel modulation of baked light |
| M-12 | Per-view palette selection and blending | todo | P3 | — | — | Portal views inherit palette overrides |
| M-13 | Texture-space animation families (UV scroll, frame cycling) | todo | P3 | — | — | Color-cycling mechanism, in-shader via simulation clock |

*Already adopted by DB (not in backlog):* cadence scheduler with custom process groups (`scheduler.gd`), scene-first composition, partition streaming, delta save/load, Lego-block entity composition. Off-screen simulation was evaluated and dropped — the scheduler + event queue already covers the need.

---

## Dependency Notes

- **G-07 (structs)** blocks or de-risks G-08 (typed dicts) and M-04 (component families).
- **G-10/G-11** are ported from the gdscript2 module's branches — cherry-pick one at a time, never wholesale.
- **C-01** is the highest-priority core fix; it is a known upstream bug and the fix is upstream-acceptable.
- **C-05 (generic field)** is the sampling infrastructure; **M-08 (stealth shadow value)** is the gameplay readout on top of it.
- **M-05 (portals/mirrors)** is a custom-node feature, not a core change — cheap to attempt, no engine surgery.
- **D-01..D-05** are complete; the agents now work from a coherent foundation.
