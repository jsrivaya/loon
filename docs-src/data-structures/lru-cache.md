# LRU Cache

An LRU (Least Recently Used) cache with O(1) get/put operations.

## Header

```cpp
#include <loon/lru.hpp>
```

## Overview

`loon::LRU` implements a fixed-capacity cache that automatically evicts the least recently used items when the capacity is exceeded. All operations (get, put, exists, remove) are O(1).

## Usage

```cpp
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

## API Reference

### Constructors

| Constructor | Description |
|-------------|-------------|
| `LRU(size_t capacity)` | Create cache with specified capacity |

### Member Functions

| Operation | Return Type | Description |
|-----------|-------------|-------------|
| `get(key)` | `std::optional<std::reference_wrapper<V>>` | Get value by key, marks as recently used |
| `put(key, value)` | `void` | Insert or update key-value pair |
| `exists(key)` | `bool` | Check if key exists (does not update recency) |
| `remove(key)` | `bool` | Remove key from cache |
| `size()` | `size_t` | Current number of entries |
| `capacity()` | `size_t` | Maximum capacity |
| `clear()` | `void` | Remove all entries |

## Complexity

| Operation | Time | Space |
|-----------|------|-------|
| `get(key)` | O(1) | O(1) |
| `put(key, value)` | O(1) | O(1) |
| `exists(key)` | O(1) | O(1) |
| `remove(key)` | O(1) | O(1) |
| `size()` | O(1) | O(1) |

## Performance

### Operation Latency

| Operation | Time | Throughput |
|-----------|------|------------|
| `get` (hit) | 12.0 ns | 83M ops/s |
| `get` (miss) | 9.75 ns | 103M ops/s |
| `put` | 109 ns | 9.2M ops/s |
| `exists` | 7.4 ns | 136M ops/s |
| Mixed (80% read, 20% write) | 37.5 ns | 26.7M ops/s |
| Eviction stress | 114 ns | 8.8M ops/s |
| Random access | 126 ns | 7.9M ops/s |

### Value Size Impact

| Value Size | Time | Throughput |
|------------|------|------------|
| 16 bytes | 9.22 ns | 1.6 GiB/s |
| 64 bytes | 9.92 ns | 6.0 GiB/s |
| 256 bytes | 13.6 ns | 17.5 GiB/s |

### vs std::unordered_map

Comparison with raw hash map (no LRU tracking):

| Operation | LRU Cache | unordered_map | Overhead |
|-----------|-----------|---------------|----------|
| `get` (hit) | 12.0 ns | 7.1 ns | 1.7x |
| `put` | 109 ns | 45 ns | 2.4x |
| `exists` | 7.4 ns | 7.1 ns | ~1x |

The overhead is expected for LRU tracking: `get` must update the recency list, and `put` handles eviction.

### vs Other Open Source Implementations

Comparison with popular C++ LRU cache libraries:

| Implementation | Get (hit) | Algorithm | Thread-safe |
|----------------|-----------|-----------|-------------|
| **loon::LRU** | 12.0 ns (83M ops/s) | True LRU | No |
| [LruClockCache](https://github.com/tugrul512bit/LruClockCache) | 16 ns (50M ops/s) | CLOCK (approx) | Yes |
| [nitnelave/lru_cache](https://github.com/nitnelave/lru_cache) | ~26 µs/100k ops | True LRU | No |
| [Cachelot](https://cachelot.io/) | ~333 ns (3M ops/s) | LRU | Yes |

**Key differences:**

| Feature | loon::LRU | LruClockCache |
|---------|-----------|---------------|
| Eviction policy | True LRU | CLOCK approximation |
| Memory layout | Pre-allocated array | Pre-allocated array |
| Multi-threaded reads | No | Up to 2.5B ops/s |
| Dependencies | None (header-only) | None |

loon::LRU provides **true LRU ordering** with competitive read performance. For multi-threaded read-heavy workloads, consider [LruClockCache](https://github.com/tugrul512bit/LruClockCache) which trades exact LRU for higher throughput.

### Why It's Fast

- **Hash map lookup**: O(1) key access via `std::unordered_map`
- **Intrusive list**: Recency updates without allocation
- **Single eviction**: Only the LRU item is removed when full
- **Reference semantics**: `get` returns a reference, avoiding copies

See [Benchmarks](../benchmarks.md) for full results.

## Typical Use Cases

- Database query caching
- API response caching
- Configuration lookups
- Session management
- DNS resolution caching

## Implementation Details

The LRU cache uses a combination of:

- Hash map for O(1) key lookup
- Pre-allocated intrusive doubly-linked list for O(1) recency tracking and eviction

All nodes are pre-allocated in a contiguous `std::vector` at construction. A free list threads through unused nodes so insert and eviction never touch the heap. Nodes are linked by `uint32_t` indices (4 bytes each vs 8 for pointers), reducing node size and improving cache density.
