# Code Map - Goblin Engine Fork

Navigation map for `modules/goblin/`. Read before implementing; update after changes.

Companion docs: `docs/gdscript_features.md` (feature semantics), `docs/ROADMAP.md` (roadmap), `docs/backlog.md` (work tracking), `.kilo/rules/` (constraints + vision).

## Layout

```
modules/goblin/
├── config.py            # Build hooks: configure(), goblin_add_library() + _GOBLIN_FILE_OVERRIDES, module trim
├── SCsub                # GOBLIN_MODULE_OVERRIDES (module swap, line 57)
├── goblin_builders.py   # Branding builders (version/splash/icons/authors/license)
├── register_types.cpp   # Module registration: calls register_branding_translations()
├── core/                # Core mirror - ONLY overridden files
│   ├── variant/variant_construct.{cpp,h}   # String ctors (core file override)
│   └── version_override.py                 # Branding metadata (name, website)
├── modules/gdscript/    # GDScript fork - compiled INSTEAD of upstream modules/gdscript/
├── editor/              # branding_translations.{cpp,h} (runtime translation fallback), icons/, README
│   └── overrides/       # Editor file mirrors - swapped by goblin_add_library(), NEVER globbed by SCsub
│       ├── gui/editor_about.cpp                  # About dialog (Goblin literals, no Donors tab)
│       ├── export/project_export.{cpp,h}         # Export dialog (debug-template-aware option, warning filter)
│       ├── project_manager/project_manager.cpp   # Project Manager (Donate button removed)
│       └── editor_node.cpp                       # EditorNode (Support Godot Development removed)
├── main/                # splash.png, splash_editor.png, app_icon.png
├── platform/windows/    # goblin.rc
├── docs/                # All fork docs (see docs/README.md)
└── goblin_manager.py    # DEV UTILITY - never run its `clean` command (hard rule 1)

modules/midi/            # STANDALONE additive feature module (ADR 0008) - see section below
```

## Build hooks (config.py)

| Symbol | What |
|---|---|
| `configure()` (:5) | Replaces upstream builders with goblin_builders, wraps add_program/add_library/add_shared_library (godot -> goblin rename), prints the module-trim canary (`28/28 modules gated off`, ADR 0012) |
| module-level trim code (import time) | Mutates `SCons.Script.ARGUMENTS` (first module loop, SConstruct:474, before `opts.Update` at 499): `module_<name>_enabled=no` per `DISABLE_MODULES` entry the user did not set on the CLI — the gate (SConstruct:1113) then skips them regardless of alphabetical position (ADR 0012; was a no-op via `env.disabled_modules` until 2026-08-16) |
| `goblin_add_library()` | Library-scoped file override: `_GOBLIN_FILE_OVERRIDES = {lib: {stem: goblin_path}}` for `core` + `editor`; swaps matching basename in the library source list BEFORE capture (ADR 0007 / B-09) |
| `get_icons_path()` | Editor icon overrides (`Logo.svg`, `Godot.svg`, `TitleBarLogo.svg`, ...) registered at configure time — MUST stay here, not in editor/SCsub: SConstruct collects `module_icons_paths` before `editor/icons/SCsub` runs, so a late append never applies |
| `can_build()` (:1) | Module enable check |

## GDScript fork (modules/gdscript/)

Whole-module override (ADR 0001). Mirror of upstream + fork features.
Diff vs upstream: `git diff --no-index --stat modules/gdscript modules/goblin/modules/gdscript` (60 files, +1623/-48).

### Pipeline stages and fork deltas

