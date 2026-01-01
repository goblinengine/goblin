# Goblin Engine Module - Documentation Index

Welcome to the Goblin Engine module documentation! This module allows you to completely rebrand Godot Engine without modifying any core files.

## 📚 Documentation Structure

### Start Here
1. **[README.md](README.md)** - Project overview and quick start guide
   - What is Goblin?
   - Quick start in 4 steps
   - Feature list
   - Platform support

### Quick Reference
2. **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** - Commands and common tasks
   - File structure
   - Build commands
   - Common tasks
   - Troubleshooting

### Deep Dive
3. **[GOBLIN_BUILD_GUIDE.md](GOBLIN_BUILD_GUIDE.md)** - Technical deep dive
   - How the build system works
   - SCons architecture
   - Builder functions explained
   - Advanced customization

4. **[ARCHITECTURE_DIAGRAMS.md](ARCHITECTURE_DIAGRAMS.md)** - Visual guides
   - System architecture diagrams
   - Build process flowcharts
   - Directory structure
   - Data flow diagrams

5. **[IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md)** - Complete overview
   - What was created
   - System architecture
   - Technical flow
   - Use cases and examples

## 🚀 Getting Started Path

### For First-Time Users
```
1. Read: README.md (Overview)
   ↓
2. Customize: branding/ files
   ↓
3. Run: goblin_manager.py check
   ↓
4. Build: scons platform=windows target=editor
   ↓
5. Refer to: QUICK_REFERENCE.md (as needed)
```

### For Advanced Users
```
1. Skim: README.md
   ↓
2. Study: GOBLIN_BUILD_GUIDE.md
   ↓
3. Review: ARCHITECTURE_DIAGRAMS.md
   ↓
4. Implement: Custom modifications
   ↓
5. Refer to: IMPLEMENTATION_SUMMARY.md
```

### For Build System Developers
```
1. Read: GOBLIN_BUILD_GUIDE.md
   ↓
2. Study: SCsub and goblin_builders.py
   ↓
3. Visualize: ARCHITECTURE_DIAGRAMS.md
   ↓
4. Reference: IMPLEMENTATION_SUMMARY.md
   ↓
5. Modify: Build system for your needs
```

## 📖 Documentation by Topic

### Branding
- **How to change engine name**: QUICK_REFERENCE.md → "Change Engine Name"
- **How to add custom icons**: README.md → "Add custom icons"
- **Where branding appears**: IMPLEMENTATION_SUMMARY.md → "What Gets Overridden"

### Build System
- **How overrides work**: GOBLIN_BUILD_GUIDE.md → "How Overriding Works"
- **Build process flow**: ARCHITECTURE_DIAGRAMS.md → "Build Process Flow"
- **SCons commands**: GOBLIN_BUILD_GUIDE.md → "Key Files and Their Roles"

### Icons & Graphics
- **Icon priority system**: GOBLIN_BUILD_GUIDE.md → "Icon Override Deep Dive"
- **Icon requirements**: QUICK_REFERENCE.md → "Icon Requirements"
- **Visual flow**: ARCHITECTURE_DIAGRAMS.md → "Icon Override System"

### Troubleshooting
- **Common issues**: QUICK_REFERENCE.md → "Troubleshooting"
- **Build errors**: GOBLIN_BUILD_GUIDE.md → "Troubleshooting Build Issues"
- **Cache problems**: QUICK_REFERENCE.md → "Build Fails After Godot Update"

### Advanced Topics
- **Adding custom classes**: QUICK_REFERENCE.md → "Advanced Customization"
- **File patching**: GOBLIN_BUILD_GUIDE.md → "File Patching (Advanced)"
- **Conditional compilation**: QUICK_REFERENCE.md → "Preprocessor Defines"
- **Runtime customization**: IMPLEMENTATION_SUMMARY.md → "Runtime Customization"

## 🛠 Utility Scripts

### goblin_manager.py
Management utility for Goblin branding:

