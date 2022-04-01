# Changelog


## 1.0

### Additions

- Added `remove_children()` function to nodes which removes all children of a node
- Added `UPPER_SNAKE_CASE, kebab-case, UPPER-KEBAB-CASE` to Node Name Casing in Project Settings.
- Added [Voxelman](https://github.com/Relintai/voxelman) and [Thread pool](https://github.com/Relintai/thread_pool) modules by [Relintai](https://github.com/Relintai). The docs still require a bit of cleanup.
- Added `tightness` to `AudioStreamPlayer3D` which controls how tight the sound playing encloses the player. Based on an unmerged 4.0 pr found [here](https://github.com/godotengine/godot/pull/42358).
- Implemented `Array` variant `for_each(obj, "function")` function which calls a function for each element of the array. The function can be any name but must have only 1 parameter to pass each element to. This is slighty faster than a for loop with 1 function and much faster than using `call()` in a for loop. Every function inside a for loop will slow down the loop by x6 where as using `call()` slows down by x14 and the effect is **cummulative!**. The idea is to use only 1 function per element to make looping faster.
- New `Rand` singleton that allows generating completely random values from anywhere. `Rand` auto randomizes every time the engine starts and since it extends `RandomNumberGenerator` you can also call `randomize()` or `set_seed()` manually.
    * `shuffle(Array)` shuffles an Array
    * `pop(Array)` removes and returns a random element from a sequence
    * `choice(Variant)` picks a random value from an Array or Dictionary or random character from a string
    * `choices(Array, count, weight, cummulative)` picks count number of random values from sequence based on weights
    * `color()` generates a completely random color
    * `decision(double)` helps generates a random decisions based on a probability from `0.0` to `1.0` (0% to 100%)
    * `roll(count, side)` simulates a random dice roll using count and side and returns an Dictionary with sum and rolls. 
    * `roll_notation(dice_notation`) similar to a roll except it uses dice notation such as `2d6`, `2x(3d6!U)`. Als returns a Dictionary but contains additional information.
    * `i(from, to)` same as `rand_range` but shorter and slightly faster (defaults `0 -> 99`)
    * `f(from, to)` same as `randf_range` but shorter (default `0.0 -> 1.0`)
    * `uuid()` generated a random UUID v4 string 8-4-4-4-12 (also known a GUID).
- Added autotile auto-transforms pr found [here](https://github.com/godotengine/godot/pull/39046) to Goblin. The proposal is [here](https://github.com/godotengine/godot-proposals/issues/893). The idea here is to allow specific transforms on autotiles so that when looking up a specific bitmask the autotile is esentially transformed dynamically, based on allowed transformations. Allows for less manual tile work in some situations and smaller texture file. The drawback is the tiles resulting from transforms are repetitive. 
- Added `eval("expression")` function in `@GDScript` which parses a string expression and outputs the result or null if couldn't parse. It does not take inputs like Expression but can be added since it actually uses Expression class in the backend. This is a common function in many interpeted languages. 
- Added [SQLite Module](https://github.com/godot-extended-libraries/godot-sqlite/tree/3.2) by K. S. Ernest (iFire) Lee (fire). By default it is disabled. Use `module_sqlite_enabled=yes` to build. In test builds is enabled everywhere except for template builds.
- Maximum number of culled lights, instances and reflection probes have been exposed to the Project Settings. This was implemented from this [rejected PR](https://github.com/godotengine/godot/pull/35447). Is already implemented in Godot 4. I have tested with 1 million meshes and although it is slow, it has no side effects. Note that GLES2 will crash with more than 32k instances (this is a built in limitation of GLES2 renderer) where as GLES3 can render any number but it will become exponentially slower after about 64k instances. The octree used in Godot is not very efficient and is not able to handle too many instances and would require core changes to make it more robust. There are still internal hard limits to minimum and maximum to prevent crashes but are much higher. The new settings are `rendering/limits/culling/max_instance_cull`, `rendering/limits/culling/max_lights_culled`, and `rendering/limits/culling/max_reflection_probes_culled`. Manually adjusting these values down should help game performance.
- You can now unify Focus and Hover behavior for Buttons that have keyboard focus enabled. A Project Setting option under `gui/theme/unify_button_focus_hover` will turn this on and off. This brings back old behavior where there was only one focus shared by mouse and keyboard before focus was split off into `mouse_hover` and `focus` (keyboard only now). With this change hovering will force keyboard focus (if control can be keyboard foucused). This can be further fine tuned later but works as is.
- External editor now has additional external editor presets for different editors. This is based on an older PR found [here](https://github.com/godotengine/godot/pull/42736).
- Implemented a `PackedScene.instance_add($node)` and `Class.new_add($node, ...)` (experimental features) to quickly add nodes into the scene. The node `new_add` requires more work to get it right and will crash the game if the prameter count is not exact but works perfectly if parameter count is correct. This is because the `new_add` function signature is not properly detected. The ideas came from this [rejected pr](https://github.com/godotengine/godot/pull/33974). Both functions return the added scene node.
- Added a new class `MidiPlayer` which plays `.mid` or `.midi` songs using `.sf2` SoundFont audio samples. It loads all data as generic MidiFile but sets internal format to `FORMAT_SF2` or `FORMAT_MIDI` depending on the file extension. Format check is based strictly on file extensions. Only the specific formats are allowed by the MidiPlayer. MidiPlayer plays data from memory and requires an import. A possible change can be made to load sf2 and midi data from extenal files (the code implementation for this does exist). SoundFont files can be somewhat large so they will make the .pck slightly bigger but midi files are tiny. There is additional API to play individual notes, to silence notes, to change instrument, to change midi program and much more. Reverb and chorus have not been implemented yet. It should be possible to pass these values from a InputEventMIDI.
- Added a generic `Visual` category with `Visual Time` in Profiler which tracks rendering time. The timing is not very precise due to OpenGL/Vsync according to Reduz in [this](https://github.com/godotengine/godot/pull/19593#issuecomment-398041766) post. However, based on my tests, is still gives pretty good representation of overall visual rendering time and paired with other information (such as scripts) you can find which areas of the code are problematic.
- Added `MixinScript` which is a new take on a very old feature of very early Godot Engine before it became open sourced. Was re-added [here](https://github.com/godotengine/godot/pull/8502) and removed again [here](https://github.com/godotengine/godot/pull/8718). MixinScript is MultiScript re-implemented, rebranded and fixed by Xrayez for [Goost Engine](https://github.com/goostengine). I implemented this feature with permission and kept all naming intact for compatibility between Goblin and Goost. My version force calls `_ready()` and `_enter_tree()` for all Mixins. Keyword `onready` now works for all Mixins.
- New `import "<path>"` function for Shader Editor that allows for basic ability to import external shader code into current shader. For Visual Shader Editor use Global Expression node. Adapted from [basic import shader](https://github.com/lyuma/godot/commit/c6b72f1f6632311aa39fe1a01ee7e982f621ed49) by iFire and Lyuma. The import functionality seems to be able to pull the file from anywhere in the local path but `res://` is recommended. The imported path is highlighted using default string color.
- New `ImageIndexed` class. Added as a custom module under modules/goblin just to keep it separated from the rest of Godot. Makes it easier to merge new 3.x changes back into Goblin. ImageIndexed allows working with pseudo-random indexed images and palettes. This was ported from [Goost Engine](https://github.com/goostengine) and implemented by Xrayez. There is internal documentation as well more documentation on over on [Goost Documentation](https://goost.readthedocs.io/en/latest/classes/class_imageindexed.html?highlight=imageindex) page in the official project.
- Tabs can now be moved to the bottom and have their own styles. Adapted from Godot 4.0 unmerged PR [#44420](https://github.com/godotengine/godot/pull/44420)

### Changes

- Output panel now has a verbose toggle
- ScriptEditor Run is now set to F4 by default and will now automatically open the Output panel and prints a message at the start of a run (so you can differentiate it from previous runs)
- AssetLib is now cleared when changing repo
- Goblin Output console now supports BBCode straight from `print()` functions. This allows for colorized text output, url, images and much more. Note that image tags do read from drive so be weary. Additionally urls such as `https://` or `res://` or c++ source code now are clickable which will either open the file if internal resource or will ask the OS to open them if external. This was inspired by two not yet approved prs found [here](https://github.com/godotengine/godot/pull/57896) and [here](https://github.com/godotengine/godot/pull/33541). BBCode is removed using RegEx from console output (it actually removes anything that matches `[a-z0-9=#./]` but only in the console). 
- Implemented custom Godot types in JSON adapting [this](https://github.com/godotengine/godot/pull/33241) rejected pr. All unsupported types will turn into their string equivalent for example Vector2(1,1) will be saved as "Vector2( 1, 1 )" which is parsed back into a Vector2. However, since JSON only has a number type, ints will still be converted to floats. 
- Imported 3D scenes are disabled from saving to a scene to avoid errors. From [this pr](https://github.com/godotengine/godot/pull/42367).
- Displays "use onready var" error message when using `get_child()` and `get_parent()` without `onready`. Adapted to 3.x from [this pr](https://github.com/godotengine/godot/pull/36889)
- Control `get_global_rect()` now returns correct rect from [this pr](https://github.com/godotengine/godot-proposals/issues/811)
- Added `blend_premul_alpha` to scene shaders adapted from [this PR](https://github.com/godotengine/godot/pull/36747)
- Visual Script nodes now move with comment node. Adapted from [this pr](https://github.com/godotengine/godot/pull/54970)
- TSCN Text Scenes (tscn) are now converted to binary scn upon export. Optionally via `"filesystem/on_save/compress_binary_resources"`. This did not work previously. Adapted from a pr found [here](https://github.com/godotengine/godot/pull/51096).
- OS.execute() has been changed on Windows to use CMD.exe by default so that it behaves similar to other operating systems. This change was also added to Script Editor Plugin when opening external editors. OS.execute is poorly implemented on Windows and causes some internal commands to not work properly. I suspect the reason exported icons never change is because it fails running rcedit.
- Fixed Signed Distance Field for Bitmap Fonts. This has been an open issue in Godot 3 since 2018 see [this post](https://github.com/godotengine/godot/issues/8022). It has been fixed in Godot 4 but not in 3. Had to completely re-implement it in GLES2 based on older Godot 2.1 code, fix it for GLES3 and manually add it to some of the controls. Before it only worked for Label and only in GLES3 and that's it. Now it works in GLES2 and GLES3 but only for the specific controls. The controls which have it enabled are Button, Label, RichTextLabel, OptionButton, ItemList, ProgressBar, LineEdit, Tabs and any control that extends or implements those controls.
- ViewportTexture now calls updating deferred removing all the missing Viewport spam
- Layer buttons now have the option to remove the text label making them smaller. The option is found in Editor setting under `interface/inspector/layer_labels`
- Clarified `AudioEffectCapture`and `hseparation``Tab` property docs
- Editor boot splash background color is gray and default boot splash background color is black. Boot splash logo is now disabled by default (upstream 3.x Godot change). Can be enabled with `boot_splash_logo=yes`.
- GDScript Plugins will now fallback to X11 if no Server plugins found. This may fail if Server is not on Linux.
- When selecting an option from OptionButton it now selects the correct index (rather than showing -1)
- Docs properties and methods are grouped alphabetically making it easier to find things
- Inspector default small reload button is now a small star instead which is less intrusive than the original circular arrow button
- FileDialog now hides the `.import` folder
- Node names and File Dialogue file name will use `snake_case` by default rather than `camelCase` or `CamelCase` as it did previously. This lines up with the official intent for Godot to use snake_case everywhere by default (mentioned all over the documents).
- The default environment will use Background Color instead of Procedural Sky which appears to cause crashes when starting the editor on systems that only support GLES2
- The workflows have been altered:
    * Editor + template are now created for Linux X11, Windows, MacOS
    * All Linux builds automatically strip the debug symbols now
    * All builds (except iOS and JavaScript) use thinlto now
    * All builds use llvm 
    * Mono test builds have been removed. Goblin will not provide mono builds. Mono should work is just too much trouble to focus both on regular and Mono. Goblin aims to stay lightweight and flexible focusing primarily on GDScript and GDNative. Other languages via GDNative should work fine.
    * All static checks have been removed as they were a nuissance
    * SQLite builds automatically on Linux server and headless and Editor builds
- Editor's auto scaling option now suggests scaling relative to 96 dpi (which is usually the recommended safe dpi).
- Script debugger now points to Goblin Engine source in the debugger (as the commit hashes for Goblin are different and original source and no longer lines up)
- Expose Bullet smooth trimesh collision settings based on a pr found [here](https://github.com/AndreaCatania/godot/commit/2b67feb49cbe32935b53f909f0a8b4f1ec980b17). May cause lag.
- GDscript default script template has been simplified.
- Version naming is now `Goblin v (Godot v) [Goblin patch]`
- GLES2/3, Batching and Async notifications have been moved to verbose
- Almost all logos have been changed to Goblin specific branding

### Deletions

- Removed rendundant thread sync `draw_pending` function. See [this pr](https://github.com/godotengine/godot/pull/35758)
- Parameters `[deps]` and `[params]` from `.import` files are no longer saved in the exported game. They are never used by exported game and use unecessary processing, memory and space in the `.pck`. Based on [this pr](https://github.com/godotengine/godot/pull/42441).
- Deprecated `enabled_focus_mode` has been completely removed from user facing code
- Deprecated enums have been removed from MultiplayerAPI and from MultiplayerAPI docs so it no longer shows in user facing code.
- About menu has been simplified and most of the Godot donation and donors have been removed