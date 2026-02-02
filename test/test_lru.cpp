#include <loon/lru.hpp>

#include <gtest/gtest.h>
#include <string>

class LRUCacheTest : public ::testing::Test {
 protected:
  loon::LRUCache<int, std::string> cache{3};
};

TEST_F(LRUCacheTest, PutAndGet) {
  cache.put(1, "one");
  auto result = cache.get(1);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->get(), "one");
}

TEST_F(LRUCacheTest, GetNonExistent) {
  auto result = cache.get(999);
  EXPECT_FALSE(result.has_value());
}

TEST_F(LRUCacheTest, Exists) {
  cache.put(1, "one");
  EXPECT_TRUE(cache.exists(1));
  EXPECT_FALSE(cache.exists(2));
}

TEST_F(LRUCacheTest, Remove) {
  cache.put(1, "one");
  EXPECT_TRUE(cache.exists(1));
  cache.remove(1);
  EXPECT_FALSE(cache.exists(1));
}

TEST_F(LRUCacheTest, Size) {
  EXPECT_EQ(cache.size(), 0);
  cache.put(1, "one");
  EXPECT_EQ(cache.size(), 1);
  cache.put(2, "two");
  EXPECT_EQ(cache.size(), 2);
}

TEST_F(LRUCacheTest, EvictsLRU) {
  cache.put(1, "one");
  cache.put(2, "two");
  cache.put(3, "three");
  cache.put(4, "four"); // Should evict key 1

  EXPECT_FALSE(cache.exists(1));
  EXPECT_TRUE(cache.exists(2));
  EXPECT_TRUE(cache.exists(3));
  EXPECT_TRUE(cache.exists(4));
}

TEST_F(LRUCacheTest, GetUpdatesRecency) {
  cache.put(1, "one");
  cache.put(2, "two");
  cache.put(3, "three");

  cache.get(1); // Access key 1, making it MRU

  cache.put(4, "four"); // Should evict key 2 (now LRU)

  EXPECT_TRUE(cache.exists(1));
  EXPECT_FALSE(cache.exists(2));
  EXPECT_TRUE(cache.exists(3));
  EXPECT_TRUE(cache.exists(4));
}

TEST_F(LRUCacheTest, UpdateExistingKey) {
  cache.put(1, "one");
  cache.put(1, "ONE");

  auto result = cache.get(1);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->get(), "ONE");
  EXPECT_EQ(cache.size(), 1);
}
