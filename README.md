# IncludeCpp

Professional C++ ↔ Python binding system with type-generic templates and native threading support.

## Features

- Type-generic functions (automatic type detection)
- Native threading with GIL release
- Python asyncio integration
- Dynamic containers (Map, Array, Vector)
- Professional CLI tools
- AppData build system

## Installation

```bash
pip install IncludeCpp
```

## Quick Start

```bash
# Create new project
mkdir MyProject && cd MyProject
includecpp init

# Create C++ module
echo 'int add(int a, int b) { return a + b; }' > include/math.cpp

# Create plugin definition
cat > plugins/math.cp << EOF
SOURCE(include/math.cpp) math
PUBLIC(
    math FUNC(add)
)
EOF

# Build
includecpp --rebuild
```

## Usage

```python
import IncludeCpp as cpp

CPP = cpp.CppApi()

# Use algorithms
algorithms = CPP.include("algorithms")
sorted_data = algorithms.fast_sort([5, 2, 9, 1])

# Use threading
game = CPP.include("threading").GameEngine()
CPP.threadThis(game)
game.startGame()

# Use containers
Map = CPP.include("map")
my_map = Map.FastMap_string_int()
my_map.set("count", 42)
```

## License

MIT License
