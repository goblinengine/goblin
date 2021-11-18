# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.0] - 2021-11-09

### Added

- New `import "<path>"` function for Shader Editor that allows for basic ability to import external shader code into current shader. For Visual Shader Editor use Global Expression node. Adapted from [basic import shader](https://github.com/lyuma/godot/commit/c6b72f1f6632311aa39fe1a01ee7e982f621ed49) by iFire and Lyuma. 
- New `RandomNumberGenerator` functions (based on code from [Goost Engine](https://github.com/goostengine)):
    * `randshuffle(Array)` which shuffles an Array
    * `randchoice(Variant)` which picks a random value from an Array or Dictionary or random character from a string
    * `randdecision(double)` which helps generates a random decisions based on a probability from `0.0` to `1.0` (0% to 100%)
    * `randroll(count,side)` simulates a random dice roll using count and side and returns an Array with sum at index 0 and all rolls starting with index 1. Count is 1 - 100, sides is 2 - 144. 
- New `ImageIndexed` class added as a custom module under modules/goblin just to keep it separated from the rest of Godot. Makes it easier to new 3.x changes back into Goblin. ImageIndexed allows for working with indexed images and palettes. This was imported from [Goost Engine](https://github.com/goostengine) and implemented by Xrayez. There is internal documentation as well more documentation on over on [Goost Documentation](https://goost.readthedocs.io/en/latest/classes/class_imageindexed.html?highlight=imageindex) page in the official project.
- Tabs can now be moved to the bottom and have their own styles. Adapted from Godot 4.0 unmerged PR [#44420](https://github.com/godotengine/godot/pull/44420)

### Changed

- Tab `hseparation` class doc adjusted to reflect actual function
- Version name has been changed to `Goblin v (Godot v) [patch]`
- Almost all logos have been changed to Goblin specific branding
- GLES2/3 and Batching notifications when running a project have been moved to verbose

### Removed

- About menu has been simplified and most of the Godot donation and donors have been removed