#pragma once

#include <algorithm>
#include <queue>
#include <vector>

namespace loon {

    template <typename T>
    class RedisList {
      public:
        explicit RedisList() {
        }
        ~RedisList() = default;

        // Copy and move semantics
        RedisList(const RedisList& other) = default;
        RedisList(RedisList&& other) noexcept = default;
        RedisList(std::vector<T>&& other)
            : impl(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end())) {
        }
        RedisList& operator=(const RedisList& other) = default;
        RedisList& operator=(RedisList&& other) noexcept = default;

        size_t lpush(const T& value) {
            impl.push_front(value);
            return impl.size();
        }
        size_t lpush(T&& value) {
            impl.push_front(std::move(value));
            return impl.size();
        }
        size_t rpush(const T& value) {
            impl.push_back(value);
            return impl.size();
        }
        size_t rpush(T&& value) {
            impl.push_back(std::move(value));
            return impl.size();
        }
        std::optional<T> lpop() {
            if (empty()) {
                return std::nullopt;
            }
            const auto result = std::move(impl.front());
            impl.pop_front();
            return result;
        }
        std::vector<T> lpop(size_t count) {
            count = std::min(count, impl.size());
            std::vector<T> result(std::make_move_iterator(impl.begin()), std::make_move_iterator(impl.begin() + count));
            // std::move(impl.begin(), impl.begin() + count, result.begin());
            impl.erase(impl.begin(), impl.begin() + count);
            return result;
        }
        std::optional<T> rpop() {
            if (empty()) {
                return std::nullopt;
            }
            const auto result = std::move(impl.back());
            impl.pop_back();
            return result;
        }
        std::vector<T> rpop(size_t count) {
            count = std::min(count, impl.size());
            std::vector<T> result(std::make_move_iterator(impl.end() - count), std::make_move_iterator(impl.end()));
            // std::move(impl.end() - count, impl.end(), result.begin());
            impl.erase(impl.end() - count, impl.end());
            return result;
        }
        std::vector<T> lrange(int start, int stop) const {
            start = start < 0 ? size() + start : start;
            stop = stop < 0 ? size() + stop : stop;

            // Check if range is valid
            if (start > stop || start >= (int) size() || stop < 0) {
                return {};
            }
            // Clamp to list bounds
            start = std::max(0, start);
            stop = std::min(stop, (int) size() - 1);

            std::vector<T> result(impl.begin() + start, impl.begin() + stop + 1);
            return result;
        }
        // std::optional<std::reference_wrapper<const T>> lindex(int index) const;
        // bool lset(int index, const T& value);
        size_t llen() const {
            return size();
        }
        size_t size() const {
            return impl.size();
        }
        bool empty() const {
            return impl.empty();
        }

      private:
        std::deque<T> impl;
    };

} // namespace loon
