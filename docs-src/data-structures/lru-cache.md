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

## Implementation Details

The LRU cache uses a combination of:

- Hash map for O(1) key lookup
- Doubly-linked list for O(1) recency tracking and eviction

This provides constant-time operations for all cache operations while maintaining proper LRU ordering.
