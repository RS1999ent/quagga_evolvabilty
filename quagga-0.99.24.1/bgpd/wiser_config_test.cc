#include <gtest/gtest.h>
#include "wiser_config.h"
#include <google/protobuf/text_format.h>

namespace quagga_bgpd {
using std::string;
class WiserConfigTest : public testing::Test {
public:
  WiserConfigTest(){
      
  }
  void SetUp() { 
  }

  WiserConfig *wiser_config;

};

TEST_F(WiserConfigTest, GetLinkCost_GivenTwoExistingLinks_GetCorrectCost)
{
  EXPECT_EQ(1, 2);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
} // namespace quagga_bgpd
