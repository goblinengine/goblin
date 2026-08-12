# Vision

Goblin Engine is a deliberately customized, lightweight fork of Godot Engine, tailored for the Dungeon Battler (DB) immersive-sim / systems-heavy FPS RPG and its successors.

It is not a general-purpose engine, not a rebranding project (branding already exists), and not a from-scratch engine. It preserves Godot's editor, renderer, asset pipeline, and 100% compatibility with the DB project, while removing unused engine components and extending the engine only where it limits the game.

## Why A Fork

The from-scratch Goblin engine (raylib + daScript, at `D:\DEV\Goblin`) is architecturally strong but 2-3 years from hosting a real game. A customized Godot fork preserves all existing DB work — scripts, scenes, CaveIni content, shaders, editor tools — while still granting the engine-internals control the game needs.

The fork follows the same trajectory Arkane Studios took moving from Unreal to their own Void Engine: more control over internals, built-in support for the game's specific needs (systemic design, modding, light fields, retro effects, MIDI, lightmap baking), without maintaining a separate engine.

## Goals

1. **Full DB compatibility.** The trimmed fork still loads, builds, and runs the DB project unmodified.
2. **Leaner engine.** Strip modules, importers, and formats the game does not use, with evidence to justify each trim.
3. **A stronger GDScript.** Close the gaps that force workarounds: value types, typed containers, null-safety operators, faster property access, and priority data structures.
4. **Surgical core changes.** Fix engine bugs and expose APIs the game needs (lightmap baking, MIDI, integer keys) with minimal file changes.
5. **Mod-friendly, data-driven by default.** Preserve and extend the data-driven architecture DB already relies on.
6. **Clean upstream relationship.** Track stable releases, keep the override surface minimal, rebase only at release boundaries.

## Non-Goals

- Not a general-purpose engine for broad consumption.
- Not a GDScript replacement or a "GDScript vs daScript" project.
- Not a from-scratch rewrite of the renderer, physics, or editor.
- Not a platform for speculative language features — every GDScript addition must solve a concrete DB pain point.
- Not a deep fork of the renderer before DB 1.0. Stay on GL Compatibility through release.

## Core Principles

These are locked decisions, each with an ADR. See [adr/](adr/) for the full records.

1. **Source Override, Not Source Modification.** All changes live inside `modules/goblin/`. The build system redirects compilation to goblin-owned files.
2. **Surgical, Not Sweeping.** Every core change must justify itself against a concrete DB pain point.
3. **Trim With Evidence.** Strip only what is verified unused against DB's project config, script corpus, and content.
4. **Stable-Release Tracking.** Track official stable release tags (`4.7-stable`), not `master`. Rebase only at release boundaries.
5. **ADR-Governed.** Structural decisions go through ADRs modeled on the from-scratch Goblin engine. No feature lands without a locked ADR.
6. **Cherry-Pick GDScript Features.** Port individual language features into the fork one at a time, each tested and reviewed — do not merge external GDScript forks wholesale.
7. **Minimal Override Surface.** Prefer additive overrides over whole-file replacement. Every replacement is a merge-conflict liability.

## Direction By Area

### GDScript
Add value types (structs), typed dictionaries, `?.`/`??`, `swap()`, and a built-in priority queue. Port performance work (inline caching, opcode fusing) from the gdscript2 module's branches one feature at a time. Long-term: generics, blocks, generators, zero-marshaling C++ interop.

### Core Engine
Fix the LightmapGI frustum culling bug (#71585), expose the runtime lightmap baker as public API, add MIDI support to `AudioStreamPlayer3D`, and add `Vector3i` keys to AStar3D. Each is 1-2 files, surgical, and justified by DB pain points.

### Modules & Build
Trim unused modules per the evidence standard. Keep all networking (multiplayer is planned), all platforms (cross-platform desired), and `noise` (procedural content potential).

### Renderer
Stay on GL Compatibility through DB 1.0. Re-evaluate Forward+ (Vulkan) afterward; the pixel-art aesthetic ports cleanly.

### Migration From The From-Scratch Engine
Port architectural ideas that fit: the generic spatial field system (light + audio + effects) with the stealth shadow value on top, portals/mirrors as custom nodes, retro-native rendering as editor-provided tools, hitscan surface metadata, kinematic brush movers, and basis-frame transform conventions. The cadence scheduler and scene-composition ideas are already adopted by DB — nothing further to port there.
