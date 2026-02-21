#include <loon/redis_list.hpp>

#include <array>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <deque>
#include <list>
#include <string>

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
// loon::RedisList benchmarks - Push operations
// ----------------------------------------------------------------------------

static void BM_RedisList_LPush(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    loon::RedisList<int> list;
    for (size_t i = 0; i < count; ++i) {
      list.lpush(static_cast<int>(i));
    }
    benchmark::DoNotOptimize(list);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_RedisList_LPush)->Range(64, 4096);

static void BM_RedisList_RPush(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    loon::RedisList<int> list;
    for (size_t i = 0; i < count; ++i) {
      list.rpush(static_cast<int>(i));
    }
    benchmark::DoNotOptimize(list);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_RedisList_RPush)->Range(64, 4096);

// ----------------------------------------------------------------------------
// loon::RedisList benchmarks - Pop operations
// ----------------------------------------------------------------------------

static void BM_RedisList_LPop(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    state.PauseTiming();
    loon::RedisList<int> list;
    for (size_t i = 0; i < count; ++i) {
      list.rpush(static_cast<int>(i));
    }
    state.ResumeTiming();

    for (size_t i = 0; i < count; ++i) {
      auto val = list.lpop();
      benchmark::DoNotOptimize(val);
    }
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_RedisList_LPop)->Range(64, 4096);

static void BM_RedisList_RPop(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    state.PauseTiming();
    loon::RedisList<int> list;
    for (size_t i = 0; i < count; ++i) {
      list.rpush(static_cast<int>(i));
    }
    state.ResumeTiming();

    for (size_t i = 0; i < count; ++i) {
      auto val = list.rpop();
      benchmark::DoNotOptimize(val);
    }
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_RedisList_RPop)->Range(64, 4096);

// ----------------------------------------------------------------------------
// Interleaved push/pop (steady state)
// ----------------------------------------------------------------------------

static void BM_RedisList_PushPop_Interleaved(benchmark::State& state) {
  loon::RedisList<int> list;
  int value = 0;
  for (auto _ : state) {
    list.rpush(value++);
    auto val = list.lpop();
    benchmark::DoNotOptimize(val);
  }
  state.SetItemsProcessed(state.iterations() * 2);
}
BENCHMARK(BM_RedisList_PushPop_Interleaved);

// ----------------------------------------------------------------------------
// Batch pop operations
// ----------------------------------------------------------------------------

static void BM_RedisList_LPop_Batch(benchmark::State& state) {
  const auto batch_size = static_cast<size_t>(state.range(0));
  const size_t list_size = 4096;

  for (auto _ : state) {
    state.PauseTiming();
    loon::RedisList<int> list;
    for (size_t i = 0; i < list_size; ++i) {
      list.rpush(static_cast<int>(i));
    }
    state.ResumeTiming();

    while (list.llen() >= batch_size) {
      auto batch = list.lpop(batch_size);
      benchmark::DoNotOptimize(batch);
    }
  }
  state.SetItemsProcessed(state.iterations() * list_size);
}
BENCHMARK(BM_RedisList_LPop_Batch)->Arg(1)->Arg(10)->Arg(100)->Arg(1000);

static void BM_RedisList_RPop_Batch(benchmark::State& state) {
  const auto batch_size = static_cast<size_t>(state.range(0));
  const size_t list_size = 4096;

  for (auto _ : state) {
    state.PauseTiming();
    loon::RedisList<int> list;
    for (size_t i = 0; i < list_size; ++i) {
      list.rpush(static_cast<int>(i));
    }
    state.ResumeTiming();

    while (list.llen() >= batch_size) {
      auto batch = list.rpop(batch_size);
      benchmark::DoNotOptimize(batch);
    }
  }
  state.SetItemsProcessed(state.iterations() * list_size);
}
BENCHMARK(BM_RedisList_RPop_Batch)->Arg(1)->Arg(10)->Arg(100)->Arg(1000);

// ----------------------------------------------------------------------------
// Range queries
// ----------------------------------------------------------------------------

