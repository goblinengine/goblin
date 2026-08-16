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

## 3. The Node seam — what the MainLoop route costs (source-verified)

`Node` is compile-time typed against `SceneTree`:

| Site | Declaration |
|------|-------------|
| node.h:219 | `SceneTree *tree = nullptr;` (Node::Data) |
| node.h:334 | `void _set_tree(SceneTree *p_tree);` |
| node.h:558 | `_FORCE_INLINE_ SceneTree *get_tree() const` — the GDScript-bound API |
| node.cpp | ~40 friend-access call sites (`data.tree->add_to_group()`, `->node_added()`, `->default_process_group`, `->_add_process_group()`, `->nodes_in_tree_count`, `->queue_delete()`, `->get_multiplayer()` …) |
| viewport.cpp / window.cpp | 24 + 13 `get_tree()` users (public API only) |

A `FastSceneTree *` cannot be stored in or returned from a `SceneTree *` slot. Therefore the
MainLoop route **mandates** owning the Node seam:

1. **node.h direct edit (ADR 0009 precedent, additive):** add `FastSceneTree *fast_tree = nullptr;`
   to `Node::Data` + modify `_set_tree`/`get_tree` to serve both. One additive field + return-type
   change — the smallest sanctioned header touch.
2. **node.cpp swap (mechanism #2):** dual-path at ~40 sites — `fast_tree` present → FastSceneTree
   API; absent (editor, base SceneTree) → existing behavior byte-identical. Editor keeps
   `memnew(SceneTree)`; one binary serves both.
3. **Type-identity caveat (honest incompatibility):** a MainLoop-derived class is **not** a
   `SceneTree`. GDScript `is SceneTree` checks and typed `var t: SceneTree` annotations fail at
   runtime against a FastSceneTree instance. Dynamic calls (`get_tree().call_group(...)`) work if
   the re-implementation exposes the same API/signals. Corpus audit for typed SceneTree usage is a
   P0/P5 gate.

### 3.1 Hardcoded `SceneTree` references outside the tree (full inventory, 2026-08-16)

Grep-verified across `scene/`, `main/`, `servers/` (editor excluded — always base SceneTree):

**A. `SceneTree::get_singleton()` static uses — 10 game-relevant files:**

| Site | Guarded? | Handling |
|------|----------|----------|
| `main/performance.cpp:148` | ✅ `if (!sml) return` | no-op in game — none |
| `main/main.cpp:4434` (`cast_to<SceneTree>(main_loop)`) | ✅ `if (sml)` | debug hints skipped — none |
| `main/main.cpp:5170` | ✅ `scene_tree &&` | accessibility — none |
| `scene/resources/shader.cpp:147` | ⚠️ used immediately | audit at P0; TOOLS-ish path |
| `scene/resources/material.cpp:124` | ⚠️ used immediately | audit at P0; TOOLS-ish path |
| `scene/resources/3d/shape_3d.cpp:122` | ✅ `if (scene_tree)` | debug draw — none |
| `scene/main/window.cpp:3284` | ❌ unguarded `->get_root()` | swap or guard; reached via window embedding paths |
| `scene/main/viewport.cpp:1485` | ❌ unguarded | swap or guard; mouse-pos fallback |
| `scene/main/node.cpp:2624,3467` | — | owned via node.cpp swap |
| `tests/**` | — | harness constructs own tree — open q4 |

**B. Friend-private accesses through `get_tree()` — 3 files (the real seam):**
`node_3d.cpp` (`xform_change_list` ×5), `canvas_item.cpp` (`xform_change_list` ×5),
`viewport.cpp` (`_call_input_pause` ×4). FastSceneTree must declare the **same private members +
friend relationships** (`friend class Node3D/CanvasItem/Viewport`) so these resolve to our
implementation.

**C. All other `get_tree()->` call sites (~326 across 61 files) use public API**
(`call_group`, `get_root`, `is_debugging_collisions_hint`, accessibility, FTI, …) — resolve
dynamically on the actual object; satisfied by reimplementing the public surface. No handling.

**Verdict (user, 2026-08-16): "probably not a big deal, should mostly work" — confirmed by
inventory.** 6/10 singleton sites guarded no-ops; 4 need audit/swap; 3 files need friend-private
equivalents; the rest is public API.

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

M4–M7 live in the node.cpp seam or tree-owned lists; all within the dual-path swap scope.

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
| node.cpp dual-path must be byte-identical on editor path | Editor smoke test at every phase; guard on `fast_tree != nullptr` |
| Re-implementation semantics drift (notification order, flags, timers) | Contract test matrix (P1) + full suite + corpus + 342 tests + level load per phase |
| Typed SceneTree usage in corpus breaks | Corpus audit at P0/P5; documented type-identity divergence |
| Scope (full tree re-implementation is the largest fork change) | Hard phase gates; P2 pure-contract baseline before optimization |
| Rebase drift of node.h/node.cpp | Porting-skill mirror discipline; friend-contract grep canary |

## 10. Open questions (decide at P0/P1)

1. Evidence gate first: instrument reference title — node counts, per-frame tree costs, scheduler `call_group` frequency. Kill: tree+group < 5% of frame budget → park M-14.
2. Corpus audit: how many typed `SceneTree` annotations / `is SceneTree` checks exist in the reference corpus? (Determines whether the type-identity divergence is acceptable.)
3. T5 cadence API shape: exact `register_cadence` signature + SimServer integration point.
4. Test harness: which main loop do the 1397 GDScript tests run under — do they exercise FastSceneTree?
