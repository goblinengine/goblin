import os

def can_build(env, platform):
    return True

def configure(env):
    pass

def get_icons_path():
    return "editor/icons"

def get_doc_path():
    return "doc"

def get_doc_classes():
    return [
        "ImageIndexed", "MixinScript",
    ]