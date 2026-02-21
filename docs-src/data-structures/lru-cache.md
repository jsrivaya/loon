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
| `get` (hit) | 15.0 ns | 67M ops/s |
| `get` (miss) | 10.6 ns | 95M ops/s |
| `put` | 255 ns | 4M ops/s |
| `exists` | 7.8 ns | 130M ops/s |
| Mixed (80% read, 20% write) | 68 ns | 15M ops/s |

### Value Size Impact

| Value Size | Time | Throughput |
|------------|------|------------|
| 16 bytes | 13.6 ns | 1.1 GiB/s |
| 64 bytes | 14.2 ns | 4.2 GiB/s |
| 256 bytes | 16.3 ns | 14.7 GiB/s |

### vs std::unordered_map

Comparison with raw hash map (no LRU tracking):

| Operation | LRU Cache | unordered_map | Overhead |
|-----------|-----------|---------------|----------|
| `get` (hit) | 15.0 ns | 7.5 ns | 2x |
| `put` | 255 ns | 49 ns | 5x |
| `exists` | 7.8 ns | 7.5 ns | ~1x |

The overhead is expected for LRU tracking: `get` must update the recency list, and `put` handles eviction.

### vs Other Open Source Implementations

Comparison with popular C++ LRU cache libraries:

| Implementation | Get (hit) | Algorithm | Thread-safe |
|----------------|-----------|-----------|-------------|
| **loon::LRU** | 15 ns (67M ops/s) | True LRU | No |
| [LruClockCache](https://github.com/tugrul512bit/LruClockCache) | 16 ns (50M ops/s) | CLOCK (approx) | Yes |
| [nitnelave/lru_cache](https://github.com/nitnelave/lru_cache) | ~26 µs/100k ops | True LRU | No |
| [Cachelot](https://cachelot.io/) | ~333 ns (3M ops/s) | LRU | Yes |

**Key differences:**

| Feature | loon::LRU | LruClockCache |
|---------|-----------|---------------|
| Eviction policy | True LRU | CLOCK approximation |
| Memory layout | `std::list` nodes | Pre-allocated array |
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
- Doubly-linked list for O(1) recency tracking and eviction

This provides constant-time operations for all cache operations while maintaining proper LRU ordering.
