# Backlog

Single source of truth for ALL work on Goblin Engine: planned, in-progress, completed, and rejected. Update this file whenever work is planned, started, or completed. Do not let a task live only in a prompt or chat. Detailed specs live in `.kilo/plans/`; this file tracks status.

Status legend: `todo` (planned), `doing` (in progress), `done` (complete), `blocked` (waiting), `rejected` (decided against).

Priorities: `P0` (critical), `P1` (high), `P2` (medium), `P3` (low).

---

## 0. Documentation & Governance

| ID | Item | Status | Priority | ADR/RFC | Notes |
|----|------|--------|----------|---------|-------|
| D-01 | Rewrite `INDEX.md` (module root) to reflect the fork, not branding-only | done | P0 | â€” | Rewritten as fork-focused index |
| D-02 | Rewrite `STRUCTURE.md` to document actual override mechanisms | done | P0 | â€” | Rewritten to document the three override mechanisms |
| D-03 | Update `GOBLIN_FORK_PLAN.md` Â§4 to match implemented override mechanisms | done | P0 | â€” | Now documents `GOBLIN_MODULE_OVERRIDES` + `goblin_add_library()` |
| D-04 | Write `gdscript_features.md` documenting fork language additions | done | P0 | â€” | Full feature doc: unions, @private, String ctors, shaped dicts, then/elthen state |
| D-05 | Create ADRs for accepted decisions | done | P0 | â€” | 0001-0003, 0007 accepted, 0004-0006 proposed |
| D-06 | Keep `LIGHTMAP_INVESTIGATION.md` as reference for lightmap core changes | done | P3 | â€” | Decision: it informs C-01/C-02, so it stays in the fork |
| D-07 | Verify editor texture import works with compression modules disabled | todo | P1 | 0003 | `basis_universal`/`ktx`/`astcenc`/`etcpak` trimmed unconditionally â€” needs editor build test |
| D-08 | Create `CODE_MAP.md` (navigation map) | done | P0 | â€” | Read before implementing, update after; wired into `.kilo/rules/rules.md` |
| D-09 | Sync `gdscript_features.md` with code state | done | P0 | â€” | Added shaped dicts (implemented) + then/elthen (partial); `?.`/`??` removed |
| D-10 | Vision single-sourced | done | P0 | â€” | `.kilo/rules/vision.md` canonical (genre family, Godot compat, decision hierarchy); `docs/vision.md` = pointer |
| D-11 | Backlog cleanup: rejected section, plan-file tickets merged, recent work logged | done | P0 | â€” | This file |
| D-12 | Record locked `then`/`elthen` semantics + debug-only shaped validation | done | P1 | â€” | â€” | Done 2026-08-13: semantics locked (`then` null-only, `elthen` truthy â€” deliberate); "tokenizer only" claims corrected in `gdscript_features.md`, `CODE_MAP.md` (incl. landmine 3), `GOBLIN_FORK_PLAN.md`, plan Â§3.2 (superseded note); DEBUG-only validation rationale documented. Remaining: TD-02 tests + G-04/G-05 corpus gate |
| D-12 | Sync `then`/`elthen` state in docs: implementation present, not "tokenizer only" | doing | P1 | â€” | â€” | MAJOR (2026-08-13 review): full parser/analyzer/compiler wiring verified in code, but `gdscript_features.md` Â§then/elthen ("operators do not compile yet"), CODE_MAP landmine 3 ("tokens present, rest missing"), and backlog G-04/G-05 status all claim otherwise. Update all three; remove landmine 3 |

---

