# IncludeCPP

Write C++ code, use it in Python. Auto-generates pybind11 bindings.

```bash
pip install IncludeCPP
```

# First Steps

## Project Setup

```bash
includecpp init
```

Creates:
- `cpp.proj` - project configuration
- `include/` - your C++ source files
- `plugins/` - generated binding definitions

## Write C++ Code

Put your code in `namespace includecpp`:

```cpp
// include/fast_list.cpp
#include <vector>

namespace includecpp {

class FastList {
public:
    void append(int val) { data.push_back(val); }
    int get(int i) { return data[i]; }
private:
    std::vector<int> data;
};

int add(int a, int b) { return a + b; }

}  // namespace includecpp
```

The parser only scans code inside `namespace includecpp`. Everything else is ignored.

## Generate Bindings

```bash
includecpp plugin fast_list include/fast_list.cpp
```

This runs a C++ parser that:
1. Scans your source files for classes, methods, functions
2. Extracts signatures, return types, parameter names
3. Generates `plugins/fast_list.cp` with binding instructions

## Build

```bash
includecpp rebuild
```

Compiles your C++ into a Python extension module (`.pyd` on Windows, `.so` on Linux/Mac).

## Use in Python

```python
from includecpp import fast_list

my_list = fast_list.FastList()
my_list.append(42)
print(fast_list.add(1, 2))  # 3
```

Alternative syntax:

```python
from includecpp import CppApi

api = CppApi()
fast_list = api.include("fast_list")
```

# How to Start

## Minimal Example

```bash
# 1. Create project
mkdir myproject && cd myproject
includecpp init

# 2. Write C++ (include/math.cpp)
cat > include/math.cpp << 'EOF'
namespace includecpp {
    int square(int x) { return x * x; }
}
EOF

# 3. Generate plugin
includecpp plugin math include/math.cpp

# 4. Build
includecpp rebuild

# 5. Use
python -c "from includecpp import math; print(math.square(7))"
```

## Development Workflow

For active development, use `auto`:

```bash
includecpp auto math
```

This regenerates the `.cp` file from source and rebuilds in one command.

For fastest iteration:

```bash
includecpp rebuild --fast
```

Skips unchanged files. ~0.4s when nothing changed.

# How IncludeCPP Works

## Architecture

```
Your C++ Source            Plugin File (.cp)           Python Module
include/math.cpp    --->   plugins/math.cp     --->    math.cpXXX.pyd
     ^                           ^                          ^
     |                           |                          |
  C++ parser              Binding config             pybind11 compiled
  extracts API            (editable)                 extension
```

## The Parser

The C++ parser (`parser.cpp`) runs as a compiled executable. It:

1. Tokenizes your C++ source files
2. Identifies the `namespace includecpp` block
3. Extracts:
   - Class names and inheritance
   - Method signatures (name, return type, parameters)
   - Function signatures
   - Template instantiations
   - Const/static qualifiers
4. Outputs structured binding instructions to `.cp` files

## Plugin Files (.cp)

The `.cp` format is a declarative binding specification:

```
SOURCE(math.cpp) math

PUBLIC(
    math CLASS(Calculator) {
        CONSTRUCTOR()
        CONSTRUCTOR(int)
        METHOD(add)
        METHOD_CONST(getValue)
        FIELD(value)
    }

    math FUNC(square)
    math TEMPLATE_FUNC(maximum) TYPES(int, float, double)
)
```

Key directives:
- `SOURCE(file.cpp) module_name` - links source to module
- `PUBLIC(...)` - defines public bindings
- `CLASS(Name)` - expose a class
- `STRUCT(Name)` - expose a struct
- `FUNC(name)` - expose a free function
- `METHOD(name)` - expose a class method
- `METHOD_CONST(name, signature)` - for overloaded methods
- `TEMPLATE_FUNC(name) TYPES(...)` - instantiate template
- `CONSTRUCTOR(args)` - expose constructor
- `FIELD(name)` - expose member variable
- `DEPENDS(mod1, mod2)` - declare module dependencies

## Build System

The build manager:

1. Reads `cpp.proj` configuration
2. Parses all `.cp` files in `plugins/`
3. Generates pybind11 binding code
4. Compiles using CMake with detected compiler (MSVC, GCC, Clang)
5. Places output in `~/.includecpp/builds/` (not in your project)
6. Creates a registry so Python can find modules

Caching layers:
- **Object files**: Only recompile changed `.cpp` files
- **Generator cache**: Compiler/CMake detection runs once
- **SHA256 hashes**: Skip unchanged modules entirely

# CLI Reference

Use `includecpp <command> --help` for details.

