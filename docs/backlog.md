# Backlog

Single source of truth for ALL work on Goblin Engine: planned, in-progress, completed, and rejected. Update this file whenever work is planned, started, or completed. Do not let a task live only in a prompt or chat. Detailed specs live in `modules/goblin/docs/plans/`; this file tracks status.

Status legend: `todo` (planned), `doing` (in progress), `done` (complete), `blocked` (waiting), `rejected` (decided against).

Priorities: `P0` (critical), `P1` (high), `P2` (medium), `P3` (low).

---

## 0. Documentation & Governance

| ID | Item | Status | Priority | ADR/RFC | Notes |
|----|------|--------|----------|---------|-------|
| D-01 | Rewrite `INDEX.md` (module root) to reflect the fork, not branding-only | done | P0 | — | Rewritten as fork-focused index |
| D-02 | Rewrite `STRUCTURE.md` to document actual override mechanisms | done | P0 | — | Rewritten to document the three override mechanisms |
| D-03 | Update `ROADMAP.md` §4 to match implemented override mechanisms | done | P0 | — | Now documents `GOBLIN_MODULE_OVERRIDES` + `goblin_add_library()` |
| D-04 | Write `gdscript_features.md` documenting fork language additions | done | P0 | — | Full feature doc: unions, @private, String ctors, shaped dicts, then/elthen state |
| D-05 | Create ADRs for accepted decisions | done | P0 | — | 0001-0003, 0007 accepted, 0004-0006 proposed |
| D-06 | Keep `LIGHTMAP_INVESTIGATION.md` as reference for lightmap core changes | done | P3 | — | Decision: it informs C-01/C-02, so it stays in the fork |
| D-07 | Verify editor texture import works with compression modules disabled | todo | P1 | 0003 | `basis_universal`/`ktx`/`astcenc`/`etcpak` trimmed unconditionally — needs editor build test |
| D-08 | Create `CODE_MAP.md` (navigation map) | done | P0 | — | Read before implementing, update after; wired into `.kilo/rules/rules.md` |
| D-09 | Sync `gdscript_features.md` with code state | done | P0 | — | Added shaped dicts (implemented) + then/elthen (partial); `?.`/`??` removed |
| D-10 | Vision single-sourced | done | P0 | — | `.kilo/rules/master_prompt.md` canonical (genre family, Godot compat, decision hierarchy); `docs/vision.md` pointer deleted 2026-08-14 (D-15) — ROADMAP §1 carries the vision in docs |
| D-11 | Backlog cleanup: rejected section, plan-file tickets merged, recent work logged | done | P0 | — | This file |
| D-12 | Record locked `then`/`elthen` semantics + debug-only shaped validation | done | P1 | — | — | Done 2026-08-13: semantics locked (`then` null-only, `elthen` truthy — deliberate); "tokenizer only" claims corrected in `gdscript_features.md`, `CODE_MAP.md` (incl. landmine 3), `ROADMAP.md`, plan §3.2 (superseded note); DEBUG-only validation rationale documented. Remaining: TD-02 tests + G-04/G-05 corpus gate |
| D-12 | Sync `then`/`elthen` state in docs: implementation present, not "tokenizer only" | doing | P1 | — | — | MAJOR (2026-08-13 review): full parser/analyzer/compiler wiring verified in code, but `gdscript_features.md` §then/elthen ("operators do not compile yet"), CODE_MAP landmine 3 ("tokens present, rest missing"), and backlog G-04/G-05 status all claim otherwise. Update all three; remove landmine 3 |
| D-13 | Architect auto-creates plan + RFC artifacts when planning starts | done | P1 | — | `.kilo/agents/architect.md` "Plan Artifacts (automatic)": triggers (explicit plan/breakdown/"how to implement" ask OR spec reached implementation depth), always writes `modules/goblin/docs/plans/<slug>-plan.md`, conditional RFC in `modules/goblin/docs/rfc/` when exploratory, registers row here, dedups against existing plans; developer flow step 1 points at newest matching plan |
| D-14 | Rename `.kilo/rules/vision.md` -> `master_prompt.md`; living charter wired | done | P1 | — | Master prompt = living document (maintainer: architect; updated when locked decisions change a principle/hierarchy/non-negotiable — architect Job + rules.md checklist). `docs/vision.md` stays as engine-side pointer. `docs/proposal/` -> `docs/rfc/`; `GOBLIN_FORK_PLAN.md` -> `ROADMAP.md` (strategic layer above rfc/plans/adr) |
| D-15 | Telegraphic final: plain prompts + telegraphic outputs only | done | P1 | — | `docs/vision.md` pointer deleted: redundant — master_prompt.md serves agents (injected), ROADMAP §1 serves docs readers. Style measured (cl100k bench, 2026-08-14): prose-strip −53%, telegraphic output −41% zero info loss — kept. Aliases ≈0–1.5% on real prompts — removed. Unicode symbols cosmetic — removed. TOON −12% on small data — removed (display format only; tool/MCP calls need real JSON). Skill v3 = telegraphic prose only |

