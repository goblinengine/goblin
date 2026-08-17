# Goblin Engine — EntityNode / EntityComponent RFC

- **Proposal file:** `modules/goblin/docs/rfc/entity-node-rfc.md`
- **Date:** 2026-08-16
- **Status:** Deferred — implementation blocked on in-place SceneTree optimizations (T2/M4 need node.h/node.cpp access)
- **Base:** Godot 4.7.1-stable
- **Backlog:** M-14

---

## 0. Status (2026-08-17 pivot)

The original FastSceneTree module design was **REJECTED 2026-08-17** (user
directive) — see `fast-scene-tree-rfc.md`. Direction shifted to **in-place
`SceneTree` optimizations** in the goblin mirror (`scene/main/scene_tree.cpp`)
+ a narrow `scene/main/scene_tree.h` edit. SceneTree is **not being replaced**
— it's being optimized in place.

This RFC's design (hybrid tree + ECS on tree-owned pools) is **still valid** —
the EntityRegistry + ComponentPools concept transfers cleanly: instead of
"FastSceneTree-owned pools," they are **`SceneTree`-owned pools** (the same
singleton, now with copy-free group iteration via T1/T6). The dependency on a
separate `FastSceneTree` class is removed. Implementation remains **deferred**
pending the in-place scene-tree optimizations shipping (P5 M-items T2/T4/M4–M7
touch node.cpp/node.h, which EntityNode needs for full benefit).

The design details below are unchanged in substance — only the "tree-owned"
seam shifts from `FastSceneTree` to `SceneTree`, and the module path is
removed (entity components live in the goblin mirror, same as scene_tree.cpp).

---

## 1. Purpose

A **hybrid tree + ECS layer** on top of `SceneTree`: entities that *appear and
behave like normal nodes* in the editor and in scripts, but whose data is
**not stored in the objects** — it lives in `SceneTree`-owned per-type component
pools (SoA structs). The goal is cache-friendly iteration and near-zero spawn/
despawn overhead for the repetitive, high-volume 3D objects (meshes, transforms,
collisions) while keeping full Godot compatibility for everything else.

**Not a full ECS.** No entity system, no archetypes, no systems pipeline. Each
EntityNode is a container of components; all data is non-local; the objects are
"a collection of functionality" assembled via the editor tree.

## 2. Why hybrid (locked decisions, 2026-08-16)

1. **Naming:** `EntityNode` / `EntityComponent` — plain names, upstream style.
2. **Why EntityNode : Node (full Node):** scripts, `get_tree()`, inspector,
   `.tscn` serialization, undo/redo all work for free. The Node shell is
   acceptable — data lives in pools, iteration never touches the shell.
3. **Why Component : Object sibling (NOT : Node):** the Node lifecycle is the
   cost — every `add_child` pays hash insert + cache-dirty rebuild +
   ENTER_TREE/READY propagation + signal cascades. An Object-sibling gets
   ~100ns attach/detach: pool insert + mask flip + direct `_attach()` call. Zero
   tree notifications, zero cache rebuilds, zero signal cascades.

## 3. Architecture

### 3.1 Class hierarchy

```
Object
 ├── Node        → all legacy nodes + EntityNode
 └── Component   → Transform3DComponent, MeshInstanceComponent, CollisionShapeComponent, ...
```

- **`Component`** = slim base (~100–150B): `name` (editor display + serialization),
  `parent` (EntityNode pointer), `order`, `type` tag, `enabled` flag, virtual
  `_attach()` / `_detach()` — called **directly by the pool manager**, no tree
  machinery.
- **`EntityNode`** = full Node subclass carrying only: `entity_id`, `type_mask`,
  script. Everything else is in the pools.
- `SceneTree` accepts `Component` objects as EntityNode children — **for editor
  display + scene serialization only**; the runtime knows them only through the
  pools.

### 3.2 Data model (SceneTree-owned)

```
SceneTree
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
- Add/remove component = pool insert/swap-remove + id remap + type-mask bit flip
  + direct `_attach()`/`_detach()`. O(1), zero heap churn.
- **Frame flow:** entity scripts run in the normal node process pass →
  components mark dirty → SceneTree flushes **one batched pass per component
  type** to RenderingServer/PhysicsServer3D (transform updates, instance updates,
  shape changes) instead of N per-node notification dispatches.

### 3.3 Components as batched server drivers

Each component is the stripped runtime of a legacy node: `Transform3DComponent`
= transform + dirty flags pushed to RenderingServer; `MeshInstanceComponent` =
mesh/materials/skin + instance RID; `CollisionShapeComponent` = shape + physics
body RID. ~24–64B of data plus a handle object.

## 4. Editor integration

| Item | Scope |
|------|-------|
| CreateDialog patch | `EntityComponent` category + icons; standalone-add rejected |
| Tree rendering | Component children rendered as rows under EntityNode (custom rows) |
| Scene save/load + undo | component list serialized under EntityNode; undo entries |

EntityNode itself stays native — tree position, inspector selection, undo of the
entity all work untouched.

**Editor rules:** components only inside EntityNodes; script attachment rejected
on components; collapse/expand; inspector grows one section per component.

## 5. Scope

### Batch 1 (spatial, repetitive, 3D — the "easy to compose" set)

- `Transform3DComponent`
- `MeshInstanceComponent`
- `CollisionShapeComponent`

### Deferred

| Candidate | Why deferred |
|-----------|-------------|
| Camera | single-instance, stateful, hard to compose |
| Physics bodies (RigidBody/CharacterBody) | body RIDs want one owner; compose poorly |
| Visibility/culling, light, animation | not in batch 1 |

## 6. Compatibility

- Regular nodes work everywhere, mixed freely with EntityNodes.
- EntityNode is a real Node: scripts, `get_tree()`, signals, `.tscn`, undo — standard.
- Component is NOT a Node: separate component API, no process, no transform inheritance.
- SceneTree handles both: legacy nodes via normal paths; EntityNodes via registry + pools.

## 7. Risks

| Risk | Mitigation |
|------|-----------|
| Editor tree/serialization/undo for Component children is custom work | Bounded (days); EntityNode native keeps the rest free |
| Pools + entity_id remap bugs on despawn | swap-remove remap tested at pool level; determinism replay |
| Server flush batching changes visual timing vs per-node notify | frame-boundary flush keeps semantics; visual regression suite |

## 8. Sequencing (2026-08-17 pivot)

EntityNode/EntityComponent are **Phase 2** — implementation begins only after
the in-place SceneTree optimizations (T1/T6) validate. The T2 (intrusive
groups) and M4 (children cache) items — which EntityNode benefits most from
(components bypass the node children cache entirely under M4) — remain deferred
(out of tree-only scope).

EntityNode components live in the goblin mirror (`modules/goblin/scene/main/`),
same as `scene_tree.cpp` — a core-file swap via `config.py` `add_library`
hook, not a standalone module. The `SceneTree` class gains an `EntityRegistry`
+ `ComponentPools` member set (small additive core header edit, same seam as
the scene-tree header edits).