| Command | Description |
|---------|-------------|
| `init` | Create project structure |
| `plugin <name> <files>` | Generate .cp from C++ sources |
| `auto <plugin>` | Regenerate .cp and rebuild |
| `auto --all` | Regenerate and rebuild all plugins |
| `auto --all -x <name>` | All plugins except specified |
| `fix <module>` | Analyze C++ code for issues |
| `fix --all` | Analyze all modules |
| `fix --undo` | Revert last fix changes |
| `fix --ai <module>` | AI-enhanced code analysis |
| `ai key <key>` | Set OpenAI API key |
| `ai enable` | Enable AI features |
| `ai disable` | Disable AI features |
| `ai model --list` | List available models |
| `ai model set <name>` | Set active model |
| `ai --info` | Show AI configuration and usage |
| `ai optimize <module>` | AI code optimization |
| `ai optimize --agent "<task>"` | Custom AI task |
| `ai ask "<question>"` | Ask about project with full context |
| `ai ask "<question>" <module>` | Ask about specific module |
| `ai edit "<task>"` | Edit code with AI assistance |
| `ai edit "<task>" --think2` | Thorough edit mode |
| `ai undo` | Restore files after AI changes |
| `rebuild` / `build` | Compile all modules |
| `get <module>` | Show module API (classes, methods, functions) |
| `install <name>` | Install community module |
| `update` | Update IncludeCPP |
| `bug` | Report an issue |
| `--doc` | Show documentation |
| `--changelog` | Show latest version changelog |
| `cppy convert <files> --cpp` | Convert Python to C++ |
| `cppy convert <files> --py` | Convert C++ to Python |
| `cppy convert <files> --cpp --no-h` | Convert without header |
| `cppy convert <files> --cpp --ai` | AI-assisted conversion |
| `cppy analyze <files>` | Analyze code structure |
| `cppy types` | Show type mapping tables |

## Build Flags

```bash
includecpp rebuild                  # Standard build
includecpp rebuild --clean          # Full rebuild, clear caches
includecpp rebuild --fast           # Fast incremental (~0.4s if unchanged)
includecpp rebuild --verbose        # Show compiler output
includecpp rebuild -m crypto        # Build specific module only
includecpp rebuild -j 8             # Use 8 parallel jobs
includecpp rebuild --keep           # Keep generator between builds
includecpp rebuild --no-incremental # Force full recompilation
includecpp rebuild --this           # Build current directory as module
```

## Fast Mode

`--fast` enables object file caching:

| Scenario | Time |
|----------|------|
| No changes | ~0.4s |
| Source changed | ~5-10s |
| Full rebuild | ~30s |

Clear caches with `--clean`.

## Incompatible Flag Combinations

| Flags | Reason |
|-------|--------|
| `--fast` + `--no-incremental` | Fast mode requires incremental |
| `--fast` + `--clean` | Fast uses caches, clean deletes them |
| `--fast` + `--this` | Not supported together |
| `--incremental` + `--no-incremental` | Contradictory |

# Advanced Features

## AI Integration

IncludeCPP integrates with OpenAI for intelligent code analysis and optimization.

### Setup

```bash
includecpp ai key sk-your-api-key-here
includecpp ai enable
```

### Available Models

```bash
includecpp ai model --list
```

- `gpt-5` (default) - 256k context
- `gpt-5-nano` - 32k context, fast
- `gpt-4o` - 128k context
- `gpt-4-turbo` - 128k context
- `gpt-3.5-turbo` - 16k context

### AI-Enhanced Fix

```bash
includecpp fix --ai mymodule
includecpp fix --ai --all
```

Sends source files to AI for analysis, suggests improvements while preserving all existing functions.

### AI Optimize

```bash
includecpp ai optimize mymodule                        # Optimize module sources
includecpp ai optimize --file src/utils.cpp            # Optimize specific files
includecpp ai optimize --agent mymodule "add SIMD"     # Custom task
```

### AI Ask

Ask questions about your project with full context awareness:

```bash
includecpp ai ask "where is collision detection?"              # Search all modules
includecpp ai ask "how does chunk generation work?" chunk_utils  # Specific module
includecpp ai ask "explain the biome system" --file include/biomes.cpp
includecpp ai ask "list all public methods" --all -x tests      # All except tests
```

Supports: module name, --file, --all, -x/--exclude

### AI Edit

Edit code with AI assistance:

```bash
includecpp ai edit "add logging to all methods" collision
includecpp ai edit "optimize the loop" --file include/utils.cpp
includecpp ai edit "add error handling" --all --think2          # Thorough mode
```

Flags:
- `--file` - specific files
- `--all` - all modules
- `-x/--exclude` - exclude modules
- `--think2` - thorough analysis (more tokens)

