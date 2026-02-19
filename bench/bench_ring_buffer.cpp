#include <loon/ring_buffer.hpp>

#include <benchmark/benchmark.h>

#include <array>
#include <cstdint>
#include <queue>

// ----------------------------------------------------------------------------
// Message types of different sizes
// ----------------------------------------------------------------------------

struct Msg16B {
  int64_t id;
  int64_t timestamp;
};  // 16 bytes

struct Msg64B {
  int64_t id;
  int64_t timestamp;
  std::array<char, 48> payload;
};  // 64 bytes

struct Msg256B {
  int64_t id;
  int64_t timestamp;
  std::array<char, 240> payload;
};  // 256 bytes

// ----------------------------------------------------------------------------
// loon::RingBuffer benchmarks
// ----------------------------------------------------------------------------

static void BM_RingBuffer_Push(benchmark::State& state) {
  const auto size = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    loon::RingBuffer<int, 4096> buffer;
    for (size_t i = 0; i < size; ++i) {
      buffer.push(static_cast<int>(i));
    }
    benchmark::DoNotOptimize(buffer);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_RingBuffer_Push)->Range(64, 4096);

static void BM_RingBuffer_Pop(benchmark::State& state) {
  const auto size = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    state.PauseTiming();
    loon::RingBuffer<int, 4096> buffer;
    for (size_t i = 0; i < size; ++i) {
      buffer.push(static_cast<int>(i));
    }
    state.ResumeTiming();

    for (size_t i = 0; i < size; ++i) {
      auto val = buffer.pop();
      benchmark::DoNotOptimize(val);
    }
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_RingBuffer_Pop)->Range(64, 4096);

static void BM_RingBuffer_PushPop_Interleaved(benchmark::State& state) {
  loon::RingBuffer<int, 1024> buffer;
  int value = 0;
  for (auto _ : state) {
    buffer.push(value++);
    auto val = buffer.pop();
    benchmark::DoNotOptimize(val);
  }
  state.SetItemsProcessed(state.iterations() * 2);
}
BENCHMARK(BM_RingBuffer_PushPop_Interleaved);

static void BM_RingBuffer_Override(benchmark::State& state) {
  loon::RingBuffer<int, 256> buffer(true); // override mode
  // Pre-fill to full
  for (size_t i = 0; i < 256; ++i) {
    buffer.push(static_cast<int>(i));
  }

  int value = 0;
  for (auto _ : state) {
    buffer.push(value++);
    benchmark::DoNotOptimize(buffer);
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_RingBuffer_Override);

// ----------------------------------------------------------------------------
// std::queue comparison (baseline)
// ----------------------------------------------------------------------------

static void BM_StdQueue_Push(benchmark::State& state) {
  const auto size = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    std::queue<int> queue;
    for (size_t i = 0; i < size; ++i) {
      queue.push(static_cast<int>(i));
    }
    benchmark::DoNotOptimize(queue);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_StdQueue_Push)->Range(64, 4096);

static void BM_StdQueue_Pop(benchmark::State& state) {
  const auto size = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    state.PauseTiming();
    std::queue<int> queue;
    for (size_t i = 0; i < size; ++i) {
      queue.push(static_cast<int>(i));
    }
    state.ResumeTiming();

    for (size_t i = 0; i < size; ++i) {
      auto val = queue.front();
      queue.pop();
      benchmark::DoNotOptimize(val);
    }
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_StdQueue_Pop)->Range(64, 4096);

static void BM_StdQueue_PushPop_Interleaved(benchmark::State& state) {
  std::queue<int> queue;
  int value = 0;
  for (auto _ : state) {
    queue.push(value++);
    auto val = queue.front();
    queue.pop();
    benchmark::DoNotOptimize(val);
  }
  state.SetItemsProcessed(state.iterations() * 2);
}
BENCHMARK(BM_StdQueue_PushPop_Interleaved);

// ----------------------------------------------------------------------------
// Message size benchmarks - measures latency and throughput for different sizes
// ----------------------------------------------------------------------------

// Template for push+pop round-trip with different message sizes
template <typename T>
static void BM_RingBuffer_RoundTrip(benchmark::State& state) {
  loon::RingBuffer<T, 1024> buffer;
  T msg{};

  for (auto _ : state) {
    buffer.push(msg);
    auto val = buffer.pop();
    benchmark::DoNotOptimize(val);
  }

  state.SetItemsProcessed(state.iterations() * 2);
  state.SetBytesProcessed(state.iterations() * sizeof(T) * 2);
}

BENCHMARK(BM_RingBuffer_RoundTrip<Msg16B>)->Name("RingBuffer/RoundTrip/16B");
BENCHMARK(BM_RingBuffer_RoundTrip<Msg64B>)->Name("RingBuffer/RoundTrip/64B");
BENCHMARK(BM_RingBuffer_RoundTrip<Msg256B>)->Name("RingBuffer/RoundTrip/256B");

// Throughput benchmark - batch operations
template <typename T>
static void BM_RingBuffer_Throughput(benchmark::State& state) {
  loon::RingBuffer<T, 4096> buffer;
  T msg{};
  constexpr size_t batch = 1000;

  for (auto _ : state) {
    for (size_t i = 0; i < batch; ++i) {
      buffer.push(msg);
    }
    for (size_t i = 0; i < batch; ++i) {
      auto val = buffer.pop();
      benchmark::DoNotOptimize(val);
    }
  }

  state.SetItemsProcessed(state.iterations() * batch * 2);
  state.SetBytesProcessed(state.iterations() * batch * sizeof(T) * 2);
}

BENCHMARK(BM_RingBuffer_Throughput<Msg16B>)->Name("RingBuffer/Throughput/16B");
BENCHMARK(BM_RingBuffer_Throughput<Msg64B>)->Name("RingBuffer/Throughput/64B");
BENCHMARK(BM_RingBuffer_Throughput<Msg256B>)->Name("RingBuffer/Throughput/256B");

// std::queue comparison for message sizes
template <typename T>
static void BM_StdQueue_RoundTrip(benchmark::State& state) {
  std::queue<T> queue;
  T msg{};

  for (auto _ : state) {
    queue.push(msg);
    auto val = queue.front();
    queue.pop();
    benchmark::DoNotOptimize(val);
  }

  state.SetItemsProcessed(state.iterations() * 2);
  state.SetBytesProcessed(state.iterations() * sizeof(T) * 2);
}

BENCHMARK(BM_StdQueue_RoundTrip<Msg16B>)->Name("std::queue/RoundTrip/16B");
BENCHMARK(BM_StdQueue_RoundTrip<Msg64B>)->Name("std::queue/RoundTrip/64B");
BENCHMARK(BM_StdQueue_RoundTrip<Msg256B>)->Name("std::queue/RoundTrip/256B");