| Stage | File | Fork work |
|---|---|---|
| Tokenizer | gdscript_tokenizer.{h,cpp} | `then`/`elthen` tokens + keywords (full feature: parser/analyzer/compiler wired — see Features table) |
| Tokenizer buffer | gdscript_tokenizer_buffer.{h,cpp} | Save/restore support (parser lookahead) |
| Parser | gdscript_parser.{h,cpp} | `DataType::UNION` kind, `@private` annotation, shaped dict literals (`key: Type = value`), datatype shape |
| Analyzer | gdscript_analyzer.cpp | Union resolve/compat, private-access blocking, shape inference + entry-type refinement |
| Compiler | gdscript_compiler.cpp | `OPCODE_CONSTRUCT_SHAPED_DICTIONARY` emit, UNION -> runtime VARIANT |
| Bytecode gen | gdscript_byte_codegen.{h,cpp} | `append_goblin_datatype()` - recursive datatype as raw instruction words |
| VM | gdscript_vm.cpp | Shaped-dict opcode dispatch + runtime validation, datatype decode |
| Function | gdscript_function.{h,cpp} | Datatype shape payload + validate/decode helpers |
| Editor | gdscript_editor.cpp | Autocomplete recursion (shapes), private filter (`p_recursion_depth > 0`) |
| Disassembler | gdscript_disassembler.cpp | Datatype/shape printing |

### Features (verified in code)

| Feature | Where | Notes |
|---|---|---|
| Union types | parser + analyzer + compiler | `int | float`, `null` singleton; compiler maps UNION -> VARIANT |
| `@private` | parser + analyzer | Same-script access allowed; no `@export` combo; name still occupies slot in subclasses |
| String ctors | `core/variant/variant_construct.{cpp,h}` | `String(int/float/bool)`, `1 as String` -> `"1"` |
| Shaped dictionaries | parser/analyzer/compiler/vm/editor | Typed entries, recursive shape, `OPCODE_CONSTRUCT_SHAPED_DICTIONARY`, access refinement |
| `then`/`elthen` | tokenizer + parser + analyzer + compiler | `then` null-only (`a != null ? b : a`), `elthen` truthy (`a ? a : b`) — locked 2026-08-13; no VM changes; tests pending (TD-02) |

## MIDI / SoundFont module (`modules/midi/`)

Standalone additive feature module at the repo root (ADR 0008) — standard Godot module anatomy, auto-discovered by the build with the full module lifecycle: `SCsub`, `config.py` (`can_build`/`get_doc_classes`/`get_icons_path`), `register_types.{h,cpp}` (registered via the generated `register_module_types.gen.cpp`, `MODULE_MIDI_ENABLED`), `doc_classes/`, `tests/`, `editor/icons/`, in-module `thirdparty/`. Not part of the override machinery; no goblin hooks, no upstream file touched. Kills the external MidiStream GDExtension dependency (backlog C-07). Class/property/importer names are identical to the GDExtension so existing projects and `.import` files keep working.

- `MidiStream` is an `AudioStream` -> works in any stream player (`AudioStreamPlayer`, `AudioStreamPlayer3D`, ...) — this delivers backlog C-03 (3D spatialized MIDI) with no extra node.
- The synthesizer is TinySoundFont v0.9 (MIT) + TinyMidiLoader v0.7 (zlib), single-header libs vendored verbatim under `thirdparty/tinysoundfont/`; `TSF_IMPLEMENTATION`/`TML_IMPLEMENTATION` compile into `midi_stream_playback.cpp` only.
- Playback is engine-style: overrides public `start/stop/is_playing/get_loop_count/get_playback_position/seek` + `_mix_internal` + `get_stream_sampling_rate` (AudioStreamPlaybackWAV pattern), NOT the GDVIRTUAL `_start`/`_mix_resampled` hooks (those are for script subclasses).
- Lazy loading (`_ensure_loaded`) parses SF2 + MIDI on first mix (audio thread) — same design as the GDExtension, proven on the reference title. Live notes via `note_on(preset, key, vel)`.
- Importers are `ResourceImporter` subclasses registered in `ResourceFormatImporter` at EDITOR level (WAV-importer pattern). Importer names `midi_stream.mid` / `midi_stream.sf2` match the GDExtension.
- Test note: `--test` mode has no AudioServer; the doctest suite bootstraps `AudioDriverManager::get_driver(0)` (the dummy) via `set_singleton()` + `init()`, and recreates `AudioServer` whenever missing — `GodotTestCaseListener::test_case_end` deletes the singleton after every test case.

## Combat module (`modules/combat/`)

Standalone additive feature module at the repo root (ADR 0008) — standard Godot module anatomy, auto-discovered with the full module lifecycle (`MODULE_COMBAT_ENABLED`). Same structure as `modules/midi/`. No overrides, no upstream file touched.

