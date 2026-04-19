#include <loon/bloom_filter.hpp>

#include <gtest/gtest.h>
#include <string>

namespace {

struct LengthHash {
  size_t operator()(const std::string& value) const {
    return value.size() * 1469598103934665603ULL;
  }
};

struct RollingHash {
  size_t operator()(const std::string& value) const {
    size_t hash = 0;
    for (const auto ch : value) {
      hash = hash * 131 + static_cast<unsigned char>(ch);
    }
    return hash;
  }
};

} // namespace

class BloomFilterTest : public ::testing::Test {
 protected:
  loon::BloomFilter<4096> filter;
};

TEST_F(BloomFilterTest, EmptyFilterContainsNothing) {
  EXPECT_FALSE(filter.contains("alpha"));
}

TEST_F(BloomFilterTest, InsertAndContains) {
  filter.insert("alpha");
  filter.insert("beta");
  filter.insert("gamma");

  EXPECT_TRUE(filter.contains("alpha"));
  EXPECT_TRUE(filter.contains("beta"));
  EXPECT_TRUE(filter.contains("gamma"));
}

TEST_F(BloomFilterTest, ClearResetsFilter) {
  filter.insert("alpha");
  ASSERT_TRUE(filter.contains("alpha"));
  ASSERT_GT(filter.false_positive_rate(), 0.0);

  filter.clear();

  EXPECT_FALSE(filter.contains("alpha"));
  EXPECT_EQ(filter.size(), 0);
  EXPECT_DOUBLE_EQ(filter.false_positive_rate(), 0.0);
}

TEST_F(BloomFilterTest, HashFunctionCountIsConfigurable) {
  loon::BloomFilter<4096> tuned_filter(7);

  EXPECT_EQ(tuned_filter.hash_functions(), 7);
  tuned_filter.insert("value");
  EXPECT_TRUE(tuned_filter.contains("value"));
}

TEST_F(BloomFilterTest, FalsePositiveRateTracksInsertions) {
  EXPECT_DOUBLE_EQ(filter.false_positive_rate(), 0.0);

  filter.insert("alpha");
  const auto after_first_insert = filter.false_positive_rate();
  filter.insert("beta");
  const auto after_second_insert = filter.false_positive_rate();

  EXPECT_GT(after_first_insert, 0.0);
  EXPECT_GT(after_second_insert, after_first_insert);
}

TEST_F(BloomFilterTest, WithCapacityChoosesTargetRate) {
  constexpr size_t expected_items = 200;
  constexpr double target_fpr = 0.01;
  auto tuned_filter = loon::BloomFilter<8192>::with_capacity(expected_items, target_fpr);

  EXPECT_GE(tuned_filter.hash_functions(), 1);

  for (size_t i = 0; i < expected_items; ++i) {
    tuned_filter.insert("item-" + std::to_string(i));
  }

  EXPECT_LE(tuned_filter.false_positive_rate(), target_fpr + 1e-12);
}

TEST_F(BloomFilterTest, SupportsCustomHashAlgorithms) {
  loon::BloomFilter<2048, std::string, LengthHash, RollingHash> custom_filter(4);

  custom_filter.insert("loon");
  custom_filter.insert("bloom");

  EXPECT_TRUE(custom_filter.contains("loon"));
  EXPECT_TRUE(custom_filter.contains("bloom"));
}