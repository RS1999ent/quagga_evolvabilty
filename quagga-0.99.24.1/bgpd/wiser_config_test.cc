#include <gtest/gtest.h>
#include "wiser_config.h"
#include "general_configuration.h"
#include <google/protobuf/text_format.h>
#include "integrated_advertisement_functions.h"
#include "integrated_advertisement.pb.h"

using std::string;
class WiserConfigTest : public testing::Test {
  public:
    WiserConfigTest(){
      const string kSampleConfig = R"(
      topology {
        node_links {
          primary_node {
            node_name: 'a1'
            interface_ip: '172.0.1.1'
          }
          links {
            adjacent_node {
              node_name: 'b1'
              interface_ip: '172.0.2.1'
            }
            link_cost: 1000
          }
          links {
            adjacent_node {
              node_name: 'c1'
              interface_ip: '172.0.3.1'
            }
            link_cost: 10
          }
        }
        node_links {
          primary_node {
            node_name: 'b1'
            interface_ip: '172.0.2.1'
          }
          links {
            adjacent_node {
              node_name: 'a1'
              interface_ip: '172.0.1.1'
            }
            link_cost: 1000
          }
          links {
            adjacent_node {
              node_name: 'c1'
              interface_ip: '172.0.3.1'
            }
            link_cost: 10
          }
        }
        node_links {
          primary_node {
            node_name: 'c1'
            interface_ip: '172.0.3.1'
          }
          links {
            adjacent_node {
              node_name: 'b1'
              interface_ip: '172.0.2.1'
            }
            link_cost: 10
          }
          links {
            adjacent_node {
              node_name: 'a1'
              interface_ip: '172.0.1.1'
            }
            link_cost: 10
          }
        }
      })";

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
    const string kIp1 = "172.0.1.1";
    const string kIp2 = "172.0.2.1";
    const int kCorrectOutput = 1000;

    // Act
    int result = wiser_config->GetLinkCost(kIp1, kIp2);

    // Assert
         EXPECT_EQ(result, kCorrectOutput);
  }

TEST_F(WiserConfigTest, GetLinkCost_GivenExistingLinkReverse_GetCorrectCost)
{
  // Arrange
  const string kIp1 = "172.0.2.1";
  const string kIp2 = "172.0.1.1";
  const int kCorrectOutput = 1000;
  
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

TEST_F(GeneralConfigurationTest, IsRemoteAsAnIslandMember_InputASIsMember_Return1) {
  // Arrange
  const string kSampleConfiguration = R"(
    island_member_ases : 1
    island_member_ases : 2
    island_member_ases : 3
    )";
  const int input_remote_as = 2;
  const int correct_output = 1;

  Configuration working_configuration;
  google::protobuf::TextFormat::ParseFromString(kSampleConfiguration, &working_configuration);
  GeneralConfiguration general_config(working_configuration);

  // Act
  int result = general_config.IsRemoteAsAnIslandMember(input_remote_as);

  EXPECT_EQ(result, correct_output);

}

TEST_F(GeneralConfigurationTest, IsRemoteAsAnIslandMember_InputASIsNotMember_Return0) {
  // Arrange
  const string kSampleConfiguration = R"(
    island_member_ases : 1
    island_member_ases : 2
    island_member_ases : 3
    )";
  const int input_remote_as = 22;
  const int correct_output = 0;

  Configuration working_configuration;
  google::protobuf::TextFormat::ParseFromString(kSampleConfiguration, &working_configuration);
  GeneralConfiguration general_config(working_configuration);

  // Act
  int result = general_config.IsRemoteAsAnIslandMember(input_remote_as);

  EXPECT_EQ(result, correct_output);

}


////////////////////////////////////////////////////////////
/////////// integrated_advertisement_functions tests////////
////////////////////////////////////////////////////////////

//this was to test what is going on with serialize to array
// TEST(CreateEmptyIntegratedAdvertisementTest, TestOutput){
//   int size = 0;
//   char *serialized_integrated_advertisement = CreateEmptyIntegratedAdvertisement(&size);
  