## 1. GDScript Language Features

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| G-01 | Union types (`int \| String`, `Dictionary \| null`) | done | P1 | â€” | 0004 | In fork; regression tests added (union dedup/collapse, null typing) |
| G-02 | `@private` annotation | done | P2 | â€” | â€” | Enforced for vars/funcs/consts/inner classes; same-script access policy; `@export` conflict error. Enforcement gaps fixed (2026-08-13): private method calls now blocked, private inner-class access no longer cascades a "cannot find member" error. Subclass name reuse deliberately NOT supported (O(n) scan cost on instance creation + sparse member indices) |
| G-03 | String constructors (`String(int)`, `String(float)`, `String(bool)`) | done | P3 | â€” | â€” | In fork, via `core/variant` override |
| G-16 | Regression tests for G-01..G-03 (`private_member_access`, `null_type_assignment`, `null_null_union`, etc.) | done | P1 | â€” | â€” | Added to mirror `tests/scripts/`; `.out` files written by hand â€” verify with `--gdscript-generate-tests` on a `tests=yes` build. Test runner/completion/LSP paths fixed to target the fork's own `tests/` dir (previously pointed at the upstream copy, so fork tests were unrunnable from repo root). Full GDScript suite is green (1379/1379 test cases) |
| G-17 | Shaped dictionary literals (typed entries, Lua style) | done | P1 | â€” | â€” | Parser/analyzer/runtime/autocomplete. Shape preserved across all declaration styles (`:=`, `: Dictionary`, `: Dictionary[K,V]`, untyped `=`) with compile-time write enforcement on typed keys; runtime construction validation + typed-container normalization (plain `Array` -> `Array[T]`). Recursive shape serialized inline in the instruction stream, decoded by `GDScriptFunction::decode_datatype()`. 11 regression test files. Fix (2026-08-13 review): typed-container declarations (`: Dictionary[K,V]`) used to drop the per-key shape at runtime (compiler preferred `CONSTRUCT_TYPED_DICTIONARY`, so entries were stored un-normalized); compiler now prefers the shaped opcode when a shape is present, and the VM applies `set_typed` + per-entry `set()` so the dict is typed as declared while entries still normalize (runtime-verified). Note: nested typed collections in *declarations* (`Dictionary[StringName, Array[int]]`) are an upstream 4.7.1 parser limitation â€” deep entries are covered via flat declarations (`Dictionary[StringName, Variant]`). Tests extended (runtime + analyzer) |
| G-04 | Safe navigation `then` | doing | P1 | 1-2d | â€” | Keywords locked (NOT `?.`); full parser/analyzer/compiler wiring present (verified 2026-08-13 review). Semantics locked: null-only `a != null ? b : a`, chainable. No tests yet â€” TD-02 |
| G-05 | Null coalescing `elthen` | doing | P1 | 1-2d | â€” | Pairs with G-04; wiring present. Semantics locked: TRUTHY `a ? a : b` (deliberate, 2026-08-13; `0 elthen 5` â†’ `5`) â€” not the earlier null-only plan note. No tests yet â€” TD-02 |
| G-20 | `then`/`elthen` test suite + doc sync | doing | P1 | 1d | â€” | Superseded by D-12 (docs) + TD-02 (tests). Semantics locked as implemented â€” no code change planned |
| G-18 | Template dictionaries (`_template` reserved key, creation-time default expansion) | todo | P1 | 3-4.5d | 0009/0010 | `.kilo/plans/` Â§2; builds on G-17 shaped-dict infra |
| G-19 | Callable shorthand (`fn(3)` -> `fn.call(3)`, dict member callables) | todo | P2 | 1-2d | 0011 | `.kilo/plans/` Â§2.5 |
| G-07 | Structs / value types | todo | P1 | 4-6w | â€” | Biggest gap. Dict-heavy entity model, 60+ `duplicate(true)`. De-risk with 50 parser-only test cases first |
| G-08 | Typed dictionaries `Dictionary[K, V]` | todo | P1 | 1-2w | â€” | Kills ~30 `typeof()`+`as` checks in navigation; rides on G-17 infra |
| G-09 | Built-in `PriorityQueue` | todo | P2 | 2-3d | â€” | Navigation Dijkstra is O(NÂ²) with no heap |
| G-10 | Inline caching (property access) | todo | P2 | 2-3d | â€” | From gdscript2; faster physics/AI hot paths |
| G-11 | Opcode fusing | todo | P2 | 3-5d | â€” | From gdscript2 `opcode_fusing`; fused array/dict/iterate |
| G-06 | `swap(a, b)` built-in | todo | P3 | 1h | â€” | Already in gdscript2; trivial port |
| G-12 | Blocks / stack-bound callables | todo | P3 | 2-3w | â€” | Kills 33+ `sort_custom` lambda allocations |
| G-13 | `yield` generators | todo | P3 | 3-4w | â€” | Lazy iteration without intermediate arrays |
| G-14 | Generics + `typeinfo` | todo | P3 | 4-6w | â€” | Typed containers without boxing; long-term |
| G-15 | Named args, destructuring | todo | P3 | â€” | â€” | Readability only |

