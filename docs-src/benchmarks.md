# Benchmarks

Performance benchmarks comparing loon data structures against standard library alternatives.

## Performance Summary

| Component | Best Latency | Peak Throughput | vs std:: |
|-----------|--------------|-----------------|----------|
| [RingBuffer](data-structures/ring-buffer.md) | 0.95 ns | 2.1G ops/s | **3.2x faster** |
| [SPSC Queue](data-structures/spsc-queue.md) | 9.1 ns | 220M ops/s | **3.4x faster** |
| LRU Cache | - | - | - |
| Redis List | - | - | - |

## RingBuffer vs std::queue

### Round-Trip Latency (push + pop)

| Message Size | RingBuffer | std::queue | Speedup |
|--------------|------------|------------|---------|
| 16 bytes | 1.9 ns | 6.1 ns | 3.2x |
| 64 bytes | 2.3 ns | 7.4 ns | 3.2x |
| 256 bytes | 4.1 ns | 12.8 ns | 3.1x |

### Throughput

| Message Size | RingBuffer | std::queue | Speedup |
|--------------|------------|------------|---------|
| 16 bytes | 2.1G ops/s | 656M ops/s | 3.2x |
| 64 bytes | 1.7G ops/s | 541M ops/s | 3.1x |
| 256 bytes | 976M ops/s | 312M ops/s | 3.1x |

## SPSC Queue vs Mutex Queue

Lock-free SPSC queue compared against a mutex-protected `std::queue`.

### Round-Trip Latency (push + pop)

| Message Size | SpscQueue | MutexQueue | Speedup |
|--------------|-----------|------------|---------|
| 16 bytes | 12.6 ns | 42.4 ns | **3.4x** |
| 64 bytes | 12.6 ns | 43.5 ns | **3.5x** |
| 256 bytes | 24.7 ns | 52.3 ns | **2.1x** |

### Throughput

| Message Size | SpscQueue | MutexQueue | Speedup |
|--------------|-----------|------------|---------|
| 16 bytes | 1.51 GiB/s | 719 MiB/s | **2.1x** |
| 64 bytes | 6.08 GiB/s | 2.74 GiB/s | **2.2x** |
| 256 bytes | 14.3 GiB/s | 9.11 GiB/s | **1.6x** |

### Multi-threaded Producer/Consumer

Real-world scenario with separate producer and consumer threads:

| Items | SpscQueue | MutexQueue | Speedup |
|-------|-----------|------------|---------|
| 1,024 | 70.3M ops/s | 22.3M ops/s | **3.2x** |
| 4,096 | 114.9M ops/s | 25.8M ops/s | **4.5x** |
| 32,768 | 86.6M ops/s | 24.9M ops/s | **3.5x** |
| 65,536 | 90.7M ops/s | 23.1M ops/s | **3.9x** |

## Running Benchmarks

```bash
# Build with benchmarks enabled
make bench-build

# Run all benchmarks
make bench

# Or run directly with options
./build/Release/bench/loon_benchmarks

# Filter specific benchmarks
./build/Release/bench/loon_benchmarks --benchmark_filter="Spsc"

# Output to JSON for analysis
./build/Release/bench/loon_benchmarks --benchmark_format=json > results.json

# Longer runs for more stable results
./build/Release/bench/loon_benchmarks --benchmark_min_time=2s
```

## Test Environment

- **CPU**: Intel Core i7 @ 2.2 GHz (8 cores)
- **Cache**: L1 32 KiB, L2 256 KiB, L3 6 MiB shared
- **Compiler**: Apple Clang 14, C++23, -O3
- **OS**: macOS

## Why RingBuffer is Faster

1. **Stack allocation**: Uses `std::array` with no heap allocations
2. **Contiguous memory**: Cache-friendly access patterns
3. **No node overhead**: Unlike `std::queue<T, std::deque<T>>` which uses chunked storage
4. **Compile-time capacity**: No runtime size checks or reallocation

## Why SPSC Queue is Faster

1. **Lock-free**: Uses atomic operations instead of mutex locks
2. **No contention**: Designed for exactly one producer and one consumer
3. **Cache-line friendly**: Head and tail pointers can be on separate cache lines
4. **Minimal synchronization**: Only acquire/release memory ordering where needed

## Understanding the Metrics

| Metric | Description |
|--------|-------------|
| **Time** | Wall-clock time per iteration (latency) |
| **CPU** | CPU time per iteration |
| **Iterations** | Number of benchmark runs |
| **items_per_second** | Throughput in operations/sec |
| **bytes_per_second** | Data throughput (for sized messages) |