- `Hitbox3D` — active damage detector (extends Area3D): monitoring on, monitorable off. Carries attack data (damage, knockback, damage_types, element, source); on Hurtbox3D overlap dedups per activation, forwards via `Hurtbox3D.apply_hit`, emits `hit(hurtbox, hit_data)`.
- `Hurtbox3D` — passive damage receiver (extends Area3D): monitoring off, monitorable on. `apply_hit(attacker, hit_data)` emits `hurt(attacker, hit_data)` when active (virtual, C++ subclasses can intercept damage).
- `Projectile3D` — manual-velocity projectile (extends Area3D, NOT RigidBody3D): owns velocity/gravity/homing, continuous swept collision via an internal `ShapeCast3D` child (no tunneling), bounce/expiry/range behavior, emits `hit(hit_data)` + `expired()`. Forwards hits to Hurtbox3D on the collider.
- Hit-data contract: `CombatUtils` (combat_utils.h) defines the stable Dictionary keys (`damage`, `knockback`, `damage_types`, `element`, `source`, `position`, `normal`, `velocity`, `collider`) shared by all three classes.
- Physics tick: native nodes receive it via `_notification(NOTIFICATION_PHYSICS_PROCESS)` (not a C++ virtual `_physics_process` — GDVIRTUAL only in 4.7).

## Where new code goes

| Change | Location |
|---|---|
| Language feature (parser+analyzer+compiler) | `modules/goblin/modules/gdscript/` |
| Single core .cpp | `modules/goblin/core/<mirror path>/` + dict entry in `goblin_add_library()` |
| Fast scene tree (M-14): SceneTree modified IN PLACE | `modules/goblin/scene/main/scene_tree.cpp` (swap in config.py `"scene"` dict) — content is a faithful upstream copy; optimizations land here directly. Companion core edit: `scene/main/scene_tree.h` (+7 lines: 2 ProcessGroup compaction flags + 3 cached StringName members — the only upstream file touched). No module, no base-class seam: `get_tree()`/`SceneTree::get_singleton()` stay upstream, editor/PM/games all run the one tree. Batches landed: T1 (lazy compaction / copy-free `_process_group`) + T6 (copy-free group calls via `ptr()` / ref-efficient timers / cached signal names).  |
| Single editor .cpp | `modules/goblin/editor/overrides/<mirror path>/` + dict entry in `goblin_add_library()` (NEVER a globbed dir — `editor/SCsub` globs `*.cpp` non-recursively; unmodified headers stay upstream, rewrite the bare own-header include to root-relative) |
| New additive feature module (zero overrides) | standalone `modules/<name>/` with standard module anatomy (ADR 0008) — auto-discovered, full lifecycle; never inside `modules/goblin/` |
| New native class (override-adjacent) | .cpp/.h in `modules/goblin/` + `GDREGISTER_CLASS` in register_types.cpp |
| Build-time generator | `goblin_builders.py` + assignment in `configure()` |
| Branding assets | `main/`, `editor/icons/`, `platform/windows/` |

## Tests

- Fork tests: `modules/goblin/modules/gdscript/tests/` (mirror of upstream suite + new cases under `parser/`, `analyzer/`, `runtime/`). The test harness (`gdscript_test_runner_suite.h`, `test_completion.h`, `test_lsp.h`) targets the fork's own tests dir.
- MIDI tests: `modules/midi/tests/test_midi_stream.h` (doctest `TEST_CASE`s, picked up via `modules_tests.gen.h` when `tests=yes`). Generates a minimal SF2 + SMF in memory; covers length, synth render, song-end stop, loop restart, manual notes. Run: `bin/goblin.windows.editor.x86_64.exe --test --test-case="*MidiStream*"`.
- Combat tests: `modules/combat/tests/test_combat.h` — 11 doctest cases (defaults, hit registration, dedup/reset, inactive states, motion/gravity, bounce math, lifetime expiry, hit-data contract). Name prefix `[SceneTree]` is required: physics nodes crash without the per-case physics-server bootstrap that `[SceneTree]`-prefixed cases get in `tests/test_main.cpp`. Run: `bin/goblin.windows.editor.x86_64.exe --headless --test --test-case="[SceneTree][Combat]*"`.
- Run: build with `tests=yes` (`scons platform=windows target=editor module_mono_enabled=no accesskit=no angle=no tests=yes -j4`), then `bin/goblin.windows.editor.x86_64.exe --headless --test --test-case "[Modules][GDScript]*"`.
- Regenerate expected outputs from current behavior: `bin/goblin.windows.editor.x86_64.exe --headless --gdscript-generate-tests` (writes `.out` files — use with care; it encodes whatever the engine currently does).
- Gotchas:
  - `.out` files must end with a trailing newline: `GDScriptTest::check_output()` compares against `strip_edges(output) + "\n"`.
  - `init_language()` forces `ProjectSettings`' resource path to the test scripts dir so `res://`-relative reads (LSP/completion suites) work even after other suites (e.g. GLTF) leave the singleton's resource path set.