### AI Generate (Super Assistant)

The most powerful AI command - a full assistant with file operations and command execution.

```bash
# Basic usage
includecpp ai generate "add error handling to all functions" --file mymodule.cp

# Create new module from scratch
includecpp ai generate "fast SIMD math library" --t-new-module simd_math

# Planning mode (search, analyze, then execute)
includecpp ai generate "refactor for better performance" --t-plan --think2

# With Python file (auto-detect module usage)
includecpp ai generate "update api methods" --file mymod.cp --python main.py

# Full context mode
includecpp ai generate "comprehensive optimization" --t-max-context --think3
```

**Flags:**
| Flag | Description |
|------|-------------|
| `--file <path>` | Add files (multiple allowed). .cp files auto-resolve to source |
| `--think/2/3` | Extended context and planning |
| `--websearch` | Enable web research |
| `--t-max-context` | No context reduction |
| `--t-plan` | Search/grep before executing |
| `--t-new-module <name>` | Create new module (cpp + plugin + build) |
| `--python <file.py>` | Include Python file, auto-detect modules |
| `--confirm` | Skip confirmations |

### AI Tools

List available tools for the generate command:

```bash
includecpp ai tools
```

Available tools: READ_FILE, WRITE_FILE, EDIT_FILE, DELETE_FILE, CREATE_FOLDER, LIST_FOLDER, SEARCH_FILES, GREP, RUN_CMD, INCLUDECPP_CMD

### Build Error Analysis

When AI is enabled and `rebuild` fails, the build error is automatically analyzed:
- Root cause identification
- Code fix suggestions
- Prevention tips

### Configuration

```bash
includecpp ai --info          # Show status, model, usage stats
includecpp ai disable         # Disable without removing key
```

API key stored in `~/.includecpp/.secret`.

## Overloaded Methods

Specify the signature to disambiguate:

```
MODULE CLASS(Circle) {
    METHOD_CONST(intersects, const Circle&)
    METHOD_CONST(intersects, const Rect&)
}
```

## Template Instantiation

```
MODULE TEMPLATE_FUNC(maximum) TYPES(int, float, double)

MODULE STRUCT(Point) TYPES(int, float) {
    FIELD(x)
    FIELD(y)
}
```

Generates `maximum_int`, `maximum_float`, `maximum_double` and `Point_int`, `Point_float`.

## Module Dependencies

```
DEPENDS(math_utils, geometry)
```

Ensures dependent modules build first.

## VSCode IntelliSense

Generates `.pyi` stub files for autocomplete. Enable in `cpp.proj`:

```json
{
  "CPI-IntelliSense": true
}
```

## CPPY Code Conversion

Convert code between Python and C++ with full support for classes, functions, and type hints.

### Python to C++

```bash
includecpp cppy convert math_utils.py --cpp
includecpp cppy convert data.py --cpp --no-h  # Skip header
includecpp cppy convert src/*.py --cpp -o include/
```

Converts Python code to optimized C++ with:
- Type hints mapped to C++ types (int, str -> std::string, List -> std::vector)
- Classes with constructors, methods, fields
- Functions with proper signatures
- List comprehensions to STL algorithms
- Exception handling to try/catch

### C++ to Python

```bash
includecpp cppy convert utils.cpp --py
includecpp cppy convert mymodule.cp --py  # Auto-resolve SOURCE()
```

Converts C++ to Python with:
- STL types mapped to Python equivalents
- Classes with type hints
- Methods become class methods with self
- Structs become dataclasses

### AI-Assisted Conversion

```bash
includecpp cppy convert complex_lib.py --cpp --ai
includecpp cppy convert advanced.cpp --py --ai -v
```

The `--ai` flag enables intelligent conversion with:
- Section-by-section analysis and processing
- Automatic `--think2` context level
- Comprehensive rulebase for precise conversions
- pybind11 wrappers for Python features without C++ equivalents:
  - Generators -> callback pattern / py::iterator
  - Async/await -> std::async / std::future
  - Context managers -> RAII pattern
  - Duck typing -> templates with concepts
- Reports API changes to user
- Fallback to standard conversion if AI unavailable

**Workaround Examples:**
| Python Feature | C++ Workaround |
|----------------|----------------|
| `yield` | Callback pattern or py::iterator |
| `async/await` | std::async + std::future |
| `with` context | RAII ScopedResource class |
| List comprehension | transform_to_vector template |
| Dict comprehension | transform_to_map template |
| Dynamic attributes | py::object |

### Analyze Code

```bash
includecpp cppy analyze math.py           # View structure
includecpp cppy analyze utils.cpp --json  # JSON output
```

### Type Mapping

```bash
includecpp cppy types  # Show conversion tables
```

