# GDScript Fork Features

The GDScript fork lives at `modules/goblin/modules/gdscript/` and is compiled instead of upstream `modules/gdscript/` via the whole-module override (ADR 0001).

This file documents the divergence from upstream GDScript. When porting across engine versions, diff `modules/goblin/modules/gdscript/` against `modules/gdscript/`.

## Features

### Union Types

Syntax: `int | float`, `Dictionary | null`

- Parser: `parse_type()` / `parse_type_single()`, `DataType::UNION` kind.
- Analyzer: `resolve_datatype()`, `check_type_compatibility()`, cast reduction.
- Compiler: maps `UNION` → runtime `VARIANT`.

Purpose: eliminate `typeof()` + `as` branching on values that may be one of a bounded set of types (e.g. physics shape `size` being `float` or `Vector3`).

### `@private` Annotation

Syntax: `@private var x`, `@private func f()`

- Parser registers the annotation; sets `is_private` on `VariableNode` / `FunctionNode`.
- Analyzer blocks external access (`Cannot access private member "X" of class "Y"`).
- Autocomplete filters private members from outside the declaring class (`p_recursion_depth > 0` check in `_find_identifiers_in_class`).

Purpose: encapsulation for internal members without a visibility keyword in the language.

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
