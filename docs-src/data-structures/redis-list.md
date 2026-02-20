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
