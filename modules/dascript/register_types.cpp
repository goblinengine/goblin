#include "register_types.h"
#include "core/class_db.h"
#include "dascript.h"
//#include "wren_manager.h"

static DaScriptLanguage *script_da_script = nullptr;

void register_dascript_types() {
	// Da Script
	script_da_script = memnew(DaScriptLanguage);
	ScriptServer::register_language(script_da_script);
	ClassDB::register_class<DaScript>();
}

void unregister_dascript_types() {
   // Nothing to do here in this example.
}