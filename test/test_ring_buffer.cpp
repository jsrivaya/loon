#include <loon/ring_buffer.hpp>

#include <gtest/gtest.h>

class RingBufferTest : public ::testing::Test {
 protected:
  loon::RingBuffer<int, 10> buffer;
};

TEST_F(RingBufferTest, EmptyOnConstruction) {
  EXPECT_TRUE(buffer.empty());
  EXPECT_FALSE(buffer.full());
  EXPECT_FALSE(buffer.overrides());
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.capacity(), 10);
}

TEST_F(RingBufferTest, PushAndPop) {
  EXPECT_TRUE(buffer.push(42));
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_FALSE(buffer.full());
  EXPECT_FALSE(buffer.empty());
  auto value = buffer.pop();
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), 42);
  EXPECT_TRUE(buffer.empty());
}

TEST_F(RingBufferTest, PopEmpty) {
  auto result = buffer.pop();
  EXPECT_FALSE(result.has_value());
}

TEST_F(RingBufferTest, FrontAndBackEmpty) {
  EXPECT_FALSE(buffer.front().has_value());
  EXPECT_FALSE(buffer.back().has_value());
}

TEST_F(RingBufferTest, FrontAndBack) {
  buffer.push(1);
  buffer.push(2);
  buffer.push(3);

  EXPECT_EQ(buffer.front().value(), 1);
  EXPECT_EQ(buffer.back().value(), 3);
}

TEST_F(RingBufferTest, Discard) {
  buffer.push(1);
  buffer.push(2);

  EXPECT_TRUE(buffer.discard());
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.front().value(), 2);

  EXPECT_TRUE(buffer.discard());
  EXPECT_TRUE(buffer.empty());

  EXPECT_FALSE(buffer.discard()); // empty, should fail
}

TEST_F(RingBufferTest, Full) {
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(buffer.push(i));
  }

  EXPECT_TRUE(buffer.full());
  EXPECT_EQ(buffer.size(), 10);
  EXPECT_FALSE(buffer.push(100)); // should fail when full
}

TEST_F(RingBufferTest, OverridePush) {
  loon::RingBuffer<int, 5> override_buffer(true); // enable overrides
  for (int i = 0; i < 5; ++i) {
    EXPECT_TRUE(override_buffer.push(i));
  }
  EXPECT_TRUE(override_buffer.full());
  EXPECT_EQ(override_buffer.size(), 5);
  EXPECT_TRUE(override_buffer.push(100)); // should succeed, overriding oldest
  EXPECT_TRUE(override_buffer.full());
  EXPECT_EQ(override_buffer.size(), 5);
}