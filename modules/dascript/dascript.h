#include "core/script_language.h"
#include "core/os/thread_safe.h"

#ifndef DASCRIPT_H
#define DASCRIPT_H

class DaScript : public Script {
	GDCLASS(DaScript, Script)
	bool tool;

	friend class DaScriptLanguage;

    DaScript *object; //fast pointer access
	DaScript *owner; //for subclasses

protected:
	static void _bind_methods();

public:
	virtual bool can_instance() const { return true; }

	virtual StringName get_instance_base_type() const;
	virtual ScriptInstance *instance_create(Object *p_this);
	virtual bool instance_has(const Object *p_this) const;

	virtual bool has_source_code() const { return false; }
	virtual String get_source_code() const { return ""; }
	virtual void set_source_code(const String &p_code) {};
	virtual Error reload(bool p_keep_state = false);

	virtual bool is_tool() const;
	virtual bool is_valid() const;

	virtual bool inherits_script(const Ref<Script> &p_script) const;

	virtual String get_node_type() const { return ""; }

	virtual ScriptLanguage *get_language() const;

	virtual Ref<Script> get_base_script() const;
	virtual bool has_method(const StringName &p_method) const;
	virtual MethodInfo get_method_info(const StringName &p_method) const;
	virtual bool has_script_signal(const StringName &p_signal) const;
	virtual void get_script_signal_list(List<MethodInfo> *r_signals) const;
	virtual bool get_property_default_value(const StringName &p_property, Variant &r_value) const;
	virtual void get_script_method_list(List<MethodInfo> *p_list) const;
	virtual void get_script_property_list(List<PropertyInfo> *p_list) const;
	virtual void update_exports();

	DaScript();
	~DaScript();
};

class DaScriptInstance : public ScriptInstance {
	friend class DaScript;

	Object *owner;
	Ref<DaScript> script;

public:
	virtual Object *get_owner() { return owner; }

	virtual bool set(const StringName &p_name, const Variant &p_value);
	virtual bool get(const StringName &p_name, Variant &r_ret) const;
	virtual void get_property_list(List<PropertyInfo> *p_properties) const;
	virtual Variant::Type get_property_type(const StringName &p_name, bool *r_is_valid = nullptr) const;

	virtual void get_method_list(List<MethodInfo> *p_list) const;
	virtual bool has_method(const StringName &p_method) const;
	virtual Variant call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);
	virtual void call_multilevel(const StringName &p_method, const Variant **p_args, int p_argcount);
	virtual void call_multilevel_reversed(const StringName &p_method, const Variant **p_args, int p_argcount);

	virtual void notification(int p_notification);
	String to_string(bool *r_valid);

	virtual Ref<Script> get_script() const;
	virtual ScriptLanguage *get_language();

	DaScriptInstance();
	~DaScriptInstance();
};

class DaScriptLanguage : public ScriptLanguage {

	static DaScriptLanguage *singleton;

	Variant *_global_array;
	Vector<Variant> global_array;
	Map<StringName, int> globals;
	Map<StringName, Variant> named_globals;

	int _debug_parse_err_line;
	String _debug_parse_err_file;
	String _debug_error;
	int _debug_call_stack_pos;
	int _debug_max_call_stack;

	void _add_global(const StringName &p_name, const Variant &p_value);

	friend class DaScriptInstance;

	Mutex lock;

	friend class DaScript;

	SelfList<DaScript>::List script_list;

	bool profiling;
	uint64_t script_frame_time;

	Map<String, ObjectID> orphan_subclasses;

public:
	int calls;

	bool debug_break(const String &p_error, bool p_allow_continue = true);
	bool debug_break_parse(const String &p_file, int p_line, const String &p_error);

	_FORCE_INLINE_ int get_global_array_size() const { return global_array.size(); }
	_FORCE_INLINE_ Variant *get_global_array() { return _global_array; }
	_FORCE_INLINE_ const Map<StringName, int> &get_global_map() const { return globals; }
	_FORCE_INLINE_ const Map<StringName, Variant> &get_named_globals_map() const { return named_globals; }

