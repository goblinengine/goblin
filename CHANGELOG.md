# Changelog


## [1.0]

### New

- Implemented a `PackedScene.instance_add($node)` and `Class.new_add($node, ...)` (experimental features) to quickly add nodes into the scene. The new_add requires more work to get it right and will crash the game if the prameter count is not exact but works perfectly if parameter count is correct. The ideas came from this [rejected pr](https://github.com/godotengine/godot/pull/33974). Both functions return the added scene node.
- Added a new class `MidiPlayer` which plays `.mid` Midi songs using `.sf2` SoundFont2 audio samples. It loads all data as generic MidiFile (for now internal format is not implemented). It plays from memory from the imported data rather than from file which makes it work in the exported game. SoundFont files can be somewhat large so they will make the executable larger. It can also play nodes individually, can silence all notes or specific notes, can pitchwheel, change midi program, use different instruments. All these advanced functions require more in depth understanding about MIDI. 
- Brought back `_fixed_process` from Godot 2 but works differently. Fixed process runs in sync with physics process but can be set on a lower frame rate controlled by `application\run\fixed_process_frames` setting. A value of 1 means it is called on every physics frame. By default is 20 which means it is called every 20 frames. Maximum is set to 400. The purpose is to create an alternative physics process for lower latency game logic. 
- Added a generic `Visual` category with `Visual Time` in Profiler which tracks rendering time. The timing is not very precise due to OpenGL/Vsync according to Reduz in [this](https://github.com/godotengine/godot/pull/19593#issuecomment-398041766) post. However, based on my tests, is still gives pretty good representation of overall visual rendering time and paired with other information (such as scripts) you can find which areas of the code are problematic.
- Added `MixinScript` which is a new take on a very old feature of very early Godot Engine before it became open sourced. Was re-added [here](https://github.com/godotengine/godot/pull/8502) and removed again [here](https://github.com/godotengine/godot/pull/8718). MixinScript is MultiScript re-implemented, rebranded and fixed by Xrayez for [Goost Engine](https://github.com/goostengine). I implemented this feature with permission and kept all naming intact for compatibility between Goblin and Goost. My version force calls `_ready()` and `_enter_tree()` for all Mixins which causes the first such functions to call twice (just be aware of this glitch)
- You can now add C style multiline comments in GDScript `/* multi line commment */`. Implemented from a rejected PR found [here](https://github.com/godotengine/godot/pull/18258)
- New `import "<path>"` function for Shader Editor that allows for basic ability to import external shader code into current shader. For Visual Shader Editor use Global Expression node. Adapted from [basic import shader](https://github.com/lyuma/godot/commit/c6b72f1f6632311aa39fe1a01ee7e982f621ed49) by iFire and Lyuma. The import functionality seems to be able to pull the file from anywhere in the local path but `res://` is recommended. The imported path is highlighted using default string color.
- New `ImageIndexed` class. Added as a custom module under modules/goblin just to keep it separated from the rest of Godot. Makes it easier to merge new 3.x changes back into Goblin. ImageIndexed allows working with pseudo-random indexed images and palettes. This was ported from [Goost Engine](https://github.com/goostengine) and implemented by Xrayez. There is internal documentation as well more documentation on over on [Goost Documentation](https://goost.readthedocs.io/en/latest/classes/class_imageindexed.html?highlight=imageindex) page in the official project.
- New `RandomNumberGenerator` functions (based on code from [Goost Engine](https://github.com/goostengine)):
    * `randshuffle(Array)` shuffles an Array
    * `randchoice(Variant)` picks a random value from an Array or Dictionary or random character from a string
    * `randdecision(double)` helps generates a random decisions based on a probability from `0.0` to `1.0` (0% to 100%)
    * `randroll(count,side)` simulates a random dice roll using count and side and returns an Array with sum at index 0 and all rolls starting with index 1. Count is 1 - 100, sides is 2 - 144. 
- Tabs can now be moved to the bottom and have their own styles. Adapted from Godot 4.0 unmerged PR [#44420](https://github.com/godotengine/godot/pull/44420)

### Changes

- The default environment will use Background Color instead of Procedural Sky which appears to cause crashes when starting the editor on systems that only support GLES2.
- EditorSpinSlider `hide_slide` exported for plugins.
- Editor help has been altered to group methods and parameters by letters. This creates a much easier to read experience for built in docs.
- Changed the Node naming as well as resource File Save to suggest `snake_case` names by default rather than `camelCase` or `CamelCase` as it did previously. This lines up with the official intent for Godot to use snake_case everywhere by default (mentioned all over the documents).
- Parameters `[deps]` and `[params]` from `.import` files are no longer saved in the exported game. They are never used by exported game and use unecessary processing power, memory and space in the `.pck`. 
- The workflows have been slightly altered:
    * Editor + template are now created for Linux X11, Windows, MacOS. 
    * All Linux builds automatically strip the debug symbols now. 
    * All builds (except iOS and JavaScript) use thinlto now. 
    * Mono test builds have been removed. Goblin will not provide mono builds. Mono should work is just too much trouble to focus both on regular and Mono. Goblin aims to stay lightweight and flexible focusing primarily on GDScript and GDNative. Other languages via GDNative should work fine.
    * The static tests have been slightly altered to accomodate Goblin branding
- Editor Settings Display Scale auto suggestions will now suggest scaling relative to 96 dpi (which is usually the recommended safe dpi). 
- Script debugger now points to Goblin Engine source in the debugger (as the commit hashes for Goblin are different and original source and no longer lines up)
- Editor boot splash background color is gray same as editor now and default boot splash background color is black.
- Expose Bullet smooth trimesh collision settings based on a pr found [here](https://github.com/AndreaCatania/godot/commit/2b67feb49cbe32935b53f909f0a8b4f1ec980b17). May cause lag.
- GDscript built in template has been simplified.
- Tab `hseparation` class doc adjusted to reflect actual function
- Version naming is now `Goblin v (Godot v) [Goblin patch]`
- GLES2/3, Batching and Async notifications have been moved to verbose
- Almost all logos have been changed to Goblin specific branding

### Removed

- About menu has been simplified and most of the Godot donation and donors have been removed