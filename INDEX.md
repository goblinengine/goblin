# Goblin Engine — Index

Goblin Engine is a deliberately customized, lightweight fork of Godot Engine, tailored for immersive-sim / systems-heavy FPS RPG development. All changes live inside this `modules/goblin/` directory; upstream Godot source files are never modified.

## Documentation

The canonical docs live in [docs/](docs/). Start at [docs/README.md](docs/README.md).

| Document | Purpose |
|----------|---------|
| [docs/vision.md](docs/vision.md) | Vision pointer — canonical charter in `.kilo/rules/vision.md` |
| [docs/backlog.md](docs/backlog.md) | Single source of truth for pending, in-progress, and proposed work |
| [docs/GOBLIN_FORK_PLAN.md](docs/GOBLIN_FORK_PLAN.md) | Phased roadmap, pain-point → solution map, risk assessment |
| [docs/gdscript_features.md](docs/gdscript_features.md) | GDScript fork features and divergence surface |
| [docs/CODE_MAP.md](docs/CODE_MAP.md) | Navigation map: where files live, what each layer does, where new code goes |
| [docs/adr/](docs/adr/) | Architecture Decision Records (accepted + proposed) |
| [docs/proposal/](docs/proposal/) | RFCs for exploratory designs |

## Module Layout

```
modules/goblin/
├── config.py                    # configure() hook: builders, add_library swap, module trim
├── SCsub                        # Build script + GOBLIN_MODULE_OVERRIDES
├── goblin_builders.py           # Branding builders (version, splash, icons, authors/license)
├── register_types.cpp/h         # Module registration
│
├── core/                        # Mirrors upstream core/ for file overrides
│   ├── variant/                 #   variant_construct.cpp/h (String ctors)
│   └── version_override.py + branding files (AUTHORS/DONORS/COPYRIGHT/LICENSE)
│
├── modules/                     # Mirrors upstream modules/ for whole-module overrides
│   └── gdscript/                #   The GDScript fork (compiled instead of upstream)
│
├── editor/                      # Editor branding (translation overrides, export tweaks)
├── main/                        # Splash / app icon overrides
├── platform/windows/            # Platform overrides
├── tools/                       # Utility scripts
└── docs/                        # All documentation (see above)
```

## Override Mechanisms

Three mechanisms inject changes at build time; upstream files are never modified. See [docs/adr/0001-source-override-architecture.md](docs/adr/0001-source-override-architecture.md) and [.kilo/rules/rules.md](../.kilo/rules/rules.md).

1. **Module Directory Override** — `GOBLIN_MODULE_OVERRIDES` in `SCsub` swaps `env.module_list` entries for whole-module forks.
2. **Core File Override** — `goblin_add_library()` in `config.py` swaps a single source at library creation.
3. **Builder Monkey-Patching** — `configure()` replaces build-time generator functions and renames binaries.

## Build

```
scons platform=windows target=editor module_mono_enabled=no accesskit=no angle=no -j4
```

Optional faster incremental: add `--max-drift=1 --implicit-deps-unchanged`. Output: `bin/goblin.windows.editor.x86_64.exe`.

Never delete or clean `bin/`. Never modify files outside `modules/goblin/` without explicit permission.
