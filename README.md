[![Build Status](https://github.com/jsrivaya/loon/actions/workflows/ci.yml/badge.svg)](https://github.com/jsrivaya/loon/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/jsrivaya/loon/graph/badge.svg)](https://codecov.io/gh/jsrivaya/loon)
[![Documentation](https://github.com/jsrivaya/loon/actions/workflows/docs.yml/badge.svg)](https://jsrivaya.github.io/loon/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Header Only](https://img.shields.io/badge/header--only-yes-green.svg)]() 

<svg width="600" height="300" viewBox="0 0 600 300" xmlns="http://www.w3.org/2000/svg" overflow="hidden">
  <style>
    @font-face {
        font-display: swap; /* Check https://developer.mozilla.org/en-US/docs/Web/CSS/@font-face/font-display for other options. */
        font-family: 'Press Start 2P';
        font-style: normal;
        font-weight: 400;
        src: url('../fonts/press-start-2p-v16-latin-regular.woff2') format('woff2'); /* Chrome 36+, Opera 23+, Firefox 39+, Safari 12+, iOS 10+ */
    }
    h1 {
        font-family: 'Press Start 2P', monospace;
    }
    .title {
      font-family: 'Press Start 2P', monospace;
      font-size: 32px;
      fill: #1a365d;
    }
    .subtitle {
      font-family: 'JetBrains Mono', monospace;
      font-size: 14px;
      fill: #2d3748;
    }
  </style>
  <foreignObject width="100%" height="100%">
    <div xmlns="http://www.w3.org/1999/xhtml" style="text-align:center; padding:20px; background:linear-gradient(135deg, #f1f5f9 0%, #e2e8f0 100%); border-radius:12px;">
      <h1 style="font-size:32px; margin:0; color:#1a365d;">loon</h1>
      <p style="font-family:'JetBrains Mono', monospace; font-size:14px; color:#2d3748; opacity:0.7; margin:8px 0 0;">low latency, maximum performance</p>
        <svg width="160" height="160" viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg">
            <!-- 16x16 pixel loon - refined original -->
            <!-- Body -->
            <rect x="1" y="10" width="2" height="2" fill="#1a365d"/>
            <rect x="3" y="9" width="2" height="3" fill="#1a365d"/>
            <rect x="5" y="8" width="2" height="4" fill="#1a365d"/>
            <rect x="7" y="9" width="2" height="3" fill="#1a365d"/>
            <rect x="9" y="10" width="1" height="2" fill="#1a365d"/>
            <!-- White chest -->
            <rect x="3" y="10" width="1" height="2" fill="white"/>
            <rect x="4" y="9" width="2" height="2" fill="white"/>
            <rect x="6" y="10" width="1" height="1" fill="white"/>
            <!-- Neck -->
            <rect x="8" y="7" width="2" height="2" fill="#1a365d"/>
            <rect x="9" y="5" width="2" height="2" fill="#1a365d"/>
            <!-- Head -->
            <rect x="10" y="3" width="3" height="3" fill="#1a365d"/>
            <!-- Eye -->
            <rect x="11" y="4" width="1" height="1" fill="white"/>
            <!-- Beak -->
            <rect x="13" y="4" width="2" height="1" fill="#4a5568"/>
        </svg>
    </div>
  </foreignObject>
</svg>


![Loon Banner](assets/banner.svg)

## loon – High-Performance, Header-Only Modern C++ Library

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
| `loon/ring_buffer.hpp` | Fixed-size ring buffer (circular queue) with O(1) push/pop |

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

#### Ring Buffer

```cpp
#include <loon/ring_buffer.hpp>

loon::RingBuffer<int, 1024> buffer;          // fixed capacity, reject when full
loon::RingBuffer<int, 1024> ring(true);      // override oldest when full

buffer.push(42);
buffer.push(43);

auto val = buffer.pop();       // returns std::optional<int>, removes front
auto f = buffer.front();       // peek front without removing
auto b = buffer.back();        // peek back without removing

buffer.discard();              // drop front element without returning it
buffer.size();                 // current element count
buffer.empty();                // true if no elements
buffer.full();                 // true if at capacity
```

**Ring Buffer Complexity:**

| Operation | Time | Space |
|-----------|------|-------|
| `push(value)` | O(1) | O(1) |
| `pop()` | O(1) | O(1) |
| `front()` / `back()` | O(1) | O(1) |
| `discard()` | O(1) | O(1) |
| `size()` / `empty()` / `full()` | O(1) | O(1) |

**Performance notes:** Stack-allocated `std::array` backing — zero heap allocation, fully contiguous memory layout. Ideal for real-time, embedded, and latency-critical applications.

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
