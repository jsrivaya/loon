#include <loon/ring_buffer.hpp>

#include <gtest/gtest.h>

#include <string>

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
