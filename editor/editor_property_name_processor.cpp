/*************************************************************************/
/*  editor_property_name_processor.cpp                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "editor_property_name_processor.h"

#include "editor_settings.h"

EditorPropertyNameProcessor *EditorPropertyNameProcessor::singleton = nullptr;

String EditorPropertyNameProcessor::_capitalize_name(const String &p_name) const {
	const Map<String, String>::Element *cached = capitalize_string_cache.find(p_name);
	if (cached) {
		return cached->value();
	}

	Vector<String> parts = p_name.split("_", false);
	for (int i = 0; i < parts.size(); i++) {
		const Map<String, String>::Element *remap = capitalize_string_remaps.find(parts[i]);
		if (remap) {
			parts.write[i] = remap->get();
		} else {
			parts.write[i] = parts[i].capitalize();
		}
	}
	const String capitalized = String(" ").join(parts);

	capitalize_string_cache[p_name] = capitalized;
	return capitalized;
}

String EditorPropertyNameProcessor::process_name(const String &p_name) const {
	const String capitalized_string = _capitalize_name(p_name);
	if (EDITOR_GET("interface/editor/translate_properties")) {
		return TTRGET(capitalized_string);
	}
	return capitalized_string;
}

String EditorPropertyNameProcessor::make_tooltip_for_name(const String &p_name) const {
	const String capitalized_string = _capitalize_name(p_name);
	if (EDITOR_GET("interface/editor/translate_properties")) {
		return capitalized_string;
	}
	return TTRGET(capitalized_string);
}

EditorPropertyNameProcessor::EditorPropertyNameProcessor() {
	ERR_FAIL_COND(singleton != nullptr);
	singleton = this;

	// The following initialization is parsed in `editor/translations/extract.py` with a regex.
	// The map name and value definition format should be kept synced with the regex.
	capitalize_string_remaps["2d"] = "2D";
	capitalize_string_remaps["3d"] = "3D";
	capitalize_string_remaps["aa"] = "AA";
	capitalize_string_remaps["aabb"] = "AABB";
	capitalize_string_remaps["adb"] = "ADB";
	capitalize_string_remaps["ao"] = "AO";
	capitalize_string_remaps["apk"] = "APK";
	capitalize_string_remaps["arvr"] = "ARVR";
	capitalize_string_remaps["bg"] = "BG";
	capitalize_string_remaps["bp"] = "BP";
	capitalize_string_remaps["bpc"] = "BPC";
	capitalize_string_remaps["bptc"] = "BPTC";
	capitalize_string_remaps["bvh"] = "BVH";
	capitalize_string_remaps["ca"] = "CA";
	capitalize_string_remaps["cd"] = "CD";
	capitalize_string_remaps["cpu"] = "CPU";
	capitalize_string_remaps["csg"] = "CSG";
	capitalize_string_remaps["db"] = "dB";
	capitalize_string_remaps["dof"] = "DoF";
	capitalize_string_remaps["dpi"] = "DPI";
	capitalize_string_remaps["dtls"] = "DTLS";
	capitalize_string_remaps["etc"] = "ETC";
	capitalize_string_remaps["etc2"] = "ETC2";
	capitalize_string_remaps["fft"] = "FFT";
	capitalize_string_remaps["fov"] = "FOV";
	capitalize_string_remaps["fps"] = "FPS";
	capitalize_string_remaps["fs"] = "FS";
	capitalize_string_remaps["fsr"] = "FSR";
	capitalize_string_remaps["fxaa"] = "FXAA";
	capitalize_string_remaps["gdscript"] = "GDScript";
	capitalize_string_remaps["ggx"] = "GGX";
	capitalize_string_remaps["gi"] = "GI";
	capitalize_string_remaps["glb"] = "GLB";
	capitalize_string_remaps["gles2"] = "GLES2";
	capitalize_string_remaps["gles3"] = "GLES3";
	capitalize_string_remaps["gpu"] = "GPU";
	capitalize_string_remaps["gui"] = "GUI";
	capitalize_string_remaps["hdr"] = "HDR";
	capitalize_string_remaps["hidpi"] = "hiDPI";
	capitalize_string_remaps["hipass"] = "High-pass";
	capitalize_string_remaps["hsv"] = "HSV";
	capitalize_string_remaps["http"] = "HTTP";
	capitalize_string_remaps["id"] = "ID";
	capitalize_string_remaps["igd"] = "IGD";
	capitalize_string_remaps["ik"] = "IK";
	capitalize_string_remaps["image@2x"] = "Image @2x";
	capitalize_string_remaps["image@3x"] = "Image @3x";
	capitalize_string_remaps["ios"] = "iOS";
	capitalize_string_remaps["iod"] = "IOD";
	capitalize_string_remaps["ip"] = "IP";
	capitalize_string_remaps["ipad"] = "iPad";
	capitalize_string_remaps["iphone"] = "iPhone";
	capitalize_string_remaps["ipv6"] = "IPv6";
	capitalize_string_remaps["jit"] = "JIT";
	capitalize_string_remaps["k1"] = "K1";
	capitalize_string_remaps["k2"] = "K2";
	capitalize_string_remaps["kb"] = "(KB)"; // Unit.
	capitalize_string_remaps["ldr"] = "LDR";
	capitalize_string_remaps["lod"] = "LOD";
	capitalize_string_remaps["lowpass"] = "Low-pass";
	capitalize_string_remaps["macos"] = "macOS";
	capitalize_string_remaps["mb"] = "(MB)"; // Unit.
	capitalize_string_remaps["ms"] = "(ms)"; // Unit
	// Not used for now as AudioEffectReverb has a `msec` property.
	//capitalize_string_remaps["msec"] = "(msec)"; // Unit.
	capitalize_string_remaps["msaa"] = "MSAA";
	capitalize_string_remaps["normalmap"] = "Normal Map";
	capitalize_string_remaps["ok"] = "OK";
	capitalize_string_remaps["opengl"] = "OpenGL";
	capitalize_string_remaps["opentype"] = "OpenType";
	capitalize_string_remaps["openxr"] = "OpenXR";
	capitalize_string_remaps["pck"] = "PCK";
	capitalize_string_remaps["png"] = "PNG";
	capitalize_string_remaps["po2"] = "(Power of 2)"; // Unit.
	capitalize_string_remaps["pvs"] = "PVS";
	capitalize_string_remaps["pvrtc"] = "PVRTC";
	capitalize_string_remaps["rgb"] = "RGB";
	capitalize_string_remaps["rid"] = "RID";
	capitalize_string_remaps["rmb"] = "RMB";
	capitalize_string_remaps["rpc"] = "RPC";
	capitalize_string_remaps["s3tc"] = "S3TC";
	capitalize_string_remaps["sdf"] = "SDF";
	capitalize_string_remaps["sdfgi"] = "SDFGI";
	capitalize_string_remaps["sdk"] = "SDK";
	capitalize_string_remaps["sec"] = "(sec)"; // Unit.
	capitalize_string_remaps["srgb"] = "sRGB";
	capitalize_string_remaps["ssao"] = "SSAO";
	capitalize_string_remaps["ssh"] = "SSH";
	capitalize_string_remaps["ssil"] = "SSIL";
	capitalize_string_remaps["ssl"] = "SSL";
	capitalize_string_remaps["stderr"] = "stderr";
	capitalize_string_remaps["stdout"] = "stdout";
	capitalize_string_remaps["svg"] = "SVG";
	capitalize_string_remaps["tcp"] = "TCP";
	capitalize_string_remaps["ui"] = "UI";
	capitalize_string_remaps["url"] = "URL";
	capitalize_string_remaps["urls"] = "URLs";
	capitalize_string_remaps["us"] = String::utf8("(µs)"); // Unit.
	capitalize_string_remaps["usec"] = String::utf8("(µsec)"); // Unit.
	capitalize_string_remaps["uuid"] = "UUID";
	capitalize_string_remaps["uv"] = "UV";
	capitalize_string_remaps["uv1"] = "UV1";
	capitalize_string_remaps["uv2"] = "UV2";
	capitalize_string_remaps["uwp"] = "UWP";
	capitalize_string_remaps["vector2"] = "Vector2";
	capitalize_string_remaps["vram"] = "VRAM";
	capitalize_string_remaps["vsync"] = "V-Sync";
	capitalize_string_remaps["webp"] = "WebP";
	capitalize_string_remaps["webrtc"] = "WebRTC";
	capitalize_string_remaps["websocket"] = "WebSocket";
	capitalize_string_remaps["wifi"] = "Wi-Fi";
	capitalize_string_remaps["xr"] = "XR";
	capitalize_string_remaps["xy"] = "XY";
	capitalize_string_remaps["xz"] = "XZ";
	capitalize_string_remaps["yz"] = "YZ";
}

EditorPropertyNameProcessor::~EditorPropertyNameProcessor() {
	singleton = nullptr;
}
