#pragma once

#include <list>
#include <unordered_map>
#include <optional>

namespace looneytools {

template <typename K, typename V>
class LRUCache {
public:
    explicit LRUCache(size_t size) : capacity(size) {}

    std::optional<std::reference_wrapper<V>> get(const K& key) {
        if(map.find(key) == map.end())
            return std::nullopt;

        setMRU(key);
        return map[key]->second;
    }
    void put(const K& key, const V& value) {
        if(map.find(key) != map.end()) {
           map[key]->second = value;
           setMRU(key);
        } else {
            if(capacity == store.size()) {
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

private:
    size_t capacity;
    std::list<std::pair<K, V>> store;  // MRU at front, LRU at back
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map;

    void setMRU(const K& key) {
        auto element_itr = map[key];
        store.splice(store.begin(), store, element_itr);
    }
};

} // namespace looneytools