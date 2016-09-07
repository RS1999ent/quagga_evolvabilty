#include <gtest/gtest.h>
#include "wiser_config.h"
#include "general_configuration.h"
#include <google/protobuf/text_format.h>

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

class GeneralConfigurationTest : public testing::Test {
public:
  GeneralConfigurationTest(){
    const string kSampleConfig = R"(
    protocol_type: PT_WISER
wiser_protocol_config {
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
}
)";
    Configuration sample_general_config;
    google::protobuf::TextFormat::ParseFromString(kSampleConfig, &sample_general_config);
    general_config_.reset(new GeneralConfiguration(sample_general_config));
  }
  void SetUp() { 
  }

  std::unique_ptr<GeneralConfiguration> general_config_;

};

TEST_F(GeneralConfigurationTest, ProtocolType_ConfigHasPTWISER_GETWISERENUM){
  const string kInput = R"(
    protocol_type: PT_WISER
)";
  dbgp_protocol_t kCorrectOutput = dbgp_critical_wiser;
  Configuration sample_config;
  google::protobuf::TextFormat::ParseFromString(kInput, &sample_config);
  GeneralConfiguration general_config(sample_config);

  dbgp_protocol_t output = general_config.GetProtocolType();

  EXPECT_EQ(kCorrectOutput, output);



}

TEST_F(GeneralConfigurationTest, ProtocolType_ConfigHasPTUNKNOWN_GetUnknown){
  const string kInput = R"(
    protocol_type: PT_UNKNOWN
)";
  dbgp_protocol_t kCorrectOutput = dbgp_protocol_baseline;
  Configuration sample_config;
  google::protobuf::TextFormat::ParseFromString(kInput, &sample_config);
  GeneralConfiguration general_config(sample_config);

  dbgp_protocol_t output = general_config.GetProtocolType();

  EXPECT_EQ(kCorrectOutput, output);


}

TEST_F(GeneralConfigurationTest, GetWiserConfig_ConfigHasWiserConfig_NonNullReferenceReturned){
  const string kInput = R"(
    protocol_type: PT_UNKNOWN
    wiser_protocol_config {

    }
)";
  Configuration sample_config;
  google::protobuf::TextFormat::ParseFromString(kInput, &sample_config);
  GeneralConfiguration general_config(sample_config);

  WiserConfig* output = general_config.GetWiserConfig();

  EXPECT_TRUE(output != NULL);

}

TEST_F(GeneralConfigurationTest, GetWiserConfig_ConfigHasNoWiserConfig_NullReferenceReturned){
  const string kInput = R"(
    protocol_type: PT_UNKNOWN
)";
  Configuration sample_config;
  google::protobuf::TextFormat::ParseFromString(kInput, &sample_config);
  GeneralConfiguration general_config(sample_config);

  WiserConfig* output = general_config.GetWiserConfig();

  EXPECT_TRUE(output == NULL);

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