static void BM_RedisList_LRange_Small(benchmark::State& state) {
  loon::RedisList<int> list;
  for (int i = 0; i < 1000; ++i) {
    list.rpush(i);
  }

  for (auto _ : state) {
    auto range = list.lrange(0, 9);  // First 10 elements
    benchmark::DoNotOptimize(range);
  }
  state.SetItemsProcessed(state.iterations() * 10);
}
BENCHMARK(BM_RedisList_LRange_Small);

static void BM_RedisList_LRange_Medium(benchmark::State& state) {
  loon::RedisList<int> list;
  for (int i = 0; i < 1000; ++i) {
    list.rpush(i);
  }

  for (auto _ : state) {
    auto range = list.lrange(0, 99);  // First 100 elements
    benchmark::DoNotOptimize(range);
  }
  state.SetItemsProcessed(state.iterations() * 100);
}
BENCHMARK(BM_RedisList_LRange_Medium);

static void BM_RedisList_LRange_All(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  loon::RedisList<int> list;
  for (size_t i = 0; i < count; ++i) {
    list.rpush(static_cast<int>(i));
  }

  for (auto _ : state) {
    auto range = list.lrange(0, -1);  // All elements
    benchmark::DoNotOptimize(range);
  }
  state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}
BENCHMARK(BM_RedisList_LRange_All)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_RedisList_LRange_Tail(benchmark::State& state) {
  loon::RedisList<int> list;
  for (int i = 0; i < 1000; ++i) {
    list.rpush(i);
  }

  for (auto _ : state) {
    auto range = list.lrange(-10, -1);  // Last 10 elements
    benchmark::DoNotOptimize(range);
  }
  state.SetItemsProcessed(state.iterations() * 10);
}
BENCHMARK(BM_RedisList_LRange_Tail);

// ----------------------------------------------------------------------------
// Size check (llen)
// ----------------------------------------------------------------------------

