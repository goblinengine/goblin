import os


def can_build(env, platform):
    return True


def configure(env):
    pass


def get_icons_path():
    return "icons"


def get_doc_path():
    return "doc_classes"


def get_doc_classes():
    return [
        "ImageIndexed",
        "Mixin",
        "MixinScript",
        "MidiPlayer",
        "MidiFile"
    ]
