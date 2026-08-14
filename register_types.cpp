/**************************************************************************/
/*  register_types.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                            GOBLIN ENGINE                               */
/**************************************************************************/

#include "register_types.h"

#include "core/config/engine.h"
#include "core/object/class_db.h"

#include "midi/midi_register_types.h"

#ifdef TOOLS_ENABLED
#include "editor/branding_translations.h"
#endif

void preregister_goblin_types() {
	// Called before other modules for docgen
}

void initialize_goblin_module(ModuleInitializationLevel p_level) {
	// Project Manager + Editor UI are initialized at EDITOR level.
	// We also initialize at SCENE level to cover non-editor runtime usage.
	// MIDI classes register at SCENE level; importers at EDITOR level.
	initialize_midi_module(p_level);
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR || p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		// Runtime translation overrides rebrand remaining "Godot" strings in the
		// editor UI (shortcut names, tooltips).
		register_branding_translations();
	}
#endif
}

void uninitialize_goblin_module(ModuleInitializationLevel p_level) {
	uninitialize_midi_module(p_level);
	// No per-level cleanup: branding translations are not unregistered (parity
	// with the previous runtime singletons, which never removed them).
}
