# Goblin Engine

<p align="center">
  <a href="https://goblinengine.github.io">
    <img src="logo_outlined.svg" width="400" alt="Goblin Engine logo">
  </a>
</p>

## Description

[Goblin Engine](https://goblinengine.github.io) is a custom build of [Godot Engine](https://godotengine.org) created for educational and personal use. It provides additional functionality not officially supported.

The goal of this project is multifold:
- Make the engine more flexible providing more functionality out of the box
- Implement useful features from scratch or from PRs, Godot forks, modules or adapt GDNative addons into modules so they can be shipped with the engine
- Make minor changes to core without breaking compatibility
- Maintain full compatibility with latest Godot 3.x branch
- All new functionality is lean and compact adding very little overhead

Goblin is compatible with Godot 3.x but implements some custom functionality that Godot does not have. Some of this functionality overlaps with [Goost Engine](https://goostengine.github.io/).

Please have a look at list of [Goblin Changes](https://github.com/goblinengine/goblin/blob/main/CHANGELOG.md) to find out more.

## Builds

Note that at this time Goblin has no official releases since is very new. You would need to compile it from scratch yourself for now or you can download test builds. 

To download test builds look beside the commit hashtag for the green checkmark click that then Details then Summary. 

All test builds there use [thin LTO](http://blog.llvm.org/2016/06/thinlto-scalable-and-incremental-lto.html) which is 90% of the speed/size of a full release, have no debug symbols and provide a release template and an editor if supported for each platform. The builds are created automatically by GitHub in a sterile environment  but have no certificates making OSes complain but are safe to run.

Goblin will not provide any Mono builds since the goal is to keep the engine lean and compact. The focus is primarily on GDScript and GDNative. 


## Community and contributing

There is no community as of yet but PRs welcome.

## Documentation and demos

Since Goblin Engine is compatible with latest 3.x Godot branch, you can find most the documentation you need over at [Godot Docs](https://docs.godotengine.org/en/stable/).

The [class reference](https://docs.godotengine.org/en/latest/classes/) is also accessible from the Godot editor. Goblin adds some new functionality which is only documented in the built in Class references.

The official demos are maintained in their own [GitHub repository](https://github.com/godotengine/godot-demo-projects)
as well.

There are also a number of other
[learning resources](https://docs.godotengine.org/en/stable/community/tutorials.html) such as text and video tutorials, demos, etc.

