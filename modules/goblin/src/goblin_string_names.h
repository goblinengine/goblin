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
