# Performance Benchmarks

This directory contains performance benchmarks for loon data structures using [Google Benchmark](https://github.com/google/benchmark).

## Quick Start

```bash
# Build with benchmarks enabled
make bench-build

# Run all benchmarks
make bench

# Or run directly
./build/Release/bench/loon_benchmarks
```

## Performance Summary

| Component | Best Latency | Peak Throughput | vs std:: |
|-----------|--------------|-----------------|----------|
| [RingBuffer](RING_BUFFER.md) | 0.95 ns | 2.1G ops/s | **3.2x faster** |
| LRU Cache | - | - | - |
| Redis List | - | - | - |
| SPSC Queue | - | - | - |

## Detailed Results

- [RingBuffer Benchmarks](RING_BUFFER.md) - Comparison with std::queue

## Test Environment

- **CPU**: Intel Core i7 @ 2.2 GHz (8 cores)
- **Cache**: L1 32 KiB, L2 256 KiB, L3 6 MiB shared
- **Compiler**: Apple Clang 14, C++23, -O3
- **OS**: macOS

## Running Options

```bash
# Filter specific benchmarks
./build/Release/bench/loon_benchmarks --benchmark_filter="RingBuffer"

# Output to JSON for analysis
./build/Release/bench/loon_benchmarks --benchmark_format=json > results.json

# Longer runs for more stable results
./build/Release/bench/loon_benchmarks --benchmark_min_time=2s

# Show counters in table format
./build/Release/bench/loon_benchmarks --benchmark_counters_tabular=true
```

## Understanding the Metrics

| Metric | Description |
|--------|-------------|
| **Time** | Wall-clock time per iteration (latency) |
| **CPU** | CPU time per iteration |
| **Iterations** | Number of benchmark runs |
| **items_per_second** | Throughput in operations/sec |
| **bytes_per_second** | Data throughput (for sized messages) |

## Benchmark Descriptions

### RingBuffer Benchmarks

| Benchmark | Description |
|-----------|-------------|
| `BM_RingBuffer_Push/N` | Push N elements to empty buffer |
| `BM_RingBuffer_Pop/N` | Pop N elements from full buffer |
| `BM_RingBuffer_PushPop_Interleaved` | Steady-state: push then pop |
| `BM_RingBuffer_Override` | Push to full buffer (override mode) |
| `RingBuffer/RoundTrip/*` | Single push+pop for different message sizes |
| `RingBuffer/Throughput/*` | Batch operations for different sizes |

### Comparison Benchmarks

| Benchmark | Description |
|-----------|-------------|
| `BM_StdQueue_*` | Equivalent operations using std::queue |
| `std::queue/RoundTrip/*` | Round-trip comparison for message sizes |

## Adding New Benchmarks

1. Add benchmark function in `bench_<component>.cpp`
2. Register with `BENCHMARK()`
3. Update [bench/CMakeLists.txt](CMakeLists.txt) if adding new files

Example:

```cpp
static void BM_MyBenchmark(benchmark::State& state) {
  // Setup (not measured)
  MyDataStructure ds;

  for (auto _ : state) {
    // Measured code
    ds.operation();
    benchmark::DoNotOptimize(ds);
  }

  // Report metrics
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_MyBenchmark);
```
