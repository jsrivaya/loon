#include <atomic>
#include <cstddef>

namespace loon {

template <typename T, size_t N>
class SpscQueue {
 public:
  SpscQueue() : capacity_(N + 1) {};

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

  size_t capacity() const { return capacity_ - 1; }

  bool empty() const { return head_ == tail_; }

  bool full() const {
    return (tail_.load(std::memory_order_acquire) + 1) % capacity_ == head_.load(std::memory_order_acquire);
  }

 private:
  size_t capacity_{0};
  T data_[N + 1];
  std::atomic<size_t> head_{0};
  std::atomic<size_t> tail_{0};
};

} // namespace loon