# FastList Module for IncludeCPP

High-performance list operations module with optimized C++ implementations.

## Installation

```bash
python -m includecpp install fast_list
python -m includecpp rebuild
```

## Features

### FastList Class
A fast integer list implementation with direct access to underlying vector.

**Methods:**
- `add(value: int)` - Add an integer to the list
- `get(index: int) -> int` - Get value at index (returns 0 if out of bounds)
- `size() -> int` - Get list size
- `clear()` - Clear all elements
- `data: List[int]` - Direct access to underlying vector (zero-copy)

### Utility Functions

- `fast_sort(input: List[int]) -> List[int]` - Sort a list in ascending order
- `fast_reverse(input: List[int]) -> List[int]` - Reverse a list
- `fast_sum(input: List[int]) -> int` - Sum all elements (uses int64 to prevent overflow)
- `fast_max(input: List[int]) -> int` - Find maximum value (returns 0 if empty)
- `fast_min(input: List[int]) -> int` - Find minimum value (returns 0 if empty)

## Usage Example

```python
from includecpp import CppAPI

api = CppAPI()
fast_list = api.include('fast_list')

# Using FastList class
my_list = fast_list.FastList()
my_list.add(10)
my_list.add(20)
my_list.add(30)
print(my_list.size())  # 3
print(my_list.get(1))  # 20

# Direct data access (zero-copy with numpy)
import numpy as np
arr = np.array(my_list.data, copy=False)
arr[0] = 999
print(my_list.get(0))  # 999

# Utility functions
numbers = [5, 2, 8, 1, 9]
sorted_nums = fast_list.fast_sort(numbers)  # [1, 2, 5, 8, 9]
reversed_nums = fast_list.fast_reverse(numbers)  # [9, 1, 8, 2, 5]
total = fast_list.fast_sum(numbers)  # 25
max_val = fast_list.fast_max(numbers)  # 9
min_val = fast_list.fast_min(numbers)  # 1
```

## Performance

All operations are implemented in C++ with optimized STL algorithms:
- Sorting uses `std::sort` (O(n log n))
- Min/Max use `std::min_element`/`std::max_element` (O(n))
- Sum uses `std::accumulate` with int64 accumulator
- Zero-copy data access via pybind11 buffer protocol

## Requirements

- IncludeCPP >= 2.0.2
- C++11 or later
- Python >= 3.8
