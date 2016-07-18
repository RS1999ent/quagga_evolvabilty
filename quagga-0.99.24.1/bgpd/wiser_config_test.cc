#include <gtest/gtest.h>

class WiserConfigTest : public testing::Test {
public:
  WiserConfigTest(){
    test = 1; 
  }
  void SetUp() { 
  }

  int test;

};

TEST_F(WiserConfigTest, blanktest)
{
  EXPECT_EQ(1, 2);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
