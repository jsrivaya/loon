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
| RingBuffer | 0.95 ns | 2.1G ops/s | **3.2x faster** |
| SPSC Queue | 9.1 ns | 220M ops/s | **4.9x faster** |
| LRU Cache | 7.8 ns | 130M ops/s | O(1) ops |
| Redis List | 0.33 ns | 560M ops/s | **22x** vs std::list |

See [full benchmark documentation](https://jsrivaya.github.io/loon/benchmarks/) for detailed results.

## Benchmark Files

| File | Description |
|------|-------------|
| `bench_ring_buffer.cpp` | RingBuffer vs std::queue |
| `bench_spsc.cpp` | SPSC Queue vs mutex-protected queue |
| `bench_lru.cpp` | LRU Cache operations and comparisons |
| `bench_redis_list.cpp` | Redis List vs std::deque and std::list |

## Test Environment

- **CPU**: Intel Core i7 @ 2.2 GHz (8 cores)
- **Cache**: L1 32 KiB, L2 256 KiB, L3 6 MiB shared
- **Compiler**: Apple Clang 14, C++23, -O3
- **OS**: macOS

## Running Options

```bash
# Filter specific benchmarks
./build/Release/bench/loon_benchmarks --benchmark_filter="RingBuffer"
./build/Release/bench/loon_benchmarks --benchmark_filter="Spsc"
./build/Release/bench/loon_benchmarks --benchmark_filter="LRU"
./build/Release/bench/loon_benchmarks --benchmark_filter="RedisList"

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

### SPSC Queue Benchmarks

| Benchmark | Description |
|-----------|-------------|
| `BM_SpscQueue_Push/N` | Push N elements |
| `BM_SpscQueue_Pop/N` | Pop N elements |
| `BM_SpscQueue_PushPop_Interleaved` | Single-threaded push+pop |
| `SpscQueue/RoundTrip/*` | Round-trip for different message sizes |
| `SpscQueue/Throughput/*` | Batch throughput for different sizes |
| `BM_SpscQueue_ProducerConsumer/N` | Multi-threaded producer/consumer |

### LRU Cache Benchmarks

| Benchmark | Description |
|-----------|-------------|
| `BM_LRU_Put/N` | Put operations with cache size N |
| `BM_LRU_Get_Hit/N` | Get operations (cache hits) |
| `BM_LRU_Get_Miss/N` | Get operations (cache misses) |
| `BM_LRU_PutGet_Mixed/N` | 80% reads, 20% writes workload |
| `BM_LRU_Exists/N` | Key existence checks |
| `LRU/PutGet/*` | Mixed ops for different value sizes |
| `BM_LRU_StringKey_*` | String key operations |
| `BM_LRU_Eviction_Stress` | Continuous eviction scenario |
| `BM_LRU_Random_Access/N` | Random access pattern |

### Redis List Benchmarks

| Benchmark | Description |
|-----------|-------------|
| `BM_RedisList_LPush/N` | Push N elements to front |
| `BM_RedisList_RPush/N` | Push N elements to back |
| `BM_RedisList_LPop/N` | Pop N elements from front |
| `BM_RedisList_RPop/N` | Pop N elements from back |
| `BM_RedisList_PushPop_Interleaved` | Steady-state push/pop |
| `BM_RedisList_LPop_Batch/N` | Batch pop of N elements |
| `BM_RedisList_LRange_*` | Range query benchmarks |
| `BM_RedisList_LLen` | Size check |
| `RedisList/PushPop/*` | Different value sizes |
| `BM_RedisList_Mixed_Workload` | Realistic usage pattern |

### Comparison Benchmarks

| Benchmark | Description |
|-----------|-------------|
| `BM_StdQueue_*` | std::queue equivalents |
| `BM_MutexQueue_*` | Mutex-protected queue equivalents |
| `BM_UnorderedMap_*` | std::unordered_map equivalents |
| `BM_StdDeque_*` | std::deque equivalents |
| `BM_StdList_*` | std::list equivalents |

## Adding New Benchmarks

1. Add benchmark function in `bench_<component>.cpp`
2. Register with `BENCHMARK()`
3. Update [CMakeLists.txt](CMakeLists.txt) if adding new files

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
