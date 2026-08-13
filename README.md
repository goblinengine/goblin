# Goblin Engine Module

Goblin Engine is a fork of Godot 4.7.x for immersive-sim / systems-heavy FPS RPG development. All fork changes live inside `modules/goblin/`; upstream Godot source files are never modified. Changes are injected at build time via three override mechanisms (whole-module override, single-file core swap, builder patching).

This is a production fork, not a generic rebranding template.

## Build

```
scons platform=windows target=editor module_mono_enabled=no accesskit=no angle=no -j4
```

Optional faster incremental: `--max-drift=1 --implicit-deps-unchanged`.
Output: `bin/goblin.windows.editor.x86_64.exe` (+ `.console.exe`).

**Never run `scons -c` or delete anything in `bin/`** — the build cache must never be cleared.

## Layout

```
modules/goblin/
├── config.py            # Build hooks: configure(), goblin_add_library(), module trim
├── SCsub                # GOBLIN_MODULE_OVERRIDES (whole-module swap)
├── goblin_builders.py   # Branding builders
├── register_types.cpp   # Module registration (GoblinBranding, GoblinExportTweaks)
├── core/                # Core mirror: variant_construct.{cpp,h}, version_override.py, branding files
├── modules/gdscript/    # GDScript fork (compiled instead of upstream modules/gdscript/)
├── editor/              # goblin_about.cpp, goblin_export.cpp (runtime UI patches)
├── main/                # Splash / app icon overrides
├── platform/windows/    # goblin.rc
├── tools/               # sync_godot_icons.py
└── docs/                # All fork documentation
```

## Documentation

- `docs/README.md` — documentation index (start here).
- `docs/CODE_MAP.md` — navigation map: where files live, where new code goes.
- `docs/backlog.md` — single source of truth for all work (planned / doing / done / rejected).
- `docs/GOBLIN_FORK_PLAN.md` — roadmap, pain-point map, risk assessment.
- `docs/gdscript_features.md` — GDScript fork features and divergence from upstream.
- `docs/adr/` — architecture decision records.

## Rules

- Never clean `bin/`, never run `scons -c`.
- Never modify files outside `modules/goblin/`.
- Code uses upstream Godot naming conventions — no `Goblin*` / `goblin_*` prefixes on classes, methods, or files.
- Full rules + vision: `.kilo/rules/` in the repo root.