---

## 1. GDScript Language Features

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| G-01 | Union types (`int \| String`, `Dictionary \| null`) | done | P1 | — | 0004 | In fork; regression tests added (union dedup/collapse, null typing) |
| G-02 | `@private` annotation | done | P2 | — | — | Enforced for vars/funcs/consts/inner classes; same-script access policy; `@export` conflict error. Enforcement gaps fixed (2026-08-13): private method calls now blocked, private inner-class access no longer cascades a "cannot find member" error. Subclass name reuse deliberately NOT supported (O(n) scan cost on instance creation + sparse member indices) |
| G-03 | String constructors (`String(int)`, `String(float)`, `String(bool)`) | done | P3 | — | — | In fork, via `core/variant` override |
| G-16 | Regression tests for G-01..G-03 (`private_member_access`, `null_type_assignment`, `null_null_union`, etc.) | done | P1 | — | — | Added to mirror `tests/scripts/`; `.out` files written by hand — verify with `--gdscript-generate-tests` on a `tests=yes` build. Test runner/completion/LSP paths fixed to target the fork's own `tests/` dir (previously pointed at the upstream copy, so fork tests were unrunnable from repo root). Full GDScript suite is green (1379/1379 test cases) |
| G-17 | Shaped dictionary literals (typed entries, Lua style) | done | P1 | — | — | Parser/analyzer/runtime/autocomplete. Shape preserved across all declaration styles (`:=`, `: Dictionary`, `: Dictionary[K,V]`, untyped `=`) with compile-time write enforcement on typed keys; runtime construction validation + typed-container normalization (plain `Array` -> `Array[T]`). Recursive shape serialized inline in the instruction stream, decoded by `GDScriptFunction::decode_datatype()`. 11 regression test files. Fix (2026-08-13 review): typed-container declarations (`: Dictionary[K,V]`) used to drop the per-key shape at runtime (compiler preferred `CONSTRUCT_TYPED_DICTIONARY`, so entries were stored un-normalized); compiler now prefers the shaped opcode when a shape is present, and the VM applies `set_typed` + per-entry `set()` so the dict is typed as declared while entries still normalize (runtime-verified). Note: nested typed collections in *declarations* (`Dictionary[StringName, Array[int]]`) are an upstream 4.7.1 parser limitation — deep entries are covered via flat declarations (`Dictionary[StringName, Variant]`). Tests extended (runtime + analyzer) |
| G-04 | Safe navigation `then` | doing | P1 | 1-2d | — | Keywords locked (NOT `?.`); full parser/analyzer/compiler wiring present (verified 2026-08-13 review). Semantics locked: null-only `a != null ? b : a`, chainable. No tests yet — TD-02 |
| G-05 | Null coalescing `elthen` | doing | P1 | 1-2d | — | Pairs with G-04; wiring present. Semantics locked: TRUTHY `a ? a : b` (deliberate, 2026-08-13; `0 elthen 5` → `5`) — not the earlier null-only plan note. No tests yet — TD-02 |
| G-20 | `then`/`elthen` test suite + doc sync | doing | P1 | 1d | — | Superseded by D-12 (docs) + TD-02 (tests). Semantics locked as implemented — no code change planned |
| G-18 | Template dictionaries (`_template` reserved key, creation-time default expansion) | todo | P1 | 3-4.5d | 0009/0010 | `modules/goblin/docs/rfc/native-game-features-rfc.md` §2; builds on G-17 shaped-dict infra |
| G-19 | Callable shorthand (`fn(3)` -> `fn.call(3)`, dict member callables) | todo | P2 | 1-2d | 0011 | `modules/goblin/docs/rfc/native-game-features-rfc.md` §2.5 |
| G-07 | Structs / value types | todo | P1 | 4-6w | — | Biggest gap. Dict-heavy entity model, 60+ `duplicate(true)`. De-risk with 50 parser-only test cases first |
| G-08 | Typed dictionaries `Dictionary[K, V]` | todo | P1 | 1-2w | — | Kills ~30 `typeof()`+`as` checks in navigation; rides on G-17 infra |
| G-09 | Built-in `PriorityQueue` | todo | P2 | 2-3d | — | Navigation Dijkstra is O(N²) with no heap |
| G-10 | Inline caching (property access) | todo | P2 | 2-3d | — | From gdscript2; faster physics/AI hot paths |
| G-11 | Opcode fusing | todo | P2 | 3-5d | — | From gdscript2 `opcode_fusing`; fused array/dict/iterate |
| G-06 | `swap(a, b)` built-in | todo | P3 | 1h | — | Already in gdscript2; trivial port |
| G-12 | Blocks / stack-bound callables | todo | P3 | 2-3w | — | Kills 33+ `sort_custom` lambda allocations |
| G-13 | `yield` generators | todo | P3 | 3-4w | — | Lazy iteration without intermediate arrays |
| G-14 | Generics + `typeinfo` | todo | P3 | 4-6w | — | Typed containers without boxing; long-term |
| G-15 | Named args, destructuring | todo | P3 | — | — | Readability only |

