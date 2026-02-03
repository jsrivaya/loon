#pragma once

/// @file lru.hpp
/// @brief Thread-safe LRU (Least Recently Used) cache implementation.

#include <list>
#include <optional>
#include <unordered_map>

namespace loon {

/// @brief A Least Recently Used (LRU) cache with O(1) access and eviction.
///
/// This cache maintains a fixed capacity and automatically evicts the least
/// recently used entries when the capacity is exceeded. Both get() and put()
/// operations update the recency of the accessed key.
///
/// @tparam K Key type (must be hashable for std::unordered_map)
/// @tparam V Value type
///
/// @code
/// loon::LRU<std::string, int> cache(100);
/// cache.put("key", 42);
/// if (auto val = cache.get("key")) {
///     std::cout << val->get() << std::endl;
/// }
/// @endcode
template <typename K, typename V>
class LRU {
 public:
  /// @brief Constructs an LRU cache with the specified capacity.
  /// @param size Maximum number of entries the cache can hold.
  explicit LRU(size_t size) : capacity(size) {}

  /// @brief Retrieves a value from the cache.
  ///
  /// If the key exists, it is marked as most recently used.
  ///
  /// @param key The key to look up.
  /// @return A reference to the value if found, std::nullopt otherwise.
  std::optional<std::reference_wrapper<V>> get(const K& key) {
    const auto it = map.find(key);
    if (it == map.end())
      return std::nullopt;

    setMRU(key);
    const auto store_entry_itr = it->second;
    return store_entry_itr->second;
  }

  /// @brief Inserts or updates a key-value pair in the cache.
  ///
  /// If the key already exists, its value is updated and it becomes the most
  /// recently used. If the cache is at capacity, the least recently used entry
  /// is evicted before inserting the new entry.
  ///
  /// @param key The key to insert or update.
  /// @param value The value to associate with the key.
  void put(const K& key, const V& value) {
    const auto it = map.find(key);
    if (it != map.end()) {
      auto store_entry_itr = it->second;
      store_entry_itr->second = value;
      setMRU(key);
    } else {
      if (capacity == store.size()) {
        map.erase(store.back().first);
        store.pop_back();
      }
      store.emplace_front(std::pair{key, value});
      map[key] = store.begin();
    }
  }

  /// @brief Checks if a key exists in the cache.
  ///
  /// This operation does not affect the recency of the key.
  ///
  /// @param key The key to check.
  /// @return true if the key exists, false otherwise.
  bool exists(const K& key) const { return map.find(key) != map.end(); }

  /// @brief Removes a key-value pair from the cache.
  ///
  /// If the key does not exist, this operation has no effect.
  ///
  /// @param key The key to remove.
  void remove(const K& key) {
    const auto it = map.find(key);
    if (it == map.end())
      return;
    store.erase(it->second);
    map.erase(it);
  }

  /// @brief Returns the current number of entries in the cache.
  /// @return The number of cached entries.
  size_t size() const { return store.size(); }

 private:
  size_t capacity;
  std::list<std::pair<K, V>> store; ///< MRU at front, LRU at back
  std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map;

  /// @brief Moves a key to the most recently used position.
  /// @param key The key to mark as most recently used.
  void setMRU(const K& key) {
    auto element_itr = map[key];
    store.splice(store.begin(), store, element_itr);
  }
};

} // namespace loon
