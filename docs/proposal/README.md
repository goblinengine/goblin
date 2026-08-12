# Proposal (RFC) Index

Exploratory design documents for Goblin Engine (the Godot fork). These hold designs that are directionally approved but not yet frozen into ADRs.

## When To Use An RFC

Use an RFC when:
- the design is still exploratory
- the implementation shape is not proven enough to freeze
- multiple credible options still need evaluation

Promote an RFC to an ADR (in [../adr/](../adr/)) when direction and boundary are both stable enough to freeze.

## Active Proposals

None yet. Candidate RFCs, from the backlog:

| Candidate | Area | Notes |
|-----------|------|-------|
| gdscript-structs-rfc | Language | Struct memory layout, copy semantics, Variant round-trip |
| gdscript-typed-dictionaries-rfc | Language | `Dictionary[K, V]` syntax and runtime representation |
| gdscript-performance-rfc | Language | Opcode fusing, inline caching, PIC — what to port and order |
| generic-field-system-rfc | Core | Generic spatial field (light + audio + effects); sampling model and consumers |
| stealth-shadow-value-rfc | Core | Thief-style gameplay shadow readout (direct + occlusion + ambient) |
| portals-as-nodes-rfc | Module | `PortalSurface3D`/`MirrorSurface3D` custom nodes; `SubViewport` + teleport + portal-aware queries |
| retro-native-editor-rfc | Editor | Palette, dither, color cycling, posterization as editor-provided nodes/plugins |
| hitscan-surface-metadata-rfc | Core | Raycast → surface class + object ID + impact UV |
| brush-mover-contract-rfc | Core | Kinematic doors/lifts/crushers; deterministic hull traces |
| lightstyle-surface-class-rfc | Core | Style-channel modulation of baked light + retro surface classes |

Each RFC must state: context, the problem, options considered, a recommended direction, and open questions. RFCs are strong directional guidance unless explicitly marked superseded.
