# IncludeCPP

Write C++ code, use it in Python. Auto-generates pybind11 bindings.

```bash
pip install IncludeCPP
```

## Setup

```bash
includecpp init
```

Creates `cpp.proj`, `include/`, `plugins/`.

## Write C++ Code

**Put your code in `namespace includecpp`:**

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

Without `namespace includecpp`, your code won't be found.

## Generate Plugin

```bash
includecpp plugin fast_list include/fast_list.cpp
```

Creates `plugins/fast_list.cp` with detected classes, methods, functions.

## Build

```bash
includecpp rebuild
```

Or `includecpp build` (same thing).

## Use in Python

```python
from includecpp import fast_list

my_list = fast_list.FastList()
my_list.append(42)
print(fast_list.add(1, 2))  # 3
```

Or classic style:

```python
from includecpp import CppApi

api = CppApi()
fast_list = api.include("fast_list")
```

## Commands

| Command | What it does |
|---------|--------------|
| `init` | Create project structure |
| `plugin <name> <files>` | Generate .cp from C++ |
| `rebuild` / `build` | Compile modules |
| `get <module>` | Show module API |
| `install <name>` | Install community module |
| `update` | Update IncludeCPP |
| `bug` | Report issue |
| `--doc` | Show docs |

### rebuild options

```bash
includecpp rebuild --clean          # Full rebuild
includecpp rebuild --verbose        # See compiler output
includecpp rebuild -m crypto        # Rebuild specific module
includecpp rebuild -j 8             # 8 parallel jobs
```

## Plugin Syntax (.cp files)

```
SOURCE(fast_list.cpp) fast_list

PUBLIC:

fast_list CLASS(FastList) {
    CONSTRUCTOR()
    METHOD(append)
    METHOD(get)
}

fast_list FUNC(add)
```

### Overloaded Methods

```
MODULE CLASS(Circle) {
    METHOD_CONST(intersects, const Circle&)
    METHOD_CONST(intersects, const Rect&)
}
```

### Templates

```
MODULE TEMPLATE_FUNC(maximum) TYPES(int, float, double)

MODULE STRUCT(Point) TYPES(int, float) {
    FIELD(T, x)
    FIELD(T, y)
}
```

### Dependencies

```
DEPENDS(math_utils, geometry)
```

## VSCode Support

Generates `.pyi` stubs automatically. Full autocomplete works.

Enable in `cpp.proj`:

```json
{
  "CPI-IntelliSense": true
}
```

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

## Requirements

- Python 3.8+
- C++ compiler (g++, clang++, MSVC)
- pybind11 (installed automatically)

## Notes

- Builds go to AppData/IncludeCPP, source stays clean
- SHA256 change detection, only rebuilds what changed
- Parallel compilation by default

---

MIT License | v2.9.4 | [GitHub](https://github.com/liliassg/IncludeCPP)
