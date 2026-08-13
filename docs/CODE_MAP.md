# Code Map - Goblin Engine Fork

Navigation map for `modules/goblin/`. Read before implementing; update after changes.

Companion docs: `docs/gdscript_features.md` (feature semantics), `docs/GOBLIN_FORK_PLAN.md` (roadmap), `docs/backlog.md` (work tracking), `.kilo/rules/` (constraints + vision).

## Layout

```
modules/goblin/
├── config.py            # Build hooks: configure(), goblin_add_library(), module trim
├── SCsub                # GOBLIN_MODULE_OVERRIDES (module swap, line 57)
├── goblin_builders.py   # Branding builders (version/splash/icons/authors/license)
├── register_types.cpp   # Module registration: GoblinBranding + GoblinExportTweaks
├── core/                # Core mirror - ONLY overridden files
│   ├── variant/variant_construct.{cpp,h}   # String ctors (core file override)
│   └── version_override.py                 # Branding metadata (name, website)
├── modules/gdscript/    # GDScript fork - compiled INSTEAD of upstream modules/gdscript/
├── editor/              # goblin_about.cpp, goblin_export.cpp (runtime UI patches)
├── main/                # splash.png, splash_editor.png, app_icon.png
├── platform/windows/    # goblin.rc
├── docs/                # All fork docs (see docs/README.md)
└── goblin_manager.py    # DEV UTILITY - never run its `clean` command (hard rule 1)
```

## Build hooks (config.py)

| Symbol | What |
|---|---|
| `configure()` (:5) | Replaces upstream builders with goblin_builders, wraps add_program/add_library/add_shared_library (godot -> goblin rename), applies DISABLE_MODULES trim |
| `goblin_add_library()` | Core file override: swaps matching basename in core.lib source list BEFORE capture |
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

## Where new code goes

| Change | Location |
|---|---|
| Language feature (parser+analyzer+compiler) | `modules/goblin/modules/gdscript/` |
| Single core .cpp | `modules/goblin/core/<mirror path>/` + hook in `goblin_add_library()` |
| New native class | .cpp/.h in `modules/goblin/` + `GDREGISTER_CLASS` in register_types.cpp |
| Build-time generator | `goblin_builders.py` + assignment in `configure()` |
| Branding assets | `main/`, `editor/icons/`, `platform/windows/` |

## Tests

- Fork tests: `modules/goblin/modules/gdscript/tests/` (mirror of upstream suite + new cases under `parser/`, `analyzer/`, `runtime/`). The test harness (`gdscript_test_runner_suite.h`, `test_completion.h`, `test_lsp.h`) targets the fork's own tests dir.
- Run: build with `tests=yes` (`scons platform=windows target=editor module_mono_enabled=no accesskit=no angle=no tests=yes -j4`), then `bin/goblin.windows.editor.x86_64.exe --headless --test --test-case "[Modules][GDScript]*"`.
- Regenerate expected outputs from current behavior: `bin/goblin.windows.editor.x86_64.exe --headless --gdscript-generate-tests` (writes `.out` files — use with care; it encodes whatever the engine currently does).
- Gotchas:
  - `.out` files must end with a trailing newline: `GDScriptTest::check_output()` compares against `strip_edges(output) + "\n"`.
  - `init_language()` forces `ProjectSettings`' resource path to the test scripts dir so `res://`-relative reads (LSP/completion suites) work even after other suites (e.g. GLTF) leave the singleton's resource path set.
- Gate: DB corpus compile + 342 unit tests + level load. Never claim a change verified without a build.

## Landmines / drift (verified today)

1. `goblin_manager.py` `build` subcommand targets `linuxbsd` - wrong for this project (Windows). Never reintroduce a `clean` command (`scons --clean` violates hard rule 1).
2. `gdscript.h` layout must stay identical to upstream: `main/main.cpp` + `editor/doc/editor_help.cpp` include it outside the module (ABI).
3. `then`/`elthen` semantics: `then` null-only, `elthen` truthy — locked 2026-08-13, do not "fix" to null-only.

## Fast lookup

| Task | Start at |
|---|---|
| Fix analyzer error | `gdscript_analyzer.cpp` (reduce_*/resolve_*) |
| Add keyword | tokenizer.h enum + tokenizer.cpp names/keywords, then parser precedence table |
| Add VM opcode | `gdscript_byte_codegen.h` + `gdscript_vm.cpp` (enum + dispatch) |
| Autocomplete behavior | `gdscript_editor.cpp` `_find_identifiers_in_class` |
| Branding string | `core/version_override.py` / `goblin_builders.py` / `editor/goblin_about.cpp` |
| Port upstream commit | `porting` skill + diff mirrors vs upstream |

## Update discipline

Code changed -> update this map + `docs/gdscript_features.md`. Stale map is a landmine, not a doc.
