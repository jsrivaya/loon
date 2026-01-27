#pragma once

#include <list>
#include <unordered_map>
#include <optional>

namespace looneytools {

    template <typename K, typename V>
    class LRUCache {
      public:
        explicit LRUCache(size_t size) : capacity(size) {
        }

        std::optional<std::reference_wrapper<V>> get(const K& key) {
            const auto it = map.find(key);
            if (it == map.end())
                return std::nullopt;

            setMRU(key);
            const auto store_entry_itr = it->second;
            return store_entry_itr->second;
        }
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

        bool exists(const K& key) const {
            return map.find(key) != map.end();
        }

        void remove(const K& key) {
            const auto it = map.find(key);
            if (it == map.end())
                return;
            store.erase(it->second);
            map.erase(it);
        }

        size_t size() const {
            return store.size();
        }

      private:
        size_t capacity;
        std::list<std::pair<K, V>> store; // MRU at front, LRU at back
        std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map;

        void setMRU(const K& key) {
            auto element_itr = map[key];
            store.splice(store.begin(), store, element_itr);
        }
    };

} // namespace looneytools