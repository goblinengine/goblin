# FastSceneTree — Implementation Plan

Spec: `modules/goblin/docs/rfc/fast-scene-tree-rfc.md` (proposed 2026-08-16). Companion to backlog M-14. Direction locked 2026-08-16 (user): **`FastSceneTree : public MainLoop` — full re-implementation** (extending SceneTree rejected — inherits unoptimizable private machinery).

## Goal

Additive module `modules/fast_scene_tree/` providing `FastSceneTree : public MainLoop`, a full re-implementation of the SceneTree contract with new internals; selected per-project via `application/run/main_loop_type`. Mandatory seam work: node.h additive `fast_tree` field + return-type change (ADR 0009), node.cpp swap with dual-path (mechanism #2). Editor untouched (base SceneTree). **The module also hosts the EntityNode/EntityComponent layer (phase 2, `entity-node-rfc.md`) — same module, own subdirs, shipped after the tree validates.**

## Phases

| Phase | What | Files | Effort | Gate |
|-------|------|-------|--------|------|
| **P0** | Evidence + audit: instrument reference title (node counts, per-frame tree costs, scheduler `call_group` frequency); corpus audit for typed `SceneTree` annotations / `is SceneTree` checks; answer harness question (open q4) | temp instrumentation | 0.5–1 d | Numbers + audit counts recorded. Kill: tree+group < 5% of frame budget → park M-14. Type-identity divergence accepted or rejected from audit |
| **P1** | Contract test matrix: notification order, signal semantics, group flags, timer modes, pause/quit/scene-change — pinned as tests against BASE SceneTree first (reference behavior) | `modules/fast_scene_tree/tests/` | 1–2 d | Matrix green on base SceneTree — it IS the spec |
| **P2** | Module skeleton + seam: `modules/fast_scene_tree/` (ADR 0008), `FastSceneTree : MainLoop` class registered in ClassDB; node.h additive `fast_tree` field + `_set_tree`/`get_tree` dual-path (ADR 0009); title sets `application/run/main_loop_type`; **hardcoded-reference handling per RFC §3.1**: audit shader/material inspector sites, guard/swap window.cpp:3284 + viewport.cpp:1485, declare friend-private equivalents (`xform_change_list`, `_call_input_pause`) | `modules/fast_scene_tree/**`, node.h edit, window.cpp/viewport.cpp swap, title project.godot | 2–4 d | Game runs on FastSceneTree; P1 matrix green on FastSceneTree; editor smoke green (base path); suite green |
| **P3** | Contract implementation: frame loop (initialize/process/physics_process/iteration_prepare/end/finalize), notifications, timers, pause, scene change, signals — against the P1 matrix | `fast_scene_tree.{h,cpp}` | 4–6 d | P1 matrix green; suite + corpus + 342 tests + level load green; editor smoke green |
| **P4** | Groups + determinism: group API with flag semantics; ordered unique-call flush | `fast_scene_tree.{h,cpp}` | 2–3 d | Group-flag matrix green; determinism test (call order pinned) |
| **P5** | node.cpp dual-path complete: ~40 sites route via `fast_tree` when present, base otherwise (editor) | node.cpp swap | 2–3 d | Editor smoke (base path byte-identical); game path green; profile delta vs P2 |
| **P6** | Optimizations: T1 flat process lists + epochs, T2 intrusive groups + Callable dispatch, T4 subtree mode flags, T6 allocation-free iteration; **M4–M7 memory-churn items** (incremental children cache, coalesced `tree_changed()`, iterative propagation, timer pooling) | `fast_scene_tree.{h,cpp}`, node.cpp seam | 4–6 d | Behavior identical (matrix + suite); profile deltas recorded per M-item |
| **P7** | T5 cadence API + frame hooks (SimServer integration point) | `fast_scene_tree.{h,cpp}` | 1–2 d | Cadence-equivalent of title scheduler passes; determinism test |
| **P8** | Full validation: P1 matrix + full GDScript suite + corpus + 342 tests + level load + determinism replay + editor smoke | — | 2–3 d | All gates green |
| **P9** | Docs lock: CODE_MAP, STRUCTURE, rfc/plan statuses, backlog; porting-skill mirror discipline for node.h/node.cpp | docs | 1 d | Docs consistent |

**Total: ~18–30 d.** P0 gate first; P1 matrix is the spec; every phase independently testable and revertible.

## Mechanism wiring

- ADR 0008 additive module: `modules/fast_scene_tree/` standard anatomy, auto-discovered.
- node.h: direct edit (ADR 0009 precedent — additive field `FastSceneTree *fast_tree` + `_set_tree`/`get_tree` dual-path). Smallest sanctioned header touch; needs explicit permission per rules.
- node.cpp: `_GOBLIN_FILE_OVERRIDES["scene"] = {"node": ...}` — existing mechanism #2 dict (ADR 0007).
- Selection: `application/run/main_loop_type = "FastSceneTree"` in the reference title's project.godot (main.cpp:4361 → `ClassDB::instantiate` at :4416).

## Risks & mitigations

Per RFC §9. Operational: (1) node.h edit is upstream-touch — ADR 0009 route, one field, additive-only; (2) node.cpp dual-path editor path must be byte-identical — guard on `fast_tree != nullptr`, editor smoke at P2/P5/P8; (3) re-implementation semantics drift — P1 matrix is the executable spec; (4) type-identity divergence (`is SceneTree` fails) — P0 corpus audit decides acceptability.

## Open questions (resolved at P0/P1)

1. Kill/go from P0 numbers.
2. Type-identity acceptance from corpus audit.
3. T5 cadence API shape + SimServer integration.
4. Test harness main-loop identity.
