/**************************************************************************/
/*  register_types.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                            GOBLIN ENGINE                               */
/**************************************************************************/

#include "register_types.h"

#include "editor/goblin_about.h"
#include "core/config/engine.h"
#include "core/object/class_db.h"

static GoblinBranding *goblin_branding = nullptr;

void preregister_goblin_types() {
	// Called before other modules for docgen
}

void initialize_goblin_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		// Register Goblin branding class
		GDREGISTER_CLASS(GoblinBranding);

		// Initialize branding overrides
		goblin_branding = memnew(GoblinBranding);
		goblin_branding->initialize();
	}
}

void uninitialize_goblin_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		if (goblin_branding) {
			memdelete(goblin_branding);
			goblin_branding = nullptr;
		}
	}
}
