def can_build(env, platform):
    return True


def configure(env):
    """
    SURGICAL ENVIRONMENT INTERCEPTION
    
    Only intercept what we need, preserve everything else.
    Platform-aware to handle differences between Windows/Linux/macOS.
    """
    # Keep config hooks minimal and safe: branding is handled via
    # post-actions in modules/goblin/SCsub.


    # IMPORTANT: Do not capture bound `env.add_*` methods here.
    # Those are bound to a specific environment instance; if we store and reuse them,
    # cloned environments (e.g. `env_icu`, `env_graphite`) will accidentally build with
    # the *wrong* env and lose their custom CPPPATH/CPPDEFINES.
    import methods as godot_methods

    def goblin_add_program(self_env, program, source, **kw):
        program_str = str(program)
        if program_str.startswith("#bin/godot") and "console" not in program_str.lower():
            program = program_str.replace("godot", "goblin")
            if self_env.get("verbose"):
                print(f"Goblin: Renaming {program_str} -> {program}")
        return godot_methods.add_program(self_env, program, source, **kw)

    def goblin_add_library(self_env, program, source, **kw):
        program_str = str(program)
        if program_str.startswith("#bin/godot"):
            program = program_str.replace("godot", "goblin")
            if self_env.get("verbose"):
                print(f"Goblin: Renaming {program_str} -> {program}")
        return godot_methods.add_library(self_env, program, source, **kw)

    def goblin_add_shared_library(self_env, program, source, **kw):
        program_str = str(program)
        if program_str.startswith("#bin/godot"):
            program = program_str.replace("godot", "goblin")
            if self_env.get("verbose"):
                print(f"Goblin: Renaming {program_str} -> {program}")
        return godot_methods.add_shared_library(self_env, program, source, **kw)

    env.AddMethod(goblin_add_program, "add_program")
    env.AddMethod(goblin_add_library, "add_library")
    env.AddMethod(goblin_add_shared_library, "add_shared_library")

    env.Append(CPPDEFINES=["GOBLIN_ENGINE"])

    if env.get("verbose"):
        print("Goblin: Build hooks enabled")


def get_doc_classes():
    return []


def get_doc_path():
    return "doc_classes"
