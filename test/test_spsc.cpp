#include <loon/spsc.hpp>

#include <gtest/gtest.h>

class SpscQueueTest : public ::testing::Test {
 protected:
  loon::SpscQueue<int, 3> queue_buffer;
};

TEST_F(SpscQueueTest, PopEmpty) {
  int actual(-1);
  ASSERT_TRUE(queue_buffer.empty());
  ASSERT_FALSE(queue_buffer.pop(actual));
}

TEST_F(SpscQueueTest, PushFull) {
  ASSERT_TRUE(queue_buffer.push(1));
  ASSERT_TRUE(queue_buffer.push(2));
  ASSERT_TRUE(queue_buffer.push(3));
  ASSERT_TRUE(queue_buffer.full());
  ASSERT_FALSE(queue_buffer.push(4));
}

TEST_F(SpscQueueTest, PushAndPop) {
  int expected(1);
  ASSERT_TRUE(queue_buffer.push(expected));
  int actual(-1);
  ASSERT_TRUE(queue_buffer.pop(actual));
  ASSERT_EQ(expected, actual);
}