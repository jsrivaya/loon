# Ring Buffer

A fixed-size circular buffer (ring buffer) with FIFO semantics and O(1) operations.

## Header

```cpp
#include <loon/ring_buffer.hpp>
```

## Overview

`loon::RingBuffer` provides O(1) push and pop operations with a compile-time fixed capacity. When full, it can either reject new elements or override the oldest element depending on configuration.

## Usage

```cpp
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

## API Reference

### Constructors

| Constructor | Description |
|-------------|-------------|
| `RingBuffer()` | Default constructor, rejects when full |
| `RingBuffer(bool override_when_full)` | If true, overwrites oldest element when full |

### Member Functions

| Operation | Return Type | Description |
|-----------|-------------|-------------|
| `push(value)` | `bool` | Push value to back. Returns false if full and override disabled |
| `pop()` | `std::optional<T>` | Remove and return front element |
| `front()` | `std::optional<T>` | Peek front element without removing |
| `back()` | `std::optional<T>` | Peek back element without removing |
| `discard()` | `bool` | Drop front element without returning |
| `size()` | `size_t` | Current number of elements |
| `capacity()` | `size_t` | Maximum capacity (N) |
| `empty()` | `bool` | True if buffer is empty |
| `full()` | `bool` | True if buffer is full |
| `overrides()` | `bool` | True if override mode is enabled |

## Complexity

| Operation | Time | Space |
|-----------|------|-------|
| `push(value)` | O(1) | O(1) |
| `pop()` | O(1) | O(1) |
| `front()` / `back()` | O(1) | O(1) |
| `discard()` | O(1) | O(1) |
| `size()` / `empty()` / `full()` | O(1) | O(1) |

## Performance Notes

- Stack-allocated `std::array` backing — zero heap allocation
- Fully contiguous memory layout
- Ideal for real-time, embedded, and latency-critical applications
- 3.2x faster than `std::queue` in benchmarks

See [Benchmarks](../benchmarks.md) for detailed performance comparisons.