//   std::cout << size << std::endl;
//   EXPECT_TRUE(false);
// }

TEST(SetWiserControlInfoTest, GiveEmptyExistAdvertWithPathCost_GetAdvertWithCostBack){
  // Arrange
  const string kInputAdvert = "";
  const int kAdditivePathCost = 2;
  const string kCorrectAdvertisement = R"(
path_group_descriptors {
  protocol: P_WISER
  key_values {
    key: "PathCost"
    value: "\010\002"
  }
}
)";

  int modified_advert_size;
  IntegratedAdvertisement input_advert, correct_advert, result_advert;
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvertisement, &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int input_advert_size = input_advert.ByteSize();
  char * serialized_input_advert = new char[input_advert_size];
  input_advert.SerializeToArray(serialized_input_advert, input_advert_size);

  // Act
  char* result = SetWiserControlInfo(serialized_input_advert,
                                     input_advert_size,
                                     kAdditivePathCost,
                                     &modified_advert_size);
  result_advert.ParseFromArray(result, modified_advert_size);

  // Assert
  EXPECT_STREQ(result_advert.DebugString().c_str(), correct_advert.DebugString().c_str());


}

TEST(SetWiserControlInfoTest, GiveExistAdvertWithPathCost_GetAdvertWithCostBack){
  // Arrange
  const string kInputAdvert = R"(
path_group_descriptors {
  protocol: P_WISER
  key_values {
    key: "PathCost"
    value: "\010\002"
  }
}
)";
  const int kAdditivePathCost = 2;

  const string kCorrectAdvertisement = R"(
path_group_descriptors {
  protocol: P_WISER
  key_values {
    key: "PathCost"
    value: "\010\004"
  }
}
)";

  int modified_advert_size;
  IntegratedAdvertisement input_advert, correct_advert, result_advert;
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvertisement, &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int input_advert_size = input_advert.ByteSize();
  char * serialized_input_advert = new char[input_advert_size];
  input_advert.SerializeToArray(serialized_input_advert, input_advert_size);

  // Act
  char* result = SetWiserControlInfo(serialized_input_advert,
                                     input_advert_size,
                                     kAdditivePathCost,
                                     &modified_advert_size);
  result_advert.ParseFromArray(result, modified_advert_size);

  // Assert
  EXPECT_STREQ(result_advert.DebugString().c_str(), correct_advert.DebugString().c_str());


}

TEST(SetWiserControlInfoTest, GiveExistAdvertWithNoPathCost_GetAdvertWithCostBack){
  // Arrange
  const string kInputAdvert = R"(
path_group_descriptors {
  protocol: P_WISER
  key_values {
    key: "JUNK"
    value: "\010\002"
  }
}
)";
  const int kAdditivePathCost = 2;

  const string kCorrectAdvertisement = R"(
path_group_descriptors {
  protocol: P_WISER
  key_values {
    key: "JUNK"
    value: "\010\002"
  }
  key_values {
    key: "PathCost"
    value: "\010\002"
  }
}
)";

  int modified_advert_size;
  IntegratedAdvertisement input_advert, correct_advert, result_advert;
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvertisement, &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int input_advert_size = input_advert.ByteSize();
  char * serialized_input_advert = new char[input_advert_size];
  input_advert.SerializeToArray(serialized_input_advert, input_advert_size);

  // Act
  char* result = SetWiserControlInfo(serialized_input_advert,
                                     input_advert_size,
                                     kAdditivePathCost,
                                     &modified_advert_size);
  result_advert.ParseFromArray(result, modified_advert_size);

  // Assert
  EXPECT_STREQ(result_advert.DebugString().c_str(), correct_advert.DebugString().c_str());


}

