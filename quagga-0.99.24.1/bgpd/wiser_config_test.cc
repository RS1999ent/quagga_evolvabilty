#include <gtest/gtest.h>
#include "wiser_config.h"
#include <google/protobuf/text_format.h>

namespace quagga_bgpd {
  using std::string;
  class WiserConfigTest : public testing::Test {
  public:
    WiserConfigTest(){
      const string kSampleConfig = R"(
    topology {
        node_links {
            primary_node {
                node_name : 'a1'
                interface_ip : '192.168.1.1'
            }
            links {
                adjacent_node {
                    node_name : 'b1'
                    interface_ip : '192.168.1.2'
                }
                link_cost : 500
            }
        }
    }
)";
      WiserProtocolConfig sample_wiser_config;
      google::protobuf::TextFormat::ParseFromString(kSampleConfig, &sample_wiser_config);
      wiser_config.reset(new WiserConfig(sample_wiser_config));
    }
    void SetUp() { 
    }

    std::unique_ptr<WiserConfig> wiser_config;

  };

  // test todos: wiser_config does not have topology (todo maybe)
  // two existing links, reverse direction (testdone)
  // non exiting link ( testdone)

    TEST_F(WiserConfigTest, GetLinkCost_GivenExistingLink_GetCorrectCost)
  {
    // Arrange
    const string kIp1 = "192.168.1.1";
    const string kIp2 = "192.168.1.2";
    const int kCorrectOutput = 500;

    // Act
    int result = wiser_config->GetLinkCost(kIp1, kIp2);

    // Assert
         EXPECT_EQ(result, kCorrectOutput);
  }

  TEST_F(WiserConfigTest, GetLinkCost_GivenExistingLinkReverse_GetCorrectCost)
  {
    // Arrange
    const string kIp1 = "192.168.1.2";
    const string kIp2 = "192.168.1.1";
    const int kCorrectOutput = 500;

    // Act
    int result = wiser_config->GetLinkCost(kIp1, kIp2);

    // Assert
         EXPECT_EQ(result, kCorrectOutput);
  }

  TEST_F(WiserConfigTest, GetLinkCost_GivenNonExistentLink_GetCorrectCost)
  {
    // Arrange
    const string kIp1 = "999.999.999";
    const string kIp2 = "192.168.1.1";
    const int kCorrectOutput = -1;

    // Act
    int result = wiser_config->GetLinkCost(kIp1, kIp2);

    // Assert
         EXPECT_EQ(result, kCorrectOutput);
  }

} // namespace quagga_bgpd

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
