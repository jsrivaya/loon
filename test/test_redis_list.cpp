#include <gtest/gtest.h>
#include <loon/redis_list.hpp>

#include <string>

class RedisListTest : public ::testing::Test {
protected:
    loon::RedisList<int> list;
};

TEST_F(RedisListTest, EmptyOnConstruction) {
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(list.llen(), 0);
}

TEST_F(RedisListTest, LPush) {
    EXPECT_EQ(list.lpush(1), 1);
    EXPECT_EQ(list.lpush(2), 2);
    EXPECT_EQ(list.size(), 2);
}

TEST_F(RedisListTest, RPush) {
    EXPECT_EQ(list.rpush(1), 1);
    EXPECT_EQ(list.rpush(2), 2);
    EXPECT_EQ(list.size(), 2);
}

TEST_F(RedisListTest, LPop) {
    list.lpush(1);
    list.lpush(2); // List is now [2, 1]

    auto result = list.lpop();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 2);
    EXPECT_EQ(list.size(), 1);
}

TEST_F(RedisListTest, LPopEmpty) {
    auto result = list.lpop();
    EXPECT_FALSE(result.has_value());
}

TEST_F(RedisListTest, RPop) {
    list.rpush(1);
    list.rpush(2); // List is now [1, 2]

    auto result = list.rpop();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 2);
    EXPECT_EQ(list.size(), 1);
}

TEST_F(RedisListTest, RPopEmpty) {
    auto result = list.rpop();
    EXPECT_FALSE(result.has_value());
}

TEST_F(RedisListTest, LPopMultiple) {
    list.rpush(1);
    list.rpush(2);
    list.rpush(3); // List is now [1, 2, 3]

    auto result = list.lpop(2);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(list.size(), 1);
}

TEST_F(RedisListTest, RPopMultiple) {
    list.rpush(1);
    list.rpush(2);
    list.rpush(3); // List is now [1, 2, 3]

    auto result = list.rpop(2);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 2);
    EXPECT_EQ(result[1], 3);
    EXPECT_EQ(list.size(), 1);
}

TEST_F(RedisListTest, LRange) {
    list.rpush(1);
    list.rpush(2);
    list.rpush(3);
    list.rpush(4); // List is now [1, 2, 3, 4]

    auto result = list.lrange(1, 2);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 2);
    EXPECT_EQ(result[1], 3);
}

TEST_F(RedisListTest, LRangeNegativeIndex) {
    list.rpush(1);
    list.rpush(2);
    list.rpush(3);
    list.rpush(4); // List is now [1, 2, 3, 4]

    auto result = list.lrange(-2, -1);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 3);
    EXPECT_EQ(result[1], 4);
}

TEST_F(RedisListTest, LRangeFullList) {
    list.rpush(1);
    list.rpush(2);
    list.rpush(3);

    auto result = list.lrange(0, -1);
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 3);
}

TEST_F(RedisListTest, LRangeInvalidRange) {
    list.rpush(1);
    list.rpush(2);

    auto result = list.lrange(5, 10);
    EXPECT_TRUE(result.empty());
}

TEST_F(RedisListTest, MoveConstruction) {
    std::vector<int> vec = {1, 2, 3};
    loon::RedisList<int> moved_list(std::move(vec));

    EXPECT_EQ(moved_list.size(), 3);
    auto result = moved_list.lrange(0, -1);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 3);
}