static void BM_RedisList_LLen(benchmark::State& state) {
  loon::RedisList<int> list;
  for (int i = 0; i < 1000; ++i) {
    list.rpush(i);
  }

  for (auto _ : state) {
    auto len = list.llen();
    benchmark::DoNotOptimize(len);
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_RedisList_LLen);

// ----------------------------------------------------------------------------
// Different value sizes
// ----------------------------------------------------------------------------

template <typename V>
static void BM_RedisList_PushPop_ValueSize(benchmark::State& state) {
  loon::RedisList<V> list;
  V value{};

  for (auto _ : state) {
    list.rpush(value);
    auto val = list.lpop();
    benchmark::DoNotOptimize(val);
  }
  state.SetItemsProcessed(state.iterations() * 2);
  state.SetBytesProcessed(state.iterations() * sizeof(V) * 2);
}

BENCHMARK(BM_RedisList_PushPop_ValueSize<Value16B>)->Name("RedisList/PushPop/16B");
BENCHMARK(BM_RedisList_PushPop_ValueSize<Value64B>)->Name("RedisList/PushPop/64B");
BENCHMARK(BM_RedisList_PushPop_ValueSize<Value256B>)->Name("RedisList/PushPop/256B");

// ----------------------------------------------------------------------------
// String values benchmark
// ----------------------------------------------------------------------------

static void BM_RedisList_String_Push(benchmark::State& state) {
  loon::RedisList<std::string> list;
  int i = 0;
  for (auto _ : state) {
    list.rpush("value_" + std::to_string(i % 10000));
    ++i;
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_RedisList_String_Push);

static void BM_RedisList_String_Pop(benchmark::State& state) {
  loon::RedisList<std::string> list;
  // Pre-fill
  for (int i = 0; i < 10000; ++i) {
    list.rpush("value_" + std::to_string(i));
  }

  for (auto _ : state) {
    state.PauseTiming();
    if (list.empty()) {
      for (int i = 0; i < 10000; ++i) {
        list.rpush("value_" + std::to_string(i));
      }
    }
    state.ResumeTiming();
    auto val = list.lpop();
    benchmark::DoNotOptimize(val);
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_RedisList_String_Pop);

// ----------------------------------------------------------------------------
// std::deque comparison (baseline - same underlying structure)
// ----------------------------------------------------------------------------

static void BM_StdDeque_PushBack(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    std::deque<int> deque;
    for (size_t i = 0; i < count; ++i) {
      deque.push_back(static_cast<int>(i));
    }
    benchmark::DoNotOptimize(deque);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_StdDeque_PushBack)->Range(64, 4096);

static void BM_StdDeque_PushFront(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    std::deque<int> deque;
    for (size_t i = 0; i < count; ++i) {
      deque.push_front(static_cast<int>(i));
    }
    benchmark::DoNotOptimize(deque);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_StdDeque_PushFront)->Range(64, 4096);

static void BM_StdDeque_PopFront(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    state.PauseTiming();
    std::deque<int> deque;
    for (size_t i = 0; i < count; ++i) {
      deque.push_back(static_cast<int>(i));
    }
    state.ResumeTiming();

    for (size_t i = 0; i < count; ++i) {
      auto val = deque.front();
      deque.pop_front();
      benchmark::DoNotOptimize(val);
    }
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_StdDeque_PopFront)->Range(64, 4096);

static void BM_StdDeque_PushPop_Interleaved(benchmark::State& state) {
  std::deque<int> deque;
  int value = 0;
  for (auto _ : state) {
    deque.push_back(value++);
    auto val = deque.front();
    deque.pop_front();
    benchmark::DoNotOptimize(val);
  }
  state.SetItemsProcessed(state.iterations() * 2);
}
BENCHMARK(BM_StdDeque_PushPop_Interleaved);

// ----------------------------------------------------------------------------
// std::list comparison
// ----------------------------------------------------------------------------

static void BM_StdList_PushBack(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    std::list<int> list;
    for (size_t i = 0; i < count; ++i) {
      list.push_back(static_cast<int>(i));
    }
    benchmark::DoNotOptimize(list);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_StdList_PushBack)->Range(64, 4096);

static void BM_StdList_PushFront(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    std::list<int> list;
    for (size_t i = 0; i < count; ++i) {
      list.push_front(static_cast<int>(i));
    }
    benchmark::DoNotOptimize(list);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_StdList_PushFront)->Range(64, 4096);

static void BM_StdList_PopFront(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    state.PauseTiming();
    std::list<int> list;
    for (size_t i = 0; i < count; ++i) {
      list.push_back(static_cast<int>(i));
    }
    state.ResumeTiming();

    for (size_t i = 0; i < count; ++i) {
      auto val = list.front();
      list.pop_front();
      benchmark::DoNotOptimize(val);
    }
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_StdList_PopFront)->Range(64, 4096);

static void BM_StdList_PushPop_Interleaved(benchmark::State& state) {
  std::list<int> list;
  int value = 0;
  for (auto _ : state) {
    list.push_back(value++);
    auto val = list.front();
    list.pop_front();
    benchmark::DoNotOptimize(val);
  }
  state.SetItemsProcessed(state.iterations() * 2);
}
BENCHMARK(BM_StdList_PushPop_Interleaved);

// ----------------------------------------------------------------------------
// Mixed workload (realistic usage pattern)
// ----------------------------------------------------------------------------

static void BM_RedisList_Mixed_Workload(benchmark::State& state) {
  loon::RedisList<int> list;
  // Pre-fill with some elements
  for (int i = 0; i < 100; ++i) {
    list.rpush(i);
  }

  int key = 100;
  for (auto _ : state) {
    // 40% rpush, 40% lpop, 20% lrange
    int op = key % 10;
    if (op < 4) {
      list.rpush(key);
    } else if (op < 8) {
      if (!list.empty()) {
        auto val = list.lpop();
        benchmark::DoNotOptimize(val);
      }
    } else {
      auto range = list.lrange(0, 9);
      benchmark::DoNotOptimize(range);
    }
    ++key;
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_RedisList_Mixed_Workload);
