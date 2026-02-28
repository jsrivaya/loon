[![Build Status](https://github.com/jsrivaya/loon/actions/workflows/ci.yml/badge.svg)](https://github.com/jsrivaya/loon/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/jsrivaya/loon/graph/badge.svg)](https://codecov.io/gh/jsrivaya/loon)
[![Latest Release](https://img.shields.io/github/v/release/jsrivaya/loon)](https://github.com/jsrivaya/loon/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Header Only](https://img.shields.io/badge/header--only-yes-green.svg)]()

<p align="center">
  <img src="assets/banner-embedded.svg" alt="Loon Banner">
</p>

## loon – High-Performance, Header-Only Modern C++ Library

**loon** is a lightweight, header-only C++ library designed for low memory footprint, low latency, and maximum performance. It provides optimized data structures that outperform standard STL counterparts while maintaining familiar interfaces.

Perfect for **HFT**, **gaming**, **embedded systems**, and **real-time processing**.

### Key Features

| Feature | Description |
|---------|-------------|
| **Zero-Cost Abstractions** | Header-only, no external dependencies |
| **STL-Compliant** | Drop-in replacements with familiar APIs |
| **Cache-Efficient** | Optimized memory layout, contiguous storage |
| **Low Latency** | No dynamic allocation on critical paths |
| **High Performance** | Up to 3-4x faster than std:: alternatives |

---

## Data Structures

| Structure | Header | Description | Performance |
|-----------|--------|-------------|-------------|
| [**RingBuffer**](https://jsrivaya.github.io/loon/data-structures/ring-buffer/) | `loon/ring_buffer.hpp` | Fixed-size circular queue | **3.2x** faster than `std::queue` |
| [**SpscQueue**](https://jsrivaya.github.io/loon/data-structures/spsc-queue/) | `loon/spsc.hpp` | Lock-free single-producer single-consumer queue | **18.7x** faster than mutex queue |
| [**LRU Cache**](https://jsrivaya.github.io/loon/data-structures/lru-cache/) | `loon/lru.hpp` | Least Recently Used cache | O(1) get/put |
| [**RedisList**](https://jsrivaya.github.io/loon/data-structures/redis-list/) | `loon/redis_list.hpp` | Redis-style doubly-linked list | O(1) push/pop |

### Complexity Summary

| Structure | push | pop | get | put | front/back |
|-----------|------|-----|-----|-----|------------|
| RingBuffer | O(1) | O(1) | - | - | O(1) |
| SpscQueue | O(1) | O(1) | - | - | - |
| LRU Cache | - | - | O(1) | O(1) | - |
| RedisList | O(1) | O(1) | - | - | O(1) |

### Benchmark Highlights

| Benchmark | loon | std/mutex | Speedup |
|-----------|------|-----------|---------|
| RingBuffer round-trip (16B) | 1.9 ns | 6.1 ns | 3.2x |
| SpscQueue interleaved | 2.40 ns | 44.9 ns | 18.7x |
| SpscQueue producer/consumer | 485M ops/s | 26M ops/s | 19.5x |
| LRU Cache get (hit) | 12.0 ns | - | 83M ops/s |
| LRU Cache exists | 7.4 ns | - | 136M ops/s |
| RedisList push/pop | 3.6 ns | 78.5 ns | 22x vs std::list |

See [full benchmarks](https://jsrivaya.github.io/loon/benchmarks/) for detailed results.

---

## Installation

### Using Conan (recommended)

```ini
# conanfile.txt
[requires]
loon/0.2.0
```

```python
# conanfile.py
def requirements(self):
    self.requires("loon/0.2.0")
```

### Header-only (manual)

Copy the `include/loon` directory to your project and add it to your include path.

### CMake

```cmake
find_package(loon REQUIRED)
target_link_libraries(your_target PRIVATE loon::loon)
```

---

## Quick Start

### [RingBuffer](https://jsrivaya.github.io/loon/data-structures/ring-buffer/)

```cpp
#include <loon/ring_buffer.hpp>

loon::RingBuffer<int, 1024> buffer;       // fixed capacity, reject when full
loon::RingBuffer<int, 1024> ring(true);   // override oldest when full

buffer.push(42);
auto val = buffer.pop();    // std::optional<int>
buffer.front();             // peek without removing
```

### [SPSC Queue](https://jsrivaya.github.io/loon/data-structures/spsc-queue/)

```cpp
#include <loon/spsc.hpp>

loon::SpscQueue<int, 1024> queue;

// Producer thread
queue.push(42);

// Consumer thread
int value;
if (queue.pop(value)) {
    // use value
}
```

### [LRU Cache](https://jsrivaya.github.io/loon/data-structures/lru-cache/)

```cpp
#include <loon/lru.hpp>

loon::LRU<int, std::string> cache(100);  // capacity of 100

cache.put(1, "hello");
auto val = cache.get(1);  // std::optional<std::reference_wrapper<V>>
if (val) {
    std::cout << val->get();  // "hello"
}
```

### [RedisList](https://jsrivaya.github.io/loon/data-structures/redis-list/)

```cpp
#include <loon/redis_list.hpp>

loon::RedisList<int> list;

list.lpush(1);                    // push front
list.rpush(2);                    // push back
auto val = list.lpop();           // pop front
auto range = list.lrange(0, -1);  // get all (supports negative indices)
```

---

## Building from Source

### Prerequisites

- C++23 compatible compiler (GCC 13+, Clang 14+)
- CMake 3.20+
- Conan 2.x

### Build & Test

```bash
make conan-setup    # one-time setup
make deps           # install dependencies
make build          # build and run tests
make package        # create Conan package
```

### Other Targets

```bash
make help           # show all targets
make bench          # run benchmarks
make coverage       # generate coverage report
make docs-serve     # serve documentation locally
make format         # apply clang-format
```

---

## Documentation

Full documentation available at **[jsrivaya.github.io/loon](https://jsrivaya.github.io/loon/)**

- [Getting Started](https://jsrivaya.github.io/loon/getting-started/)
- [API Reference](https://jsrivaya.github.io/loon/api/)
- [Benchmarks](https://jsrivaya.github.io/loon/benchmarks/)

---

## License

MIT License - see [LICENSE](LICENSE) for details.