TEST (GetLastWiserNode, AdvertHasNoExistingLastWiserNode_ReturnNeg1){
  // Arrange
  const string kInputAdvert = R"()";

  const int kCorrectOutput= -1;

  IntegratedAdvertisement input_advert;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int size = input_advert.ByteSize();
  char *serialized_input_advert= new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int result = GetLastWiserNode(serialized_input_advert, size);

  // Assert
  EXPECT_EQ(result, kCorrectOutput);


}

TEST (GetLastWiserNode, AdvertHasNoLastWiserButWiserInfo_ReturnNeg1){
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    )";

  const int kCorrectOutput= -1;

  IntegratedAdvertisement input_advert;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int size = input_advert.ByteSize();
  char *serialized_input_advert= new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int result = GetLastWiserNode(serialized_input_advert, size);

  // Assert
  EXPECT_EQ(result, kCorrectOutput);


}

TEST (GetLastWiserNode, AdvertHasLastWiserNode_ReturnCorrect){
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
        key_values {
            key : 'LastWiserNode'
            value : '22'
        }
    }
    )";

  const int kCorrectOutput= 22;

  IntegratedAdvertisement input_advert;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int size = input_advert.ByteSize();
  char *serialized_input_advert= new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int result = GetLastWiserNode(serialized_input_advert, size);

  // Assert
  EXPECT_EQ(result, kCorrectOutput);


}

TEST (SetLastWiserNode, AdvertHasNoKeyValue_ReturnCorrect){
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    )";

  const int kInput= 22;

  const string kCorrectOutput = R"(
    path_group_descriptors {
        protocol : P_WISER
        key_values {
            key : 'LastWiserNode'
            value: '22'
        }
    }
)";

  IntegratedAdvertisement input_advert, output_advert, correct_output;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);
  google::protobuf::TextFormat::ParseFromString(kCorrectOutput, &correct_output);
  

  int size = input_advert.ByteSize();
  char *serialized_input_advert= new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int return_size;
  char* result = SetLastWiserNode(serialized_input_advert, size, kInput, &return_size );
  output_advert.ParseFromArray(result, return_size);

  // Assert
  EXPECT_STREQ(output_advert.DebugString().c_str(), correct_output.DebugString().c_str() );


}

TEST (SetLastWiserNode, AdvertHasNoWiserInfo_ReturnCorrect){
  // Arrange
  const string kInputAdvert = R"(
    )";

  const int kInput= 22;

  const string kCorrectOutput = R"(
    path_group_descriptors {
        protocol : P_WISER
        key_values {
            key : 'LastWiserNode'
            value: '22'
        }
    }
)";

  IntegratedAdvertisement input_advert, output_advert, correct_output;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);
  google::protobuf::TextFormat::ParseFromString(kCorrectOutput, &correct_output);
  

  int size = input_advert.ByteSize();
  char *serialized_input_advert= new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int return_size;
  char* result = SetLastWiserNode(serialized_input_advert, size, kInput, &return_size );
  output_advert.ParseFromArray(result, return_size);

  // Assert
  EXPECT_STREQ(output_advert.DebugString().c_str(), correct_output.DebugString().c_str() );


}

TEST (SetLastWiserNode, AdvertHasWiserInfoAndOldLastWiserNode_ReturnCorrect){
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
        key_values {
            key : 'LastWiserNode'
            value: '15'
        }
    }
    )";

  const int kInput= 22;

  const string kCorrectOutput = R"(
    path_group_descriptors {
        protocol : P_WISER
        key_values {
            key : 'LastWiserNode'
            value: '22'
        }
    }
)";

  IntegratedAdvertisement input_advert, output_advert, correct_output;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);
  google::protobuf::TextFormat::ParseFromString(kCorrectOutput, &correct_output);
  

  int size = input_advert.ByteSize();
  char *serialized_input_advert= new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int return_size;
  char* result = SetLastWiserNode(serialized_input_advert, size, kInput, &return_size );
  output_advert.ParseFromArray(result, return_size);

  // Assert
  EXPECT_STREQ(output_advert.DebugString().c_str(), correct_output.DebugString().c_str() );


}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
