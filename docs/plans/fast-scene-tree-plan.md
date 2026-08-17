# FastSceneTree — Implementation Plan

> **SUPERSEDED 2026-08-17 (user directive): module + BaseSceneTree seam REJECTED.**
> Direction now: **modify `SceneTree` in place** — edit home is the goblin mirror
> `modules/goblin/scene/main/scene_tree.cpp` (single core-file swap, content =
> faithful upstream copy). No module, no base-class seam, no retype ripple:
> `get_tree()`/`SceneTree::get_singleton()`/editor/PM stay upstream-typed, and
> the optimized tree runs everywhere (editor, PM, games) for free. Lost vs this
> plan: A/B benchmark baseline and per-project opt-out; acceptance = suite green
> + no regressions. P5 T1–T6/M1–M7 optimizations land directly in the mirror
> (upstream scene_tree.cpp changes port manually). Full re-lock pending architect.

Spec: `modules/goblin/docs/rfc/fast-scene-tree-rfc.md` (proposed 2026-08-16). Companion to backlog M-14. Direction locked 2026-08-16 (user): **`FastSceneTree : public MainLoop` — full re-implementation** (extending SceneTree rejected — inherits unoptimizable private machinery).

## Goal

Additive module `modules/fast_scene_tree/` providing `FastSceneTree : public MainLoop`, a full re-implementation of the SceneTree contract with new internals; selected per-project via `application/run/main_loop_type`. **Seam via generic interface (locked 2026-08-16):** `BaseSceneTree : MainLoop` lives **in core** as a NEW additive header `scene/main/base_scene_tree.h` (header-only, pure virtuals — core header cannot include a module header); narrow upstream edits — `scene_tree.h` base change (`SceneTree : BaseSceneTree` + 4 moved members, ~20 lines, user-sanctioned last resort), `node.h` pointer/`get_tree()` retyped to `BaseSceneTree *` (permission granted). No dual-path, no hardcoded tree class names, no node.cpp/node_3d/viewport/window swaps. Editor untouched (base SceneTree). **The module also hosts the EntityNode/EntityComponent layer (phase 2, `entity-node-rfc.md`) — same module, own subdirs, shipped after the tree validates.**

## P0 — A/B benchmark, not a gate (locked 2026-08-16)

