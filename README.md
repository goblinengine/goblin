# Goblin Engine Module

**Transform Godot Engine into your own branded engine without modifying core files!**

## What is Goblin?

Goblin is a Godot module that implements a complete rebranding system. It allows you to:

- ✅ **Change the engine name** from "Godot" to your brand
- ✅ **Replace all icons and graphics** with your custom designs  
- ✅ **Customize credits and about dialog** with your team info
- ✅ **Override documentation URLs** to point to your own docs
- ✅ **Rebrand without touching core files** - easy to update Godot upstream

## How It Works

Goblin hooks into Godot's SCons build system to override generated header files at build time:

```
Godot Core Build → Goblin Module Intercepts → Generates Custom Headers → Compiles Branded Engine
```

The result is a fully branded engine binary that says "Goblin Engine" (or whatever you name it) instead of "Godot Engine" - all without modifying a single line of Godot's core code!

## Quick Start

### 1. Place in modules directory
```bash
# If starting fresh
cd godot/modules/
git clone https://your-repo/goblin.git

# Or symlink from separate repo
ln -s /path/to/goblin-module godot/modules/goblin
```

### 2. Customize branding
Edit these files with your information:
- `branding/AUTHORS.md` - Your team
- `branding/DONORS.md` - Your sponsors  
- `branding/version_override.py` - Engine name and URLs
- `branding/COPYRIGHT.txt` & `LICENSE.txt` - Legal info

### 3. Add custom icons (optional)
```bash
# Replace the main icon
cp your_icon.svg modules/goblin/icons/icon.svg

# Replace the logo
cp your_logo.svg modules/goblin/icons/logo.svg
```

### 4. Build
```bash
cd godot/
scons platform=windows target=editor -j8
```

Your branded engine is now in `bin/` directory!

## Features

### ✨ Complete Branding Override
- Engine name in window title, about dialog, splash screen
- Website URLs in help menus
- Documentation links
- Version strings and build info

### 🎨 Icon & Graphics Replacement  
- Application icon (taskbar/dock)
- Editor logo
- All UI icons can be overridden
- Automatic priority system (your icons > Godot defaults)

### 📝 Credits & Licensing
- Custom authors list
- Sponsor/donor credits
- Copyright information
- License text (must remain MIT-compatible)

### 🔧 Build System Integration
- Zero runtime overhead (all compile-time)
- Works with all Godot platforms
- Compatible with other modules
- Easy to update Godot versions

### 🛠 Developer Tools
- `goblin_manager.py` - Management utility
- Preprocessor defines for conditional code
- Module API for runtime customization
- Detailed documentation

## Directory Structure

```
modules/goblin/
├── branding/
│   ├── AUTHORS.md           # Your project authors
│   ├── DONORS.md            # Your project donors/sponsors
│   ├── COPYRIGHT.txt        # Your copyright information
│   ├── LICENSE.txt          # Your license text
│   └── version_override.py  # Version string overrides
├── icons/                   # Custom editor icons (SVG)
│   ├── icon.svg            # Main application icon
│   ├── logo.svg            # Editor logo
│   └── ...                 # Other editor UI icons you want to override
├── doc_classes/            # Documentation for custom classes (optional)
├── config.py               # Module configuration
├── SCsub                   # Build script with overrides
├── goblin_builders.py      # Custom build functions
├── register_types.cpp      # Module initialization
└── register_types.h        # Module header
```

## Usage

### 1. Customize Branding Files

Edit the files in `modules/goblin/branding/`:

- **AUTHORS.md** - Add your team members
- **DONORS.md** - Add your patrons/sponsors
- **COPYRIGHT.txt** - Update copyright notices
- **LICENSE.txt** - Customize license text (must remain compatible)
- **version_override.py** - Change engine name, website URL

### 2. Add Custom Icons

Place SVG icons in `modules/goblin/icons/`. To replace a Godot icon:

1. Find the icon name in `editor/icons/` (e.g., `MainPlay.svg`)
2. Create your custom version in `modules/goblin/icons/MainPlay.svg`
3. The build system will automatically use your version

Key icons to customize:
- `icon.svg` - Application icon
- `logo.svg` - Editor splash/about logo
- `GuiGraphNodePort.svg`, `PlayStart.svg`, etc. - Editor UI elements

### 3. Build Goblin Engine

```bash
# Standard Godot build with the goblin module enabled
scons platform=windows target=editor

# The goblin module is automatically detected and applied
```

The compiled binary will be branded as **Goblin Engine** with all your customizations.

### 4. Advanced: Runtime Branding

You can also modify branding at runtime in `register_types.cpp`:

```cpp
void initialize_goblin_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        // Your runtime customizations here
        Engine::get_singleton()->set_write_movie_path("goblin");
    }
}
```

## Adding Custom Code

Since this is a full module, you can add custom classes:

1. Create `.cpp`/`.h` files in `modules/goblin/`
2. Register classes in `register_types.cpp`
3. Add documentation in `doc_classes/`
4. They'll be compiled into Goblin Engine automatically

## Build System Details

The module uses these SCons commands to override generation:

- `env.CommandNoCache()` - Rebuilds headers every time
- `env.Run(goblin_builders.xxx)` - Calls custom builder functions
- Source file prioritization - Goblin files processed before Godot files

The build order ensures Goblin-generated files replace Godot-generated files before compilation starts.

## Conditional Compilation

Use these defines in your code:

```cpp
#ifdef GOBLIN_ENGINE
    // Goblin-specific code
#endif

#ifdef GOBLIN_BRANDING_ENABLED
    // Code that uses Goblin branding
#endif
```

## Important Notes

1. **No Core Modifications** - This module doesn't modify any Godot core files
2. **Version Control Friendly** - Keep Godot as a submodule, Goblin as your custom module
3. **License Compliance** - Goblin Engine must retain Godot's MIT license and credit original authors
4. **Updates** - You can update Godot by pulling upstream changes; your Goblin module stays separate

## Troubleshooting

**Icons not changing?**
- Make sure SVG files are valid and same dimensions as originals
- Check file names match exactly (case-sensitive)
- Clear SCons cache: `scons --clean`

**Build errors?**
- Check that `goblin_builders.py` can import `core_builders`
- Verify all branding files exist in `branding/` directory
- Make sure `register_types.cpp` matches the module pattern

**Text still says "Godot"?**
- Some strings are hardcoded in C++ files (not in generated headers)
- You can use the `text_replacer_builder()` function to patch specific files
- Or add runtime string replacement in your module code

## Contributing

This module is designed to be a template. Fork it and customize for your project!

## License

This module itself is MIT licensed. Your Goblin Engine build must comply with:
- Godot Engine's MIT license
- All third-party library licenses used by Godot
- Proper attribution to Godot Engine contributors
