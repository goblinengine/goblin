# CUT Upscalers (GL Compatibility 3D Scaling)

Engine-side CUT1/CUT2/CUT3 modes for `scaling_3d_mode` on the Compatibility (GLES3)
renderer. Fragment-only — GLES3 has no compute, so FSR-family upscalers cannot run.

## Modes

| Mode | Passes | Samples | Angle resolution | Soft edges |
|------|--------|---------|------------------|------------|
| `CUT 1` (value 6) | 1 | 4/px | 45° | no |
| `CUT 2` (value 7) | 2 (intermediate at input res) | 12·I + 5·O | 30° | yes (threshold 0.20, sharpening 0.75) |
| `CUT 3` (value 8) | 3 (two intermediates) | 12·I + 4·D·I + 5·O | configurable via D | yes |

Best on stylized/pixel-art content; FSR remains the better photographic upscaler (RD only).
On Forward+/Mobile, CUT modes fall back to FSR 1.0 (warning printed once). MSAA, glow,
SSAO, and tonemapping compose normally.

## Settings (`rendering/scaling_3d/`)

| Key | Default | Notes |
|-----|---------|-------|
| `cut_blend_sharpness` | 0.5 | static blend sharpness (0–1) |
| `cut_edge_min_value` | 0.05 | minimum edge value for triangulation |
| `cut_fast_luma` | false | fast-luma toggle |
| `cut_soft_threshold` | 0.20 | soft-edge blend threshold (CUT 2/3) |
| `cut_sharpening_amount` | 0.75 | soft-edge sharpening (CUT 2/3) |
| `cut_search_min_contrast` | 0.50 | edge-search min contrast (CUT 3) |
| `cut_search_distance` | 4 | edge-search distance D, 1–8 (CUT 3) |

Dynamic-blend internals (min/max contrast, min/max sharpness) are fixed at reference
defaults; not exposed.

## Provenance

Clean-room implementation (ADR 0009): from Su & Willis (2004) and Reshetov (2009) plus the
README description of https://github.com/Swordfish90/cheap-upscaling-triangulation
(GPL-3.0 — no code read or adapted, including its "MIT fork"). Validation black-box
(output comparison only). Spec: `plans/cut-upscalers-plan.md`.

The "CUT" name and the algorithm design are attributed to Matteo (Swordfish90) — used descriptively; no code is derived from his GPL implementation.
