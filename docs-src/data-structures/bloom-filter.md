# Bloom Filter

A fixed-size probabilistic data structure for membership checks with controllable false positive rates.

## Header

```cpp
#include <loon/bloom_filter.hpp>
```

## Overview

`loon::BloomFilter` uses a compact bit array and multiple hash probes to provide fast membership checks:

- `contains(key) == false`: key is definitely absent
- `contains(key) == true`: key is possibly present (false positives are possible)

Bloom filters do not produce false negatives as long as the same hash configuration is used for insertion and lookup.

## Usage

```cpp
auto filter = loon::BloomFilter<8192>::with_capacity(1000, 0.01);

filter.insert("user:42");
filter.insert("user:17");

if (filter.contains("user:42")) {
  // key may exist
}

auto fpr = filter.false_positive_rate();
filter.clear();
```

## API Reference

### Constructors

| Constructor | Description |
|-------------|-------------|
| `BloomFilter(size_t hash_functions = 3)` | Creates an empty filter with configurable hash count |
| `BloomFilter::with_capacity(expected_items, target_fpr)` | Returns a filter tuned for expected load and target FPR |

### Member Functions

| Operation | Return Type | Description |
|-----------|-------------|-------------|
| `insert(key)` | `void` | Inserts a key into the filter |
| `contains(key)` | `bool` | Checks membership (`false` = definitely absent) |
| `clear()` | `void` | Clears all bits |
| `false_positive_rate()` | `double` | Returns estimated false positive rate |
| `hash_functions()` | `size_t` | Returns configured hash function count |
| `size()` | `size_t` | Returns number of insertions performed |
| `bit_count()` | `size_t` | Returns compile-time bit count `N` |

## Complexity

| Operation | Time | Space |
|-----------|------|-------|
| `insert(key)` | O(k) | O(1) |
| `contains(key)` | O(k) | O(1) |
| `clear()` | O(n / word_size) | O(1) |

Where `k` is the number of hash functions and `n` is the number of bits.

## Notes

- Memory usage is fixed at compile time (`N` bits)
- More bits and fewer insertions improve false positive rate
- Typical usage: cache pre-checks, routing filters, dedupe hints
