# Goblin Engine — EntityNode / EntityComponent RFC

- **Proposal file:** `modules/goblin/docs/rfc/entity-node-rfc.md`
- **Date:** 2026-08-16
- **Status:** Proposed — design locked in discussion (user 2026-08-16); implementation **deferred until FastSceneTree ships** (backlog M-14). Candidate → active RFC.
- **Base:** Godot 4.7.1-stable, fork plan v0.2.0
- **Dependency:** `fast-scene-tree-rfc.md` — EntityNode/EntityComponent live on FastSceneTree's entity registry + pools.

---

## 1. Purpose

A **hybrid tree + ECS layer** on top of FastSceneTree: entities that *appear and behave like normal nodes* in the editor and in scripts, but whose data is **not stored in the objects** — it lives in FastSceneTree-owned per-type component pools (SoA structs). The goal is cache-friendly iteration and near-zero spawn/despawn overhead for the repetitive, high-volume 3D objects (meshes, transforms, collisions) while keeping full Godot compatibility for everything else.

**Not a full ECS.** No entity system, no archetypes, no systems pipeline. Each EntityNode is a container of components; all data is non-local; the objects are "a collection of functionality" assembled via the editor tree.

## 2. Why hybrid (locked decisions, 2026-08-16)

1. **Naming:** `EntityNode` / `EntityComponent` — plain names, upstream style, no Goblin prefix, no ClassDB collision. Orange icons for both (entity node may get a two-color gradient — cosmetic, decided at implementation).
2. **Why EntityNode : Node (full Node), not a slimmer base:** scripts, `get_tree()`, inspector, `.tscn` serialization, undo/redo all work for free. The 500B Node shell is acceptable — the *data* lives in pools, so iteration never touches the shell. Refactoring `Node::Data` itself is rejected (modification of the most-included header; ~300B/object saved vs rebase tax; pools make it moot).
3. **Why Component : Object sibling (NOT : Node):** the Node lifecycle is the cost — every `add_child` pays hash insert + cache-dirty (O(n log n) rebuild) + ENTER_TREE/READY propagation + `tree_changed`/`node_added`/`child_entered_tree` cascades. A handle-Node keeps that per-component tax alive. An Object-sibling gets ~100ns attach/detach: pool insert + mask flip + direct `_attach()` call. Zero tree notifications, zero cache rebuilds, zero signal cascades.

## 3. Architecture

### 3.1 Class hierarchy

```
Object
 ├── Node        → all legacy nodes + EntityNode
 └── Component   → Transform3DComponent, MeshInstanceComponent, CollisionShapeComponent, ...
```

- **`Component`** = slim base (~100–150B): `name` (editor display + serialization), `parent` (EntityNode pointer), `order`, `type` tag, `enabled` flag, virtual `_attach()` / `_detach()` — called **directly by the pool manager**, no tree machinery. Optional signals emitted directly (no propagation).
- **`EntityNode`** = full Node subclass carrying only: `entity_id`, `type_mask`, script. Everything else is in the pools.
- FastSceneTree accepts `Component` objects as EntityNode children — **for editor display + scene serialization only**; the runtime knows them only through the pools.

### 3.2 Data model (FastSceneTree-owned)

```
FastSceneTree
├── EntityRegistry
│   ├── entity_ids     : flat array, swap-remove
│   └── type_masks     : per-entity bitset of attached component types
├── ComponentPools (per type — SoA)
│   ├── Transform3DComponentPool   : struct[] { entity_id, transform, dirty_flags }
│   ├── MeshInstanceComponentPool  : struct[] { entity_id, mesh_ref, materials, instance_RID, ... }
│   └── CollisionShapeComponentPool: struct[] { entity_id, shape_ref, body_RID, ... }
└── per-type dirty lists (batched flush to servers at frame boundary)
```

