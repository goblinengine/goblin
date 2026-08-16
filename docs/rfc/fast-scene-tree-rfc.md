# Goblin Engine — FastSceneTree RFC

- **Proposal file:** `modules/goblin/docs/rfc/fast-scene-tree-rfc.md`
- **Date:** 2026-08-16
- **Status:** Proposed — direction locked (user 2026-08-16): **`FastSceneTree : public MainLoop` — full re-implementation**. Extending SceneTree rejected (inherits the non-virtual private machinery that cannot be optimized).
- **Base:** Godot 4.7.1-stable, fork plan v0.2.0
- **Companion plan:** `modules/goblin/docs/plans/fast-scene-tree-plan.md`
- **Backlog:** M-14

---

## 1. Purpose

A fork-owned, SceneTree-compatible main loop class, **re-implemented from `MainLoop`**:

- New class `FastSceneTree : public MainLoop` — zero ClassDB conflict, registered like any module class, selected per-project via `application/run/main_loop_type`.
- **Full re-implementation** of the SceneTree contract (frame loop, notifications, groups, timers, pause, input, scene change, multiplayer API) — same public API, same signals, same semantics, **new internals**: no per-frame process-list copies, no per-call group copies, no string dispatch, deterministic ordering.
- Extensible afterward (cadence API, flat-data EntityNode stage).

## 2. Why MainLoop and not SceneTree (locked 2026-08-16)

- Extending `SceneTree` inherits the machinery we need to replace: `_process_group` (per-frame copy, scene_tree.cpp:1201), group-call copies (:397/469/532/1448), `_flush_ugc` hash order (:323), and the private data structures (`ProcessGroup`, `group_map`) are **private non-virtual** — a subclass can only add parallel systems, never replace the internals.
- Extending `MainLoop` and re-implementing gives full control of every hot path. The class name is ours; `ClassDB::instantiate(main_loop_type)` at main.cpp:4416 selects it; default flips via project setting (`main.cpp:4361`). Editor hardcode `memnew(SceneTree)` (main.cpp:4358) is untouched.

## 3. The Node seam — the generic interface (locked 2026-08-16)

`Node` is compile-time typed against `SceneTree`. The **proper fix** (user 2026-08-16): the seam
operates on **any main loop type** — nothing hardcodes `SceneTree` or `FastSceneTree`.

```
BaseSceneTree : MainLoop           ← NEW additive core header: scene/main/base_scene_tree.h
 ├── SceneTree : BaseSceneTree     ← narrow upstream edit (base class + moved members)
 └── FastSceneTree : BaseSceneTree ← our implementation
```

- **`BaseSceneTree`** lives **in core** as a new additive header: `scene/main/base_scene_tree.h`
  (next to scene_tree.h, same layer; header-only — pure virtual methods, no .cpp). It cannot live
  in the fast_scene_tree or goblin module: scene_tree.h must `#include` the complete definition
  (inheritance needs the full type), and a core header including a module header inverts the
  dependency and makes the module non-disableable. With the core placement, disabling the
  fast_scene_tree module is safe — SceneTree just extends the abstract BaseSceneTree, behavior
  identical. The include is `#include "base_scene_tree.h"` — same pattern as scene_tree.h:39
  (`scene/main/scene_tree_fti.h`). Public virtual methods covering everything the engine calls on
  the tree — `call_group`(+flags),
  `notify_group`, `set_group`, `get_root`, `get_edited_scene_root`, `create_timer`, `create_tween`,
  `queue_delete`, `get_multiplayer`, `is_paused`/`is_suspended`, `get_physics_process_time`/
  `get_process_time`, `is_debugging_*_hint`, accessibility (`is_accessibility_enabled/supported`,
  `_accessibility_force_update/notify_change`), `get_collision_debug_contact_count`,
  `get_scene_tree_fti`, plus process-group registration, xform-change-list add/remove,
  input-pause dispatch, node count. Private friend-accessed members
  (`xform_change_list`, `default_process_group`, `process_groups_dirty`, `nodes_in_tree_count`)
  move up to the interface.
