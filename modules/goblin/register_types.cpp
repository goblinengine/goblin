/* register_types.cpp */

#include "core/class_db.h"
#include "register_types.h"
#include "editor/editor_node.h"
#include "editor/mixin_script_editor.h"
#include "editor/mixin_script_editor_plugin.h"
#include "mixin_script.h"
#include "core/script_language.h"
#include "image_indexed.h"
#include "io/image_loader_indexed_png.h"
#include "io/resource_saver_indexed_png.h"
#include "midi_player.h"
#include "rand.h"

static ImageLoaderIndexedPNG *image_loader_indexed_png;
static Ref<ResourceSaverIndexedPNG> resource_saver_indexed_png;
static MixinScriptLanguage *script_mixin_script = nullptr;
static Ref<Rand> random;

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
	
    ClassDB::register_class<ImageIndexed>();

	image_loader_indexed_png = memnew(ImageLoaderIndexedPNG);
	ImageLoader::add_image_format_loader(image_loader_indexed_png);

	resource_saver_indexed_png.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_indexed_png);

	script_mixin_script = memnew(MixinScriptLanguage);
	ScriptServer::register_language(script_mixin_script);
	ClassDB::register_class<MixinScript>();
	ClassDB::register_class<Mixin>();

	ClassDB::register_class<MidiPlayer>();
	ClassDB::register_class<MidiFile>();

	random.instance();
	ClassDB::register_class<Rand>();
	Object *random = Object::cast_to<Object>(Rand::get_singleton());
	Engine::get_singleton()->add_singleton(Engine::Singleton("Rand", random));

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

	random.unref();
}