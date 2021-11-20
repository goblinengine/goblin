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

#pragma once

#include "../mixin_script.h"

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"

class EditorInspectorPluginMixinScript : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginMixinScript, EditorInspectorPlugin);
	
	Ref<MixinScript> script;
	
	void _on_edit_pressed();
	
protected:
	static void _bind_methods();

public:
	virtual bool can_handle(Object *p_object);
	virtual void parse_begin(Object *p_object);
};

class MixinScriptEditorPlugin : public EditorPlugin {
	GDCLASS(MixinScriptEditorPlugin, EditorPlugin);

public:
	virtual String get_name() const { return "MixinScript"; }

	MixinScriptEditorPlugin(EditorNode *p_node);
};