```bash
# Check setup
python modules/goblin/goblin_manager.py check

# List icons
python modules/goblin/goblin_manager.py icons

# Compare with Godot
python modules/goblin/goblin_manager.py compare

# Full diagnostic
python modules/goblin/goblin_manager.py all

# Test build
python modules/goblin/goblin_manager.py build

# Clean build artifacts
python modules/goblin/goblin_manager.py clean
```

See: `python modules/goblin/goblin_manager.py help`

## 📁 File Reference

### Core Files
| File | Description | Documentation |
|------|-------------|---------------|
| `config.py` | Module configuration | GOBLIN_BUILD_GUIDE.md |
| `SCsub` | Main build script | GOBLIN_BUILD_GUIDE.md |
| `goblin_builders.py` | Build functions | GOBLIN_BUILD_GUIDE.md |
| `register_types.cpp/h` | Module code | QUICK_REFERENCE.md |
| `goblin_manager.py` | Utility script | README.md |

### Branding Files
| File | Description | Documentation |
|------|-------------|---------------|
| `branding/AUTHORS.md` | Your team | README.md |
| `branding/DONORS.md` | Your sponsors | README.md |
| `branding/COPYRIGHT.txt` | Copyright info | README.md |
| `branding/LICENSE.txt` | License text | README.md |
| `branding/version_override.py` | Version strings | QUICK_REFERENCE.md |

### Documentation Files
| File | Description |
|------|-------------|
| `README.md` | Overview and quick start |
| `QUICK_REFERENCE.md` | Commands and tasks |
| `GOBLIN_BUILD_GUIDE.md` | Technical deep dive |
| `ARCHITECTURE_DIAGRAMS.md` | Visual guides |
| `IMPLEMENTATION_SUMMARY.md` | Complete overview |
| `INDEX.md` | This file |

### Example Files
| File | Description |
|------|-------------|
| `.github-workflow-example.yml` | CI/CD workflow |
| `.gitignore` | Git ignore rules |

## 🎯 Common Tasks Quick Links

