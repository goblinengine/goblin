# RFC Index

Exploratory design documents for Goblin Engine (the Godot fork). These hold designs that are directionally approved but not yet frozen into ADRs.

## When To Use An RFC

Use an RFC when:
- the design is still exploratory
- the implementation shape is not proven enough to freeze
- multiple credible options still need evaluation

Promote an RFC to an ADR (in [../adr/](../adr/)) when direction and boundary are both stable enough to freeze.

## Active RFCs

| RFC | Area | Status |
|-----|------|--------|
| [lightmapper-cpu-rfc.md](lightmapper-cpu-rfc.md) | Core rendering | Proposed 2026-08-14; awaiting architect review. Runtime CPU lightmapper via `Lightmapper::create_cpu`; supersedes ADR 0006 direction |
| [cut-upscalers-rfc.md](cut-upscalers-rfc.md) | Core rendering | Proposed 2026-08-14; clean-room CUT1/2/3 for GLES3 3D scaling (license path + enum strategy + mechanism options; locked in plan) |
| [native-game-features-rfc.md](native-game-features-rfc.md) | GDScript language | Proposed 2026-08-13 — consolidated: expanded dictionary (typed entries, template dictionaries, callable members) + recovered `then`/`elthen`; secondary candidates and rejected ideas. Not yet approved as an implementation plan |
| [simserver-rfc.md](simserver-rfc.md) | Systemic / immersive sim | Proposed 2026-08-16; direction agreed (user 2026-08-16). Additive module `modules/sim/`: clock/cadence + stimulus bus (S-01), surface registry + query (S-02), ambient field + stealth readout (S-03), interaction substrate (S-04), combat hooks (S-05). Folds C-05/C-06/M-07/M-08/M-09. Cadence pipeline (pre_tick → sim_tick → post_tick) for determinism; RID-space, orthogonal to SceneTree |

Implementation breakdowns for approved directions live in [../plans/](../plans/) (e.g. `lightmapper-cpu-plan.md` — stage-by-stage port map of `lightmapper_rd` → CPU, data structures, threading, perf model, risks; companion to the RFC).

## Candidate RFCs

| Candidate | Area | Notes |
|-----------|------|-------|
| gdscript-structs-rfc | Language | Struct memory layout, copy semantics, Variant round-trip |
| gdscript-typed-dictionaries-rfc | Language | `Dictionary[K, V]` syntax and runtime representation |
| gdscript-performance-rfc | Language | Opcode fusing, inline caching, PIC — what to port and order |
| portals-as-nodes-rfc | Module | `PortalSurface3D`/`MirrorSurface3D` custom nodes; `SubViewport` + teleport + portal-aware queries |
| retro-native-editor-rfc | Editor | Palette, dither, color cycling, posterization as editor-provided nodes/plugins |
| brush-mover-contract-rfc | Core | Kinematic doors/lifts/crushers; deterministic hull traces |
| lightstyle-surface-class-rfc | Core | Style-channel modulation of baked light + retro surface classes |
| scene-tree-hybrid-rfc | Core | SceneTree replacement / tree+ECS hybrid exploration (ComposableNode/ComposableComponent). Research only; additive opt-in if ever built; separate from SimServer (see simserver-rfc §10) |

Each RFC must state: context, the problem, options considered, a recommended direction, and open questions. RFCs are strong directional guidance unless explicitly marked superseded.