| Python | C++ |
|--------|-----|
| int | int |
| float | double |
| str | std::string |
| bool | bool |
| List[T] | std::vector<T> |
| Dict[K,V] | std::unordered_map<K,V> |
| Optional[T] | std::optional<T> |
| Tuple[...] | std::tuple<...> |

# Configuration

## cpp.proj

```json
{
  "project": "MyProject",
  "include": "/include",
  "plugins": "/plugins",
  "compiler": {
    "standard": "c++17",
    "optimization": "O3"
  }
}
```

Options:
- `project` - project name
- `include` - C++ source directory
- `plugins` - plugin file directory
- `compiler.standard` - C++ standard (c++11, c++14, c++17, c++20)
- `compiler.optimization` - optimization level (O0, O1, O2, O3)

# Requirements

- Python 3.8+
- C++ compiler (g++, clang++, MSVC)
- pybind11 (installed automatically)
- CMake (for build generation)

# Changelog

## v3.3.12
- **Smart Type Inference:**
  - Parameter types now inferred from common naming patterns (start/end → int, name/path → string, etc.)
  - Variable type tracking throughout conversion for accurate string detection
  - Loop variable types inferred from iterables (enumerate, for loops)
- **String Conversion Fix:**
  - No more `std::to_string()` on already-string variables in f-strings
  - `_is_string_expr()` method for comprehensive string type detection
  - String variables detected by name, type tracking, and method calls
- **AI Conversion Improvements:**
  - Explicit file extension enforcement (.cpp NOT .cp, .h NOT .hpp)
  - Better --no-h flag handling with clear AI instructions
  - AI can request clarification on unconvertible modules (tkinter, pygame, etc.)
  - User prompted for input when AI needs guidance

## v3.3.11
- **CPPY Converter Improvements:**
  - Added `_safe_arg()` and `_safe_get()` for robust bounds checking on all args
  - Added comprehensive try-except handling in `_convert_expr()` with warnings
  - Improved type inference with empty container handling and exception safety
  - Complete string escaping: `\0`, `\f`, `\b`, `\a`, `\v` now properly escaped
- **New Python Constructs:**
  - Dict comprehensions: `{k: v for k, v in items}` now converts to C++
  - Set comprehensions: `{x for x in items}` now converts to C++
  - Generator expressions: `(x for x in items)` now converts to vector
  - Tuple unpacking: `a, b = func()` now uses C++17 structured bindings
- **AI Conversion Flags:**
  - New `--think` flag for less context mode
  - New `--think3` flag for maximum context mode
  - New `--websearch` flag for web search in AI conversion
  - Default: `--think2` mode (unchanged behavior)

## v3.3.10
- **CPPY Converter seeded RNG fixes:**
  - Fixed `rng = random.Random(seed)` then `rng.randint()` - now properly tracks seeded RNG variables
  - Fixed `random.choices(...)[0]` subscript - returns single element directly without `[0]`
  - Seeded RNG methods (randint, uniform, choice, random) now use the tracked variable with proper C++ distributions

## v3.3.9
- **CPPY Converter fixes:**
  - Added f-string (JoinedStr) support - f"text {expr}" now converts to string concatenation
  - Fixed `random.Random(seed).method()` chained calls - now generates proper inline lambda with seeded RNG
  - Fixed `random.choices(items, weights=weights)` keyword argument handling
  - Improved string type detection in f-string expressions

## v3.3.8
- **Major rulebased converter improvements:**
  - Added Python `random` module support (randint, uniform, choice, sample, shuffle, gauss, etc.)
  - Added `os` module support (getcwd, path.join, path.exists, listdir, mkdir, etc.)
  - Added `time` module support (sleep, time, perf_counter, monotonic)
  - Added `sys` module support (exit, platform)
  - Added `math` module support (sqrt, pow, sin, cos, log, etc.)
  - Added `re` (regex) module support (match, search, sub, findall)
  - Added `threading` module support (Thread, Lock, Semaphore, etc.)
  - Added `collections` module support (deque, defaultdict, Counter)
  - Added `pathlib.Path` support
- **Unconvertible code detection:**
  - Automatically detects GUI frameworks (tkinter, PyQt, PySide, pygame) and other unconvertible modules
  - Shows red warning with line numbers when unconvertible code is found
  - New `--force` flag to convert anyway (with `/* UNCONVERTIBLE */` comments)
  - Supports 30+ modules in detection (numpy, pandas, flask, django, etc.)
- Fixed duplicate file output in `cppy convert --ai`
- Plugin command now skips inline and underscore-prefixed functions


---

MIT License | v3.3.12 | [GitHub](https://github.com/liliassg/IncludeCPP)
