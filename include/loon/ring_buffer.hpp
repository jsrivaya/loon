#pragma once

/// @file ring_buffer.hpp
/// @brief Fixed-size ring buffer (circular queue) implementation.

#include <array>
#include <cstddef>
#include <optional>

namespace loon {

/// @brief A fixed-size circular buffer (ring buffer) with FIFO semantics.
///
/// RingBuffer provides O(1) push and pop operations with a compile-time
/// fixed capacity. When full, it can either reject new elements or override
/// the oldest element depending on configuration.
///
/// @tparam T The element type to store.
/// @tparam N The fixed capacity of the buffer (must be > 0).
///
/// @par Example
/// @code
/// loon::RingBuffer<int, 10> buffer;
/// buffer.push(42);
/// buffer.push(43);
/// auto val = buffer.pop();  // returns 42
/// @endcode
template <typename T, size_t N>
class RingBuffer {
 public:
  /// @brief Constructs an empty RingBuffer with default behavior (reject when full).
  RingBuffer() = default;

  /// @brief Constructs an empty RingBuffer with configurable override behavior.
  /// @param override_when_full If true, push() overwrites oldest element when full.
  ///                           If false, push() returns false when full.
  explicit RingBuffer(bool override_when_full) : override(override_when_full) {}

  /// @brief Pushes a value to the back of the buffer.
  /// @param value The value to push (copied).
  /// @return true if the value was added, false if buffer is full and override is disabled.
  bool push(const T& value) {
    if (full()) {
      if (!override) {
        return false;
      }
      // advance read pointer to discard oldest
      read = (read + 1) % N;
    } else {
      ++count;
    }
    buffer[write] = value;
    write = (write + 1) % N;

    return true;
  }

  /// @brief Removes and returns the front element.
  /// @return The front element, or std::nullopt if empty.
  std::optional<T> pop() {
    if (empty()) {
      return std::nullopt;
    }
    T value = buffer[read];
    read = (read + 1) % N;
    --count;
    return value;
  }

  /// @brief Returns the front element without removing it.
  /// @return The front element, or std::nullopt if empty.
  std::optional<T> front() {
    if (empty()) {
      return std::nullopt;
    }
    return buffer[read];
  }

  /// @brief Returns the back element without removing it.
  /// @return The back element, or std::nullopt if empty.
  std::optional<T> back() {
    if (empty()) {
      return std::nullopt;
    }
    return buffer[(write - 1 + N) % N];
  }

  /// @brief Discards the front element without returning it.
  /// @return true if an element was discarded, false if buffer was empty.
  bool discard() {
    if (empty()) {
      return false;
    }
    read = (read + 1) % N;
    --count;
    return true;
  }

  /// @brief Returns the maximum capacity of the buffer.
  /// @return The compile-time capacity N.
  [[nodiscard]] size_t capacity() const { return N; }

  /// @brief Checks if the buffer is empty.
  /// @return true if the buffer contains no elements.
  [[nodiscard]] bool empty() const { return count == 0; }

  /// @brief Checks if the buffer is full.
  /// @return true if the buffer contains N elements.
  [[nodiscard]] bool full() const { return count == N; }

  /// @brief Checks if override mode is enabled.
  /// @return true if push() will override oldest element when full.
  [[nodiscard]] bool overrides() const { return override; }

  /// @brief Returns the current number of elements.
  /// @return The number of elements in the buffer.
  [[nodiscard]] size_t size() const { return count; }

 private:
  std::array<T, N> buffer;
  size_t write = 0;
  size_t read = 0;
  size_t count = 0;
  bool override = false;
};

} // namespace loon
