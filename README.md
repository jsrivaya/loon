[![Build Status](https://github.com/jsrivaya/loon/actions/workflows/ci.yml/badge.svg)](https://github.com/jsrivaya/loon/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/jsrivaya/loon/graph/badge.svg)](https://codecov.io/gh/jsrivaya/loon)
[![Documentation](https://github.com/jsrivaya/loon/actions/workflows/docs.yml/badge.svg)](https://jsrivaya.github.io/loon/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Header Only](https://img.shields.io/badge/header--only-yes-green.svg)]() 

<p align="center">
  <img src="assets/banner-embedded.svg" alt="Loon Banner">
</p>

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
| `loon/spsc.hpp` | Lock-free single-producer single-consumer queue |

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

#### SPSC Queue

```cpp
#include <loon/spsc.hpp>

loon::SpscQueue<int, 1024> queue;  // fixed capacity of 1024

// Producer thread
queue.push(42);
queue.push(43);

// Consumer thread
int value;
if (queue.pop(value)) {
    std::cout << value << std::endl;  // 42
}

queue.empty();     // check if empty
queue.full();      // check if full
queue.capacity();  // 1024
```

**SPSC Queue Complexity:**

| Operation | Time | Space |
|-----------|------|-------|
| `push(value)` | O(1) | O(1) |
| `pop(value)` | O(1) | O(1) |
| `empty()` / `full()` | O(1) | O(1) |
| `capacity()` | O(1) | O(1) |

**Performance notes:** Lock-free implementation using atomic operations. Safe for exactly one producer thread and one consumer thread without any mutexes. Ideal for inter-thread communication in real-time and low-latency systems.

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
