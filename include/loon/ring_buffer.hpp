#pragma once

/// @file ring_buffer.hpp
/// @brief Fixed-size ring buffer (circular queue) implementation.

#include <array>
#include <cstddef>
#include <optional>

namespace loon {

template <typename T, size_t N>
class RingBuffer {
    public:
    /// @brief Constructs an empty RingBuffer.
    RingBuffer() = default;
    RingBuffer(bool override_when_full) : override(override_when_full) {}

    /// @brief Pushes a value to the back of the buffer.
    /// @param value The value to push (copied).
    bool push(const T& value) {
        if (full() && !override) {
            return false;
        }
        head = head + 1 % N;
        buffer[head] = value;
        ++count;
        return true;
    }

    size_t capacity() const {
        return N;
    }

    bool empty() const {
        return count == 0;
    }

    bool full () const {
        return count == N;
    }

    bool overrides() const {
        return override;
    }

    size_t size() const {
        return count;
    }

    private:
    std::array<T, N> buffer;
    size_t head = 0;
    size_t tail = 0;
    size_t count = 0;
    bool override = false;
};

}  // namespace loon
