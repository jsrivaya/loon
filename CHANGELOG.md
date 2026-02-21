# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.0] - 2026-02-20

### Added

- **SpscQueue**: Lock-free single-producer single-consumer queue (`loon/spsc.hpp`)
  - O(1) push and pop operations
  - Cache-line padding to prevent false sharing (4.9x faster than mutex queues)
  - Compile-time fixed capacity
  - Static assertion for lock-free atomic operations

- **RingBuffer**: Fixed-size circular buffer (`loon/ring_buffer.hpp`)
  - Optional override mode for oldest elements
  - 3.2x faster than `std::queue`

- **Comprehensive benchmarks** using Google Benchmark
  - RingBuffer vs `std::queue` comparisons
  - SpscQueue vs mutex-protected queue comparisons
  - LRU Cache operation benchmarks
  - RedisList vs `std::deque` and `std::list` comparisons

- **MkDocs documentation site** with Material theme
  - Full API reference
  - Performance benchmarks
  - Getting started guide

### Changed

- **SpscQueue performance improvement**: Added `alignas(CACHE_LINE_SIZE)` to head and tail atomics
  - Prevents false sharing between producer and consumer threads
  - 27% faster interleaved operations (12.6 ns → 9.1 ns)
  - 4.9x speedup vs mutex-protected queues (up from 3.4x)

### Fixed

- **SpscQueue bug**: Fixed incorrect atomic variable usage in `pop()` method
  - Was using `head_` instead of local `head` variable in store operation
  - This caused an unnecessary extra atomic load on each pop

### Performance Highlights

| Component | Latency | Throughput | vs std:: |
|-----------|---------|------------|----------|
| RingBuffer | 1.9 ns | 2.1G ops/s | 3.2x faster |
| SpscQueue | 9.1 ns | 220M ops/s | 4.9x faster |
| LRU Cache | 7.8 ns | 130M ops/s | O(1) ops |
| RedisList | 3.6 ns | 560M ops/s | 22x vs std::list |

## [0.1.0] - 2024-12-01

### Added

- **LRU Cache**: O(1) least recently used cache (`loon/lru.hpp`)
  - Hash map + doubly-linked list implementation
  - Automatic eviction of least recently used items
  - Reference semantics for zero-copy access

- **RedisList**: Redis-style doubly-linked list (`loon/redis_list.hpp`)
  - lpush/rpush/lpop/rpop operations
  - lrange with negative index support
  - Backed by `std::deque` for cache efficiency

- Initial project structure
- CMake build system
- Conan package support
- MIT License

[0.2.0]: https://github.com/jsrivaya/loon/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/jsrivaya/loon/releases/tag/v0.1.0
