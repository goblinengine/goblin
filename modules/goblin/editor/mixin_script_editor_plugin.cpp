/* 
Copyright (c) 2019-2021 Andrii Doroshenko.
Copyright (c) 2020-2021 Goost contributors (cf. AUTHORS.md).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "mixin_script_editor_plugin.h"

#include "editor/plugins/script_editor_plugin.h"

bool EditorInspectorPluginMixinScript::can_handle(Object *p_object) {
	return Object::cast_to<MixinScript>(p_object) != nullptr;
}

void EditorInspectorPluginMixinScript::parse_begin(Object *p_object) {
	MixinScript *ms = Object::cast_to<MixinScript>(p_object);
	if (!ms) {
		return;
	}
	script = Ref<MixinScript>(ms);

	Button *edit = memnew(Button);
	edit->set_text(TTR("Edit"));
	edit->set_icon(EditorNode::get_singleton()->get_gui_base()->get_icon("Edit", "EditorIcons"));
	edit->connect("pressed", this, "_on_edit_pressed");

	add_custom_control(edit);
}

void EditorInspectorPluginMixinScript::_on_edit_pressed() {
	ERR_FAIL_COND(script.is_null());
	
	//ScriptEditor::get_singleton()->set_meta("_edit_mixin", true);
	ScriptEditor::get_singleton()->edit(script);
}

void EditorInspectorPluginMixinScript::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_edit_pressed"), &EditorInspectorPluginMixinScript::_on_edit_pressed);
}

MixinScriptEditorPlugin::MixinScriptEditorPlugin(EditorNode *p_node) {
	Ref<EditorInspectorPluginMixinScript> inspector_plugin;
	inspector_plugin.instance();
	add_inspector_plugin(inspector_plugin);
}
