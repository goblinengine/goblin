# GDScript Fork Features

The GDScript fork lives at `modules/goblin/modules/gdscript/` and is compiled instead of upstream `modules/gdscript/` via the whole-module override (ADR 0001).

This file documents the divergence from upstream GDScript. When porting across engine versions, diff `modules/goblin/modules/gdscript/` against `modules/gdscript/`.

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
- Analyzer blocks external access (`Cannot access private member "X" of class "Y"`).
- Access policy: private members are visible to the declaring class and to **any class in the same script** (nested classes, any depth, including siblings). Other scripts are blocked.
- Autocomplete filters private members of other classes (`p_recursion_depth > 0` check in `_find_identifiers_in_class`).
- `@private` cannot be combined with any `@export*` annotation (error in both orders).
- Name reuse: a `@private` member still occupies the name in subclasses (upstream conflict check applies: `The member "X" already exists in parent class`). Deliberate decision (2026-08-12): supporting shadowing requires separate storage slots, which makes `GDScript::member_indices` sparse and forces an O(n) scan on the instance-creation hot path (plus ABI-safe persistence of the slot count is not possible without changing `gdscript.h`). Not worth the cost until a real need appears.

Purpose: encapsulation for internal members without a visibility keyword in the language.

Implementation note: `gdscript.h` must stay layout-identical to upstream: `main/main.cpp` and `editor/doc/editor_help.cpp` include it outside the module and would break ABI on any layout change (verified the hard way — heap corruption). Field additions require a header-redirect design first.

### String Constructors

Syntax: `String(int)`, `String(float)`, `String(bool)` → makes `1 as String` produce `"1"`.

- Implemented via `VariantConstructorToString<T>` in the goblin `core/variant/variant_construct.{cpp,h}` (overridden through the core file override, ADR 0001).

## Divergence Surface

When porting to a new stable release, review these files for merge conflicts:

| Area | Files |
|------|-------|
| Parser / AST | `gdscript_parser.h`, `gdscript_parser.cpp` |
| Analyzer | `gdscript_analyzer.h`, `gdscript_analyzer.cpp` |
| Compiler | `gdscript_compiler.cpp` |
| Core variant | `core/variant/variant_construct.cpp`, `core/variant/variant_construct.h` |
| Editor (autocomplete) | `gdscript_editor.cpp` |

## Planned Features

See [backlog.md](backlog.md) §1. Next priorities: `?.` / `??` (G-04/G-05), structs (G-07), typed dictionaries (G-08).
