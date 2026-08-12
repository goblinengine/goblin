# Goblin Engine — Documentation Index

Goblin Engine is a deliberately customized, lightweight fork of Godot Engine. All changes live inside `modules/goblin/`; upstream Godot source files are never modified.

## Start Here

1. **[vision.md](vision.md)** — what this fork is, goals, and non-goals.
2. **[backlog.md](backlog.md)** — the single source of truth for pending, in-progress, and proposed work.
3. **[GOBLIN_FORK_PLAN.md](GOBLIN_FORK_PLAN.md)** — phased roadmap, pain-point → solution map, risk assessment.

## Architecture Decisions

- **[adr/](adr/)** — accepted and proposed Architecture Decision Records. Locked decisions that guide all implementation.

## Design Proposals

- **[proposal/](proposal/)** — RFCs for exploratory designs before they are promoted to ADRs.

## Reference

- **[gdscript_features.md](gdscript_features.md)** — the GDScript fork's language features and divergence from upstream.
- **[STRUCTURE.md](STRUCTURE.md)** — module layout and the override mechanisms.
- **[BRANDING_STATUS.md](BRANDING_STATUS.md)** — what gets rebranded and what does not.
- **[LIGHTMAP_INVESTIGATION.md](LIGHTMAP_INVESTIGATION.md)** — reference for the lightmap core changes (backlog C-01/C-02).

## Conventions

- ADRs lock stable, expensive-to-reverse decisions. See [adr/README.md](adr/README.md).
- RFCs hold exploratory designs before ADR promotion. See [proposal/README.md](proposal/README.md).
- The backlog is updated whenever work is planned, started, or completed. No task lives only in a prompt or chat.
