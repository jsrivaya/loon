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
 public:
  /// @brief Constructs an empty SpscQueue with a fixed capacity of N.
  /// The actual buffer size is N+1 to distinguish between full and empty states.
  /// @example
  /// loon::SpscQueue<int, 3> queue;
  SpscQueue() : capacity_(N + 1) {}

  /// @brief Pushes a value to the back of the queue.
  /// @param value The value to push (copied).
  /// @return true if the value was added, false if the queue is full.
  /// This method is safe to call from the producer thread.
  /// @example
  /// loon::SpscQueue<int, 3> queue;
  /// queue.push(42);
  bool push(const T& value) {
    auto tail = tail_.load(std::memory_order_relaxed);
    auto head = head_.load(std::memory_order_acquire);
    if ((tail + 1) % capacity_ == head) {
      return false;
    }

    data_[tail] = value;
    tail_.store((tail + 1) % capacity_, std::memory_order_release);
    return true;
  }

  /// @brief Pops a value from the front of the queue.
  /// @param value The value popped from the queue (output).
  /// @return true if a value was popped, false if the queue is empty.
  /// This method is safe to call from the consumer thread.
  /// @example
  /// int value;
  /// if (queue.pop(value)) {
  ///     // use value
  /// }
  bool pop(T& value) {
    auto tail = tail_.load(std::memory_order_acquire);
    auto head = head_.load(std::memory_order_relaxed);
    if (tail == head) {
      return false;
    }

    value = data_[head];
    head_.store((head_ + 1) % capacity_, std::memory_order_release);
    return true;
  }

  /// @brief   Returns the maximum number of elements the queue can hold.
  /// @return The maximum number of elements the queue can hold.
  /// The actual buffer size is N+1, but the usable capacity is N.
  size_t capacity() const { return capacity_ - 1; }

  /// @brief  Checks if the queue is empty.
  /// @return true if the queue is empty, false otherwise.
  bool empty() const { return head_ == tail_; }

  /// @brief Checks if the queue is full.
  /// @return true if the queue is full, false otherwise.
  bool full() const {
    return (tail_.load(std::memory_order_acquire) + 1) % capacity_ ==
           head_.load(std::memory_order_acquire);
  }

 private:
  size_t capacity_{0};
  T data_[N + 1];
  alignas(CACHE_LINE_SIZE) std::atomic<size_t> head_{0}; // Consumer-owned
  alignas(CACHE_LINE_SIZE) std::atomic<size_t> tail_{0}; // Producer-owned
  static_assert(std::atomic<size_t>::is_always_lock_free, "SpscQueue requires lock-free atomics");
};

} // namespace loon