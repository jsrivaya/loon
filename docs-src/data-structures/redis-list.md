# Redis List

A Redis-style doubly-linked list with lpush/rpush/lpop/rpop/lrange operations.

## Header

```cpp
#include <loon/redis_list.hpp>
```

## Overview

`loon::RedisList` provides a Redis-compatible list interface with efficient operations at both ends. It supports negative indexing like Redis for range operations.

## Usage

```cpp
loon::RedisList<int> list;

list.lpush(1);       // push to front
list.rpush(2);       // push to back
list.lpush(0);       // [0, 1, 2]

auto val = list.lpop();      // returns std::optional<T>, removes from front
auto vals = list.rpop(2);    // pop multiple from back

auto range = list.lrange(0, -1);  // get all elements (supports negative indices)
list.llen();  // size
```

## API Reference

### Member Functions

| Operation | Return Type | Description |
|-----------|-------------|-------------|
| `lpush(value)` | `void` | Push value to front |
| `rpush(value)` | `void` | Push value to back |
| `lpop()` | `std::optional<T>` | Remove and return front element |
| `rpop()` | `std::optional<T>` | Remove and return back element |
| `lpop(count)` | `std::vector<T>` | Remove and return multiple from front |
| `rpop(count)` | `std::vector<T>` | Remove and return multiple from back |
| `lrange(start, stop)` | `std::vector<T>` | Get elements in range (supports negative indices) |
| `llen()` | `size_t` | Number of elements |
| `size()` | `size_t` | Alias for llen() |
| `empty()` | `bool` | True if list is empty |

## Complexity

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

## Index Semantics

Like Redis, `lrange` supports negative indices:

- `0` is the first element
- `-1` is the last element
- `-2` is the second to last element, etc.

```cpp
list.lrange(0, -1);   // all elements
list.lrange(0, 0);    // first element only
list.lrange(-3, -1);  // last 3 elements
```

## Performance

### Operation Latency

| Operation | Time | Throughput |
|-----------|------|------------|
| `lpush` / `rpush` | 4.4 ns | 520M ops/s |
| `lpop` / `rpop` | 28 ns | 35M ops/s |
| Interleaved push/pop | 3.6 ns | 560M ops/s |
| `lrange` (10 elements) | 92 ns | 109M ops/s |
| `llen` | 0.33 ns | 3B ops/s |

### Value Size Impact

| Value Size | Time | Throughput |
|------------|------|------------|
| 16 bytes | 3.8 ns | 7.8 GiB/s |
| 64 bytes | 7.3 ns | 16.3 GiB/s |
| 256 bytes | 22.6 ns | 21.1 GiB/s |

### vs std::deque

RedisList uses `std::deque` internally, so performance is nearly identical:

| Operation | RedisList | std::deque | Overhead |
|-----------|-----------|------------|----------|
| push_back | 266 ns/64 | 269 ns/64 | ~1x |
| pop_front | 1.8 µs/64 | 1.8 µs/64 | ~1x |
| Interleaved | 3.6 ns | 3.2 ns | 1.1x |

The minimal overhead (~10%) comes from `std::optional` wrapping in pop operations.

### vs std::list

RedisList significantly outperforms `std::list` due to cache-friendly deque storage:

| Operation | RedisList | std::list | Speedup |
|-----------|-----------|-----------|---------|
| push (4096) | 8.2 µs | 564 µs | **69x** |
| pop (4096) | 11.8 µs | 320 µs | **27x** |
| Interleaved | 3.6 ns | 78.5 ns | **22x** |

### Why It's Fast

- **Deque backing**: Uses `std::deque` for O(1) operations at both ends
- **Cache-friendly**: Contiguous chunk storage vs linked list nodes
- **No allocation per element**: Deque allocates in chunks
- **Move semantics**: Supports efficient moves for large values

See [Benchmarks](../benchmarks.md) for full results.

## Typical Use Cases

- Message queues (producer/consumer)
- Task scheduling (FIFO/LIFO)
- Undo/redo stacks
- Sliding window algorithms
- Event buffering
