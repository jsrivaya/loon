// Copyright (c) 2026 Jorge Suarez-Rivaya
// SPDX-License-Identifier: MIT

#pragma once

/// @file bloom_filter.hpp
/// @brief Space-efficient probabilistic set membership data structure.

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <string>
#include <utility>

namespace loon {

namespace detail {

template <typename Key>
struct BloomSecondaryHash {
  size_t operator()(const Key& key) const {
    uint64_t value = static_cast<uint64_t>(std::hash<Key>{}(key));
    value += 0x9e3779b97f4a7c15ULL;
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    value ^= (value >> 31);
    return static_cast<size_t>(value);
  }
};

} // namespace detail

/// @brief Fixed-size Bloom filter for fast probabilistic membership checks.
///
/// BloomFilter stores membership bits in a compact bit array and supports
/// O(k) insert/contains operations, where k is the number of hash functions.
/// It guarantees no false negatives and allows false positives.
///
/// @tparam N Number of bits in the filter.
/// @tparam Key Key type used for insert/contains operations.
/// @tparam PrimaryHash Primary hash function.
/// @tparam SecondaryHash Secondary hash function used for double hashing.
///
/// @par Example
/// @code
/// loon::BloomFilter<4096> filter;
/// filter.insert("alpha");
/// bool may_exist = filter.contains("alpha");
/// @endcode
template <size_t N, typename Key = std::string, typename PrimaryHash = std::hash<Key>,
          typename SecondaryHash = detail::BloomSecondaryHash<Key>>
class BloomFilter {
  static_assert(N > 0, "BloomFilter bit count must be greater than 0");

 public:
  /// @brief Construct an empty Bloom filter.
  /// @param hash_functions Number of hash probes per insert/lookup.
  /// @param primary_hash Primary hash algorithm instance.
  /// @param secondary_hash Secondary hash algorithm instance.
  explicit BloomFilter(size_t hash_functions = 3, PrimaryHash primary_hash = PrimaryHash{},
                       SecondaryHash secondary_hash = SecondaryHash{})
      : hash_functions_(std::clamp(hash_functions, size_t{1}, kMaxHashFunctions)),
        primary_hash_(std::move(primary_hash)),
        secondary_hash_(std::move(secondary_hash)) {}

  /// @brief Build a Bloom filter tuned for expected load and target FPR.
  ///
  /// For fixed-size filters (N bits), this chooses the smallest hash count that
  /// meets target_fpr for expected_items when possible. If target_fpr cannot be
  /// met with this N, it falls back to the analytically optimal hash count.
  ///
  /// @param expected_items Estimated number of insertions.
  /// @param target_fpr Desired false positive rate in (0, 1).
  /// @param primary_hash Primary hash algorithm instance.
  /// @param secondary_hash Secondary hash algorithm instance.
  /// @return Tuned BloomFilter instance.
  static BloomFilter with_capacity(size_t expected_items, double target_fpr,
                                   PrimaryHash primary_hash = PrimaryHash{},
                                   SecondaryHash secondary_hash = SecondaryHash{}) {
    if (expected_items == 0) {
      return BloomFilter(1, std::move(primary_hash), std::move(secondary_hash));
    }

    if (!(target_fpr > 0.0 && target_fpr < 1.0)) {
      target_fpr = 0.01;
    }

    size_t selected_hash_functions = optimal_hash_functions(expected_items);
    for (size_t k = 1; k <= kMaxHashFunctions; ++k) {
      if (estimate_false_positive_rate(expected_items, k) <= target_fpr) {
        selected_hash_functions = k;
        break;
      }
    }

    return BloomFilter(selected_hash_functions, std::move(primary_hash), std::move(secondary_hash));
  }

  /// @brief Insert a key into the filter.
  /// @param key Key to insert.
  void insert(const Key& key) {
    const auto [base, step] = hashes_for(key);
    for (size_t i = 0; i < hash_functions_; ++i) {
      set_bit(index_for(base, step, i));
    }
    ++inserted_items_;
  }

  /// @brief Check if a key may be present.
  /// @param key Key to query.
  /// @return false means definitely absent, true means possibly present.
  [[nodiscard]] bool contains(const Key& key) const {
    const auto [base, step] = hashes_for(key);
    for (size_t i = 0; i < hash_functions_; ++i) {
      if (!is_set(index_for(base, step, i))) {
        return false;
      }
    }
    return true;
  }

  /// @brief Clear all bits in the filter.
  void clear() {
    bits_.fill(0);
    inserted_items_ = 0;
  }

  /// @brief Estimated false positive rate based on insertion count.
  [[nodiscard]] double false_positive_rate() const {
    return estimate_false_positive_rate(inserted_items_, hash_functions_);
  }

  /// @brief Return configured number of hash functions.
  [[nodiscard]] size_t hash_functions() const { return hash_functions_; }

  /// @brief Return number of performed insertions.
  [[nodiscard]] size_t size() const { return inserted_items_; }

  /// @brief Return number of bits in the filter.
  [[nodiscard]] size_t bit_count() const { return N; }

 private:
  static constexpr size_t kWordBits = 64;
  static constexpr size_t kWordCount = (N + kWordBits - 1) / kWordBits;
  static constexpr size_t kMaxHashFunctions = 64;

  std::array<uint64_t, kWordCount> bits_{};
  size_t hash_functions_;
  size_t inserted_items_ = 0;
  PrimaryHash primary_hash_;
  SecondaryHash secondary_hash_;

  static size_t optimal_hash_functions(size_t expected_items) {
    const auto ratio = static_cast<double>(N) / static_cast<double>(expected_items);
    const auto estimate = static_cast<size_t>(std::round(ratio * std::log(2.0)));
    return std::clamp(estimate, size_t{1}, kMaxHashFunctions);
  }

  static double estimate_false_positive_rate(size_t items, size_t hash_functions) {
    if (items == 0) {
      return 0.0;
    }

    const auto n = static_cast<double>(items);
    const auto m = static_cast<double>(N);
    const auto k = static_cast<double>(hash_functions);
    return std::pow(1.0 - std::exp((-k * n) / m), k);
  }

  [[nodiscard]] std::pair<size_t, size_t> hashes_for(const Key& key) const {
    const auto base = static_cast<size_t>(primary_hash_(key));
    auto step = static_cast<size_t>(secondary_hash_(key));
    if (step == 0) {
      step = 0x9e3779b97f4a7c15ULL;
    }
    return {base, step};
  }

  [[nodiscard]] static size_t index_for(size_t base, size_t step, size_t i) {
    const auto iteration = static_cast<uint64_t>(i);
    return static_cast<size_t>(
        (static_cast<uint64_t>(base) + iteration * step + iteration * iteration) % N);
  }

  void set_bit(size_t index) { bits_[index / kWordBits] |= (uint64_t{1} << (index % kWordBits)); }

  [[nodiscard]] bool is_set(size_t index) const {
    return (bits_[index / kWordBits] & (uint64_t{1} << (index % kWordBits))) != 0;
  }
};

} // namespace loon