FastSceneTree is additive/optional (swappable via Godot's own `main_loop_type`), so no
pre-implementation kill criterion. Instead: **run the same test under SceneTree, then under
FastSceneTree, diff the result** (frame time, allocs, group-call cost). Benchmark harness is built
with P8, numbers recorded as the acceptance evidence. Tree-time < 5% of frame budget no longer
parks the work — the A/B diff IS the deliverable.

## Phases

| Phase | What | Files | Effort | Gate |
|-------|------|-------|--------|------|
| **P1** | Contract test matrix: notification order, signal semantics, group flags, timer modes, pause/quit/scene-change — pinned as tests against BASE SceneTree first (reference behavior). Also route the 4 singleton sites (shader/material inspector, window:3284, viewport:1485) via BaseSceneTree accessors | `modules/fast_scene_tree/tests/` | 1–2 d | Matrix green on base SceneTree — it IS the spec |
| **P2** | Module skeleton + seam: `modules/fast_scene_tree/` (ADR 0008); `BaseSceneTree : MainLoop` (public virtual API); narrow upstream edits: `scene_tree.h` base change + moved members (user-sanctioned), `node.h` retype to `BaseSceneTree *` (permission granted); `FastSceneTree : BaseSceneTree` class registered in ClassDB; title sets `application/run/main_loop_type`; fork default = FastSceneTree when present (project-settings default injection, user-overridable) | `modules/fast_scene_tree/**`, scene_tree.h base edit, node.h retype, title project.godot | 2–4 d | Game runs on FastSceneTree; P1 matrix green on FastSceneTree; editor smoke green (base path); suite green |
| **P3** | Contract implementation: frame loop (initialize/process/physics_process/iteration_prepare/end/finalize), notifications, timers, pause, scene change, signals — against the P1 matrix | `fast_scene_tree.{h,cpp}` | 4–6 d | P1 matrix green; suite + corpus + 342 tests + level load green; editor smoke green |
| **P4** | Groups + determinism: group API with flag semantics; ordered unique-call flush | `fast_scene_tree.{h,cpp}` | 2–3 d | Group-flag matrix green; determinism test (call order pinned) |
| **P5** | Optimizations: T1 flat process lists + epochs, T2 intrusive groups + Callable dispatch, T4 subtree mode flags, T6 allocation-free iteration; **M4–M7 memory-churn items** (incremental children cache, coalesced `tree_changed()`, iterative propagation, timer pooling) | `fast_scene_tree.{h,cpp}` | 4–6 d | Behavior identical (matrix + suite); profile deltas recorded per M-item |
| **P6** | T5 cadence API + frame hooks (SimServer integration point) | `fast_scene_tree.{h,cpp}` | 1–2 d | Cadence-equivalent of title scheduler passes; determinism test |
| **P7** | Full validation: P1 matrix + full GDScript suite (FastSceneTree tests under FastSceneTree, rest under SceneTree) + corpus + 342 tests + level load + determinism replay + editor smoke | — | 2–3 d | All gates green |
| **P8** | A/B benchmark: same test under SceneTree vs FastSceneTree (frame time, allocs, group-call cost) — the P0 deliverable, now measured | benchmark harness | 1–2 d | Numbers recorded as acceptance evidence |
| **P9** | Docs lock: CODE_MAP, STRUCTURE, rfc/plan statuses, backlog; porting-skill mirror discipline for the interface + seam edits | docs | 1 d | Docs consistent |

**Total: ~17–25 d.** P1 matrix is the spec; every phase independently testable and revertible.

## Mechanism wiring

- ADR 0008 additive module: `modules/fast_scene_tree/` standard anatomy, auto-discovered.
- **Seam (generic interface, no dual-path):** `BaseSceneTree : MainLoop` lives **in core** as a new
  additive header `scene/main/base_scene_tree.h` (header-only, pure virtuals; next to scene_tree.h
  — core header cannot include a module header). Narrow sanctioned upstream edits: `scene_tree.h`
  base class → `BaseSceneTree` + move 4 private members up (~20 lines); `node.h` `data.tree` /
  `_set_tree` / `get_tree()` retyped to `BaseSceneTree *` (permission granted). No
  `goblin_add_library` swaps needed for node.cpp/node_3d/viewport/window — the 326 call sites +
  ~40 friend accesses compile unchanged via virtual dispatch. Module stays disableable (SceneTree
  extends the abstract base regardless).
- Selection: `application/run/main_loop_type = "FastSceneTree"` in the reference title's
  project.godot (main.cpp:4361 → `ClassDB::instantiate` at :4416); fork default = FastSceneTree
  when present via project-settings default injection (user-overridable).

## Risks & mitigations

Per RFC §9. Operational: (1) the two narrow upstream edits are user-sanctioned — keep them
minimal, re-diff on rebase (porting skill); (2) the 4 moved members must keep identical semantics
for the editor's base SceneTree path — editor smoke at P2/P7; (3) re-implementation semantics drift
— P1 matrix is the executable spec; (4) type-identity divergence (`is SceneTree` fails) — dynamic
calls work via the interface; lightweight corpus spot-check at P7.

## Open questions (resolved at P1)

1. A/B benchmark scope: which tests/corpus scenarios run under both trees (P8).
2. Type-identity: lightweight corpus spot-check (P7) — dynamic calls via interface are the norm.
3. T5 cadence API shape + SimServer integration.
4. Test harness: FastSceneTree tests under FastSceneTree, rest under SceneTree (locked).
