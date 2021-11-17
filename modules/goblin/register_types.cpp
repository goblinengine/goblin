/* register_types.cpp */

#include "register_types.h"

#include "core/class_db.h"
#include "image_indexed.h"
#include "io/image_loader_indexed_png.h"
#include "io/resource_saver_indexed_png.h"

static ImageLoaderIndexedPNG *image_loader_indexed_png;
static Ref<ResourceSaverIndexedPNG> resource_saver_indexed_png;

void register_goblin_types() {
    ClassDB::register_class<ImageIndexed>();

	image_loader_indexed_png = memnew(ImageLoaderIndexedPNG);
	ImageLoader::add_image_format_loader(image_loader_indexed_png);

	resource_saver_indexed_png.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_indexed_png);
}

void unregister_goblin_types() {
    ResourceSaver::remove_resource_format_saver(resource_saver_indexed_png);
	resource_saver_indexed_png.unref();
}