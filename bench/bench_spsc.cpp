#include <loon/spsc.hpp>

#include <array>
#include <atomic>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <mutex>
#include <queue>
#include <thread>

// ----------------------------------------------------------------------------
// Message types of different sizes
// ----------------------------------------------------------------------------

struct Msg16B {
  int64_t id;
  int64_t timestamp;
}; // 16 bytes

struct Msg64B {
  int64_t id;
  int64_t timestamp;
  std::array<char, 48> payload;
}; // 64 bytes

struct Msg256B {
  int64_t id;
  int64_t timestamp;
  std::array<char, 240> payload;
}; // 256 bytes

// ----------------------------------------------------------------------------
// Single-threaded benchmarks (baseline latency)
// ----------------------------------------------------------------------------

static void BM_SpscQueue_Push(benchmark::State& state) {
  const auto size = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    loon::SpscQueue<int, 4096> queue;
    for (size_t i = 0; i < size; ++i) {
      queue.push(static_cast<int>(i));
    }
    benchmark::DoNotOptimize(queue);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_SpscQueue_Push)->Range(64, 4096);

static void BM_SpscQueue_Pop(benchmark::State& state) {
  const auto size = static_cast<size_t>(state.range(0));
  for (auto _ : state) {
    state.PauseTiming();
    loon::SpscQueue<int, 4096> queue;
    for (size_t i = 0; i < size; ++i) {
      queue.push(static_cast<int>(i));
    }
    state.ResumeTiming();

    int val;
    for (size_t i = 0; i < size; ++i) {
      queue.pop(val);
      benchmark::DoNotOptimize(val);
    }
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_SpscQueue_Pop)->Range(64, 4096);

static void BM_SpscQueue_PushPop_Interleaved(benchmark::State& state) {
  loon::SpscQueue<int, 1024> queue;
  int value = 0;
  int out;
  for (auto _ : state) {
    queue.push(value++);
    queue.pop(out);
    benchmark::DoNotOptimize(out);
  }
  state.SetItemsProcessed(state.iterations() * 2);
}
BENCHMARK(BM_SpscQueue_PushPop_Interleaved);

// ----------------------------------------------------------------------------
// Message size benchmarks
// ----------------------------------------------------------------------------

template <typename T>
static void BM_SpscQueue_RoundTrip(benchmark::State& state) {
  loon::SpscQueue<T, 1024> queue;
  T msg{};
  T out{};

  for (auto _ : state) {
    queue.push(msg);
    queue.pop(out);
    benchmark::DoNotOptimize(out);
  }

  state.SetItemsProcessed(state.iterations() * 2);
  state.SetBytesProcessed(state.iterations() * sizeof(T) * 2);
}

BENCHMARK(BM_SpscQueue_RoundTrip<Msg16B>)->Name("SpscQueue/RoundTrip/16B");
BENCHMARK(BM_SpscQueue_RoundTrip<Msg64B>)->Name("SpscQueue/RoundTrip/64B");
BENCHMARK(BM_SpscQueue_RoundTrip<Msg256B>)->Name("SpscQueue/RoundTrip/256B");

template <typename T>
static void BM_SpscQueue_Throughput(benchmark::State& state) {
  loon::SpscQueue<T, 4096> queue;
  T msg{};
  T out{};
  constexpr size_t batch = 1000;

  for (auto _ : state) {
    for (size_t i = 0; i < batch; ++i) {
      queue.push(msg);
    }
    for (size_t i = 0; i < batch; ++i) {
      queue.pop(out);
      benchmark::DoNotOptimize(out);
    }
  }

  state.SetItemsProcessed(state.iterations() * batch * 2);
  state.SetBytesProcessed(state.iterations() * batch * sizeof(T) * 2);
}

BENCHMARK(BM_SpscQueue_Throughput<Msg16B>)->Name("SpscQueue/Throughput/16B");
BENCHMARK(BM_SpscQueue_Throughput<Msg64B>)->Name("SpscQueue/Throughput/64B");
BENCHMARK(BM_SpscQueue_Throughput<Msg256B>)->Name("SpscQueue/Throughput/256B");

// ----------------------------------------------------------------------------
// Multi-threaded benchmark (real SPSC use case)
// ----------------------------------------------------------------------------

static void BM_SpscQueue_ProducerConsumer(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));

  for (auto _ : state) {
    loon::SpscQueue<int, 4096> queue;
    std::atomic<bool> done{false};
    std::atomic<size_t> consumed{0};

    // Consumer thread
    std::thread consumer([&] {
      int val;
      size_t local_consumed = 0;
      while (local_consumed < count) {
        if (queue.pop(val)) {
          benchmark::DoNotOptimize(val);
          ++local_consumed;
        }
      }
      consumed.store(local_consumed, std::memory_order_release);
    });

    // Producer (main thread)
    for (size_t i = 0; i < count; ++i) {
      while (!queue.push(static_cast<int>(i))) {
        // Spin until space available
      }
    }

    consumer.join();
  }

  state.SetItemsProcessed(state.iterations() * state.range(0) * 2);
}
BENCHMARK(BM_SpscQueue_ProducerConsumer)->Range(1024, 1 << 16);

