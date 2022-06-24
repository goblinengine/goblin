/* register_types.cpp */

#include "register_types.h"
#include "core/class_db.h"
#include "core/script_language.h"
#include "editor/editor_node.h"
#include "editor/mixin_script_editor.h"
#include "editor/mixin_script_editor_plugin.h"
#include "io/image_loader_indexed_png.h"
#include "io/resource_saver_indexed_png.h"
#include "src/mixin_script.h"
#include "src/image_indexed.h"
#include "src/midi_player.h"
#include "src/rand.h"
#include "src/grid_rect.h"
#include "src/linked_list.h"
#include "src/data_container.h"
#include "src/map2d.h"
#include "src/stopwatch.h"

static ImageLoaderIndexedPNG *image_loader_indexed_png;
static Ref<ResourceSaverIndexedPNG> resource_saver_indexed_png;
static MixinScriptLanguage *script_mixin_script = nullptr;
static Ref<Rand> rand_ref;

#if defined(TOOLS_ENABLED)
static ScriptEditorBase *create_editor(const RES &p_resource) {
	if (Object::cast_to<MixinScript>(*p_resource)) {
		return memnew(MixinScriptEditor);
	}
	return nullptr;
}

static void mixin_script_register_editor_callback() {
	ScriptEditor::register_create_script_editor_function(create_editor);
}
#endif

void register_goblin_types() {	
	// ImageIndexed
	image_loader_indexed_png = memnew(ImageLoaderIndexedPNG);
	ImageLoader::add_image_format_loader(image_loader_indexed_png);
	resource_saver_indexed_png.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_indexed_png);
    ClassDB::register_class<ImageIndexed>();

	// Mixin Script
	script_mixin_script = memnew(MixinScriptLanguage);
	ScriptServer::register_language(script_mixin_script);
	ClassDB::register_class<MixinScript>();
	ClassDB::register_class<Mixin>();

	// Midi Playr
	ClassDB::register_class<MidiPlayer>();
	ClassDB::register_class<MidiFile>();

	// Rand
	rand_ref.instance();
	ClassDB::register_class<Rand>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("Rand", Object::cast_to<Object>(Rand::get_singleton())));

	ClassDB::register_class<ListNode>();
	ClassDB::register_class<LinkedList>();
	ClassDB::register_class<Map2D>();
	ClassDB::register_class<DataContainer>();
	ClassDB::register_class<GridRect>();
	ClassDB::register_class<Stopwatch>();

#ifdef TOOLS_ENABLED
	EditorNode::add_plugin_init_callback(mixin_script_register_editor_callback);
	EditorPlugins::add_by_type<MixinScriptEditorPlugin>();

	if (Engine::get_singleton()->is_editor_hint()) {
		Ref<ResourceImporterMidiFile> midiobject;
		midiobject.instance();
		ResourceFormatImporter::get_singleton()->add_importer(midiobject);

	}

#endif

}

void unregister_goblin_types() {
    ResourceSaver::remove_resource_format_saver(resource_saver_indexed_png);
	resource_saver_indexed_png.unref();

	ScriptServer::unregister_language(script_mixin_script);
	memdelete(script_mixin_script);

	rand_ref.unref();
}
