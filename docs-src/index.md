# loon

**High-Performance, Header-Only Modern C++ Library**

loon is a lightweight, header-only C++ library designed for low memory footprint, low latency, and maximum performance. It provides optimized data structures and utilities that outperform standard STL counterparts while maintaining full STL compatibility.

## Key Features

- **Zero-Cost Abstractions**: Header-only design with no external dependencies
- **STL-Compliant**: Drop-in replacements with familiar interfaces
- **Cache-Efficient**: Optimized memory layout (SoA, pooling, alignment)
- **Low Latency**: Avoids dynamic allocation on critical paths; ideal for real-time systems
- **High Performance**: Benchmarked against std:: types—faster insertion, lookup, and iteration

Perfect for performance-critical applications in HFT, gaming, embedded systems, and real-time processing.

## Components

| Header | Description |
|--------|-------------|
| [`loon/ring_buffer.hpp`](data-structures/ring-buffer.md) | Fixed-size ring buffer (circular queue) with O(1) push/pop |
| [`loon/lru.hpp`](data-structures/lru-cache.md) | LRU cache with O(1) get/put operations |
| [`loon/redis_list.hpp`](data-structures/redis-list.md) | Redis-style list with lpush/rpush/lpop/rpop/lrange |
| [`loon/spsc.hpp`](data-structures/spsc-queue.md) | Lock-free single-producer single-consumer queue |

## Quick Example

```cpp
#include <loon/ring_buffer.hpp>

loon::RingBuffer<int, 1024> buffer;
buffer.push(42);
buffer.push(43);

auto val = buffer.pop();  // returns 42
```

## License

MIT License - see [LICENSE](https://github.com/jsrivaya/loon/blob/main/LICENSE) for details.
