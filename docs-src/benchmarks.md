# Benchmarks

Performance benchmarks comparing loon data structures against standard library alternatives.

## Performance Summary

| Component | Best Latency | Peak Throughput | vs std:: |
|-----------|--------------|-----------------|----------|
| [RingBuffer](data-structures/ring-buffer.md) | 0.95 ns | 2.1G ops/s | **3.2x faster** |
| LRU Cache | - | - | - |
| Redis List | - | - | - |
| SPSC Queue | - | - | - |

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

## Running Benchmarks

```bash
# Build with benchmarks enabled
make bench-build

# Run all benchmarks
make bench

# Or run directly with options
./build/Release/bench/loon_benchmarks

# Filter specific benchmarks
./build/Release/bench/loon_benchmarks --benchmark_filter="RingBuffer"

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

## Understanding the Metrics

| Metric | Description |
|--------|-------------|
| **Time** | Wall-clock time per iteration (latency) |
| **CPU** | CPU time per iteration |
| **Iterations** | Number of benchmark runs |
| **items_per_second** | Throughput in operations/sec |
| **bytes_per_second** | Data throughput (for sized messages) |