---

## 2. Core Engine Changes

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| C-01 | Fix LightmapGI frustum culling (#71585) | todo | P0 | 1-2d | â€” | Critical bug; runtime lightmap pipeline partially works around it |
| C-02 | Runtime LightmapBaker as public API | todo | P1 | 2-3d | 0006 | Eliminates `ClassDB.class_exists("LightmapBaker")` guard |
| C-03 | MIDI in `AudioStreamPlayer3D` | done | P1 | â€” | â€” | Delivered by C-07: `MidiStream` is an `AudioStream`, so 3D spatialized MIDI works in any stream player with zero extra nodes. The old "manual node construction" need came from the GDExtension's pre-stream player design; obsolete since the extension's own "midi player -> midi stream" refactor. No separate code needed |
| C-04 | `Vector3i` keys for AStar3D | todo | P2 | 1-2d | â€” | Kills `"%d\|%d\|%d"` string keys in nav hot path |
| C-05 | Generic spatial field / probe grid (light + audio + effects) | todo | P2 | 2-3w | â€” | Replaces viewport-based `LightSensor`; one field infrastructure, many consumers (see M-07) |
| C-06 | Native 3D audio occlusion (per-source lowpass + portal re-emission) | todo | P2 | 2-3w | â€” | Feasibility verified (2026-08-13 brainstorm): swap `scene/3d/audio_stream_player_3d.cpp` via the variant_construct pattern (scene node, NOT `servers/audio/` â€” mixer stays upstream); occlusion rays via `PhysicsDirectSpaceState3D`; re-emit from open portals. Node-layer first (dynamic bus routing, zero swaps) delivers ~80%; swap only if per-source DSP proves necessary |
| C-07 | Built-in SoundFont (`.sf2`) synth module | done | P2 | — | — | Done 2026-08-14: standalone additive module `modules/midi/` at the repo root (ADR 0008 — additive features live in `modules/`, not inside `modules/goblin/`). `MidiStream` (AudioStream) + `MidiStreamPlayback` (TinySoundFont v0.9 synth + TinyMidiLoader v0.7, vendored verbatim under `midi/thirdparty/tinysoundfont/`, MIT/zlib) + `MidiFileResource`/`SoundFontResource` + `MidiImporter`/`SoundFontImporter` (engine-style `ResourceImporter`, registered at EDITOR level). Standard module anatomy: own `SCsub`/`config.py` (`can_build`, `get_doc_classes`, `get_icons_path`)/`register_types.{h,cpp}`/`doc_classes/`/`tests/`/`editor/icons/`; auto-discovered, gets `MODULE_MIDI_ENABLED` + registration via the generated `register_module_types.gen.cpp`. Class/property/importer names identical to the legacy GDExtension (`midi_stream.mid`/`midi_stream.sf2`), so DB projects and existing `.import` files keep working — the GDExtension dependency is dead. Features: loop, `midi_speed`, GM/note/drum enum constants, live `note_on`/`note_off`/`note_off_all` on the playback. Ported from `godot_extensions` (MidiStream GDExtension) — engine-native overrides (`start/stop/...` + `_mix_internal` + `get_stream_sampling_rate`, WAV pattern) instead of the GDVIRTUAL hooks. TSF/TML licenses in goblin `core/COPYRIGHT.txt` (the fork's license generator reads only that file; paths root-relative `modules/midi/thirdparty/...`). Verification: 7 doctest tests (`modules/midi/tests/test_midi_stream.h`, in-memory minimal SF2+SMF fixtures) — render/stop/loop/manual notes all green; full suite 1384/1384 + 420540 assertions; editor headless boot + real `.mid` import verified from the final location (`.import` sidecar records `importer=midi_stream`, imported `MidiFileResource.res` loads). Real-asset confirmation (2026-08-14, DB): actual `.sf2` + `.mid` files play correctly in-engine (audible + import path). Known limits: lazy SF2/MIDI parse runs on the main thread at first `play()`/length query (same as GDExtension — DB-proven); `get_playback_position()` is wall-clock, not tempo-mapped. Reviews 2026-08-14 (fixed same day, 2 passes): doc links → `AudioStreamPlayer.get_stream_playback()`; `get_length()` lazy-cached (no eager parse in `set_midi`) + `midi_speed`-scaled; GM/note/drum enum constants added to `MidiStream.xml`; TSF voice state mutex-serialized (audio-thread `_mix_internal` vs main-thread `start`/`stop`/`seek`/live notes — the upstream `stop()` fade path never calls `playback->stop()` while playing, so no stop deferral needed); failed SF2/MIDI loads not retried per mix block (resource-identity tracking in `_ensure_loaded`, reload on resource swap, one error print); `interleaved` pre-sized in `start()` (no audio-thread allocation); +2 tests (length vs `midi_speed`, seek) — 1386/1386 + 420549 assertions, editor boot clean. Tests use the dummy audio driver bootstrap (`AudioDriverManager::get_driver(0)` + `set_singleton()` + `init()`; `AudioServer` recreated per test because `GodotTestCaseListener::test_case_end` deletes it) |
| C-08 | MIDI module: tempo + `tml_get_info` extras exposure | todo | P2 | 0.5-1d | — | `tml_get_tempo_value` (vendored) + used channels/programs, note count, first-note time currently unused — nothing surfaces the tempo map. Beat-synced gameplay / cadence scheduling needs it. API shape on `MidiStream`/`MidiFileResource` to lock |
| C-09 | MIDI module: channel-level live mixing + GM-number note path | todo | P2 | 0.5-1d | — | TSF exposes `tsf_channel_set_volume`/`pan`/`sustain` + `tsf_channel_sounds_off_all`; none reach GDScript — dialogue ducking and music-intensity shifts are core systemic needs. `note_on` takes TSF preset index while enums are GM numbers; needs `tsf_get_presetindex(bank, program)` overload. One-line wrappers on `MidiStreamPlayback` |
| C-10 | MIDI module: second synthetic fixture (looped sample + drums + pitch bend) | todo | P3 | 0.5d | — | Fixture is 1 preset / 1 sample / no loop / no drums — loop-sustain, channel-9 drum, and bend paths are only proven by the real-file test, not CI |

---

## 3. Modules & Build

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| B-01 | Module trim (30 modules disabled) | done | P1 | â€” | 0003 | Implemented in `config.py` |
| B-02 | Whole-module override mechanism | done | P1 | â€” | 0001 | `GOBLIN_MODULE_OVERRIDES` in `SCsub` |
| B-03 | Single-file core override mechanism | done | P1 | â€” | 0001 | `goblin_add_library()` in `config.py` |
| B-07 | Remove `clean` command from `goblin_manager.py` | done | P1 | â€” | â€” | Ran `scons --clean` + deleted `.scons_cache` â€” hard rule 1 violation |
| B-08 | `goblin_manager.py` `build` subcommand targets linuxbsd | todo | P3 | â€” | â€” | Wrong for this project (Windows). Fix or remove the subcommand |
| B-09 | Generalize `goblin_add_library()` to a `{basename: path}` dict | todo | P1 | 2-4h | 0001 | Hook is hardwired to `variant_construct` (single basename, single path). Required before the second core file swap (C-01, C-06, ...). ADR 0001 flags it |
| B-10 | Mirror drift check in `goblin_manager.py` | todo | P3 | 1-2h | â€” | Lists every goblin mirror + diff-stat vs upstream; makes silent mirror staleness visible on demand (the one maintenance hazard of the override model) |
| B-04 | Retry-loop replacement (compile-time overrides for `editor_about.cpp` + exports + PM + editor_node) | done | P1 | 1-2d | 0007 | Done 2026-08-13: runtime singletons (`GoblinBranding`, `GoblinExportTweaks`) deleted; 120-attempt SceneTree polling + `node_added` tree scans gone. 4-file compile-time override set via library-scoped dict in `goblin_add_library()`: `editor_about.cpp` (Goblin literals, Donors tab removed), `project_export.cpp` (debug-template-aware "Export With Debug" option, warning filter, literal fixes), `project_manager.cpp` (Donate button removed), `editor_node.cpp` (Support Godot Development item/shortcut/case removed). Translation overrides relocated to `branding_translations.cpp` (kept as fallback). `Godot.svg`/`TitleBarLogo.svg` icon overrides. See `.kilo/plans/1786657934359-compile-time-ui-overrides.md` + ADR 0007 |
| B-09 | Generalize `goblin_add_library()` hook (core-only if-chain â†’ library-scoped dict) | done | P1 | â€” | â€” | Landed with B-04/ADR 0007: `_GOBLIN_FILE_OVERRIDES = {lib: {stem: path}}` covering `core` + `editor` |
| B-10 | Mirror-drift tooling / discipline for editor overrides | todo | P2 | â€” | â€” | Diff mirrors against upstream on rebase (`git diff --no-index --stat editor/<f> modules/goblin/editor/overrides/<f>`); `project_export.h` mirror + `editor_node.cpp` are the highest-churn surfaces (ADR 0007) |
| B-11 | Composed-string branding gaps (exact-key overrides never matched) | todo | P3 | â€” | â€” | `"%s - Godot Engine"` window titles (editor_dock_manager.cpp:286, script_editor_plugin.cpp:4213, game_view_plugin.cpp:1751), `"Godot Version"` (export_template_manager.cpp:1606), `"Godot Feature Profile"` (editor_feature_profile.cpp). Decide later whether to override those files |
| B-12 | Editor icon overrides did not apply (registration race) | done | P1 | â€” | â€” | Fixed 2026-08-14: `editor/SCsub` appended to `module_icons_paths` too late (SConstruct runs `editor/SCsub` before `modules/SCsub`), so About dialog/help menu/PM kept upstream Godot icons. Registration moved to `config.get_icons_path()` (configure-time). Also added `Depends` edges in `modules/goblin/core/SCsub` so authors/donors/license/version gen headers regenerate when goblin sources change. Follow-up (same day): the goblin icon SVGs kept `width="100%"` â†’ ThorVG rasterized at 1024px intrinsic size â†’ banner filled the About dialog / PM title bar. Icons resized to upstream-equivalent fixed sizes: `Logo.svg` 187Ã—76 (banner, About + credits), `Godot.svg` 16Ã—16 (face, help-menu About item), `TitleBarLogo.svg` 24Ã—24 (face, PM title bar), `LogoOutlined.svg` 187Ã—76 |
| B-14 | About/PM logo wordmark invisible (`<text>` elements) | done | P1 | â€” | â€” | Fixed 2026-08-14: ThorVG (Godot's SVG rasterizer) has NO font loader â€” `<text>` elements render nothing; that's why zero upstream editor icons use them (all path data). `Logo.svg` + `TitleBarLogo.svg` now embed the wordmark as white **path** letters. `LogoOutlined.svg` deleted (unused). Follow-ups (same day): (1) stroke-outline letters from `logo_outlined.svg` render as hollow rings ("black with white outline") â€” rebuilt as solid glyphs by keeping the outer contour + true counter holes (inset-based discriminator: stroke inner edges hug the outer bbox <15%, counters are â‰¥20% inset; area-ratio heuristics fail on small letters). (2) `Logo.svg` "Engine" (64px) was unreadable as solidified blobs â€” regenerated from the real Arial Bold font via fontTools (`fontTools.pens.svgPathPen`) as true thin glyph outlines with proper counters. (3) balanced group extraction (depth-counting, not regex) required; duplicate nested transforms from rebuilding caused layout shifts â€” final icons are single-wrapper, group-balanced, well-formed XML, render-verified via engine ThorVG (GOBLIN solid x 163-360, Engine thin glyphs x 218-300 with counters) |
| B-15 | Boot splash stale + editor splash missing | done | P1 | â€” | â€” | Fixed 2026-08-14: (1) `main/splash.gen.h` never regenerated on goblin splash.png change (SCons keyed to upstream `#main/splash.png`) â€” added `Depends` edges for splash/splash_editor/app_icon gen headers in `modules/goblin/core/SCsub`. (2) Upstream 4.7 REMOVED the editor splash (commit c283fce698) â€” `no_editor_splash` defaults True + `#main/splash_editor.png` absent forces it. Fork re-enables: config.py strips `NO_EDITOR_SPLASH` from CPPDEFINES, goblin SCsub generates `#main/splash_editor.gen.h` from the goblin splash_editor.png |
| B-16 | Windows exe icon still Godot (goblin.rc was dead code) | done | P1 | â€” | â€” | Fixed 2026-08-14: `modules/goblin/platform/windows/goblin.rc` was referenced nowhere â€” the exe icon came from upstream `godot_res.rc` â†’ `godot.ico`. Now: RES builder wrapped in config.py (`env.AddMethod` shadow) to compile `goblin.rc`/`goblin_res_wrap.rc` instead; `goblin.ico` generated at build time from `app_icon.png` (PNG-compressed ICO, `goblin_ico_builder`); version info strings goblin-branded ("Goblin Engine", https://goblin-engine.org). Verified: res obj contains goblin.ico bytes, upstream godot.ico absent |
| B-13 | About dialog licenses tab: Expat + CC-BY-4.0 bodies empty | done | P1 | â€” | â€” | Fixed 2026-08-14: `modules/goblin/core/COPYRIGHT.txt` standalone licenses had unindented bodies (parser only captures indented continuation lines) â†’ empty `LICENSE_BODIES`. Bodies reformatted to upstream format (leading space, `.` = blank line); CC-BY-4.0 body replaced with full license text |
| B-05 | Add `--max-drift=1 --implicit-deps-unchanged` to default build | todo | P3 | â€” | â€” | Faster incremental builds |
| B-06 | Platform driver trim decision | todo | P3 | â€” | â€” | Keep all platforms; decide on per-platform audio drivers |

---

## 4. Migrate From The From-Scratch Engine

| ID | Item | Status | Priority | Effort | ADR/RFC | Justification |
|----|------|--------|----------|--------|---------|---------------|
| M-02 | Script module tier system (Stable/Tooling/Expert) | todo | P3 | â€” | â€” | Governs which internals GDScript exposes |
| M-03 | Basis-frame transform convention | todo | P3 | â€” | â€” | Eliminates Euler/axis-order ambiguity |
| M-04 | Component family contracts | todo | P3 | â€” | â€” | Basis for struct-based ECS |
| M-05 | Portals & mirrors as **custom nodes** (`PortalSurface3D`/`MirrorSurface3D`), not first-class core | todo | P3 | â€” | â€” | `SubViewport` + teleport + portal-aware queries |
| M-06 | Retro-native rendering as **editor-provided** nodes/plugins (palette, dither, color cycling) | todo | P3 | â€” | â€” | Not core renderer changes |
| M-07 | Generic spatial field system (light + audio + effects probe grid) | todo | P2 | â€” | â€” | The ambient-probe field generalized; drives light/audio/music/effects |
| M-08 | Native stealth shadow value (Thief light gem) | todo | P2 | â€” | â€” | Gameplay readout on top of M-07 field; GDR-009a |
| M-09 | Hitscan surface metadata contract | todo | P3 | â€” | â€” | Raycast -> surface class + object ID + impact UV |
| M-10 | Kinematic brush movers (doors/lifts/crushers) | todo | P3 | â€” | â€” | Generalizes DB `Moving` via `AnimatableBody3D` |
| M-11 | Lightstyle channels + retro surface-class lighting | todo | P3 | â€” | â€” | Style-channel modulation of baked light |
| M-12 | Per-view palette selection and blending | todo | P3 | â€” | â€” | Portal views inherit palette overrides |
| M-13 | Texture-space animation families (UV scroll, frame cycling) | todo | P3 | â€” | â€” | Color-cycling mechanism, in-shader via simulation clock |

*Already adopted by DB (not in backlog):* cadence scheduler with custom process groups (`scheduler.gd`), scene-first composition, partition streaming, delta save/load, Lego-block entity composition. Off-screen simulation was evaluated and dropped â€” the scheduler + event queue already covers the need.

---

## 5. Rejected / Deferred

| Item | Why |
|------|-----|
| `?.` / `??` syntax | Keywords `then`/`elthen` locked (2026-08-13). See G-04/G-05 |
| `then` truthiness semantics (gdscript2 runtime) | Null-only decided at port â€” explicit `!= null` conditions (`.kilo/plans/` Â§3.2) |
| Native `CustomTree` cadence scheduler | Remnant; not necessary |
| `GoblinDataTable` native fallback class | Replaced by template dictionaries (G-18) |
| Replace `core/variant/dictionary.{h,cpp}` | Header override unsupported + max rebase surface; language layer covers it (G-17/G-18) |
| Expose editor `LightmapperRD` at runtime | Editor-only GPU module + forbidden build-flag changes; GL-compat templates lack RenderingDevice |
| GDScript `extends Dictionary` / user Variant types | Variant + ClassDB surgery; G-17/G-18 cover the need at language level |
| Renderer-side light sampling on GL Compatibility | No exposed cluster seams; CPU field (C-05/M-07) substitutes |
| Native upscaler nodes | DB's GL-compat canvas shaders already do this |
| C++ Reaction Server (stimulus Ã— material â†’ action matrix) | Game data + O(1) dict lookup; C++ adds nothing. G-07/G-08 (structs/typed dicts) cover the real gap at language level. Brainstorm 2026-08-13 |
| Background-thread perception grid (AI sensor network) | Determinism/sync nightmare; M-07/M-08 CPU field at cadence is the sanctioned answer (off-screen sim already rejected). Brainstorm 2026-08-13 |
| Palette quantization / color cycling in renderer core | Already decided: M-06/M-13 nodes + shaders, no core renderer changes. Brainstorm 2026-08-13 |
| Driver-level affine texture mapping / PS1 vertex snapping | Shader/material-level trick (vertex snap, LUT post-process), not driver work; do per-asset when a game needs it. Brainstorm 2026-08-13 |
| Raytraced acoustic propagation in `AudioServer` | Portal re-emission + volumetric reverb estimation is research-grade with no DB justification; occlusion value is delivered by C-06 + M-07 zones without mixer surgery. Brainstorm 2026-08-13 |

---

## 6. Tech Debt

Findings from the `/tech-debt-review` workflow. Fork-side debt only; upstream issues are reported to the user, not tracked here.

| ID | Item | Source | Severity | Notes |
|----|------|--------|----------|-------|
| TD-01 | Pre-existing fork test failures | G-17 verification run (2026-08-13) | Resolved | Root causes fixed: (1) `reduce_identifier_from_base` copied the member before `resolve_class_member` (introduced by G-02's @private commit), breaking out-of-order enum/const resolution; (2) @private gaps: private method calls not blocked (`get_function_signature`), cascading "cannot find member" on private inner-class access; (3) stale `.out` files (6 missing required trailing newline, `null_type_assignment` hand-written message); (4) malformed `private_same_script_access` test (accessed non-existent member); (5) harness `res://` bug â€” `ProjectSettings::setup` short-circuits when the resource path is already set (left behind by the GLTF suite), so `init_language` now forces the resource path to the test scripts dir. Full GDScript suite: 1379/1379 test cases pass, 0 failures |
| TD-02 | Missing `then`/`elthen` test suite | Feature review 2026-08-13 | Open | ~25 cases per plan Â§3.5: basic `then`/`elthen`, constant folding, type inference, chaining, null-vs-falsy distinction (`0 elthen 5` â†’ `5` must be pinned), interplay with union types, error cases. Required to close G-04/G-05 |

---

## Dependency Notes

- **G-04/G-05** follow `.kilo/plans/` Â§3 (recovered gdscript2 port map; 5 files, no VM changes).
- **G-18/G-19** follow `.kilo/plans/` Â§2 (spec + phases + test gates).
- **G-07 (structs)** blocks or de-risks G-08 (typed dicts) and M-04 (component families).
- **G-10/G-11** are ported from the gdscript2 module's branches â€” cherry-pick one at a time, never wholesale.
- **C-01** is the highest-priority core fix; it is a known upstream bug and the fix is upstream-acceptable.
- **C-05 (generic field)** is the sampling infrastructure; **M-08 (stealth shadow value)** is the gameplay readout on top of it.
- **M-05 (portals/mirrors)** is a custom-node feature, not a core change â€” cheap to attempt, no engine surgery.
- **Direct core edits** (decided 2026-08-13 brainstorm): allowed only when the swap mechanism cannot reach the change â€” header-only changes included by upstream files, files outside `add_library` source lists (platform/tools), or upstream-acceptable fixes intended for submission (then the rebase stays clean). Everything else goes through mirror+swap (default) or additive module code.
- **Additive feature modules** (ADR 0008, 2026-08-14): live at the repo root in `modules/<name>/` with standard module anatomy and full lifecycle (`MODULE_<NAME>_ENABLED`, `DISABLE_MODULES` gating, own registration/docs/icons/tests) - never inside `modules/goblin/` (goblin is override/branding-only). Current: `modules/midi/` (C-07).
