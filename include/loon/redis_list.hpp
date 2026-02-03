#pragma once

/// @file redis_list.hpp
/// @brief Redis-compatible list data structure implementation.

#include <algorithm>
#include <deque>
#include <optional>
#include <vector>

namespace loon {

/// @brief A Redis-compatible list supporting operations from both ends.
///
/// RedisList provides a double-ended queue with an API modeled after Redis
/// list commands. It supports efficient O(1) push/pop operations at both
/// ends and O(n) range queries.
///
/// @tparam T Element type stored in the list.
///
/// @code
/// loon::RedisList<std::string> list;
/// list.rpush("hello");
/// list.rpush("world");
/// auto val = list.lpop();  // returns "hello"
/// @endcode
template <typename T>
class RedisList {
 public:
  /// @brief Constructs an empty RedisList.
  explicit RedisList() {}

  /// @brief Default destructor.
  ~RedisList() = default;

  /// @brief Copy constructor.
  RedisList(const RedisList& other) = default;

  /// @brief Move constructor.
  RedisList(RedisList&& other) noexcept = default;

  /// @brief Constructs a RedisList from a vector by moving its elements.
  /// @param other Vector to move elements from.
  explicit RedisList(std::vector<T>&& other)
      : impl(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end())) {}

  /// @brief Copy assignment operator.
  RedisList& operator=(const RedisList& other) = default;

  /// @brief Move assignment operator.
  RedisList& operator=(RedisList&& other) noexcept = default;

  /// @brief Pushes a value to the front of the list (left push).
  /// @param value The value to push (copied).
  /// @return The new length of the list.
  size_t lpush(const T& value) {
    impl.push_front(value);
    return impl.size();
  }

  /// @brief Pushes a value to the front of the list (left push).
  /// @param value The value to push (moved).
  /// @return The new length of the list.
  size_t lpush(T&& value) {
    impl.push_front(std::move(value));
    return impl.size();
  }

  /// @brief Pushes a value to the back of the list (right push).
  /// @param value The value to push (copied).
  /// @return The new length of the list.
  size_t rpush(const T& value) {
    impl.push_back(value);
    return impl.size();
  }

  /// @brief Pushes a value to the back of the list (right push).
  /// @param value The value to push (moved).
  /// @return The new length of the list.
  size_t rpush(T&& value) {
    impl.push_back(std::move(value));
    return impl.size();
  }

  /// @brief Removes and returns the first element (left pop).
  /// @return The removed element, or std::nullopt if the list is empty.
  std::optional<T> lpop() {
    if (empty()) {
      return std::nullopt;
    }
    const auto result = std::move(impl.front());
    impl.pop_front();
    return result;
  }

  /// @brief Removes and returns up to count elements from the front.
  /// @param count Maximum number of elements to remove.
  /// @return Vector of removed elements (may be smaller than count).
  std::vector<T> lpop(size_t count) {
    count = std::min(count, impl.size());
    std::vector<T> result(std::make_move_iterator(impl.begin()),
                          std::make_move_iterator(impl.begin() + count));
    impl.erase(impl.begin(), impl.begin() + count);
    return result;
  }

  /// @brief Removes and returns the last element (right pop).
  /// @return The removed element, or std::nullopt if the list is empty.
  std::optional<T> rpop() {
    if (empty()) {
      return std::nullopt;
    }
    const auto result = std::move(impl.back());
    impl.pop_back();
    return result;
  }

  /// @brief Removes and returns up to count elements from the back.
  /// @param count Maximum number of elements to remove.
  /// @return Vector of removed elements (may be smaller than count).
  std::vector<T> rpop(size_t count) {
    count = std::min(count, impl.size());
    std::vector<T> result(std::make_move_iterator(impl.end() - count),
                          std::make_move_iterator(impl.end()));
    impl.erase(impl.end() - count, impl.end());
    return result;
  }

  /// @brief Returns a range of elements without removing them.
  ///
  /// Supports negative indices: -1 is the last element, -2 is second to last, etc.
  /// The range is inclusive on both ends (unlike typical C++ iterators).
  ///
  /// @param start Starting index (inclusive, can be negative).
  /// @param stop Ending index (inclusive, can be negative).
  /// @return Vector of elements in the specified range, empty if invalid range.
  ///
  /// @code
  /// list.lrange(0, -1);   // Returns all elements
  /// list.lrange(0, 2);    // Returns first 3 elements
  /// list.lrange(-3, -1);  // Returns last 3 elements
  /// @endcode
  std::vector<T> lrange(int start, int stop) const {
    start = start < 0 ? size() + start : start;
    stop = stop < 0 ? size() + stop : stop;

    // Check if range is valid
    if (start > stop || start >= (int)size() || stop < 0) {
      return {};
    }
    // Clamp to list bounds
    start = std::max(0, start);
    stop = std::min(stop, (int)size() - 1);

    std::vector<T> result(impl.begin() + start, impl.begin() + stop + 1);
    return result;
  }

  /// @brief Returns the length of the list (Redis LLEN command).
  /// @return The number of elements in the list.
  size_t llen() const { return size(); }

  /// @brief Returns the number of elements in the list.
  /// @return The number of elements in the list.
  size_t size() const { return impl.size(); }

  /// @brief Checks if the list is empty.
  /// @return true if the list contains no elements, false otherwise.
  bool empty() const { return impl.empty(); }

 private:
  std::deque<T> impl;  ///< Underlying container for O(1) operations at both ends.
};

}  // namespace loon