### Setup & Configuration
- [Initial setup](README.md#quick-start)
- [Customize branding](README.md#2-customize-branding)
- [Add custom icons](README.md#3-add-custom-icons)
- [Verify setup](QUICK_REFERENCE.md#check-setup)

### Building
- [Build commands](QUICK_REFERENCE.md#build-commands)
- [Clean build](QUICK_REFERENCE.md#clean-build)
- [Platform-specific builds](QUICK_REFERENCE.md#platform-specific-notes)
- [CI/CD integration](.github-workflow-example.yml)

### Customization
- [Change engine name](QUICK_REFERENCE.md#change-engine-name)
- [Override icons](QUICK_REFERENCE.md#override-specific-icon)
- [Add custom classes](QUICK_REFERENCE.md#add-runtime-customization)
- [Patch files](GOBLIN_BUILD_GUIDE.md#file-patching-advanced)

### Maintenance
- [Update Godot](IMPLEMENTATION_SUMMARY.md#updating-godot-base-version)
- [Add new overrides](GOBLIN_BUILD_GUIDE.md#adding-more-overrides)
- [Troubleshoot issues](QUICK_REFERENCE.md#troubleshooting)

## 💡 Learning Paths

### Path 1: Marketer/Designer
**Goal:** Rebrand the engine with new name and graphics

1. Read: README.md (overview)
2. Edit: `branding/` files with your info
3. Create: Custom icons in `icons/` directory
4. Run: `python goblin_manager.py check`
5. Build: `scons platform=windows target=editor`
6. Refer: QUICK_REFERENCE.md (for commands)

**Time:** 1-2 hours

### Path 2: Developer/Engineer
**Goal:** Understand and customize the build system

1. Read: README.md (overview)
2. Study: GOBLIN_BUILD_GUIDE.md (build system)
3. Review: `SCsub` and `goblin_builders.py` files
4. Visualize: ARCHITECTURE_DIAGRAMS.md
5. Experiment: Add custom overrides
6. Reference: IMPLEMENTATION_SUMMARY.md

**Time:** 4-6 hours

### Path 3: Build Engineer
**Goal:** Master the system and extend it

1. Read: All documentation files
2. Study: Godot's SCons build system
3. Analyze: `methods.py`, `core_builders.py`
4. Trace: Build process with debug output
5. Implement: Complex customizations
6. Contribute: Improvements to Goblin

**Time:** 8-16 hours

## 🔍 Finding Information

### By Question Type

**"How do I..."**
- Start with: QUICK_REFERENCE.md
- Then check: README.md

**"Why does it..."**
- Start with: GOBLIN_BUILD_GUIDE.md
- Then check: ARCHITECTURE_DIAGRAMS.md

**"What is..."**
- Start with: README.md
- Then check: IMPLEMENTATION_SUMMARY.md

**"Where is..."**
- Start with: INDEX.md (this file)
- Then check: ARCHITECTURE_DIAGRAMS.md

### By Error Message

**"Build fails"**
- Check: QUICK_REFERENCE.md → Troubleshooting
- Then: GOBLIN_BUILD_GUIDE.md → Troubleshooting Build Issues

**"Icons don't change"**
- Check: QUICK_REFERENCE.md → Icons Not Changing
- Then: GOBLIN_BUILD_GUIDE.md → Icon Override Deep Dive

**"Text still says Godot"**
- Check: QUICK_REFERENCE.md → Text Still Says "Godot"
- Then: GOBLIN_BUILD_GUIDE.md → File Patching

**"Python import errors"**
- Check: QUICK_REFERENCE.md → Build Errors
- Then: GOBLIN_BUILD_GUIDE.md → Troubleshooting Build Issues

## 📊 Documentation Statistics

| Document | Lines | Words | Reading Time |
|----------|-------|-------|--------------|
| README.md | ~300 | ~1500 | 7 min |
| QUICK_REFERENCE.md | ~500 | ~2500 | 12 min |
| GOBLIN_BUILD_GUIDE.md | ~800 | ~4000 | 20 min |
| ARCHITECTURE_DIAGRAMS.md | ~600 | ~2000 | 10 min |
| IMPLEMENTATION_SUMMARY.md | ~700 | ~3500 | 17 min |
| **Total** | ~2900 | ~13500 | ~66 min |

## 🎓 Educational Value

This module demonstrates:
- ✅ Advanced SCons build system techniques
- ✅ Non-invasive engine customization
- ✅ Clean separation of concerns
- ✅ Build-time code generation
- ✅ Priority-based resource override
- ✅ Module system best practices
- ✅ License compliance strategies

## 🤝 Contributing

To contribute documentation:
1. Choose a topic that needs clarification
2. Edit the appropriate markdown file
3. Test your instructions
4. Submit a pull request
5. Update this INDEX.md if needed

## 📜 License

All documentation is provided under the MIT License, same as Godot Engine and the Goblin module.

## 🔗 External Resources

- **Godot Engine**: https://godotengine.org
- **Godot Build Docs**: https://docs.godotengine.org/en/stable/contributing/development/compiling/
- **SCons Documentation**: https://scons.org/documentation.html
- **Godot Source**: https://github.com/godotengine/godot

## ❓ Still Have Questions?

1. Search this documentation for keywords
2. Check the relevant troubleshooting section
3. Review the architecture diagrams
4. Examine the source code with comments
5. Ask in your project's discussions

---

## Quick Navigation

**New Users:** [README.md](README.md) → [QUICK_REFERENCE.md](QUICK_REFERENCE.md)

**Developers:** [GOBLIN_BUILD_GUIDE.md](GOBLIN_BUILD_GUIDE.md) → [ARCHITECTURE_DIAGRAMS.md](ARCHITECTURE_DIAGRAMS.md)

**Reference:** [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md) → [QUICK_REFERENCE.md](QUICK_REFERENCE.md)

---

**Last Updated:** 2025-01-01  
**Version:** 1.0.0  
**Godot Compatibility:** 4.0+

Happy branding! 🎨
