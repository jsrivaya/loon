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

    std::optional<T> pop() {
        if (empty()) {
            return std::nullopt;
        }
        T value = buffer[read];
        read = (read + 1) % N;
        --count;
        return value;
    }

    std::optional<T> front() {
        if (empty()) {
            return std::nullopt;
        }
        return buffer[read];
    }

    std::optional<T> back() {
        if (empty()) {
            return std::nullopt;
        }
        return buffer[(write - 1 + N) % N];
    }

    bool discard() {
        if (empty()) {
            return false;
        }
        read = (read + 1) % N;
        --count;
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
    size_t write = 0;
    size_t read = 0;
    size_t count = 0;
    bool override = false;
};

}  // namespace loon