- Gate: reference corpus compile + 342 unit tests + level load. Never claim a change verified without a build.

## Landmines / drift (verified today)

1. `goblin_manager.py` `build` subcommand targets `linuxbsd` - wrong for this project (Windows). Never reintroduce a `clean` command (`scons --clean` violates hard rule 1).
2. `gdscript.h` layout must stay identical to upstream: `main/main.cpp` + `editor/doc/editor_help.cpp` include it outside the module (ABI).
3. `then`/`elthen` semantics: `then` null-only, `elthen` truthy — locked 2026-08-13, do not "fix" to null-only.
4. `editor/overrides/` must stay out of any SCsub glob (non-recursive `*.cpp` glob in `modules/goblin/editor/SCsub` would double-compile swapped sources). Only `goblin_add_library()` references it.
5. Editor mirrors drift from upstream: re-diff on rebase (`git diff --no-index --stat editor/<file> modules/goblin/editor/overrides/<file>`); `project_export.h` mirror is the only copied header (it carries the fork-only `_update_export_debug_option()`).
6. `modules/goblin/core/SCsub` adds `Depends` edges for `authors/donors/license/version_generated.gen.h` AND `main/splash.gen.h`/`splash_editor.gen.h`/`app_icon.gen.h`: the upstream SCsubs declare only upstream files as sources, so without the edges the goblin builders' inputs would never trigger regeneration.
7. Editor splash: upstream 4.7 removed it (commit c283fce698). config.py strips `NO_EDITOR_SPLASH` from CPPDEFINES; `modules/goblin/SCsub` generates `#main/splash_editor.gen.h` itself (main/SCsub skips its command because `no_editor_splash` stays True). Do not flip `no_editor_splash` to False — main/SCsub's command would then target a nonexistent `#main/splash_editor.png` and fail.
8. Windows RES builder is shadowed in config.py (`env.AddMethod` on "RES"): compiles `goblin.rc`/`goblin_res_wrap.rc` instead of upstream `godot_res.rc`. `goblin.ico` is generated at build time from `main/app_icon.png` (PNG-compressed ICO). The `.rc` files must reference the ico with a repo-root-relative path (`modules/goblin/platform/windows/goblin.ico`).

## Fast lookup

| Task | Start at |
|---|---|
| Fix analyzer error | `gdscript_analyzer.cpp` (reduce_*/resolve_*) |
| Add keyword | tokenizer.h enum + tokenizer.cpp names/keywords, then parser precedence table |
| Add VM opcode | `gdscript_byte_codegen.h` + `gdscript_vm.cpp` (enum + dispatch) |
| Autocomplete behavior | `gdscript_editor.cpp` `_find_identifiers_in_class` |
| Branding string | `core/version_override.py` / `goblin_builders.py` / `editor/branding_translations.cpp` (runtime fallback) / `editor/overrides/` (compile-time) |
| MIDI playback / synth | `modules/midi/midi_stream_playback.cpp` (TSF/TML impl lives there — single TU) |
| MIDI import | `modules/midi/midi_importers.cpp` + registration in `modules/midi/register_types.cpp` |
| Port upstream commit | `porting` skill + diff mirrors vs upstream |

## Update discipline

Code changed -> update this map + `docs/gdscript_features.md`. Stale map is a landmine, not a doc.
