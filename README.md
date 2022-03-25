# Goblin Engine

<p align="center">
  <a href="https://goblinengine.github.io">
    <img src="logo_outlined.svg" width="400" alt="Goblin Engine logo">
  </a>
</p>

## Description

[Goblin Engine](https://goblinengine.github.io) is a custom build of [Godot Engine](https://godotengine.org) created for educational and personal use. It provides additional functionality not officially supported.

The goal of this project is multifold:
- Implement useful features from scratch or from PRs, Godot forks, modules or adapt addons into C++ modules so they can be shipped with Goblin
- Full compatibility with latest Godot 3.x branch
- Minor quality of life changes to core without breaking compatibility
- New features are added to the `goblin` module which can be disabled at any time by passing   `module_goblin_enabled=no`

Here are are some Goblin features that stand out the most:
- `eval()` built in global method that evaluates string expressions
- `Rand` singleton that can be used from anywhere and which provides additional functionality such as ability to roll dice, select a random element from an iterable data structure, generate GUIDs, generate random colors and much more
- Includes `SQLite` module in all editor and server builds
- `MidiPlayer` that allows playing `.midi` files using SoundFont2 `.sf2` sample files
- `IndexedImage` that provides pseudo indexed palette graphics
- Shader `import` functionality
- `Voxel` module for building Minecraft type blocky worlds
- `Thread pool` module that makes working with threads much easier

Please have a look at list of [Goblin Changes](https://github.com/goblinengine/goblin/blob/main/CHANGELOG.md) to find out more details.

## Builds

You can download test builds [from here](https://github.com/goblinengine/goblin/actions).

All editor builds use [thin LTO](http://blog.llvm.org/2016/06/thinlto-scalable-and-incremental-lto.html) and all templates use full lto. Each platform provides a template and editor if supported. The builds are created automatically by GitHub in a sterile environment but are not signed making certain OSes complain.They might not run on M1 macs at all without Gatekeeper disabled.

Goblin will not provide any Mono builds since the goal is to keep the engine lean and compact. The focus is primarily on GDScript and GDNative ecosystem. 


## Community and contributing

There is no community as of yet but PRs welcome. There are a number of features that I am still looking to add: 
- Optimize engine size and speed
- More efficient implementations of custom code
- Custom GDScript functionality
- Porting to more platforms
- Implementing features that are beyond my understanding

## Documentation and demos

Since Goblin Engine is compatible with latest 3.x Godot branch, you can find most the documentation you need over at [Godot Docs](https://docs.godotengine.org/en/stable/). Most of the custom functionality is documented in the built in Class References accessible from the Help menu.

The official demos are maintained in their own [GitHub repository](https://github.com/godotengine/godot-demo-projects). There are also a number of other
[learning resources](https://docs.godotengine.org/en/stable/community/tutorials.html) such as text and video tutorials, demos, etc from the official project.

