# Backlog

Single source of truth for ALL work on Goblin Engine: planned, in-progress, completed, and rejected. Update this file whenever work is planned, started, or completed. Do not let a task live only in a prompt or chat. Detailed specs live in `.kilo/plans/`; this file tracks status.

Status legend: `todo` (planned), `doing` (in progress), `done` (complete), `blocked` (waiting), `rejected` (decided against).

Priorities: `P0` (critical), `P1` (high), `P2` (medium), `P3` (low).

---

## 0. Documentation & Governance

| ID | Item | Status | Priority | ADR/RFC | Notes |
|----|------|--------|----------|---------|-------|
| D-01 | Rewrite `INDEX.md` (module root) to reflect the fork, not branding-only | done | P0 | — | Rewritten as fork-focused index |
| D-02 | Rewrite `STRUCTURE.md` to document actual override mechanisms | done | P0 | — | Rewritten to document the three override mechanisms |
| D-03 | Update `GOBLIN_FORK_PLAN.md` §4 to match implemented override mechanisms | done | P0 | — | Now documents `GOBLIN_MODULE_OVERRIDES` + `goblin_add_library()` |
| D-04 | Write `gdscript_features.md` documenting fork language additions | done | P0 | — | Full feature doc: unions, @private, String ctors, shaped dicts, then/elthen state |
| D-05 | Create ADRs for accepted decisions | done | P0 | — | 0001-0003 accepted, 0004-0007 proposed |
| D-06 | Keep `LIGHTMAP_INVESTIGATION.md` as reference for lightmap core changes | done | P3 | — | Decision: it informs C-01/C-02, so it stays in the fork |
| D-07 | Verify editor texture import works with compression modules disabled | todo | P1 | 0003 | `basis_universal`/`ktx`/`astcenc`/`etcpak` trimmed unconditionally — needs editor build test |
| D-08 | Create `CODE_MAP.md` (navigation map) | done | P0 | — | Read before implementing, update after; wired into `.kilo/rules/rules.md` |
| D-09 | Sync `gdscript_features.md` with code state | done | P0 | — | Added shaped dicts (implemented) + then/elthen (partial); `?.`/`??` removed |
| D-10 | Vision single-sourced | done | P0 | — | `.kilo/rules/vision.md` canonical (genre family, Godot compat, decision hierarchy); `docs/vision.md` = pointer |
| D-11 | Backlog cleanup: rejected section, plan-file tickets merged, recent work logged | done | P0 | — | This file |

---