	_FORCE_INLINE_ static DaScriptLanguage *get_singleton() { return singleton; }

	virtual String get_name() const;

	/* LANGUAGE FUNCTIONS */
	virtual void init();
	virtual String get_type() const;
	virtual String get_extension() const;
	virtual Error execute_file(const String &p_path);
	virtual void finish();

	/* EDITOR FUNCTIONS */
	virtual void get_reserved_words(List<String> *p_words) const;
	virtual bool is_control_flow_keyword(String p_keywords) const;
	virtual void get_comment_delimiters(List<String> *p_delimiters) const;
	virtual void get_string_delimiters(List<String> *p_delimiters) const;
	virtual String _get_processed_template(const String &p_template, const String &p_base_class_name) const;
	virtual Ref<Script> get_template(const String &p_class_name, const String &p_base_class_name) const;
	virtual bool is_using_templates();
	virtual void make_template(const String &p_class_name, const String &p_base_class_name, Ref<Script> &p_script);
	virtual bool validate(const String &p_script, int &r_line_error, int &r_col_error, String &r_test_error, const String &p_path = "", List<String> *r_functions = nullptr, List<ScriptLanguage::Warning> *r_warnings = nullptr, Set<int> *r_safe_lines = nullptr) const;
	virtual Script *create_script() const;
	virtual bool has_named_classes() const;
	virtual bool supports_builtin_mode() const;
	virtual bool can_inherit_from_file() { return true; }
	virtual int find_function(const String &p_function, const String &p_code) const;
	virtual String make_function(const String &p_class, const String &p_name, const PoolStringArray &p_args) const;
	virtual Error complete_code(const String &p_code, const String &p_path, Object *p_owner, List<ScriptCodeCompletionOption> *r_options, bool &r_forced, String &r_call_hint);
#ifdef TOOLS_ENABLED
	virtual Error lookup_code(const String &p_code, const String &p_symbol, const String &p_path, Object *p_owner, LookupResult &r_result);
#endif
	virtual String _get_indentation() const;
	virtual void auto_indent_code(String &p_code, int p_from_line, int p_to_line) const;
	virtual void add_global_constant(const StringName &p_variable, const Variant &p_value);
	virtual void add_named_global_constant(const StringName &p_name, const Variant &p_value);
	virtual void remove_named_global_constant(const StringName &p_name);

	/* DEBUGGER FUNCTIONS */

	virtual String debug_get_error() const;
	virtual int debug_get_stack_level_count() const;
	virtual int debug_get_stack_level_line(int p_level) const;
	virtual String debug_get_stack_level_function(int p_level) const;
	virtual String debug_get_stack_level_source(int p_level) const;
	virtual void debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);
	virtual void debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);
	virtual ScriptInstance *debug_get_stack_level_instance(int p_level);
	virtual void debug_get_globals(List<String> *p_globals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);
	virtual String debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems = -1, int p_max_depth = -1);

	virtual void reload_all_scripts();
	virtual void reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload);

	virtual void frame();

	virtual void get_public_functions(List<MethodInfo> *p_functions) const;
	virtual void get_public_constants(List<Pair<String, Variant>> *p_constants) const;

	virtual void profiling_start();
	virtual void profiling_stop();

	virtual int profiling_get_accumulated_data(ProfilingInfo *p_info_arr, int p_info_max);
	virtual int profiling_get_frame_data(ProfilingInfo *p_info_arr, int p_info_max);

	/* LOADER FUNCTIONS */

	virtual void get_recognized_extensions(List<String> *p_extensions) const;

	/* GLOBAL CLASSES */

	virtual bool handles_global_class_type(const String &p_type) const;
	virtual String get_global_class_name(const String &p_path, String *r_base_type = nullptr, String *r_icon_path = nullptr) const;

	void add_orphan_subclass(const String &p_qualified_name, const ObjectID &p_subclass);
	Ref<DaScript> get_orphan_subclass(const String &p_qualified_name);

	DaScriptLanguage();
	~DaScriptLanguage();
};

#endif // DASCRIPT_H