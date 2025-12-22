# IncludeCPP - Use C++ in Python, The Easy Way

Write your performance-critical code in C++, use it in Python like it's native. No boilerplate, no manual binding code, no headaches.

## Why This Exists

Python is great, but sometimes you need speed. C++ extensions are fast but painful to set up. This tool bridges that gap - write C++ code, declare what you want to expose in a simple `.cp` file, and use it directly in Python. That's it.

## How To Get Started

### Install

```bash
pip install IncludeCPP
```

### Create Your First C++ Module. Do Following bash cmd in Your Projects Root Directory

**1. Initialize your project:**
```bash
python -m includecpp init
```

This creates:
- `cpp.proj` - Project config
- `include/` - Put your C++ files here
- `plugins/` - Module definitions go here

**2. Write some C++ code** (`include/math_tools.cpp`):

```cpp
#include <vector>
#include <algorithm>

namespace includecpp {

std::vector<int> fast_sort(std::vector<int> data) {
    std::sort(data.begin(), data.end());
    return data;
}

int add(int a, int b) {
    return a + b;
}

}
```

**3. Tell IncludeCPP what to expose** (`plugins/math_tools.cp`):

```
SOURCE(include/math_tools.cpp) math_tools

PUBLIC(
    math_tools FUNC(fast_sort)
    math_tools FUNC(add)
)
```

**4. Build it:**

```bash
python -m includecpp --rebuild
```

**5. Use it in Python:**

```python
import IncludeCPP as cpp

CPP = cpp.CppApi()
math = CPP.include("math_tools")

result = math.fast_sort([5, 2, 9, 1])
print(result)  # [1, 2, 5, 9]

print(math.add(10, 20))  # 30
```

That's it. No CMakeLists.txt, no setup.py, no pybind11 boilerplate.

## How It Works

1. **You write C++** - Normal C++ code in the `includecpp` namespace
2. **You declare exports** - Simple `.cp` files list what functions/classes to expose
3. **IncludeCPP generates bindings** - Automatic pybind11 code generation
4. **Everything builds to AppData** - Your project stays clean, builds are cached
5. **Import in Python** - Use your C++ code like any Python module

The system:
- Parses your `.cp` files
- Generates pybind11 bindings automatically
- Compiles everything with your system compiler
- Caches builds (rebuilds only what changed)
- Stores artifacts in `%APPDATA%/IncludeCPP` (Windows) or `~/.local/share/includecpp` (Linux)

## About The System

This is **v2.0** - a complete rewrite focused on making C++ integration actually easy.

### What's New in v2.0

**Plain-Old-Data Structs:**
```cpp
// In your .cp file:
geometry STRUCT(Point) TYPES(int, float) {
    FIELD(T, x)
    FIELD(T, y)
}
```

Auto-generates Python integration with `.to_dict()` and `.from_dict()` methods.

**Module Dependencies:**
```cpp
DEPENDS(math_utils, geometry)
```

Modules can depend on other modules. Build system handles the order automatically.

**Multi-File Modules:**
```cpp
SOURCES(module.cpp, helpers.cpp, utils.cpp)
```

Split your code across multiple files, treat them as one module.

### Type Support

Works with:
- Basic types: `int`, `float`, `double`, `string`, `bool`
- STL containers: `vector`, `map`, `set`, `array`
- Custom structs (v2.0+)
- Custom classes with methods
- Templates (define which types to instantiate)

### Features

- **Zero configuration** - Works out of the box
- **Fast incremental builds** - Only rebuilds what changed
- **Clean project structure** - Build artifacts separate from source
- **Template support** - Generate functions for multiple types
- **Thread-safe** - GIL release for parallel execution
- **Cross-platform** - Windows, Linux, macOS

## Advanced: Classes and Templates

**Expose a C++ class:**

```cpp
// include/game.cpp
class Player {
public:
    int health = 100;
    void damage(int amount) { health -= amount; }
    int get_health() { return health; }
};
```

```
// plugins/game.cp
SOURCE(include/game.cpp) game

PUBLIC(
    game CLASS(Player) {
        FIELD(health)
        METHOD(damage)
        METHOD(get_health)
    }
)
```

```python
# Use in Python
player = game.Player()
player.damage(25)
print(player.health)  # 75
```

**Template functions:**

```cpp
template<typename T>
T maximum(T a, T b) {
    return a > b ? a : b;
}
```

```
algorithms TEMPLATE_FUNC(maximum) TYPES(int, float, string)
```

```python
algorithms.maximum(10, 20)      # 20
algorithms.maximum(1.5, 2.3)    # 2.3
algorithms.maximum("a", "z")    # "z"
```

## Requirements

- Python 3.8+
- C++ compiler (g++, clang++, or MSVC)
- CMake 3.15+
- pybind11 (auto-installed)

## Configuration

Edit `cpp.proj` to customize:

```json
{
  "project": "MyProject",
  "version": "1.0.0",
  "include": "/include",
  "plugins": "/plugins",
  "compiler": {
    "standard": "c++17",
    "optimization": "O3",
    "flags": ["-Wall", "-pthread"]
  }
}
```

## The Next Generation of Coding

C++ gives you speed. Python gives you productivity. Why choose?

This tool is built for developers who:
- Need Python's ecosystem but C++'s performance
- Don't want to maintain complex build systems
- Value simplicity and fast iteration
- Want to write C++ without the usual pain

No corporate backing, no enterprise bloat. Just a tool that works.

---

**License:** MIT
**Version:** 2.0.0
**Status:** Production Ready