// ----------------------------------------------------------------------------
// Mutex-based queue comparison (baseline)
// ----------------------------------------------------------------------------

template <typename T>
class MutexQueue {
 public:
  explicit MutexQueue(size_t /*capacity*/) {}

  bool push(const T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(value);
    return true;
  }

  bool pop(T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      return false;
    }
    value = queue_.front();
    queue_.pop();
    return true;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

 private:
  mutable std::mutex mutex_;
  std::queue<T> queue_;
};

static void BM_MutexQueue_PushPop_Interleaved(benchmark::State& state) {
  MutexQueue<int> queue(1024);
  int value = 0;
  int out;
  for (auto _ : state) {
    queue.push(value++);
    queue.pop(out);
    benchmark::DoNotOptimize(out);
  }
  state.SetItemsProcessed(state.iterations() * 2);
}
BENCHMARK(BM_MutexQueue_PushPop_Interleaved);

template <typename T>
static void BM_MutexQueue_RoundTrip(benchmark::State& state) {
  MutexQueue<T> queue(1024);
  T msg{};
  T out{};

  for (auto _ : state) {
    queue.push(msg);
    queue.pop(out);
    benchmark::DoNotOptimize(out);
  }

  state.SetItemsProcessed(state.iterations() * 2);
  state.SetBytesProcessed(state.iterations() * sizeof(T) * 2);
}

BENCHMARK(BM_MutexQueue_RoundTrip<Msg16B>)->Name("MutexQueue/RoundTrip/16B");
BENCHMARK(BM_MutexQueue_RoundTrip<Msg64B>)->Name("MutexQueue/RoundTrip/64B");
BENCHMARK(BM_MutexQueue_RoundTrip<Msg256B>)->Name("MutexQueue/RoundTrip/256B");

static void BM_MutexQueue_ProducerConsumer(benchmark::State& state) {
  const auto count = static_cast<size_t>(state.range(0));

  for (auto _ : state) {
    MutexQueue<int> queue(4096);
    std::atomic<size_t> consumed{0};

    // Consumer thread
    std::thread consumer([&] {
      int val;
      size_t local_consumed = 0;
      while (local_consumed < count) {
        if (queue.pop(val)) {
          benchmark::DoNotOptimize(val);
          ++local_consumed;
        }
      }
      consumed.store(local_consumed, std::memory_order_release);
    });

    // Producer (main thread)
    for (size_t i = 0; i < count; ++i) {
      queue.push(static_cast<int>(i));
    }

    consumer.join();
  }

  state.SetItemsProcessed(state.iterations() * state.range(0) * 2);
}
BENCHMARK(BM_MutexQueue_ProducerConsumer)->Range(1024, 1 << 16);
