#include <loon/lru.hpp>

#include <gtest/gtest.h>
#include <string>

class LRUTest : public ::testing::Test {
 protected:
  loon::LRU<int, std::string> cache{3};
};

TEST_F(LRUTest, PutAndGet) {
  cache.put(1, "one");
  auto result = cache.get(1);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->get(), "one");
}

TEST_F(LRUTest, GetNonExistent) {
  auto result = cache.get(999);
  EXPECT_FALSE(result.has_value());
}

TEST_F(LRUTest, Exists) {
  cache.put(1, "one");
  EXPECT_TRUE(cache.exists(1));
  EXPECT_FALSE(cache.exists(2));
}

TEST_F(LRUTest, Remove) {
  cache.put(1, "one");
  EXPECT_TRUE(cache.exists(1));
  cache.remove(1);
  EXPECT_FALSE(cache.exists(1));
}

TEST_F(LRUTest, RemoveUnknownKey) {
  cache.put(1, "one");
  EXPECT_TRUE(cache.exists(1));
  auto current_size = cache.size();
  EXPECT_FALSE(cache.exists(2));
  cache.remove(2);
  EXPECT_EQ(cache.size(), current_size);
}

TEST_F(LRUTest, Size) {
  EXPECT_EQ(cache.size(), 0);
  cache.put(1, "one");
  EXPECT_EQ(cache.size(), 1);
  cache.put(2, "two");
  EXPECT_EQ(cache.size(), 2);
}

TEST_F(LRUTest, EvictsLRU) {
  cache.put(1, "one");
  cache.put(2, "two");
  cache.put(3, "three");
  cache.put(4, "four"); // Should evict key 1

  EXPECT_FALSE(cache.exists(1));
  EXPECT_TRUE(cache.exists(2));
  EXPECT_TRUE(cache.exists(3));
  EXPECT_TRUE(cache.exists(4));
}

TEST_F(LRUTest, GetUpdatesRecency) {
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

TEST_F(LRUTest, GetMRURepeatedly) {
  cache.put(1, "one");
  cache.put(2, "two");
  cache.put(3, "three");

  // Repeatedly access the MRU key — previously caused self-loop corruption in set_mru
  cache.get(3);
  cache.get(3);
  cache.get(3);

  // Eviction must still target the true LRU (key 1)
  cache.put(4, "four");

  EXPECT_FALSE(cache.exists(1));
  EXPECT_TRUE(cache.exists(2));
  EXPECT_TRUE(cache.exists(3));
  EXPECT_TRUE(cache.exists(4));
}

TEST_F(LRUTest, UpdateExistingKey) {
  cache.put(1, "one");
  cache.put(1, "ONE");

  auto result = cache.get(1);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->get(), "ONE");
  EXPECT_EQ(cache.size(), 1);
}
