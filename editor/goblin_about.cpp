/**************************************************************************/
/*  goblin_about.cpp                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                            GOBLIN ENGINE                               */
/**************************************************************************/
/* Copyright (c) 2014-present Goblin Engine contributors.                */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "goblin_about.h"

#include "core/config/engine.h"

#include "core/os/os.h"

#ifdef TOOLS_ENABLED
#include "core/object/object.h"
#include "editor/editor_node.h"
#include "editor/project_manager/project_manager.h"
#include "scene/gui/button.h"
#include "scene/gui/popup_menu.h"
#include "scene/gui/tab_container.h"
#include "scene/main/node.h"
#include "scene/main/scene_tree.h"
#endif

GoblinBranding *GoblinBranding::singleton = nullptr;

GoblinBranding::GoblinBranding() {
	singleton = this;
}

GoblinBranding::~GoblinBranding() {
	singleton = nullptr;
}

void GoblinBranding::initialize() {
	_add_translation_overrides();

#ifdef TOOLS_ENABLED
	_install_ui_tweaks();
#endif
}

void GoblinBranding::_add_translation_overrides() {
	TranslationServer *ts = TranslationServer::get_singleton();
	if (!ts) {
		return;
	}

	Ref<Translation> translation;
	translation.instantiate();
	translation->set_locale("en");

	translation->add_message("Thanks from the Godot community!", "Thanks from the Goblin community!");
	translation->add_message("Godot Engine contributors", "Goblin Engine contributors");
	translation->add_message(
			"Godot Engine relies on a number of third-party free and open source libraries, all compatible with the terms of its MIT license. The following is an exhaustive list of all such third-party components with their respective copyright statements and license terms.",
			"Goblin Engine relies on a number of third-party free and open source libraries, all compatible with the terms of its MIT license. The following is an exhaustive list of all such third-party components with their respective copyright statements and license terms.");

	translation->add_message("About Godot...", "About Goblin...");
	translation->add_message("About Godot", "About Goblin");

	// We still remove the menu item via UI tweaks; keeping this empty avoids showing the original text
	// in case the menu is rebuilt before our hook runs.
	translation->add_message("Support Godot Development", "");

	translation->add_message("Donate", "");
	translation->add_message("Donors", "");

	ts->add_translation(translation);
}

#ifdef TOOLS_ENABLED

void GoblinBranding::_install_ui_tweaks() {
	MainLoop *ml = OS::get_singleton()->get_main_loop();
	SceneTree *tree = Object::cast_to<SceneTree>(ml);
	if (!tree) {
		return;
	}

	if (!tree->is_connected("node_added", callable_mp(this, &GoblinBranding::_on_node_added))) {
		tree->connect("node_added", callable_mp(this, &GoblinBranding::_on_node_added));
	}

	call_deferred(SNAME("_apply_ui_tweaks"));
}

void GoblinBranding::_apply_ui_tweaks() {
	_hide_project_manager_donate();
	_strip_support_menu_item();

	// Also strip donors tab if About dialog already exists.
	MainLoop *ml = OS::get_singleton()->get_main_loop();
	SceneTree *tree = Object::cast_to<SceneTree>(ml);
	if (!tree) {
		return;
	}
	Node *root = tree->get_root();
	if (!root) {
		return;
	}
	_strip_about_donors_tab(root);
}

void GoblinBranding::_on_node_added(Node *p_node) {
	if (!p_node) {
		return;
	}

	_strip_about_donors_tab(p_node);
	_hide_project_manager_donate();
	_strip_support_menu_item();
}

void GoblinBranding::_hide_project_manager_donate() {
	ProjectManager *pm = ProjectManager::get_singleton();
	if (!pm) {
		return;
	}

	// Best-effort: hide any Button labelled "Donate".
	// We avoid touching upstream code by scanning the runtime UI tree.
	Vector<Node *> stack;
	stack.push_back(pm);
	while (!stack.is_empty()) {
		Node *n = stack[stack.size() - 1];
		stack.resize(stack.size() - 1);
		if (!n) {
			continue;
		}

		if (Button *btn = Object::cast_to<Button>(n)) {
			const String text = btn->get_text();
			if (text == "Donate" || text.contains("Donate")) {
				btn->hide();
				btn->set_disabled(true);
			}
		}

		const int child_count = n->get_child_count();
		for (int i = 0; i < child_count; i++) {
			stack.push_back(n->get_child(i));
		}
	}
}

void GoblinBranding::_strip_about_donors_tab(Node *p_node) {
	if (!p_node) {
		return;
	}

	// When an EditorAbout dialog is created, it contains a TabContainer with a ScrollContainer named "Donors".
	Vector<Node *> stack;
	stack.push_back(p_node);
	while (!stack.is_empty()) {
		Node *n = stack[stack.size() - 1];
		stack.resize(stack.size() - 1);
		if (!n) {
			continue;
		}

		if (TabContainer *tc = Object::cast_to<TabContainer>(n)) {
			// Remove any child tab with name "Donors" (or empty after translation override).
			for (int i = tc->get_child_count() - 1; i >= 0; i--) {
				Node *tab = tc->get_child(i);
				if (!tab) {
					continue;
				}
				const String tab_name = tab->get_name();
				if (tab_name == "Donors" || tab_name.contains("Donors")) {
					tc->remove_child(tab);
					tab->queue_free();
				}
			}
		}

		const int child_count = n->get_child_count();
		for (int i = 0; i < child_count; i++) {
			stack.push_back(n->get_child(i));
		}
	}
}

void GoblinBranding::_strip_support_menu_item() {
	EditorNode *en = EditorNode::get_singleton();
	if (!en) {
		return;
	}

	// Scan popup menus and remove the support entry by label.
	Vector<Node *> stack;
	stack.push_back(en);
	while (!stack.is_empty()) {
		Node *n = stack[stack.size() - 1];
		stack.resize(stack.size() - 1);
		if (!n) {
			continue;
		}

		if (PopupMenu *pm = Object::cast_to<PopupMenu>(n)) {
			for (int i = pm->get_item_count() - 1; i >= 0; i--) {
				const String item_text = pm->get_item_text(i);
				if (item_text == "Support Godot Development" || item_text.contains("Support Godot")) {
					pm->remove_item(i);
				}
			}
		}

		const int child_count = n->get_child_count();
		for (int i = 0; i < child_count; i++) {
			stack.push_back(n->get_child(i));
		}
	}
}

#endif // TOOLS_ENABLED
