#include "dascript.h"
#include "core/os/os.h"

ScriptLanguage *DaScriptInstance::get_language() {
	return DaScriptLanguage::get_singleton();
}

Script *DaScriptLanguage::create_script() const {
	return memnew(DaScript);
}

void DaScriptLanguage::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("da");
}