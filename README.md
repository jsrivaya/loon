[![Build Status](https://github.com/jsrivaya/loon/actions/workflows/ci.yml/badge.svg)](https://github.com/jsrivaya/loon/actions/workflows/ci.yml)
[![Documentation](https://github.com/jsrivaya/loon/actions/workflows/docs.yml/badge.svg)](https://jsrivaya.github.io/loon/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Header Only](https://img.shields.io/badge/header--only-yes-green.svg)]()

## loon – High-Performance, Header-Only C++ Library

**loon** is a lightweight, header-only C++ library designed for low memory footprint, low latency, and maximum performance. It provides optimized data structures and utilities that outperform standard STL counterparts while maintaining full STL compatibility.

---

### Key Features:
- Zero-Cost Abstractions: Header-only design with no external dependencies.
- STL-Compliant: Drop-in replacements with familiar interfaces.
- Cache-Efficient: Optimized memory layout (SoA, pooling, alignment).
- Low Latency: Avoids dynamic allocation on critical paths; ideal for real-time systems.
- High Performance: Benchmarked against std:: types—faster insertion, lookup, and iteration.

Perfect for performance-critical applications in HFT, gaming, embedded systems, and real-time processing.

---

### Components

| Header | Description |
|--------|-------------|
| `loon/lru.hpp` | LRU cache with O(1) get/put operations |
| `loon/redis_list.hpp` | Redis-style list with lpush/rpush/lpop/rpop/lrange |

---

### Installation

#### Using Conan (recommended)

Add to your `conanfile.txt`:
```ini
[requires]
loon/0.1.0
```

Or `conanfile.py`:
```python
def requirements(self):
    self.requires("loon/0.1.0")
```

#### Header-only (manual)

Copy the `include/loon` directory to your project and add it to your include path.

---

### Usage

#### CMake

```cmake
find_package(loon REQUIRED)
target_link_libraries(your_target PRIVATE loon::loon)
```

#### LRU Cache

```cpp
#include <loon/lru.hpp>

loon::LRU<int, std::string> cache(100);  // capacity of 100

cache.put(1, "hello");
cache.put(2, "world");

auto val = cache.get(1);  // returns std::optional<std::reference_wrapper<V>>
if (val) {
    std::cout << val->get() << std::endl;  // "hello"
}

cache.exists(1);  // true
cache.remove(1);
cache.size();     // 1
```

**LRU Cache Complexity:**

| Operation | Time | Space |
|-----------|------|-------|
| `get(key)` | O(1) | O(1) |
| `put(key, value)` | O(1) | O(1) |
| `exists(key)` | O(1) | O(1) |
| `remove(key)` | O(1) | O(1) |
| `size()` | O(1) | O(1) |

#### Redis List

```cpp
#include <loon/redis_list.hpp>

loon::RedisList<int> list;

list.lpush(1);       // push to front
list.rpush(2);       // push to back
list.lpush(0);       // [0, 1, 2]

auto val = list.lpop();      // returns std::optional<T>, removes from front
auto vals = list.rpop(2);    // pop multiple from back

auto range = list.lrange(0, -1);  // get all elements (supports negative indices)
list.llen();  // size
```

**Redis List Complexity:**

| Operation | Time | Space |
|-----------|------|-------|
| `lpush(value)` | O(1) | O(1) |
| `rpush(value)` | O(1) | O(1) |
| `lpop()` | O(1) | O(1) |
| `rpop()` | O(1) | O(1) |
| `lpop(count)` | O(count) | O(count) |
| `rpop(count)` | O(count) | O(count) |
| `lrange(start, stop)` | O(stop - start) | O(stop - start) |
| `llen()` / `size()` | O(1) | O(1) |
| `empty()` | O(1) | O(1) |

---

### Building from Source

#### Prerequisites
- C++23 compatible compiler (GCC 13+, Clang 14+)
- CMake 3.20+
- Conan 2.x

#### Build and Test

```bash
# Setup Conan (one-time)
make conan-setup

# Install dependencies
make deps

# Build and run tests
make build

# Create and test Conan package
make package
```

#### Other Targets

```bash
make help              # Show all targets
make coverage          # Generate coverage report
make check-format      # Check code formatting
make format            # Apply clang-format
make clean             # Clean build files
```

---

### License

MIT License - see [LICENSE](LICENSE) for details.