- `entity_node.get_component<T>()` → pool lookup by entity_id (O(1)).
- Add/remove component = pool insert/swap-remove + id remap + type-mask bit flip + direct `_attach()`/`_detach()`. O(1), zero heap churn.
- **Frame flow:** entity scripts run in the normal node process pass → components mark dirty → FastSceneTree flushes **one batched pass per component type** to RenderingServer/PhysicsServer3D (transform updates, instance updates, shape changes) instead of N per-node notification dispatches. The batching + contiguous iteration is the engine win.

### 3.3 Components as batched server drivers

Each component is the stripped runtime of a legacy node: `Transform3DComponent` = transform + dirty flags pushed to RenderingServer; `MeshInstanceComponent` = mesh/materials/skin + instance RID; `CollisionShapeComponent` = shape + physics body RID. ~24–64B of data plus a handle object.

## 4. Editor integration

Required either way (category, icons, non-standalone rule — CreateDialog patch). Component : Object adds three bounded items:

| Item | Scope |
|------|-------|
| CreateDialog patch | `EntityComponent` category + orange icons; standalone-add rejected |
| Tree rendering | Component children rendered as rows under EntityNode (custom rows) |
| Scene save/load + undo | component list serialized under EntityNode; undo entries |

EntityNode itself stays native — tree position, inspector selection, undo of the entity all work untouched.

**Editor rules:** components only inside EntityNodes (editor enforces); script attachment rejected on components (`set_script` → ERR_FAIL + doc note); collapse/expand; inspector grows one section per component as you compose.

## 5. Scope

### Batch 1 (spatial, repetitive, 3D — the "easy to compose" set)

- `Transform3DComponent`
- `MeshInstanceComponent`
- `CollisionShapeComponent`

Each is a stripped struct of the legacy node's runtime fields. Everything a level-prop or enemy body needs.

### Deferred (with "what would change our mind")

| Candidate | Why deferred | What would change our mind |
|-----------|--------------|---------------------------|
| Camera | single-instance, stateful, hard to compose | composable viewport stack proves needed |
| Physics bodies (RigidBody/CharacterBody) | body RIDs want one owner; compose poorly | body-owned-by-entity model proves out |
| Visibility/culling, light, animation | not in batch 1 | profiling shows pool benefit for them |

## 6. Compatibility

- Regular nodes work everywhere, mixed freely with EntityNodes.
- EntityNode is a real Node: scripts, `get_tree()`, signals, `.tscn`, undo — standard.
- Component is NOT a Node: no `get_child()` traversal of components via Node API (separate component API), no process, no transform inheritance.
- FastSceneTree handles both: legacy nodes via the normal paths; EntityNodes via registry + pools.

## 7. Risks

| Risk | Mitigation |
|------|-----------|
| Editor tree/serialization/undo for Component children is custom work | Bounded (days); EntityNode native keeps the rest free |
| Pools + entity_id remap bugs on despawn | swap-remove remap tested at pool level; determinism replay |
| Server flush batching changes visual timing vs per-node notify | frame-boundary flush keeps semantics; visual regression suite |
| Scope creep into full ECS (archetypes/systems) | explicitly not a full ECS; no systems pipeline in scope |

## 8. Open questions (decide at RFC → plan time)

1. Component serialization format in `.tscn` (component list under EntityNode) — exact shape.
2. `get_component<T>()` API shape + typed component registration (compile-time vs runtime).
3. EntityNode inspector sections: custom inspector plugin vs property-list synthesis.
4. Does EntityNode participate in `_process` (it does — it's a Node) or only via `register_cadence`?

## 9. Sequencing & module

**Same module as FastSceneTree — `modules/fast_scene_tree/` (ADR 0008).** The entity system is a *subsystem* of the tree, not a consumer: the EntityRegistry + component pools are tree-owned data (§3.2), so they live inside the same module (own subdirs, e.g. `modules/fast_scene_tree/entity/`). One module, two phases:

- **Phase 1 (M-14):** FastSceneTree itself — tree validation gates.
- **Phase 2:** EntityNode/EntityComponent — starts only after the tree's P0 gate passes and the tree validates (P8). Builds on the tree's T6 stable-iteration hooks.

Splitting into two modules was rejected: it would force exposing registry/pool internals across a module boundary with no benefit.
