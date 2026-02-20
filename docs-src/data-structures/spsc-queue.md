# SPSC Queue

A lock-free single-producer single-consumer queue.

## Header

```cpp
#include <loon/spsc.hpp>
```

## Overview

`loon::SpscQueue` is a lock-free queue designed for exactly one producer thread and one consumer thread. It uses atomic operations instead of mutexes, making it ideal for inter-thread communication in low-latency systems.

## Usage

```cpp
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

## API Reference

### Constructors

| Constructor | Description |
|-------------|-------------|
| `SpscQueue()` | Default constructor |

### Member Functions

| Operation | Return Type | Description |
|-----------|-------------|-------------|
| `push(value)` | `bool` | Push value (returns false if full) |
| `pop(value&)` | `bool` | Pop into reference (returns false if empty) |
| `empty()` | `bool` | True if queue is empty |
| `full()` | `bool` | True if queue is full |
| `capacity()` | `size_t` | Maximum capacity (N) |

## Complexity

| Operation | Time | Space |
|-----------|------|-------|
| `push(value)` | O(1) | O(1) |
| `pop(value)` | O(1) | O(1) |
| `empty()` / `full()` | O(1) | O(1) |
| `capacity()` | O(1) | O(1) |

## Thread Safety

!!! warning "Single Producer, Single Consumer Only"
    This queue is designed for exactly **one** producer thread and **one** consumer thread. Using multiple producers or multiple consumers leads to undefined behavior.

## Performance Notes

- Lock-free implementation using atomic operations
- No mutexes or locks required
- Safe for exactly one producer and one consumer thread
- Ideal for inter-thread communication in real-time and low-latency systems

## Typical Use Cases

- Audio/video processing pipelines
- Sensor data acquisition
- Log message passing
- Real-time event handling
- High-frequency trading order routing
