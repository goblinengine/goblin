#include "goblin_string_names.h"

GoblinStringNames *GoblinStringNames::singleton = nullptr;

GoblinStringNames::GoblinStringNames() :
		_create_vertex(StaticCString::create("_create_vertex")),
		_create_edge(StaticCString::create("_create_edge")),
		initialize(StaticCString::create("initialize")),
    	has_next(StaticCString::create("has_next")),
    	next(StaticCString::create("next")),
    	node_spawned(StaticCString::create("node_spawned"))
{}
