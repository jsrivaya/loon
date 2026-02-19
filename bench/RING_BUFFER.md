# RingBuffer Benchmarks

Detailed performance analysis of `loon::RingBuffer` compared to `std::queue`.

## Summary

| Metric | RingBuffer | vs std::queue |
|--------|------------|---------------|
| **Best Latency** | 0.95 ns | **3.2x faster** |
| **Peak Throughput** | 2.1G ops/s | **3.2x faster** |
| **Peak Bandwidth** | 14.3 GiB/s | - |

## Latency (Single Push+Pop Round-Trip)

| Message Size | RingBuffer | std::queue | Speedup |
|--------------|------------|------------|---------|
| 4 bytes (int) | 0.95 ns | 3.06 ns | **3.2x** |
| 16 bytes | 2.58 ns | 3.37 ns | **1.3x** |
| 64 bytes | 4.63 ns | 4.86 ns | **1.05x** |
| 256 bytes | 20.5 ns | 16.3 ns | 0.8x |

## Throughput (Batch Operations)

| Message Size | RingBuffer | Bandwidth |
|--------------|------------|-----------|
| 16 bytes | 411M ops/s | 6.1 GiB/s |
| 64 bytes | 207M ops/s | 12.3 GiB/s |
| 256 bytes | 60M ops/s | 14.3 GiB/s |

## Push Performance

| Batch Size | RingBuffer | std::queue | Speedup |
|------------|------------|------------|---------|
| 64 items | 992M ops/s | 234M ops/s | **4.2x** |
| 512 items | 1.02G ops/s | 529M ops/s | **1.9x** |
| 4096 items | 1.01G ops/s | 548M ops/s | **1.8x** |

## Pop Performance

| Batch Size | RingBuffer | std::queue | Speedup |
|------------|------------|------------|---------|
| 64 items | 39.4M ops/s | 36.5M ops/s | **1.1x** |
| 512 items | 246M ops/s | 182M ops/s | **1.4x** |
| 4096 items | 726M ops/s | 360M ops/s | **2.0x** |

## Interleaved Push+Pop (Steady State)

| Operation | RingBuffer | std::queue | Speedup |
|-----------|------------|------------|---------|
| Push+Pop | **2.1G ops/s** | 653M ops/s | **3.2x** |

## Key Observations

1. **Sub-nanosecond latency**: RingBuffer achieves ~0.95ns for interleaved push+pop
2. **3-4x faster for small messages**: Biggest speedup with small, frequent operations
3. **High bandwidth**: Up to 14.3 GiB/s throughput for larger messages
4. **Consistent performance**: No heap allocation, no GC pauses
5. **256B tradeoff**: std::queue slightly faster for large messages (memcpy optimization)

## Why RingBuffer is Faster

| Factor | RingBuffer | std::queue |
|--------|------------|------------|
| Memory | Stack-allocated array | Heap-allocated deque |
| Allocation | Zero (fixed size) | Dynamic (grows/shrinks) |
| Cache | Contiguous, predictable | Fragmented |
| Branching | Minimal | More complex |

## When to Use std::queue Instead

- Message sizes > 256 bytes (memcpy wins)
- Need unbounded capacity
- Memory footprint concerns (RingBuffer pre-allocates)

## Benchmark Descriptions

| Benchmark | Description |
|-----------|-------------|
| `BM_RingBuffer_Push/N` | Push N elements to empty buffer |
| `BM_RingBuffer_Pop/N` | Pop N elements from full buffer |
| `BM_RingBuffer_PushPop_Interleaved` | Steady-state: push then pop |
| `BM_RingBuffer_Override` | Push to full buffer (override mode) |
| `RingBuffer/RoundTrip/*` | Single push+pop for different message sizes |
| `RingBuffer/Throughput/*` | Batch operations for different sizes |

## Raw Output

```
--------------------------------------------------------------------------------------------
Benchmark                                  Time             CPU   Iterations UserCounters...
--------------------------------------------------------------------------------------------
BM_RingBuffer_Push/64                   64.8 ns         64.5 ns     10853386 items_per_second=992.337M/s
BM_RingBuffer_Push/512                   506 ns          504 ns      1351847 items_per_second=1.0163G/s
BM_RingBuffer_Push/4096                 4071 ns         4060 ns       175600 items_per_second=1.00895G/s
BM_RingBuffer_Pop/64                    1627 ns         1624 ns       432529 items_per_second=39.4144M/s
BM_RingBuffer_Pop/512                   2084 ns         2082 ns       333917 items_per_second=245.93M/s
BM_RingBuffer_Pop/4096                  5656 ns         5643 ns       121718 items_per_second=725.809M/s
BM_RingBuffer_PushPop_Interleaved      0.953 ns        0.950 ns    742004897 items_per_second=2.1062G/s
BM_RingBuffer_Override                  2.53 ns         2.52 ns    276835220 items_per_second=396.816M/s
BM_StdQueue_Push/64                      274 ns          274 ns      2606125 items_per_second=233.932M/s
BM_StdQueue_Push/512                     970 ns          968 ns       702452 items_per_second=528.987M/s
BM_StdQueue_Push/4096                   7493 ns         7476 ns        92747 items_per_second=547.892M/s
BM_StdQueue_Pop/64                      1750 ns         1752 ns       393060 items_per_second=36.5275M/s
BM_StdQueue_Pop/512                     2813 ns         2809 ns       247046 items_per_second=182.252M/s
BM_StdQueue_Pop/4096                   11422 ns        11372 ns        62416 items_per_second=360.191M/s
BM_StdQueue_PushPop_Interleaved         3.08 ns         3.06 ns    227319224 items_per_second=652.852M/s
RingBuffer/RoundTrip/16B                2.59 ns         2.58 ns    267608649 bytes_per_second=11.5639Gi/s items_per_second=776.041M/s
RingBuffer/RoundTrip/64B                4.72 ns         4.63 ns    158316955 bytes_per_second=25.7615Gi/s items_per_second=432.206M/s
RingBuffer/RoundTrip/256B               20.8 ns         20.5 ns     34153005 bytes_per_second=23.2166Gi/s items_per_second=97.3775M/s
RingBuffer/Throughput/16B               4911 ns         4865 ns       143867 bytes_per_second=6.12619Gi/s items_per_second=411.121M/s
RingBuffer/Throughput/64B               9713 ns         9674 ns        71852 bytes_per_second=12.3227Gi/s items_per_second=206.741M/s
RingBuffer/Throughput/256B             33871 ns        33329 ns        21860 bytes_per_second=14.3068Gi/s items_per_second=60.0071M/s
std::queue/RoundTrip/16B                3.41 ns         3.37 ns    210748790 bytes_per_second=8.84425Gi/s items_per_second=593.527M/s
std::queue/RoundTrip/64B                4.91 ns         4.86 ns    140895532 bytes_per_second=24.5526Gi/s items_per_second=411.924M/s
std::queue/RoundTrip/256B               16.5 ns         16.3 ns     43054138 bytes_per_second=29.2278Gi/s items_per_second=122.59M/s
```

## Run These Benchmarks

```bash
# All RingBuffer benchmarks
./build/Release/bench/loon_benchmarks --benchmark_filter="RingBuffer|StdQueue"

# Just latency tests
./build/Release/bench/loon_benchmarks --benchmark_filter="RoundTrip|Interleaved"

# Just throughput tests
./build/Release/bench/loon_benchmarks --benchmark_filter="Throughput|Push/|Pop/"
```
