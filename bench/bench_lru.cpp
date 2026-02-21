#include <loon/lru.hpp>

#include <array>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <random>
#include <string>
#include <unordered_map>

// ----------------------------------------------------------------------------
// Value types of different sizes
// ----------------------------------------------------------------------------

struct Value16B {
  int64_t id;
  int64_t data;
}; // 16 bytes

struct Value64B {
  int64_t id;
  int64_t timestamp;
  std::array<char, 48> payload;
}; // 64 bytes

struct Value256B {
  int64_t id;
  int64_t timestamp;
  std::array<char, 240> payload;
}; // 256 bytes

// ----------------------------------------------------------------------------
// loon::LRU benchmarks
// ----------------------------------------------------------------------------

static void BM_LRU_Put(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  loon::LRU<int, int> cache(count);

  int key = 0;
  for (auto _ : state) {
    cache.put(key, key);
    key = (key + 1) % static_cast<int>(count * 2);  // Mix hits and evictions
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LRU_Put)->Range(64, 4096);

static void BM_LRU_Get_Hit(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  loon::LRU<int, int> cache(count);

  // Pre-fill cache
  for (size_t i = 0; i < count; ++i) {
    cache.put(static_cast<int>(i), static_cast<int>(i));
  }

  int key = 0;
  for (auto _ : state) {
    auto val = cache.get(key);
    benchmark::DoNotOptimize(val);
    key = (key + 1) % static_cast<int>(count);
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LRU_Get_Hit)->Range(64, 4096);

static void BM_LRU_Get_Miss(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  loon::LRU<int, int> cache(count);

  // Pre-fill cache with keys 0 to count-1
  for (size_t i = 0; i < count; ++i) {
    cache.put(static_cast<int>(i), static_cast<int>(i));
  }

  // Query keys that don't exist
  int key = static_cast<int>(count);
  for (auto _ : state) {
    auto val = cache.get(key);
    benchmark::DoNotOptimize(val);
    ++key;
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LRU_Get_Miss)->Range(64, 4096);

static void BM_LRU_PutGet_Mixed(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  loon::LRU<int, int> cache(count);

  // Pre-fill cache
  for (size_t i = 0; i < count; ++i) {
    cache.put(static_cast<int>(i), static_cast<int>(i));
  }

  int key = 0;
  for (auto _ : state) {
    // 80% reads, 20% writes (typical cache workload)
    if (key % 5 == 0) {
      cache.put(key, key);
    } else {
      auto val = cache.get(key % static_cast<int>(count));
      benchmark::DoNotOptimize(val);
    }
    ++key;
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LRU_PutGet_Mixed)->Range(64, 4096);

static void BM_LRU_Exists(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  loon::LRU<int, int> cache(count);

  // Pre-fill cache
  for (size_t i = 0; i < count; ++i) {
    cache.put(static_cast<int>(i), static_cast<int>(i));
  }

  int key = 0;
  for (auto _ : state) {
    bool exists = cache.exists(key);
    benchmark::DoNotOptimize(exists);
    key = (key + 1) % static_cast<int>(count);
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LRU_Exists)->Range(64, 4096);

// ----------------------------------------------------------------------------
// Different value sizes
// ----------------------------------------------------------------------------

template <typename V>
static void BM_LRU_PutGet_ValueSize(benchmark::State& state) {
  loon::LRU<int, V> cache(1024);
  V value{};

  // Pre-fill
  for (int i = 0; i < 1024; ++i) {
    cache.put(i, value);
  }

  int key = 0;
  for (auto _ : state) {
    if (key % 5 == 0) {
      cache.put(key % 1024, value);
    } else {
      auto val = cache.get(key % 1024);
      benchmark::DoNotOptimize(val);
    }
    ++key;
  }
  state.SetItemsProcessed(state.iterations());
  state.SetBytesProcessed(state.iterations() * sizeof(V));
}

BENCHMARK(BM_LRU_PutGet_ValueSize<Value16B>)->Name("LRU/PutGet/16B");
BENCHMARK(BM_LRU_PutGet_ValueSize<Value64B>)->Name("LRU/PutGet/64B");
BENCHMARK(BM_LRU_PutGet_ValueSize<Value256B>)->Name("LRU/PutGet/256B");

// ----------------------------------------------------------------------------
// String keys benchmark (common use case)
// ----------------------------------------------------------------------------

static void BM_LRU_StringKey_Put(benchmark::State& state) {
  loon::LRU<std::string, int> cache(1024);

  int i = 0;
  for (auto _ : state) {
    std::string key = "key_" + std::to_string(i % 2048);
    cache.put(key, i);
    ++i;
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LRU_StringKey_Put);

static void BM_LRU_StringKey_Get(benchmark::State& state) {
  loon::LRU<std::string, int> cache(1024);

  // Pre-fill
  for (int i = 0; i < 1024; ++i) {
    cache.put("key_" + std::to_string(i), i);
  }

  int i = 0;
  for (auto _ : state) {
    std::string key = "key_" + std::to_string(i % 1024);
    auto val = cache.get(key);
    benchmark::DoNotOptimize(val);
    ++i;
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LRU_StringKey_Get);

// ----------------------------------------------------------------------------
// std::unordered_map comparison (baseline - no LRU tracking)
// ----------------------------------------------------------------------------

static void BM_UnorderedMap_Put(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  std::unordered_map<int, int> map;
  map.reserve(count);

  int key = 0;
  for (auto _ : state) {
    map[key] = key;
    key = (key + 1) % static_cast<int>(count * 2);
    // Simulate eviction to keep size bounded
    if (map.size() > count) {
      map.erase(map.begin());
    }
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_UnorderedMap_Put)->Range(64, 4096);

static void BM_UnorderedMap_Get(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  std::unordered_map<int, int> map;

  // Pre-fill
  for (size_t i = 0; i < count; ++i) {
    map[static_cast<int>(i)] = static_cast<int>(i);
  }

  int key = 0;
  for (auto _ : state) {
    auto it = map.find(key);
    benchmark::DoNotOptimize(it);
    key = (key + 1) % static_cast<int>(count);
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_UnorderedMap_Get)->Range(64, 4096);

// ----------------------------------------------------------------------------
// Eviction stress test
// ----------------------------------------------------------------------------

static void BM_LRU_Eviction_Stress(benchmark::State& state) {
  loon::LRU<int, int> cache(256);  // Small cache

  // Pre-fill
  for (int i = 0; i < 256; ++i) {
    cache.put(i, i);
  }

  int key = 256;  // Start with keys that will cause evictions
  for (auto _ : state) {
    cache.put(key, key);
    ++key;
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LRU_Eviction_Stress);

// ----------------------------------------------------------------------------
// Random access pattern (realistic workload)
// ----------------------------------------------------------------------------

static void BM_LRU_Random_Access(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  loon::LRU<int, int> cache(count);

  // Pre-fill
  for (size_t i = 0; i < count; ++i) {
    cache.put(static_cast<int>(i), static_cast<int>(i));
  }

  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(0, static_cast<int>(count * 2));

  for (auto _ : state) {
    int key = dist(rng);
    auto val = cache.get(key);
    if (!val) {
      cache.put(key, key);
    }
    benchmark::DoNotOptimize(val);
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LRU_Random_Access)->Range(64, 4096);
