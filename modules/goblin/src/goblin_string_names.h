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

#include "core/string_name.h"

namespace goost {
void register_core_types();
void unregister_core_types();
} // namespace goost

class GoblinStringNames {
	friend void goost::register_core_types();
	friend void goost::unregister_core_types();

	static void create() { singleton = memnew(GoblinStringNames); }
	static void free() {
		memdelete(singleton);
		singleton = nullptr;
	}
	GoblinStringNames();

public:
	_FORCE_INLINE_ static GoblinStringNames *get_singleton() { return singleton; }
	static GoblinStringNames *singleton;

	StringName _create_vertex;
	StringName _create_edge;
	StringName initialize;
	StringName has_next;
	StringName next;
	StringName node_spawned;
};
