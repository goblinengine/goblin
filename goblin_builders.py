"""Functions used to rebrand Godot to Goblin during build time"""

import os
import sys
from collections import OrderedDict

# Import core builders to reuse their logic
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "core"))
import core_builders
sys.path.pop(0)

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "editor"))
import editor_icons_builders
sys.path.pop(0)

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", ".."))
import methods
sys.path.pop(0)


def goblin_version_info_builder(target, source, env):
    """Override version info with Goblin branding"""
    # NOTE: This builder may run as a post-action on Godot's original
    # `version_generated.gen.h` target, in which case `source` may only
    # contain the version Value() (no overrides file).
    
    # Start with base version info
    version_data = source[0].read() if source else {}
    
    # Apply Goblin overrides
    version_data["name"] = "Goblin Engine"
    version_data["short_name"] = "Goblin"
    version_data["website"] = "https://goblinengine.github.io/"  # Change this
    
    with methods.generated_wrapper(str(target[0])) as file:
        file.write(
            """\
#define GODOT_VERSION_SHORT_NAME "{short_name}"
#define GODOT_VERSION_NAME "{name}"
#define GODOT_VERSION_MAJOR {major}
#define GODOT_VERSION_MINOR {minor}
#define GODOT_VERSION_PATCH {patch}
#define GODOT_VERSION_STATUS "{status}"
#define GODOT_VERSION_BUILD "{build}"
#define GODOT_VERSION_MODULE_CONFIG "{module_config}"
#define GODOT_VERSION_WEBSITE "{website}"
#define GODOT_VERSION_DOCS_BRANCH "{docs_branch}"
#define GODOT_VERSION_DOCS_URL "https://docs.yourdomain.com/en/" GODOT_VERSION_DOCS_BRANCH

// Goblin Engine specific defines
#define GOBLIN_BRANDING_ENABLED
#define GOBLIN_ENGINE_NAME "Goblin Engine"
#define GOBLIN_ENGINE_SHORT_NAME "Goblin"
#define GOBLIN_COMMUNITY_NAME "Goblin community"
#define GOBLIN_CONTRIBUTORS_NAME "Goblin Engine contributors"

// String replacements for UI text (can be used in editor code)
#ifndef GOBLIN_NO_STRING_OVERRIDE
	// These help replace Godot strings in translatable strings
	#define GODOT_COMMUNITY_OVERRIDE "Goblin community"
	#define GODOT_ENGINE_CONTRIBUTORS_OVERRIDE "Goblin Engine contributors"
#endif
""".format(**version_data)
        )


def goblin_authors_builder(target, source, env):
    """Generate authors with Goblin credits"""
    goblin_authors = os.path.join(os.path.dirname(__file__), "core", "AUTHORS.md")
    return core_builders.make_authors_header(target, [env.File(goblin_authors)], env)


def goblin_donors_builder(target, source, env):
    """Generate donors with Goblin patrons"""
    goblin_donors = os.path.join(os.path.dirname(__file__), "core", "DONORS.md")
    return core_builders.make_donors_header(target, [env.File(goblin_donors)], env)


def goblin_license_builder(target, source, env):
    """Generate license with Goblin copyright"""
    base_dir = os.path.dirname(__file__)
    goblin_copyright = os.path.join(base_dir, "core", "COPYRIGHT.txt")
    goblin_license = os.path.join(base_dir, "core", "LICENSE.txt")
    return core_builders.make_license_header(target, [env.File(goblin_copyright), env.File(goblin_license)], env)


def goblin_splash_builder(target, source, env):
    """Generate splash screen with Goblin branding"""
    goblin_splash = os.path.join(os.path.dirname(__file__), "main", "splash.png")
    buffer = methods.get_buffer(goblin_splash)

    with methods.generated_wrapper(str(target[0])) as file:
        # Use a neutral gray color to better fit various kinds of projects.
        file.write(f"""\
static const Color boot_splash_bg_color = Color(0.14, 0.14, 0.14);
inline constexpr const unsigned char boot_splash_png[] = {{
	{methods.format_buffer(buffer, 1)}
}};
""")


def goblin_splash_editor_builder(target, source, env):
    """Generate editor splash screen with Goblin branding"""
    goblin_splash_editor = os.path.join(os.path.dirname(__file__), "main", "splash_editor.png")
    buffer = methods.get_buffer(goblin_splash_editor)

    with methods.generated_wrapper(str(target[0])) as file:
        # The editor splash background color is taken from the default editor theme's background color.
        # This helps achieve a visually "smoother" transition between the splash screen and the editor.
        file.write(f"""\
static const Color boot_splash_editor_bg_color = Color(0.125, 0.145, 0.192);
inline constexpr const unsigned char boot_splash_editor_png[] = {{
	{methods.format_buffer(buffer, 1)}
}};
""")


def goblin_app_icon_builder(target, source, env):
    """Generate app icon with Goblin branding"""
    goblin_app_icon = os.path.join(os.path.dirname(__file__), "main", "app_icon.png")
    buffer = methods.get_buffer(goblin_app_icon)

    with methods.generated_wrapper(str(target[0])) as file:
        # Use a neutral gray color to better fit various kinds of projects.
        file.write(f"""\
inline constexpr const unsigned char app_icon_png[] = {{
	{methods.format_buffer(buffer, 1)}
}};
""")


def goblin_editor_icons_builder(target, source, env):
    """Override editor icons with Goblin branding
    
    This builder prioritizes Goblin icons over default Godot icons.
    Place your custom icons in modules/goblin/icons/ with the same names
    as the default Godot icons to override them.
    """
    # Filter sources to prioritize goblin icons
    goblin_icons = {}
    godot_icons = []
    
    for src in source:
        src_path = str(src)
        icon_name = os.path.basename(src_path)
        
        if "goblin/icons" in src_path:
            # Goblin icon takes precedence
            goblin_icons[icon_name] = src
        else:
            godot_icons.append(src)
    
    # Build final icon list: Goblin icons override, then remaining Godot icons
    final_sources = list(goblin_icons.values())
    
    for src in godot_icons:
        icon_name = os.path.basename(str(src))
        if icon_name not in goblin_icons:
            final_sources.append(src)
    
    # Use the editor icons builder with our custom source list
    return editor_icons_builders.make_editor_icons_action(target, final_sources, env)


def text_replacer_builder(target, source, env):
    """Post-process generated files to replace text references
    
    This can be used to patch generated C++ files to replace
    "Godot" with "Goblin" in string literals.
    """
    # Read the source file
    with open(str(source[0]), 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Apply replacements (be careful with these!)
    replacements = {
        '"Godot Engine"': '"Goblin Engine"',
        '"godot"': '"goblin"',
        # Add more careful replacements as needed
        # Avoid replacing in code identifiers, only in strings
    }
    
    for old, new in replacements.items():
        content = content.replace(old, new)
    
    # Write to target
    with open(str(target[0]), 'w', encoding='utf-8', newline='\n') as f:
        f.write(content)
