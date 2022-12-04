#pragma once

#include "modules/dascript/src/include/daScript/misc/type_name.h"
#include "modules/dascript/src/include/daScript/ast/ast_typefactory.h"
#include "modules/dascript/src/include/daScript/misc/uric.h"

/*
    URI = scheme ":" ["//" authority] path ["?" query] ["#" fragment]
    authority = [userinfo "@"] host [":" port]
*/

MAKE_EXTERNAL_TYPE_FACTORY(UriTextRangeA,UriTextRangeA)
MAKE_EXTERNAL_TYPE_FACTORY(UriIp4Struct,UriIp4Struct)
MAKE_EXTERNAL_TYPE_FACTORY(UriIp6Struct,UriIp6Struct)
MAKE_EXTERNAL_TYPE_FACTORY(UriHostDataA,UriHostDataA)
MAKE_EXTERNAL_TYPE_FACTORY(UriPathSegmentStructA,UriPathSegmentStructA)
MAKE_EXTERNAL_TYPE_FACTORY(UriUriA,UriUriA)
MAKE_EXTERNAL_TYPE_FACTORY(Uri,das::Uri);


