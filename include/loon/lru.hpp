// Copyright (c) 2026 Jorge Suarez-Rivaya
// SPDX-License-Identifier: MIT

#pragma once

/// @file lru.hpp
/// @brief Thread-safe LRU (Least Recently Used) cache implementation.
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

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
  explicit LRU(uint32_t size) : capacity(size), store(size) {
    for (uint32_t i = 0; i < size; ++i) {
      store[i].next = i + 1;
    }
    store[size - 1].next = NIL;
  }

  /// @brief Retrieves a value from the cache.
  ///
  /// If the key exists, it is marked as most recently used.
  ///
  /// @param key The key to look up.
  /// @return A reference to the value if found, std::nullopt otherwise.
  std::optional<std::reference_wrapper<V>> get(const K& key) {
    const auto it = lookup.find(key);
    if (it == lookup.end())
      return std::nullopt;

    set_mru(it->second);

    return std::ref(store[it->second].value);
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
    auto [it, inserted] = lookup.try_emplace(key);
    if (inserted) { // key didnt exist
      if (capacity < lookup.size()) {
        evict();
      }

      auto idx = emplace_front(key, value);
      it->second = idx;
    } else {
      store[it->second].value = value;
      set_mru(it->second);
    }
  }

  /// @brief Checks if a key exists in the cache.
  ///
  /// This operation does not affect the recency of the key.
  ///
  /// @param key The key to check.
  /// @return true if the key exists, false otherwise.
  bool exists(const K& key) const { return lookup.find(key) != lookup.end(); }

  /// @brief Removes a key-value pair from the cache.
  ///
  /// If the key does not exist, this operation has no effect.
  ///
  /// @param key The key to remove.
  void remove(const K& key) {
    const auto it = lookup.find(key);
    if (it == lookup.end())
      return;

    lookup.erase(it);
  }

  /// @brief Returns the current number of entries in the cache.
  /// @return The number of cached entries.
  uint32_t size() const { return lookup.size(); }

 private:
  uint32_t capacity;

  struct Node {
    K key;
    V value;
    uint32_t prev;
    uint32_t next;
  };

  std::vector<Node> store;
  uint32_t front = NIL;    ///< MRU at front
  uint32_t back = NIL;     ///< LRU at back
  uint32_t free_front = 0; ///< First free node
  std::unordered_map<K, uint32_t> lookup;

  // Moves node to the front (MRU position). Unlinks from current position,
  // updates back if it was the tail. No-op if already the front.
  void set_mru(uint32_t node) {
    if (node == front) {
      return;
    }

    auto prev = store[node].prev;
    auto next = store[node].next;

    if (prev != NIL) {
      store[prev].next = next;
      if (next != NIL) {
        store[next].prev = prev;
      } else {
        // we have to update
        back = store[node].prev;
      }
    }

    if (front == NIL && back == NIL) { // first insertion
      front = node;
      back = node;
      return;
    }
    store[node].next = front; // if first node front == NIL
    store[node].prev = NIL;

    store[front].prev = node;
    front = node;
  }

  // Sentinel: no-node. prev == NIL → head, next == NIL → tail, front/back == NIL → empty.
  static constexpr uint32_t NIL = UINT32_MAX;

  // Removes the LRU (tail) node, erases it from the map, and returns it to the free list.
  void evict() {
    lookup.erase(store[back].key);
    auto node = back;
    back = store[node].prev;
    store[back].next = NIL;        // Last element next is now NIL
    store[node].prev = NIL;        // move node to head of free nodes
    store[node].next = free_front; // move node to head of free nodes
    free_front = node;
  }

  // Pops a node from the free list, fills it, and links it at the front.
  uint32_t emplace_front(K key, const V& value) {
    auto node = free_front;
    free_front = store[node].next;

    store[node].key = key;
    store[node].value = value;
    store[node].prev = NIL;
    store[node].next = NIL;
    set_mru(node);

    return node;
  }
};

} // namespace loon