## 1. GDScript Language Features

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| G-01 | Union types (`int \| String`, `Dictionary \| null`) | done | P1 | — | 0004 | In fork; regression tests added (union dedup/collapse, null typing) |
| G-02 | `@private` annotation | done | P2 | — | — | Enforced for vars/funcs/consts/inner classes; same-script access policy; `@export` conflict error. Subclass name reuse deliberately NOT supported (O(n) scan cost on instance creation + sparse member indices) |
| G-03 | String constructors (`String(int)`, `String(float)`, `String(bool)`) | done | P3 | — | — | In fork, via `core/variant` override |
| G-16 | Regression tests for G-01..G-03 (`private_member_access`, `null_type_assignment`, `null_null_union`, etc.) | done | P1 | — | — | Added to mirror `tests/scripts/`; `.out` files written by hand — verify with `--gdscript-generate-tests` on a `tests=yes` build |
| G-17 | Shaped dictionary literals (typed entries, Lua style) | done | P1 | — | — | Parser/analyzer/runtime/autocomplete; tests added |
| G-04 | Safe navigation `then` | doing | P1 | 1-2d | — | Keywords locked (NOT `?.`); tokens landed; parser/analyzer/compiler wiring pending — `.kilo/plans/` §3 |
| G-05 | Null coalescing `elthen` | doing | P1 | 1-2d | — | Pairs with G-04; port with explicit `!= null` (null-only) |
| G-18 | Template dictionaries (`_template` reserved key, creation-time default expansion) | todo | P1 | 3-4.5d | 0009/0010 | `.kilo/plans/` §2; builds on G-17 shaped-dict infra |
| G-19 | Callable shorthand (`fn(3)` -> `fn.call(3)`, dict member callables) | todo | P2 | 1-2d | 0011 | `.kilo/plans/` §2.5 |
| G-07 | Structs / value types | todo | P1 | 4-6w | — | Biggest gap. Dict-heavy entity model, 60+ `duplicate(true)`. De-risk with 50 parser-only test cases first |
| G-08 | Typed dictionaries `Dictionary[K, V]` | todo | P1 | 1-2w | — | Kills ~30 `typeof()`+`as` checks in navigation; rides on G-17 infra |
| G-09 | Built-in `PriorityQueue` | todo | P2 | 2-3d | — | Navigation Dijkstra is O(N²) with no heap |
| G-10 | Inline caching (property access) | todo | P2 | 2-3d | — | From gdscript2; faster physics/AI hot paths |
| G-11 | Opcode fusing | todo | P2 | 3-5d | — | From gdscript2 `opcode_fusing`; fused array/dict/iterate |
| G-06 | `swap(a, b)` built-in | todo | P3 | 1h | — | Already in gdscript2; trivial port |
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
| B-07 | Remove `clean` command from `goblin_manager.py` | done | P1 | — | — | Ran `scons --clean` + deleted `.scons_cache` — hard rule 1 violation |
| B-08 | `goblin_manager.py` `build` subcommand targets linuxbsd | todo | P3 | — | — | Wrong for this project (Windows). Fix or remove the subcommand |
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
| M-09 | Hitscan surface metadata contract | todo | P3 | — | — | Raycast -> surface class + object ID + impact UV |
| M-10 | Kinematic brush movers (doors/lifts/crushers) | todo | P3 | — | — | Generalizes DB `Moving` via `AnimatableBody3D` |
| M-11 | Lightstyle channels + retro surface-class lighting | todo | P3 | — | — | Style-channel modulation of baked light |
| M-12 | Per-view palette selection and blending | todo | P3 | — | — | Portal views inherit palette overrides |
| M-13 | Texture-space animation families (UV scroll, frame cycling) | todo | P3 | — | — | Color-cycling mechanism, in-shader via simulation clock |

*Already adopted by DB (not in backlog):* cadence scheduler with custom process groups (`scheduler.gd`), scene-first composition, partition streaming, delta save/load, Lego-block entity composition. Off-screen simulation was evaluated and dropped — the scheduler + event queue already covers the need.

---

## 5. Rejected / Deferred

| Item | Why |
|------|-----|
| `?.` / `??` syntax | Keywords `then`/`elthen` locked (2026-08-13). See G-04/G-05 |
| `then` truthiness semantics (gdscript2 runtime) | Null-only decided at port — explicit `!= null` conditions (`.kilo/plans/` §3.2) |
| Native `CustomTree` cadence scheduler | Remnant; not necessary |
| `GoblinDataTable` native fallback class | Replaced by template dictionaries (G-18) |
| Replace `core/variant/dictionary.{h,cpp}` | Header override unsupported + max rebase surface; language layer covers it (G-17/G-18) |
| Expose editor `LightmapperRD` at runtime | Editor-only GPU module + forbidden build-flag changes; GL-compat templates lack RenderingDevice |
| GDScript `extends Dictionary` / user Variant types | Variant + ClassDB surgery; G-17/G-18 cover the need at language level |
| Renderer-side light sampling on GL Compatibility | No exposed cluster seams; CPU field (C-05/M-07) substitutes |
| Native upscaler nodes | DB's GL-compat canvas shaders already do this |

---

## 6. Tech Debt

Findings from the `/tech-debt-review` workflow. Fork-side debt only; upstream issues are reported to the user, not tracked here.

| ID | Item | Source | Severity | Notes |
|----|------|--------|----------|-------|

---

## Dependency Notes

- **G-04/G-05** follow `.kilo/plans/` §3 (recovered gdscript2 port map; 5 files, no VM changes).
- **G-18/G-19** follow `.kilo/plans/` §2 (spec + phases + test gates).
- **G-07 (structs)** blocks or de-risks G-08 (typed dicts) and M-04 (component families).
- **G-10/G-11** are ported from the gdscript2 module's branches — cherry-pick one at a time, never wholesale.
- **C-01** is the highest-priority core fix; it is a known upstream bug and the fix is upstream-acceptable.
- **C-05 (generic field)** is the sampling infrastructure; **M-08 (stealth shadow value)** is the gameplay readout on top of it.
- **M-05 (portals/mirrors)** is a custom-node feature, not a core change — cheap to attempt, no engine surgery.