---

## 2. Core Engine Changes

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| C-01 | Fix LightmapGI frustum culling (#71585) | todo | P0 | 1-2d | — | Root cause verified 2026-08-14 (plan §9c): empty `get_aabb()` (lightmap_gi.cpp:1820) + cull gate drops `INSTANCE_LIGHTMAP` from per-frame list (renderer_scene_cull.cpp:2930/2971); RD binds from that list (renderer_scene_render_rd.cpp:1454), GLES3 unaffected (`p_lightmaps` unused). Fix: `RS::instance_set_ignore_culling(get_instance(), true)` in POST_ENTER_TREE (lightmap_gi.cpp override; renderer-agnostic; upstream issue open since 2023-01, no fix — swap in upstream fix if one lands). Lands with the lightmapper_cpu lightmap_gi.cpp override |
| C-02 | Runtime LightmapBaker as public API | todo | P1 | 2-3d | 0006 | Direction superseded 2026-08-14: not "promote extension baker" but engine `lightmapper_cpu` module via `Lightmapper::create_cpu` (see `docs/rfc/lightmapper-cpu-rfc.md` + `docs/plans/lightmapper-cpu-plan.md`). Kills the `ClassDB.class_exists("LightmapBaker")` guard; extension baker retired after DB migrates. Verified gaps: (1) `LightmapGI::bake()` bind commented out upstream (lightmap_gi.cpp:2121) — GDScript surface ships as module `LightmapBaker` wrapper (node + descriptor paths, progress signals); (2) editor bake button + warnings hard-gated on `MODULE_LIGHTMAPPER_RD_ENABLED`; (3) editor `.exr` save broken today (`tinyexr` trimmed) — see C-11 |
| C-03 | MIDI in `AudioStreamPlayer3D` | done | P1 | — | — | Delivered by C-07: `MidiStream` is an `AudioStream`, so 3D spatialized MIDI works in any stream player with zero extra nodes. The old "manual node construction" need came from the GDExtension's pre-stream player design; obsolete since the extension's own "midi player -> midi stream" refactor. No separate code needed |
| C-04 | `Vector3i` keys for AStar3D | todo | P2 | 1-2d | — | Kills `"%d\|%d\|%d"` string keys in nav hot path |
| C-05 | Generic spatial field / probe grid (light + audio + effects) | todo | P2 | 2-3w | — | Replaces viewport-based `LightSensor`; one field infrastructure, many consumers (see M-07) |
| C-06 | Native 3D audio occlusion (per-source lowpass + portal re-emission) | todo | P2 | 2-3w | — | Feasibility verified (2026-08-13 brainstorm): swap `scene/3d/audio_stream_player_3d.cpp` via the variant_construct pattern (scene node, NOT `servers/audio/` — mixer stays upstream); occlusion rays via `PhysicsDirectSpaceState3D`; re-emit from open portals. Node-layer first (dynamic bus routing, zero swaps) delivers ~80%; swap only if per-source DSP proves necessary |
| C-07 | Built-in SoundFont (`.sf2`) synth module | done | P2 | — | — | Done 2026-08-14: standalone additive module `modules/midi/` at the repo root (ADR 0008 — additive features live in `modules/`, not inside `modules/goblin/`). `MidiStream` (AudioStream) + `MidiStreamPlayback` (TinySoundFont v0.9 synth + TinyMidiLoader v0.7, vendored verbatim under `midi/thirdparty/tinysoundfont/`, MIT/zlib) + `MidiFileResource`/`SoundFontResource` + `MidiImporter`/`SoundFontImporter` (engine-style `ResourceImporter`, registered at EDITOR level). Standard module anatomy: own `SCsub`/`config.py` (`can_build`, `get_doc_classes`, `get_icons_path`)/`register_types.{h,cpp}`/`doc_classes/`/`tests/`/`editor/icons/`; auto-discovered, gets `MODULE_MIDI_ENABLED` + registration via the generated `register_module_types.gen.cpp`. Class/property/importer names identical to the legacy GDExtension (`midi_stream.mid`/`midi_stream.sf2`), so DB projects and existing `.import` files keep working — the GDExtension dependency is dead. Features: loop, `midi_speed`, GM/note/drum enum constants, live `note_on`/`note_off`/`note_off_all` on the playback. Ported from `godot_extensions` (MidiStream GDExtension) — engine-native overrides (`start/stop/...` + `_mix_internal` + `get_stream_sampling_rate`, WAV pattern) instead of the GDVIRTUAL hooks. TSF/TML licenses in goblin `core/COPYRIGHT.txt` (the fork's license generator reads only that file; paths root-relative `modules/midi/thirdparty/...`). Verification: 7 doctest tests (`modules/midi/tests/test_midi_stream.h`, in-memory minimal SF2+SMF fixtures) — render/stop/loop/manual notes all green; full suite 1384/1384 + 420540 assertions; editor headless boot + real `.mid` import verified from the final location (`.import` sidecar records `importer=midi_stream`, imported `MidiFileResource.res` loads). Real-asset confirmation (2026-08-14, DB): actual `.sf2` + `.mid` files play correctly in-engine (audible + import path). Known limits: lazy SF2/MIDI parse runs on the main thread at first `play()`/length query (same as GDExtension — DB-proven); `get_playback_position()` is wall-clock, not tempo-mapped. Reviews 2026-08-14 (fixed same day, 2 passes): doc links → `AudioStreamPlayer.get_stream_playback()`; `get_length()` lazy-cached (no eager parse in `set_midi`) + `midi_speed`-scaled; GM/note/drum enum constants added to `MidiStream.xml`; TSF voice state mutex-serialized (audio-thread `_mix_internal` vs main-thread `start`/`stop`/`seek`/live notes — the upstream `stop()` fade path never calls `playback->stop()` while playing, so no stop deferral needed); failed SF2/MIDI loads not retried per mix block (resource-identity tracking in `_ensure_loaded`, reload on resource swap, one error print); `interleaved` pre-sized in `start()` (no audio-thread allocation); +2 tests (length vs `midi_speed`, seek) — 1386/1386 + 420549 assertions, editor boot clean. Tests use the dummy audio driver bootstrap (`AudioDriverManager::get_driver(0)` + `set_singleton()` + `init()`; `AudioServer` recreated per test because `GodotTestCaseListener::test_case_end` deletes it) |
| C-08 | MIDI module: tempo + `tml_get_info` extras exposure | todo | P2 | 0.5-1d | — | `tml_get_tempo_value` (vendored) + used channels/programs, note count, first-note time currently unused — nothing surfaces the tempo map. Beat-synced gameplay / cadence scheduling needs it. API shape on `MidiStream`/`MidiFileResource` to lock |
| C-09 | MIDI module: channel-level live mixing + GM-number note path | todo | P2 | 0.5-1d | — | TSF exposes `tsf_channel_set_volume`/`pan`/`sustain` + `tsf_channel_sounds_off_all`; none reach GDScript — dialogue ducking and music-intensity shifts are core systemic needs. `note_on` takes TSF preset index while enums are GM numbers; needs `tsf_get_presetindex(bank, program)` overload. One-line wrappers on `MidiStreamPlayback` |
| C-10 | MIDI module: second synthetic fixture (looped sample + drums + pitch bend) | todo | P3 | 0.5d | — | Fixture is 1 preset / 1 sample / no loop / no drums — loop-sustain, channel-9 drum, and bend paths are only proven by the real-file test, not CI |
| C-12 | Engine-side CUT 1 upscaler for GL Compatibility (shared core) | todo | P1 | ~1wk | 0009 + RFC | Fragment-only 3D upscaler for GLES3 (FSR is compute-only). Clean-room (GPL boundary). Shared core: 2x2 luma triangulation + pattern recognition; 1 pass, 4 samples, 45 deg. Direct header edit (enum 6, first header precedent) + 5 mirror swaps + goblin cut.glsl. Spec: `docs/plans/cut-upscalers-plan.md`; docs: `rfc/cut-upscalers-rfc.md`, `cut-upscalers.md` |
| C-13 | CUT 2 + CUT 3 variants | todo | P2 | 5-8d | 0009 + RFC | Same shared core. CUT2: 2 passes, 12*I+5*O, 30 deg, soft edges (0.20/0.75). CUT3: 3 passes, 12*I+4*D*I+5*O, edge search D=1-8, MIN_CONTRAST 0.5. Independent ship gates; same plan |
| C-11 | Lightmap editor pipeline fixes (lightmapper_cpu companion) | todo | P1 | 0.5-1d | — | Verified 2026-08-14: (1) `editor/scene/3d/lightmap_gi_editor_plugin.cpp` bake button hard-disabled without `MODULE_LIGHTMAPPER_RD_ENABLED` — gate on CPU module too (editor override, B-04 dict); (2) `get_configuration_warnings()` same gate (in the lightmap_gi.cpp override); (3) editor `.exr` lightmap save broken in fork today — `Image::save_exr` is `ERR_UNAVAILABLE` without `tinyexr` (trimmed) — re-enable `tinyexr` with ADR 0003 evidence (editor bake requires it). Runtime path unaffected (in-memory) |

---

## 3. Modules & Build

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| B-01 | Module trim (30 modules disabled) | done | P1 | — | 0003 | Implemented in `config.py` |
| B-02 | Whole-module override mechanism | done | P1 | — | 0001 | `GOBLIN_MODULE_OVERRIDES` in `SCsub` |
| B-03 | Single-file core override mechanism | done | P1 | — | 0001 | `goblin_add_library()` in `config.py` |
| B-07 | Remove `clean` command from `goblin_manager.py` | done | P1 | — | — | Ran `scons --clean` + deleted `.scons_cache` — hard rule 1 violation |
| B-08 | `goblin_manager.py` `build` subcommand targets linuxbsd | todo | P3 | — | — | Wrong for this project (Windows). Fix or remove the subcommand |
| B-09 | Generalize `goblin_add_library()` to a `{basename: path}` dict | todo | P1 | 2-4h | 0001 | Hook is hardwired to `variant_construct` (single basename, single path). Required before the second core file swap (C-01, C-06, ...). ADR 0001 flags it |
| B-10 | Mirror drift check in `goblin_manager.py` | todo | P3 | 1-2h | — | Lists every goblin mirror + diff-stat vs upstream; makes silent mirror staleness visible on demand (the one maintenance hazard of the override model) |
| B-04 | Retry-loop replacement (compile-time overrides for `editor_about.cpp` + exports + PM + editor_node) | done | P1 | 1-2d | 0007 | Done 2026-08-13: runtime singletons (`GoblinBranding`, `GoblinExportTweaks`) deleted; 120-attempt SceneTree polling + `node_added` tree scans gone. 4-file compile-time override set via library-scoped dict in `goblin_add_library()`: `editor_about.cpp` (Goblin literals, Donors tab removed), `project_export.cpp` (debug-template-aware "Export With Debug" option, warning filter, literal fixes), `project_manager.cpp` (Donate button removed), `editor_node.cpp` (Support Godot Development item/shortcut/case removed). Translation overrides relocated to `branding_translations.cpp` (kept as fallback). `Godot.svg`/`TitleBarLogo.svg` icon overrides. See ADR 0007 |
| B-09 | Generalize `goblin_add_library()` hook (core-only if-chain → library-scoped dict) | done | P1 | — | — | Landed with B-04/ADR 0007: `_GOBLIN_FILE_OVERRIDES = {lib: {stem: path}}` covering `core` + `editor` |
| B-10 | Mirror-drift tooling / discipline for editor overrides | todo | P2 | — | — | Diff mirrors against upstream on rebase (`git diff --no-index --stat editor/<f> modules/goblin/editor/overrides/<f>`); `project_export.h` mirror + `editor_node.cpp` are the highest-churn surfaces (ADR 0007) |
| B-11 | Composed-string branding gaps (exact-key overrides never matched) | todo | P3 | — | — | `"%s - Godot Engine"` window titles (editor_dock_manager.cpp:286, script_editor_plugin.cpp:4213, game_view_plugin.cpp:1751), `"Godot Version"` (export_template_manager.cpp:1606), `"Godot Feature Profile"` (editor_feature_profile.cpp). Decide later whether to override those files |
| B-12 | Editor icon overrides did not apply (registration race) | done | P1 | — | — | Fixed 2026-08-14: `editor/SCsub` appended to `module_icons_paths` too late (SConstruct runs `editor/SCsub` before `modules/SCsub`), so About dialog/help menu/PM kept upstream Godot icons. Registration moved to `config.get_icons_path()` (configure-time). Also added `Depends` edges in `modules/goblin/core/SCsub` so authors/donors/license/version gen headers regenerate when goblin sources change. Follow-up (same day): the goblin icon SVGs kept `width="100%"` → ThorVG rasterized at 1024px intrinsic size → banner filled the About dialog / PM title bar. Icons resized to upstream-equivalent fixed sizes: `Logo.svg` 187×76 (banner, About + credits), `Godot.svg` 16×16 (face, help-menu About item), `TitleBarLogo.svg` 24×24 (face, PM title bar), `LogoOutlined.svg` 187×76 |
| B-14 | About/PM logo wordmark invisible (`<text>` elements) | done | P1 | — | — | Fixed 2026-08-14: ThorVG (Godot's SVG rasterizer) has NO font loader — `<text>` elements render nothing; that's why zero upstream editor icons use them (all path data). `Logo.svg` + `TitleBarLogo.svg` now embed the wordmark as white **path** letters. `LogoOutlined.svg` deleted (unused). Follow-ups (same day): (1) stroke-outline letters from `logo_outlined.svg` render as hollow rings ("black with white outline") — rebuilt as solid glyphs by keeping the outer contour + true counter holes (inset-based discriminator: stroke inner edges hug the outer bbox <15%, counters are ≥20% inset; area-ratio heuristics fail on small letters). (2) `Logo.svg` "Engine" (64px) was unreadable as solidified blobs — regenerated from the real Arial Bold font via fontTools (`fontTools.pens.svgPathPen`) as true thin glyph outlines with proper counters. (3) balanced group extraction (depth-counting, not regex) required; duplicate nested transforms from rebuilding caused layout shifts — final icons are single-wrapper, group-balanced, well-formed XML, render-verified via engine ThorVG (GOBLIN solid x 163-360, Engine thin glyphs x 218-300 with counters) |
| B-15 | Boot splash stale + editor splash missing | done | P1 | — | — | Fixed 2026-08-14: (1) `main/splash.gen.h` never regenerated on goblin splash.png change (SCons keyed to upstream `#main/splash.png`) — added `Depends` edges for splash/splash_editor/app_icon gen headers in `modules/goblin/core/SCsub`. (2) Upstream 4.7 REMOVED the editor splash (commit c283fce698) — `no_editor_splash` defaults True + `#main/splash_editor.png` absent forces it. Fork re-enables: config.py strips `NO_EDITOR_SPLASH` from CPPDEFINES, goblin SCsub generates `#main/splash_editor.gen.h` from the goblin splash_editor.png |
| B-16 | Windows exe icon still Godot (goblin.rc was dead code) | done | P1 | — | — | Fixed 2026-08-14: `modules/goblin/platform/windows/goblin.rc` was referenced nowhere — the exe icon came from upstream `godot_res.rc` → `godot.ico`. Now: RES builder wrapped in config.py (`env.AddMethod` shadow) to compile `goblin.rc`/`goblin_res_wrap.rc` instead; `goblin.ico` generated at build time from `app_icon.png` (PNG-compressed ICO, `goblin_ico_builder`); version info strings goblin-branded ("Goblin Engine", https://goblin-engine.org). Verified: res obj contains goblin.ico bytes, upstream godot.ico absent |
| B-13 | About dialog licenses tab: Expat + CC-BY-4.0 bodies empty | done | P1 | — | — | Fixed 2026-08-14: `modules/goblin/core/COPYRIGHT.txt` standalone licenses had unindented bodies (parser only captures indented continuation lines) → empty `LICENSE_BODIES`. Bodies reformatted to upstream format (leading space, `.` = blank line); CC-BY-4.0 body replaced with full license text |
| B-05 | Add `--max-drift=1 --implicit-deps-unchanged` to default build | todo | P3 | — | — | Faster incremental builds |
| B-06 | Platform driver trim decision | todo | P3 | — | — | Keep all platforms; decide on per-platform audio drivers |

---

## 4. Migrate From The From-Scratch Engine

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| M-02 | Script module tier system (Stable/Tooling/Expert) | todo | P3 | — | — | Governs which internals GDScript exposes |
| M-03 | Basis-frame transform convention | todo | P3 | — | — | Eliminates Euler/axis-order ambiguity |
| M-04 | Component family contracts | todo | P3 | — | — | Basis for struct-based ECS |
| M-05 | Portals & mirrors as **custom nodes** (`PortalSurface3D`/`MirrorSurface3D`), not first-class core | todo | P3 | — | — | `SubViewport` + teleport + portal-aware queries |
| M-06 | Retro-native rendering as **editor-provided** nodes/plugins (palette, dither, color cycling) | todo | P3 | — | — | Not core renderer changes |
| M-07 | Generic spatial field system (light + audio + effects probe grid) | todo | P2 | — | — | The ambient-probe field generalized; drives light/audio/music/effects |
| M-08 | Native stealth shadow value (Thief light gem) | todo | P2 | — | — | Gameplay readout on top of M-07 field; GDR-009a |
| M-09 | Hitscan surface metadata contract | todo | P3 | — | — | Raycast -> surface class + object ID + impact UV |
| M-10 | Kinematic brush movers (doors/lifts/crushers) | todo | P3 | — | — | Generalizes DB `Moving` via `AnimatableBody3D` |
| M-11 | Lightstyle channels + retro surface-class lighting | todo | P3 | — | — | Style-channel modulation of baked light |
| M-12 | Per-view palette selection and blending | todo | P3 | — | — | Portal views inherit palette overrides |
| M-13 | Texture-space animation families (UV scroll, frame cycling) | todo | P3 | — | — | Color-cycling mechanism, in-shader via simulation clock |

*Already adopted by DB (not in backlog):* cadence scheduler with custom process groups (`scheduler.gd`), scene-first composition, partition streaming, delta save/load, Lego-block entity composition. Off-screen simulation was evaluated and dropped — the scheduler + event queue already covers the need.

---

## 5. Rejected / Deferred

| Item | Why |
|------|-----|
| `?.` / `??` syntax | Keywords `then`/`elthen` locked (2026-08-13). See G-04/G-05 |
| `then` truthiness semantics (gdscript2 runtime) | Null-only decided at port — explicit `!= null` conditions (`modules/goblin/docs/rfc/native-game-features-rfc.md` §3.2) |
| Native `CustomTree` cadence scheduler | Remnant; not necessary |
| `GoblinDataTable` native fallback class | Replaced by template dictionaries (G-18) |
| Replace `core/variant/dictionary.{h,cpp}` | Header override unsupported + max rebase surface; language layer covers it (G-17/G-18) |
| Expose editor `LightmapperRD` at runtime | Editor-only GPU module + forbidden build-flag changes; GL-compat templates lack RenderingDevice. Replaced by the `lightmapper_cpu` RFC (CPU implementation, same contract) |
| GDScript `extends Dictionary` / user Variant types | Variant + ClassDB surgery; G-17/G-18 cover the need at language level |
| Renderer-side light sampling on GL Compatibility | No exposed cluster seams; CPU field (C-05/M-07) substitutes |
| Native upscaler nodes | DB's GL-compat canvas shaders already do this. GLES3 3D-scaling CUT modes are SEPARATE work (C-12/C-13): canvas shaders cannot reach the 3D upscale path |
| C++ Reaction Server (stimulus × material → action matrix) | Game data + O(1) dict lookup; C++ adds nothing. G-07/G-08 (structs/typed dicts) cover the real gap at language level. Brainstorm 2026-08-13 |
| Background-thread perception grid (AI sensor network) | Determinism/sync nightmare; M-07/M-08 CPU field at cadence is the sanctioned answer (off-screen sim already rejected). Brainstorm 2026-08-13 |
| Palette quantization / color cycling in renderer core | Already decided: M-06/M-13 nodes + shaders, no core renderer changes. Brainstorm 2026-08-13 |
| Driver-level affine texture mapping / PS1 vertex snapping | Shader/material-level trick (vertex snap, LUT post-process), not driver work; do per-asset when a game needs it. Brainstorm 2026-08-13 |
| Raytraced acoustic propagation in `AudioServer` | Portal re-emission + volumetric reverb estimation is research-grade with no DB justification; occlusion value is delivered by C-06 + M-07 zones without mixer surgery. Brainstorm 2026-08-13 |

---

## 6. Tech Debt

Findings from the `/tech-debt-review` workflow. Fork-side debt only; upstream issues are reported to the user, not tracked here.

| ID | Item | Source | Severity | Notes |
|----|------|--------|----------|-------|
| TD-01 | Pre-existing fork test failures | G-17 verification run (2026-08-13) | Resolved | Root causes fixed: (1) `reduce_identifier_from_base` copied the member before `resolve_class_member` (introduced by G-02's @private commit), breaking out-of-order enum/const resolution; (2) @private gaps: private method calls not blocked (`get_function_signature`), cascading "cannot find member" on private inner-class access; (3) stale `.out` files (6 missing required trailing newline, `null_type_assignment` hand-written message); (4) malformed `private_same_script_access` test (accessed non-existent member); (5) harness `res://` bug — `ProjectSettings::setup` short-circuits when the resource path is already set (left behind by the GLTF suite), so `init_language` now forces the resource path to the test scripts dir. Full GDScript suite: 1379/1379 test cases pass, 0 failures |
| TD-02 | Missing `then`/`elthen` test suite | Feature review 2026-08-13 | Open | ~25 cases per plan §3.5: basic `then`/`elthen`, constant folding, type inference, chaining, null-vs-falsy distinction (`0 elthen 5` → `5` must be pinned), interplay with union types, error cases. Required to close G-04/G-05 |

---

## Dependency Notes

- **G-04/G-05** follow `modules/goblin/docs/rfc/native-game-features-rfc.md` §3 (recovered gdscript2 port map; 5 files, no VM changes).
- **G-18/G-19** follow `modules/goblin/docs/rfc/native-game-features-rfc.md` §2 (spec + phases + test gates).
- **G-07 (structs)** blocks or de-risks G-08 (typed dicts) and M-04 (component families).
- **G-10/G-11** are ported from the gdscript2 module's branches — cherry-pick one at a time, never wholesale.
- **C-01** is the highest-priority core fix; it is a known upstream bug and the fix is upstream-acceptable.
- **C-05 (generic field)** is the sampling infrastructure; **M-08 (stealth shadow value)** is the gameplay readout on top of it.
- **M-05 (portals/mirrors)** is a custom-node feature, not a core change — cheap to attempt, no engine surgery.
- **Direct core edits** (decided 2026-08-13 brainstorm): allowed only when the swap mechanism cannot reach the change — header-only changes included by upstream files, files outside `add_library` source lists (platform/tools), or upstream-acceptable fixes intended for submission (then the rebase stays clean). Everything else goes through mirror+swap (default) or additive module code.
- **Additive feature modules** (ADR 0008, 2026-08-14): live at the repo root in `modules/<name>/` with standard module anatomy and full lifecycle (`MODULE_<NAME>_ENABLED`, `DISABLE_MODULES` gating, own registration/docs/icons/tests) - never inside `modules/goblin/` (goblin is override/branding-only). Current: `modules/midi/` (C-07).
- **C-12/C-13 (CUT upscalers)** follow `rfc/cut-upscalers-rfc.md` + `docs/plans/cut-upscalers-plan.md`; ADR 0009 (clean-room boundary + first header direct-edit precedent: `rendering_server_enums.h`).
