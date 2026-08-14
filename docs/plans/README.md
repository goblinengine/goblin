# Plan Index

Implementation breakdowns for Goblin Engine (the Godot fork). These are the execution specs the developer follows: locked semantics, mechanism/placement, phases with files and effort, test gates, risks.

## When To Use A Plan vs An RFC vs An ADR

Use a **plan** when:
- the design direction is settled enough to break into implementation phases
- the engineer needs files, effort estimates, and test gates to execute

Use an **RFC** (see [../rfc/](../rfc/)) when:
- the design is still exploratory and multiple credible options need evaluation

Use an **ADR** (see [../adr/](../adr/)) when:
- a decision is stable, expensive to reverse, and guides multiple implementations

Lifecycle: RFC (explore) → plan (execute) → ADR (lock). All three are kept permanently; outdated plans get a superseded marker inside the file, never deleted. Backlog rows track status and link the plan.

## Active Plans

| Plan | Area | Status |
|------|------|--------|
| [lightmapper-cpu-plan.md](lightmapper-cpu-plan.md) | Core rendering | 2026-08-14. Stage-by-stage port map of `lightmapper_rd` → CPU, data structures, threading, perf model, risks. Companion to the [lightmapper-cpu-rfc](../rfc/lightmapper-cpu-rfc.md) |
