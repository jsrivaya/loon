#include <atomic>
#include <cstddef>
#include <new>

#ifndef CACHE_LINE_SIZE
#if defined(__cpp_lib_hardware_interference_size)
#define CACHE_LINE_SIZE std::hardware_destructive_interference_size
#else
#define CACHE_LINE_SIZE 64
#endif
#endif

namespace loon {

/// @brief A lock-free single-producer single-consumer (SPSC) queue with fixed capacity.
///
/// SpscQueue provides O(1) push and pop operations without locks, suitable for
/// communication between a single producer thread and a single consumer thread. The
/// capacity is fixed at compile time and the queue will reject new elements when full.
///
/// Indices are ever-increasing (never explicitly wrapped), relying on well-defined
/// unsigned integer overflow. Buffer access uses modulo N.
///
/// @tparam T The element type to store.
/// @tparam N The maximum number of elements the queue can hold (must be > 0).
/// @par Example
/// @code
/// loon::SpscQueue<int, 3> queue;
/// queue.push(42);
/// int value;
/// if (queue.pop(value)) {
///     // use value
/// }
/// @endcode
template <typename T, size_t N>
class SpscQueue {
  static_assert(N > 0, "SpscQueue capacity must be greater than 0");
  static_assert(std::atomic<size_t>::is_always_lock_free, "SpscQueue requires lock-free atomics");

 public:
  /// @brief Pushes a value to the back of the queue.
  /// @param value The value to push (copied).
  /// @return true if the value was added, false if the queue is full.
  /// This method is safe to call from the producer thread only.
  [[nodiscard]] bool push(const T& value) {
    auto write = write_idx_.load(std::memory_order_relaxed);
    if (write - read_idx_cache_ == N) {
      read_idx_cache_ = read_idx_.load(std::memory_order_acquire);
      if (write - read_idx_cache_ == N)
        return false;
    }
    data_[write % N] = value;
    write_idx_.store(write + 1, std::memory_order_release);
    return true;
  }

  /// @brief Pops a value from the front of the queue.
  /// @param value The value popped from the queue (output).
  /// @return true if a value was popped, false if the queue is empty.
  /// This method is safe to call from the consumer thread only.
  [[nodiscard]] bool pop(T& value) {
    auto read = read_idx_.load(std::memory_order_relaxed);
    if (write_idx_cache_ == read) {
      write_idx_cache_ = write_idx_.load(std::memory_order_acquire);
      if (write_idx_cache_ == read)
        return false;
    }
    value = data_[read % N];
    read_idx_.store(read + 1, std::memory_order_release);
    return true;
  }

  /// @brief Returns the maximum number of elements the queue can hold.
  size_t capacity() const { return N; }

  /// @brief Checks if the queue is empty.
  [[nodiscard]] bool empty() const {
    return write_idx_.load(std::memory_order_acquire) == read_idx_.load(std::memory_order_acquire);
  }

  /// @brief Checks if the queue is full.
  [[nodiscard]] bool full() const {
    return write_idx_.load(std::memory_order_acquire) - read_idx_.load(std::memory_order_acquire) ==
           N;
  }

 private:
  T data_[N];
  alignas(CACHE_LINE_SIZE) std::atomic<size_t> write_idx_{0}; // Producer-owned
  alignas(CACHE_LINE_SIZE) size_t write_idx_cache_{0};        // Producer's cache of read_idx_
  alignas(CACHE_LINE_SIZE) std::atomic<size_t> read_idx_{0};  // Consumer-owned
  alignas(CACHE_LINE_SIZE) size_t read_idx_cache_{0};         // Consumer's cache of write_idx_
};

} // namespace loon
