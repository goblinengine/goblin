# config.py

def can_build(env, platform):
    return True

def is_enabled():
    # disabled by default use module_sqlite_enabled=yes to build
    return False

def configure(env):
    pass

def get_doc_classes():
    return [
        "SQLite",
    ]

def get_doc_path():
    return "doc_classes"