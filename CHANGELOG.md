# Changelog


## [1.0]

### Added

- Brought back `_fixed_process` from Godot 2 but works differently. Fixed process runs in sync with physics process but can be set on a lower frame rate controlled by `application\run\fixed_process_frames` setting. A value of 1 means it is called on every physics frame. By default is 20 which means it is called every 20 frames. The purpose is to create an alternative physics process for lower latency game logic.
- Added a generic `Visual` category with `Visual Time` in Profiler which tracks rendering time. The timing is not very precise due to OpenGL/Vsync according to Reduz, however, it should help track down major issues.   
- Added `MixinScript` which is a new take on a very old feature of very early Godot Engine before it became open sourced. Was re-added [here](https://github.com/godotengine/godot/pull/8502) and removed again [here](https://github.com/godotengine/godot/pull/8718). MixinScript is MultiScript re-implemented, rebranded and fixed by Xrayez for [Goost Engine](https://github.com/goostengine). I implemented this feature with permission and kept all naming intact for compatibility between Goblin and Goost. My version force calls `_ready()` and `_enter_tree()` for all Mixins which causes the first such functions to call twice (just be aware of this glitch)
- You can now add C style multiline comments in GDScript `/* multi line commment */`. Implemented from a rejected PR found [here](https://github.com/godotengine/godot/pull/18258)
- New `import "<path>"` function for Shader Editor that allows for basic ability to import external shader code into current shader. For Visual Shader Editor use Global Expression node. Adapted from [basic import shader](https://github.com/lyuma/godot/commit/c6b72f1f6632311aa39fe1a01ee7e982f621ed49) by iFire and Lyuma. 
- New `ImageIndexed` class. Added as a custom module under modules/goblin just to keep it separated from the rest of Godot. Makes it easier to merge new 3.x changes back into Goblin. ImageIndexed allows working with pseudo-random indexed images and palettes. This was ported from [Goost Engine](https://github.com/goostengine) and implemented by Xrayez. There is internal documentation as well more documentation on over on [Goost Documentation](https://goost.readthedocs.io/en/latest/classes/class_imageindexed.html?highlight=imageindex) page in the official project.
- New `RandomNumberGenerator` functions (based on code from [Goost Engine](https://github.com/goostengine)):
    * `randshuffle(Array)` shuffles an Array
    * `randchoice(Variant)` picks a random value from an Array or Dictionary or random character from a string
    * `randdecision(double)` helps generates a random decisions based on a probability from `0.0` to `1.0` (0% to 100%)
    * `randroll(count,side)` simulates a random dice roll using count and side and returns an Array with sum at index 0 and all rolls starting with index 1. Count is 1 - 100, sides is 2 - 144. 
- Tabs can now be moved to the bottom and have their own styles. Adapted from Godot 4.0 unmerged PR [#44420](https://github.com/godotengine/godot/pull/44420)

### Changed

- Script debugger now points to Goblin Engine source in the debugger (as the commit hashes for Goblin are different and original source no longer lines up)
- Editor boot splash background color is gray same as editor now and default boot splash background color is black.
- Expose Bullet smooth trimesh collision settings based on a pr found [here](https://github.com/AndreaCatania/godot/commit/2b67feb49cbe32935b53f909f0a8b4f1ec980b17)
- GDscript template has been simplified.
- Tab `hseparation` class doc adjusted to reflect actual function
- Version name has been changed to `Goblin v (Godot v) [patch]`
- GLES2/3, Batching and Async notifications have been moved to verbose
- Almost all logos have been changed to Goblin specific branding

### Removed

- About menu has been simplified and most of the Godot donation and donors have been removed