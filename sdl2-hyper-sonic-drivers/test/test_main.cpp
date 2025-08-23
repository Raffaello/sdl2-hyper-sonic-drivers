// Fallback test main if the project doesn't already define one.
// Testing library: GoogleTest
#include <gtest/gtest.h>
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}