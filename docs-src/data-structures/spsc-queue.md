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

## Performance

**18.7x faster** than mutex-protected queues in benchmarks.

### Operation Latency

| Operation | SpscQueue | MutexQueue | Speedup |
|-----------|-----------|------------|---------|
| Interleaved push/pop | 2.40 ns | 44.9 ns | **18.7x** |
| Round-trip (16B) | 2.42 ns | 46.0 ns | **19.0x** |
| Round-trip (64B) | 4.21 ns | 47.2 ns | **11.2x** |
| Round-trip (256B) | 13.3 ns | 55.8 ns | **4.2x** |

### Multi-threaded Producer/Consumer

| Items | SpscQueue | MutexQueue | Speedup |
|-------|-----------|------------|---------|
| 4,096 | 297M ops/s | 25.8M ops/s | **11.5x** |
| 65,536 | 367M ops/s | 23.1M ops/s | **15.9x** |

See [Benchmarks](../benchmarks.md) for full results.

### Why It's Fast

- **Lock-free**: Uses atomic operations instead of mutex locks
- **No contention**: Designed for exactly one producer and one consumer
- **Cache-line padding**: Producer and consumer indices on separate cache lines to prevent false sharing
- **Cached index optimization**: Each thread caches the other's index, avoiding cross-cache-line reads on the hot path
- **Ever-increasing indices**: Full/empty checks use subtraction only — no modulo
- **Minimal synchronization**: Only acquire/release memory ordering where needed

## Typical Use Cases

- Audio/video processing pipelines
- Sensor data acquisition
- Log message passing
- Real-time event handling
- High-frequency trading order routing
