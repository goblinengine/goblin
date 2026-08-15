# ADR 0003: Module Trim Evidence Standard

Status: Accepted

Date: 2026-08-12

## Context

A lightweight fork should strip modules the game does not use to reduce compile time, binary size, and maintenance surface. But trimming the wrong module breaks the editor, import pipeline, or a feature the reference title relies on. Trims must be justified by evidence, not assumption.

## Decision

A module may be disabled only when all of the following are verified:

1. The reference title does not directly reference the classes or APIs it provides (checked against `project.godot`, the script corpus, and content files).
2. It is not a dependency of another module the reference title uses (checked via `config.py` dependency chains).
3. Removing it does not break editor functionality needed for reference-title development (editor-only modules are trimmed from export templates, not the editor, unless verified safe).

The trim list is fixed in `modules/goblin/config.py` under `DISABLE_MODULES`. Networking (`enet`, `websocket`, `webrtc`, `upnp`, `multiplayer`, `jsonrpc`) is explicitly kept because multiplayer is planned. `noise` is kept for potential procedural content. All target platforms are kept because cross-platform support is desired.

The current fixed trim list disables 30 modules across image formats, audio/video, VR/XR, 3D nodes/importers, navigation, physics, shader compilation, and utility.

## Consequences

Positive:
- Every trim is defensible and reversible by editing one set.
- Trims are auditable against reference-title evidence.

Negative:
- The trim list is a manual decision surface; a future reference-title feature may require re-enabling a trimmed module.
- Some modules are disabled unconditionally (e.g. texture compression modules in the editor build) — this must be verified against the editor import pipeline (see backlog D-07).

## Related Documents

- [ROADMAP.md](../ROADMAP.md) — §5 Module Trim Plan
- [backlog.md](../backlog.md) — D-07
