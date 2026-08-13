# GDScript Fork Features

The GDScript fork lives at `modules/goblin/modules/gdscript/` and is compiled instead of upstream `modules/gdscript/` via the whole-module override (ADR 0001).

This file documents the divergence from upstream GDScript. When porting across engine versions, diff `modules/goblin/modules/gdscript/` against `modules/gdscript/`. When changing code, update this file + `docs/CODE_MAP.md`.

## Features

### Union Types

Syntax: `int | float`, `Dictionary | null`, `var x: null | null` (redundant members dedup; single-member unions collapse to the member type)

The `null` type is a singleton: only `null` can be assigned/converted to it. `check_type_compatibility` has a guard because upstream `Variant::can_convert_strict` reports every type as convertible to NIL ("nil can convert to anything") — that rule is for the internal NIL Variant and would silently accept `x = 5` on a `null`-typed variable.

- Parser: `parse_type()` / `parse_type_single()`, `DataType::UNION` kind.
- Analyzer: `resolve_datatype()`, `check_type_compatibility()`, cast reduction.
- Compiler: maps `UNION` → runtime `VARIANT`.

Purpose: eliminate `typeof()` + `as` branching on values that may be one of a bounded set of types (e.g. physics shape `size` being `float` or `Vector3`).

### `@private` Annotation

Syntax: `@private var x`, `@private func f()`, `@private const K`, `@private class Inner`

- Parser registers the annotation; sets `is_private` on `VariableNode` / `FunctionNode` / `ConstantNode` / `ClassNode`.
- Analyzer blocks external access (`Cannot access private member "X" of class "Y"`), including calls to private methods; a blocked access reports one clean error (no cascading "cannot find member").
- Access policy: private members are visible to the declaring class and to **any class in the same script** (nested classes, any depth, including siblings). Other scripts are blocked.
- Autocomplete filters private members of other classes (`p_recursion_depth > 0` check in `_find_identifiers_in_class`).
- `@private` cannot be combined with any `@export*` annotation (error in both orders).
- Name reuse: a `@private` member still occupies the name in subclasses (upstream conflict check applies: `The member "X" already exists in parent class`). Deliberate decision (2026-08-12): supporting shadowing requires separate storage slots, which makes `GDScript::member_indices` sparse and forces an O(n) scan on the instance-creation hot path (plus ABI-safe persistence of the slot count is not possible without changing `gdscript.h`). Not worth the cost until a real need appears.

Purpose: encapsulation for internal members without a visibility keyword in the language.

Implementation note: `gdscript.h` must stay layout-identical to upstream: `main/main.cpp` and `editor/doc/editor_help.cpp` include it outside the module and would break ABI on any layout change (verified the hard way — heap corruption). Field additions require a header-redirect design first.

### String Constructors

Syntax: `String(int)`, `String(float)`, `String(bool)` → makes `1 as String` produce `"1"`.

- Implemented via `VariantConstructorToString<T>` in the goblin `core/variant/variant_construct.{cpp,h}` (overridden through the core file override, ADR 0001).

### Shaped Dictionary Literals

Syntax: typed entries, Lua style — `key: Type = value` (default optional):

```gdscript
var tpl := {
    hp: int = 10,
    reach: float = 1.5,
    tags: Array[String] = [],
}
```

- Parser: typed entries in `{}` literals; recursive shape = key set + entry types (`GDScriptDataType` shape payload in `gdscript_function.h`).
- Analyzer: shape inference; the shape is preserved across all declaration styles (`:=`, `: Dictionary`, `: Dictionary[K,V]`, and untyped `=`), and writes to typed keys are compile-time errors; attribute access (`dict.key`) and constant-index access (`dict["key"]`) refine to the entry type; autocomplete recurses into shapes (`gdscript_editor.cpp`).
- Runtime: `OPCODE_CONSTRUCT_SHAPED_DICTIONARY` validates every literal value against its declared entry type (debug safety net) and normalizes typed containers (plain `Array` → typed `Array[T]`); the recursive datatype travels as raw instruction words (`append_datatype()`, `gdscript_byte_codegen.h`), decoded by `GDScriptFunction::decode_datatype()` (`gdscript_function.{h,cpp}`). Runtime validation applies at construction only — later writes are enforced at compile time, not re-checked at runtime.
- Style rules: typed entries are Lua style only; mixing with Python-style untyped literals errors (tests: `shaped_dictionary_style_mixing_*`, `shaped_dictionary_typed_in_python`).
- NOT implemented (planned): template dictionaries (`_template` reserved key, creation-time default expansion) — see `.kilo/plans/` §2.

Purpose: typed dictionaries with zero runtime lookups for data-driven entity templates — the language-layer answer to DB's dict-heavy entity model.

### `then` / `elthen` (safe navigation / null coalescing) — PARTIAL

Keywords `then` and `elthen` are the fork's syntax. `?.` / `??` are NOT planned.

State (verified): tokenizer only — `Token::THEN` / `Token::ELTHEN` (`gdscript_tokenizer.h:65-66`), token names + keywords (`gdscript_tokenizer.cpp:60-61, 503, 533`). Parser, analyzer, and compiler wiring are NOT present; the operators do not compile yet.

Planned semantics (recovered from gdscript2, `.kilo/plans/` §3): binary operators at `PREC_NULLISH`, both null-only (`a then b` → `a != null ? b : null`, `a elthen b` → `a != null ? a : b`). gdscript2's runtime reused ternary truthiness (a wart: `0 then b` → `b`); the fork ports them with explicit `!= null` conditions.

## Divergence Surface

When porting to a new stable release, review these files for merge conflicts:

| Area | Files |
|------|-------|
| Tokenizer | `gdscript_tokenizer.h`, `gdscript_tokenizer.cpp`, `gdscript_tokenizer_buffer.{h,cpp}` |
| Parser / AST | `gdscript_parser.h`, `gdscript_parser.cpp` |
| Analyzer | `gdscript_analyzer.h`, `gdscript_analyzer.cpp` |
| Compiler | `gdscript_compiler.cpp` |
| Bytecode gen | `gdscript_byte_codegen.{h,cpp}`, `gdscript_codegen.h` |
| VM | `gdscript_vm.cpp` |
| Function | `gdscript_function.{h,cpp}` |
| Editor (autocomplete) | `gdscript_editor.cpp` |
| Core variant | `core/variant/variant_construct.cpp`, `core/variant/variant_construct.h` |

## Planned Features

See [backlog.md](backlog.md) §1. Next priorities: `then`/`elthen` wiring (G-04/G-05), structs (G-07), typed dictionaries (G-08), template dictionaries (`.kilo/plans/` §2).
