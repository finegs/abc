#include <gtest/gtest.h>

// Function to be tested
int add(int a, int b) { return a + b; }

int minus(int a, int b) { return a - b; }

// Test case for the add function
TEST(AdderTest, BasicAddition) {
  EXPECT_EQ(add(2, 3), 5);
  EXPECT_EQ(add(-1, 1), 0);
  EXPECT_EQ(add(0, 0), 0);
}

TEST(MinusTest, BasicMinus) {
  EXPECT_GE(minus(2, 1), 1);
  EXPECT_GE(minus(2, 2), 0);
  EXPECT_GE(minus(2, -2), 4);
}

// Main function to run tests
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