- **node.h edit (permission granted, keep narrow):** `data.tree` (node.h:219) / `_set_tree`
  (:334) / `get_tree()` (:558) → `BaseSceneTree *`. The ~326 upstream `get_tree()->` call sites
  and the ~40 friend accesses then compile **unchanged** via virtual dispatch — no swaps of
  node_3d/canvas_item/viewport/window required.
- **Upstream core edit (last resort, user-sanctioned, narrow):** `scene_tree.h` — `class SceneTree
  : public BaseSceneTree` + move the 4 private members up; ~20 lines. Editor path untouched
  (`memnew(SceneTree)` at main.cpp:4358 still constructs a working BaseSceneTree).
- **No hardcoded class names in the seam.** Any future main loop type implements BaseSceneTree.
- **Type-identity caveat (unchanged, honest):** GDScript `is SceneTree` and typed
  `var t: SceneTree` annotations fail on a FastSceneTree instance (type identity, not API).
  Dynamic calls work via the interface. Lightweight corpus spot-check at validation.

### 3.1 Hardcoded `SceneTree` references outside the tree (reclassified 2026-08-16)

Grep-verified across `scene/`, `main/`, `servers/` (editor excluded — always base SceneTree):

| Site | With BaseSceneTree |
|------|--------------------|
| ~326 `get_tree()->` call sites (61 files) — public API | ✅ compile unchanged, virtual dispatch |
| friend-private accesses: `xform_change_list` (node_3d/canvas_item), `_call_input_pause` (viewport) | ✅ members/methods on BaseSceneTree |
| `SceneTree::get_singleton()` statics: performance.cpp:148, main.cpp:4434/5170, shape_3d.cpp:122 | ✅ null-guarded; no-op in game |
| `SceneTree::get_singleton()` statics: shader.cpp:147, material.cpp:124 | ⚠️ audit at P1; TOOLS-ish paths |
| `SceneTree::get_singleton()` statics: window.cpp:3284, viewport.cpp:1485 (unguarded) | ⚠️ route via BaseSceneTree accessor at P1 |
| tests/** | harness: FastSceneTree tests under FastSceneTree, rest under SceneTree |

**Verdict (user, 2026-08-16): "probably not a big deal, should mostly work" — confirmed.** The
generic interface makes the seam type-agnostic; the 4 unguarded/TOOLS sites are P1 audit items.

## 4. Architecture

- **Module:** additive `modules/fast_scene_tree/` (ADR 0008 anatomy): `SCsub`, `config.py`, `register_types.{h,cpp}`, `doc_classes/`, `tests/`, `editor/icons/`. **Self-contained** — owns FastSceneTree (phase 1) AND the EntityNode/EntityComponent layer (phase 2, `entity-node-rfc.md`): the entity registry + component pools are tree-owned data, so the entity system is a subsystem of this module, not a separate one.
- **Class:** `FastSceneTree : public MainLoop`, `GDCLASS`, registered in ClassDB.
- **Selection:** reference title `project.godot`: `application/run/main_loop_type = "FastSceneTree"`. Games get it; editor untouched.
- **Re-implementation surface** (must match upstream semantics): `initialize`, `process(delta)`, `physics_process(delta)`, `iteration_prepare/end`, `finalize` (MainLoop virtuals); groups (`call_group` + flags DEFAULT/REVERSE/DEFERRED/UNIQUE, `get_nodes_in_group`, `notify_group`, `set_group`); timers (`create_timer` semantics incl. process_always/physics/ignore_time_scale); pause/suspend; scene change API; multiplayer API; accessibility hooks; signals (`process_frame`, `physics_frame`, `node_added/removed/renamed`, `tree_changed`, `tree_process_mode_changed`); `get_node_count`, `queue_delete`, `add_idle_callback`, physics-interpolation flags.

## 5. What we optimize (new internals)

| Tier | What | Replaces |
|------|------|----------|
| T1 | Flat process lists with tombstone/epoch iteration | per-frame `nodes_copy` (scene_tree.cpp:1201) + `nodes_removed_on_group_call` hashes (:1208) |
| T2 | Intrusive group lists, direct Callable dispatch, incremental order | group-call copies (:397/469/532/1448), StringName dispatch, `_update_group_order` re-sorts |
| T3 | Ordered unique-group-call flush (call order) | `_flush_ugc` HashMap order (:323) |
| T4 | Subtree mode flags (skip paused/suspended subtrees wholesale) | per-node `can_process()` checks (:1214) |
| T5 | Cadence API: `register_cadence(name, rate_hz, callable)` direct dispatch; frame hooks for SimServer | scheduler `call_group` hot path |
| T6 | Allocation-free iteration + stable iteration for C++ consumers | `get_children()` TypedArray (:1867) |

### 5.1 Memory-churn specifics (full list, 2026-08-16)

| # | Upstream cost | Replacement |
|---|---------------|-------------|
| M1 | per-frame process vector copy: ~40KB alloc+copy per 5k nodes, 60×/s | zero-copy epoch iteration (T1) |
| M2 | `get_children()` TypedArray alloc + refcounts per call (node.cpp:1867) | allocation-free iteration API (T6); GDScript keeps TypedArray |
| M3 | group-call vector copies per cadence per tick | intrusive group lists (T2) |
| M4 | children cache full dump+sort rebuild on every remove (node.cpp:1771→1786) | incremental cache maintenance (remove_at memmove; no rebuild) — node.cpp seam |
| M5 | `tree_changed()` signal per mutation — storm | coalesced: dirty flag, emit once at frame end |
| M6 | recursive `_propagate_*` walks O(subtree) per add/remove (node.cpp:595) | flat worklist + dirty flags, iterative |
| M7 | `SceneTreeTimer` alloc per `create_timer` | free-list pooling, reuse instances |

M4–M7 live in tree-owned lists and the children-cache maintenance; M4/M6 touch the Node children path via the interface seam (still no file swaps).

## 6. What stays upstream

`scene_tree.{h,cpp}` themselves (editor path + compatibility reference — we re-implement the contract in the new class, we do not shadow or replace the upstream tree). Node seam changes are the only upstream touch: node.h additive field + node.cpp swap.

## 7. Compatibility surface (must be re-implemented identically)

- Notification order: ENTER_TREE → READY → PROCESS; EXIT_TREE ordering; PARENTED/UNPARENTED; CHILD_ORDER_CHANGED
- Signal semantics (list in §4); group-call flags; process mode/priority/thread-group semantics; pause/suspend; quit; timers/tweens; scene-change API; multiplayer; accessibility
- GDScript dynamic API: `get_tree().*` calls resolve on the actual object — must expose identical method names/signatures
- **Known divergence:** `is SceneTree` / typed `SceneTree` annotations fail on FastSceneTree (type identity, not API). Documented; corpus audit gates P5.

## 8. Deferred

EntityNode/EntityComponent flat-data stage — future RFC on T6 stable-iteration hooks
(`entity-node-rfc.md`, deferred until FastSceneTree ships). SimServer (RID-space) orthogonal;
S-01 cadence pipeline may consume T5 `register_cadence`.

## 9. Risks

| Risk | Mitigation |
|------|-----------|
| node.h edit is upstream-touch | ADR 0009 additive-field precedent; one field + return-type change; smallest sanctioned header edit |
| node.h/scene_tree.h seam edits must keep base-SceneTree semantics byte-identical | Editor smoke test at every phase; seam edits kept narrow and re-diffed on rebase |
| Re-implementation semantics drift (notification order, flags, timers) | Contract test matrix (P1) + full suite + corpus + 342 tests + level load per phase |
| Typed SceneTree usage in corpus breaks | Corpus audit at P0/P5; documented type-identity divergence |
| Scope (full tree re-implementation is the largest fork change) | Hard phase gates; P2 pure-contract baseline before optimization |
| Rebase drift of node.h/node.cpp | Porting-skill mirror discipline; friend-contract grep canary |

## 10. Open questions (decide at P1/P7)

1. A/B benchmark scope (P8): which tests/corpus scenarios run under both trees.
2. Type-identity: lightweight corpus spot-check at P7 — dynamic calls via the interface are the norm.
3. T5 cadence API shape: exact `register_cadence` signature + SimServer integration point.
4. Test harness (locked 2026-08-16): FastSceneTree tests run under FastSceneTree; all other tests under the regular SceneTree; fork default = FastSceneTree when present (project-settings default injection, user-overridable).
