#include "wiser_config.h"
#include <google/protobuf/text_format.h>
#include <gtest/gtest.h>
#include "general_configuration.h"
#include "integrated_advertisement.pb.h"
#include "integrated_advertisement_functions.h"
#include "pathlet_internal_state.h"

using std::string;
using namespace std;
class WiserConfigTest : public testing::Test {
 public:
  WiserConfigTest() {
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
    google::protobuf::TextFormat::ParseFromString(kSampleConfig,
                                                  &sample_wiser_config);
    wiser_config.reset(new WiserConfig(sample_wiser_config));
  }
  void SetUp() {}

  std::unique_ptr<WiserConfig> wiser_config;
};

// test todos: wiser_config does not have topology (todo maybe)
// two existing links, reverse direction (testdone)
// non exiting link ( testdone)

TEST_F(WiserConfigTest, GetLinkCost_GivenExistingLink_GetCorrectCost) {
  // Arrange
  const string kIp1 = "172.0.1.1";
  const string kIp2 = "172.0.2.1";
  const int kCorrectOutput = 1000;

  // Act
  int result = wiser_config->GetLinkCost(kIp1, kIp2);

  // Assert
  EXPECT_EQ(result, kCorrectOutput);
}

TEST_F(WiserConfigTest, GetLinkCost_GivenExistingLinkReverse_GetCorrectCost) {
  // Arrange
  const string kIp1 = "172.0.2.1";
  const string kIp2 = "172.0.1.1";
  const int kCorrectOutput = 1000;

  // Act
  int result = wiser_config->GetLinkCost(kIp1, kIp2);

  // Assert
  EXPECT_EQ(result, kCorrectOutput);
}

TEST_F(WiserConfigTest, GetLinkCost_GivenNonExistentLink_GetCorrectCost) {
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
  GeneralConfigurationTest() {
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
    google::protobuf::TextFormat::ParseFromString(kSampleConfig,
                                                  &sample_general_config);
    general_config_.reset(new GeneralConfiguration(sample_general_config));
  }
  void SetUp() {}

  std::unique_ptr<GeneralConfiguration> general_config_;
};

TEST_F(GeneralConfigurationTest, ProtocolType_ConfigHasPTWISER_GETWISERENUM) {
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

TEST_F(GeneralConfigurationTest, ProtocolType_ConfigHasPTUNKNOWN_GetUnknown) {
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

TEST_F(GeneralConfigurationTest,
       GetWiserConfig_ConfigHasWiserConfig_NonNullReferenceReturned) {
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

TEST_F(GeneralConfigurationTest,
       GetWiserConfig_ConfigHasNoWiserConfig_NullReferenceReturned) {
  const string kInput = R"(
    protocol_type: PT_UNKNOWN
)";
  Configuration sample_config;
  google::protobuf::TextFormat::ParseFromString(kInput, &sample_config);
  GeneralConfiguration general_config(sample_config);

  WiserConfig* output = general_config.GetWiserConfig();

  EXPECT_TRUE(output == NULL);
}

TEST_F(GeneralConfigurationTest,
       IsRemoteAsAnIslandMember_InputASIsMember_Return1) {
  // Arrange
  const string kSampleConfiguration = R"(
    island_member_ases : 1
    island_member_ases : 2
    island_member_ases : 3
    )";
  const int input_remote_as = 2;
  const int correct_output = 1;

  Configuration working_configuration;
  google::protobuf::TextFormat::ParseFromString(kSampleConfiguration,
                                                &working_configuration);
  GeneralConfiguration general_config(working_configuration);

  // Act
  int result = general_config.IsRemoteAsAnIslandMember(input_remote_as);

  EXPECT_EQ(result, correct_output);
}

TEST_F(GeneralConfigurationTest,
       IsRemoteAsAnIslandMember_InputASIsNotMember_Return0) {
  // Arrange
  const string kSampleConfiguration = R"(
    island_member_ases : 1
    island_member_ases : 2
    island_member_ases : 3
    )";
  const int input_remote_as = 22;
  const int correct_output = 0;

  Configuration working_configuration;
  google::protobuf::TextFormat::ParseFromString(kSampleConfiguration,
                                                &working_configuration);
  GeneralConfiguration general_config(working_configuration);

  // Act
  int result = general_config.IsRemoteAsAnIslandMember(input_remote_as);

  EXPECT_EQ(result, correct_output);
}

////////////////////////////////////////////////////////////
/////////// integrated_advertisement_functions tests////////
////////////////////////////////////////////////////////////

// this was to test what is going on with serialize to array
// TEST(CreateEmptyIntegratedAdvertisementTest, TestOutput){
//   int size = 0;
//   char *serialized_integrated_advertisement =
//   CreateEmptyIntegratedAdvertisement(&size);

//   std::cout << size << std::endl;
//   EXPECT_TRUE(false);
// }

TEST(SetWiserControlInfoTest,
     GiveEmptyExistAdvertWithPathCost_GetAdvertWithCostBack) {
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
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvertisement,
                                                &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int input_advert_size = input_advert.ByteSize();
  char* serialized_input_advert = new char[input_advert_size];
  input_advert.SerializeToArray(serialized_input_advert, input_advert_size);

  // Act
  char* result =
      SetWiserControlInfo(serialized_input_advert, input_advert_size,
                          kAdditivePathCost, &modified_advert_size, 1);
  result_advert.ParseFromArray(result, modified_advert_size);

  // Assert
  EXPECT_STREQ(result_advert.DebugString().c_str(),
               correct_advert.DebugString().c_str());
}

TEST(SetWiserControlInfoTest,
     GiveExistAdvertWithPathCost_GetAdvertWithCostBack) {
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
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvertisement,
                                                &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int input_advert_size = input_advert.ByteSize();
  char* serialized_input_advert = new char[input_advert_size];
  input_advert.SerializeToArray(serialized_input_advert, input_advert_size);

  // Act
  char* result =
      SetWiserControlInfo(serialized_input_advert, input_advert_size,
                          kAdditivePathCost, &modified_advert_size, 1);
  result_advert.ParseFromArray(result, modified_advert_size);

  // Assert
  EXPECT_STREQ(result_advert.DebugString().c_str(),
               correct_advert.DebugString().c_str());
}

TEST(SetWiserControlInfoTest,
     GiveExistAdvertWithNoPathCost_GetAdvertWithCostBack) {
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
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvertisement,
                                                &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int input_advert_size = input_advert.ByteSize();
  char* serialized_input_advert = new char[input_advert_size];
  input_advert.SerializeToArray(serialized_input_advert, input_advert_size);

  // Act
  char* result =
      SetWiserControlInfo(serialized_input_advert, input_advert_size,
                          kAdditivePathCost, &modified_advert_size, 1);
  result_advert.ParseFromArray(result, modified_advert_size);

  // Assert
  EXPECT_STREQ(result_advert.DebugString().c_str(),
               correct_advert.DebugString().c_str());
}

TEST(GetWierPathCost, GivenAdvertWithMultipleKeyVals_ReturnProperCost) {
  // Arrange
  const string kInputAdvert = R"(
path_group_descriptors {
  protocol: P_WISER
  key_values {
    key: 'LastWiserNode'
    value: '\010e'
  }
  key_values {
    key: 'PathCost'
    value: '\010\n'
  }
})";

  const int kCorrectPathCost = 10;

  IntegratedAdvertisement input_advert;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);
  int input_advert_size = input_advert.ByteSize();
  char* serialized_input_advert = new char[input_advert_size];
  input_advert.SerializeToArray(serialized_input_advert, input_advert_size);

  // Act
  int result = GetWiserPathCost(serialized_input_advert, input_advert_size);

  // Assert
  EXPECT_EQ(result, kCorrectPathCost);
}

TEST(GetWiserPathCost, IntegrationTestWithSetLastWiserNode_ReturnProperCost) {
  // Arrange
  const string kInputAdvert = R"()";

  const int kCorrectPathCost = 10;
  const uint32_t kInputLastWiser = 20;

  IntegratedAdvertisement input_advert;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);
  int input_advert_size = input_advert.ByteSize();
  char* serialized_input_advert = new char[input_advert_size];
  input_advert.SerializeToArray(serialized_input_advert, input_advert_size);

  // SetLastWiserNodeInIntegratedAdvertisement
  char* old_integrated_advertisement = serialized_input_advert;
  int old_integrated_advertisement_size = input_advert_size;
  int new_size;
  char* new_integrated_advertisement_info = SetLastWiserNode(
      old_integrated_advertisement, old_integrated_advertisement_size,
      kInputLastWiser, &new_size);
  free(old_integrated_advertisement);

  new_integrated_advertisement_info = SetWiserControlInfo(
      new_integrated_advertisement_info, new_size, 10, &new_size, 1);

  // Act
  int result = GetWiserPathCost(new_integrated_advertisement_info, new_size);

  // Assert
  EXPECT_EQ(result, kCorrectPathCost);
}

TEST(GetLastWiserNode, AdvertHasNoExistingLastWiserNode_ReturnNeg1) {
  // Arrange
  const string kInputAdvert = R"()";

  const int kCorrectOutput = -1;

  IntegratedAdvertisement input_advert;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int size = input_advert.ByteSize();
  char* serialized_input_advert = new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int result = GetLastWiserNode(serialized_input_advert, size);

  // Assert
  EXPECT_EQ(result, kCorrectOutput);
}

TEST(GetLastWiserNode, AdvertHasNoLastWiserButWiserInfo_ReturnNeg1) {
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    )";

  const int kCorrectOutput = -1;

  IntegratedAdvertisement input_advert;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int size = input_advert.ByteSize();
  char* serialized_input_advert = new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int result = GetLastWiserNode(serialized_input_advert, size);

  // Assert
  EXPECT_EQ(result, kCorrectOutput);
}

TEST(GetLastWiserNode, AdvertHasLastWiserNode_ReturnCorrect) {
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

  const int kCorrectOutput = 22;

  IntegratedAdvertisement input_advert;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int size = input_advert.ByteSize();
  char* serialized_input_advert = new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int result = GetLastWiserNode(serialized_input_advert, size);

  // Assert
  EXPECT_EQ(result, kCorrectOutput);
}

TEST(SetLastWiserNode, AdvertHasNoKeyValue_ReturnCorrect) {
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    )";

  const int kInput = 22;

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
  google::protobuf::TextFormat::ParseFromString(kCorrectOutput,
                                                &correct_output);

  int size = input_advert.ByteSize();
  char* serialized_input_advert = new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int return_size;
  char* result =
      SetLastWiserNode(serialized_input_advert, size, kInput, &return_size);
  output_advert.ParseFromArray(result, return_size);

  // Assert
  EXPECT_STREQ(output_advert.DebugString().c_str(),
               correct_output.DebugString().c_str());
}

TEST(SetLastWiserNode, AdvertHasNoWiserInfo_ReturnCorrect) {
  // Arrange
  const string kInputAdvert = R"(
    )";

  const int kInput = 22;

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
  google::protobuf::TextFormat::ParseFromString(kCorrectOutput,
                                                &correct_output);

  int size = input_advert.ByteSize();
  char* serialized_input_advert = new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int return_size;
  char* result =
      SetLastWiserNode(serialized_input_advert, size, kInput, &return_size);
  output_advert.ParseFromArray(result, return_size);

  // Assert
  EXPECT_STREQ(output_advert.DebugString().c_str(),
               correct_output.DebugString().c_str());
}

TEST(SetLastWiserNode, AdvertHasWiserInfoAndOldLastWiserNode_ReturnCorrect) {
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

  const int kInput = 22;

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
  google::protobuf::TextFormat::ParseFromString(kCorrectOutput,
                                                &correct_output);

  int size = input_advert.ByteSize();
  char* serialized_input_advert = new char[size];
  input_advert.SerializeToArray(serialized_input_advert, size);

  // Act
  int return_size;
  char* result =
      SetLastWiserNode(serialized_input_advert, size, kInput, &return_size);
  output_advert.ParseFromArray(result, return_size);

  // Assert
  EXPECT_STREQ(output_advert.DebugString().c_str(),
               correct_output.DebugString().c_str());
}

TEST(GenerateExternalPathletControlInfo,
     InsertSomePathletsIn_GetCorrectAdvertBack) {
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
)";
  const int kInputIslandid = 1;
  const vector<string> kInsertPathlets = {
      R"(
    fid: 1
    vnodes: 1
    vnodes: 2
    )",
      R"(
    fid: 2
    vnodes: 1
    vnodes: 3
    )",
      R"(
    fid: 3
    vnodes: 2
    vnodes: 4
    )",
      R"(
    fid: 4
    vnodes: 2
    vnodes: 5
    )"};
  const string kCorrectAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    hop_descriptors{
        protocol: P_PATHLETS
        island_id: 1
        key_values{
            key : 'PathletGraph'
            value: "\n\006\010\001\020\001\020\002\n\006\010\002\020\001\020\003\n\006\010\003\020\002\020\004\n\006\010\004\020\002\020\005"
        }
    }
    )";

  PathletInternalState pathlet_internal_state("");

  IntegratedAdvertisement correct_advert, input_advert, result_advert;
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvert,
                                                &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  for (const string& pathlet_string : kInsertPathlets) {
    Pathlet insert_pathlet;
    google::protobuf::TextFormat::ParseFromString(pathlet_string,
                                                  &insert_pathlet);
    pathlet_internal_state.InsertPathletIntoGraph(insert_pathlet);
  }

  int size = input_advert.ByteSize();
  int newsize;
  char serialized_input[size];
  input_advert.SerializeToArray(serialized_input, size);

  // act
  char* result = GenerateExternalPathletControlInfo(
      &pathlet_internal_state, kInputIslandid, serialized_input, size,
      &newsize);

  result_advert.ParseFromArray(result, newsize);

  // Assert
  EXPECT_STREQ(result_advert.DebugString().c_str(),
               correct_advert.DebugString().c_str());
}

TEST(GenerateExternalPathletControlInfo,
     ExistingPathletHopDescriptorsInsertSomePathletsIn_GetCorrectAdvertBack) {
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    hop_descriptors {
        island_id: 1
        protocol : P_PATHLETS
    }
    )";
  const int kInputIslandid = 1;
  const vector<string> kInsertPathlets = {
      R"(
    fid: 1
    vnodes: 1
    vnodes: 2
    )",
      R"(
    fid: 2
    vnodes: 1
    vnodes: 3
    )",
      R"(
    fid: 3
    vnodes: 2
    vnodes: 4
    )",
      R"(
    fid: 4
    vnodes: 2
    vnodes: 5
    )"};
  const string kCorrectAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    hop_descriptors{
        protocol: P_PATHLETS
        island_id: 1
        key_values{
            key : 'PathletGraph'
            value: "\n\006\010\001\020\001\020\002\n\006\010\002\020\001\020\003\n\006\010\003\020\002\020\004\n\006\010\004\020\002\020\005"
        }
    }
    )";

  PathletInternalState pathlet_internal_state("");

  IntegratedAdvertisement correct_advert, input_advert, result_advert;
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvert,
                                                &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  for (const string& pathlet_string : kInsertPathlets) {
    Pathlet insert_pathlet;
    google::protobuf::TextFormat::ParseFromString(pathlet_string,
                                                  &insert_pathlet);
    pathlet_internal_state.InsertPathletIntoGraph(insert_pathlet);
  }

  int size = input_advert.ByteSize();
  int newsize;
  char serialized_input[size];
  input_advert.SerializeToArray(serialized_input, size);

  // act
  char* result = GenerateExternalPathletControlInfo(
      &pathlet_internal_state, kInputIslandid, serialized_input, size,
      &newsize);

  result_advert.ParseFromArray(result, newsize);

  // Assert
  EXPECT_STREQ(result_advert.DebugString().c_str(),
               correct_advert.DebugString().c_str());
}

TEST(HasPathletInformation, ItDoesHavePathletInfo_Get1) {
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    hop_descriptors {
        island_id: 1
        protocol : P_PATHLETS
    }
    )";
  const int kInputIslandid = 1;
  const int kCorrectResult = 1;

  IntegratedAdvertisement input_advert;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int size = input_advert.ByteSize();
  int newsize;
  char serialized_input[size];
  input_advert.SerializeToArray(serialized_input, size);

  // act
  int result = HasPathletInformation(serialized_input, size, kInputIslandid);

  // Assert
  EXPECT_EQ(result, kCorrectResult);
}

TEST(HasPathletInformation, ItDoesNotHavePathletInfo_Get0) {
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    )";
  const int kInputIslandid = 1;
  const int kCorrectResult = 0;

  IntegratedAdvertisement input_advert;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int size = input_advert.ByteSize();
  int newsize;
  char serialized_input[size];
  input_advert.SerializeToArray(serialized_input, size);

  // act
  int result = HasPathletInformation(serialized_input, size, kInputIslandid);

  // Assert
  EXPECT_EQ(result, kCorrectResult);
}

TEST(MergePathletInformationIntoGraph,
     GivePathletWithAbunchOfInformation_GetCorrectAdvertBack) {
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    hop_descriptors {
        island_id: 1
        protocol : P_PATHLETS
        key_values{
            key : 'PathletGraph'
            value: "\n\006\010\001\020\001\020\002\n\006\010\002\020\001\020\003\n\006\010\003\020\002\020\004\n\006\010\004\020\002\020\005"
        }
    }
    )";
  const string kCorrectAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    hop_descriptors{
        protocol: P_PATHLETS
        island_id: 1
        key_values{
            key : 'PathletGraph'
            value: "\n\006\010\001\020\001\020\002\n\006\010\002\020\001\020\003\n\006\010\003\020\002\020\004\n\006\010\004\020\002\020\005"
        }
    }
    )";

  PathletInternalState pathlet_internal_state("");

  IntegratedAdvertisement correct_advert, input_advert, result_advert;
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvert,
                                                &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int size = input_advert.ByteSize();
  int newsize;
  char serialized_input[size];
  input_advert.SerializeToArray(serialized_input, size);

  // act
  MergePathletInformationIntoGraph(&pathlet_internal_state, serialized_input,
                                   size, 1);

  // Assert
  char* result = GenerateExternalPathletControlInfo(
      &pathlet_internal_state, 1, serialized_input, size, &newsize);
  result_advert.ParseFromArray(result, newsize);

  EXPECT_STREQ(result_advert.DebugString().c_str(),
               correct_advert.DebugString().c_str());
}

TEST(GenerateInternalPathletControlInfo,
     IpHasControlInfoAssocaited_GetCorrectAdvertBack) {
  // Arrange
  const string kInputAdvert = R"()";
  const string kCorrectAdvert = R"(
    hop_descriptors {
        island_id: 1
        protocol: P_PATHLETS
        key_values {
            key: "PathletGraph"
            value: "\n\006\010\001\020\001\020\002"
        }
}
    )";
  const map<string, string> kAssociatedIpToPathlet = {{"192.168.1.1",
                                                       R"(
        fid: 1
        vnodes: 1
        vnodes: 2
    )"}};

  const string kAssociatedIp = "192.168.1.1";
  PathletInternalState pathlet_internal_state("");
  for (auto kv : kAssociatedIpToPathlet) {
    Pathlet pathlet;
    google::protobuf::TextFormat::ParseFromString(kv.second, &pathlet);
    pathlet_internal_state.InsertPathletToSend(kv.first, pathlet);
  }

  IntegratedAdvertisement correct_advert, input_advert, result_advert;
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvert,
                                                &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int size = input_advert.ByteSize();
  int newsize;
  char serialized_input[size];
  input_advert.SerializeToArray(serialized_input, size);

  // act
  char* result = GenerateInternalPathletControlInfo(
      &pathlet_internal_state, serialized_input, size, kAssociatedIp.c_str(),
      &newsize, 1);
  result_advert.ParseFromArray(result, newsize);

  // Assert
  EXPECT_STREQ(result_advert.DebugString().c_str(),
               correct_advert.DebugString().c_str());
}

TEST(GenerateInternalPathletControlInfo, IpHasNoControlInfoAssocaited_GetNull) {
  // Arrange
  const string kInputAdvert = R"()";
  const string kCorrectAdvert = R"()";
  const map<string, string> kAssociatedIpToPathlet = {{"192.168.1.1",
                                                       R"(
        fid: 1
        vnodes: 1
        vnodes: 2
    )"}};

  const string kAssociatedIp = "999.999.9999";
  PathletInternalState pathlet_internal_state("");
  for (auto kv : kAssociatedIpToPathlet) {
    Pathlet pathlet;
    google::protobuf::TextFormat::ParseFromString(kv.second, &pathlet);
    pathlet_internal_state.InsertPathletToSend(kv.first, pathlet);
  }

  IntegratedAdvertisement correct_advert, input_advert, result_advert;
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvert,
                                                &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  int size = input_advert.ByteSize();
  int newsize;
  char serialized_input[size];
  input_advert.SerializeToArray(serialized_input, size);

  // act
  char* result = GenerateInternalPathletControlInfo(
      &pathlet_internal_state, serialized_input, size, kAssociatedIp.c_str(),
      &newsize, 1);

  EXPECT_TRUE(result == NULL);
}

TEST(GenerateExternalPathletDestinationControlInfo,
     InsertSomePathletsIn_GetCorrectAdvertBack) {
  // Arrange
  const string kInputAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
)";
  const int kInputIslandid = 1;
  const int kAsNum = 1;
  const string kDest = "192.168.1.1/24";
  const vector<string> kInsertPathlets = {
      R"(
    fid: 1
    vnodes: 1
    vnodes: 2
    )",
      R"(
    fid: 2
    vnodes: 1
    vnodes: 3
    destination: '192.168.1.1/24'
    )",
      R"(
    fid: 3
    vnodes: 2
    vnodes: 4
    )",
      R"(
    fid: 4
    vnodes: 2
    vnodes: 5
    )"};

  const string kCorrectAdvert = R"(
    path_group_descriptors {
        protocol : P_WISER
    }
    hop_descriptors{
        protocol: P_PATHLETS
        island_id: 1
        key_values{
            key : 'PathletGraph'
            value: "\n\030\010\002\020\001\020\003\032\016192.168.1.1/24 \001"
        }
    }
    )";

  PathletInternalState pathlet_internal_state("");

  IntegratedAdvertisement correct_advert, input_advert, result_advert;
  google::protobuf::TextFormat::ParseFromString(kCorrectAdvert,
                                                &correct_advert);
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);

  for (const string& pathlet_string : kInsertPathlets) {
    Pathlet insert_pathlet;
    google::protobuf::TextFormat::ParseFromString(pathlet_string,
                                                  &insert_pathlet);
    pathlet_internal_state.InsertPathletIntoGraph(insert_pathlet);
  }

  int size = input_advert.ByteSize();
  int newsize;
  char* serialized_input = (char*)malloc(size);
  input_advert.SerializeToArray(serialized_input, size);

  // act
  char* result = GenerateExternalPathletDestinationControlInfo(
      &pathlet_internal_state, kDest.c_str(), kInputIslandid, kAsNum,
      serialized_input, size, &newsize);

  result_advert.ParseFromArray(result, newsize);

  // Assert
  EXPECT_STREQ(result_advert.DebugString().c_str(),
               correct_advert.DebugString().c_str());
}

TEST(CreatePathletsFromIA,
     IaWithPathletsInIt_GetCorrectIpsBackAndCorrectIpsToSendMap) {
  // Arrange
  const string kInputPathlets = R"(
        pathlets {
        fid : 1
        vnodes : 1
        vnodes : 2
        })";
  const string kInputAdvert = R"(
    hop_descriptors{
        protocol: P_PATHLETS
        island_id: 1
        key_values{
            key : 'PathletGraph'
            value: ''
        }
    }
)";
  const int kInputIslandid = 1;
  const int kAsNum = 1;
  const int kAsPathSize = 5;
  int kAsPathArray[kAsPathSize] = {1, 2, 3, 4, 5};
  const string kCorrectAnnounceIps[256] = {"192.168.1.1/32"};
  const int kCorrectNumIps = 1;
  const map<string, string> kCorrectIpToPathlet = {
    {"192.168.1.1",
     R"(
    fid: 1
    vnodes: 1
    vnodes: 2
    path_vector : 1
    path_vector : 2
    path_vector : 3
    path_vector : 4
    path_vector : 5
    )"}
  };

  PathletInternalState pathlet_internal_state("192.168.1.1");

  char* correct_announce_ips[256];
  int arraypos = 0;
  for(string astring : kCorrectAnnounceIps){
    char* buf = (char*) malloc(astring.size() + 1);
    memset(buf, 0, astring.size() + 1);
    strcpy(buf, astring.c_str());
    correct_announce_ips[arraypos] = buf;
  }

  map<string, string> correct_ip_to_pathlet;
  for (auto kv : kCorrectIpToPathlet) {
    Pathlet pathlet;
    google::protobuf::TextFormat::ParseFromString(kv.second, &pathlet);
    correct_ip_to_pathlet[kv.first] = pathlet.DebugString();
  }

  IntegratedAdvertisement input_advert;
  google::protobuf::TextFormat::ParseFromString(kInputAdvert, &input_advert);
  Pathlets pathlets;
  google::protobuf::TextFormat::ParseFromString(kInputPathlets, &pathlets);
  input_advert.mutable_hop_descriptors(0)->mutable_key_values(0)->set_value(
      pathlets.SerializeAsString());

  int size = input_advert.ByteSize();
  int num_ips;
  char* serialized_input = (char*)malloc(size);
  input_advert.SerializeToArray(serialized_input, size);
  char* result_announce_ips[256];

  // Act
  CreatePathletsFromIA(&pathlet_internal_state, serialized_input, size,
                       kAsPathArray, kAsPathSize, kInputIslandid, kAsNum,
                       result_announce_ips, &num_ips);

  map<string, Pathlet> result_ip_to_pathlet =
      pathlet_internal_state.GetIpToPathletToSend();
  map<string, string> result_ip_to_pathlet_string;
  for (auto kv : result_ip_to_pathlet) {
    result_ip_to_pathlet_string[kv.first] = kv.second.DebugString();
  }

  // Assert
  EXPECT_EQ(result_ip_to_pathlet_string, correct_ip_to_pathlet);
  for (int i = 0; i < num_ips; i++) {
    EXPECT_STREQ(correct_announce_ips[i], result_announce_ips[i]);
  }
}

////////////////////////////////
/// pathlets tests below here//
//////////////////////////////
TEST(GetNextIp, CallThisFunction1Time_GetCorrectIpBack) {
  // Arrange
  const int kNumTimesToCall = 1;
  const string kCorrectIpToGetBack = "192.168.1.1";

  PathletInternalState pathlet_internal_state =
      PathletInternalState("192.168.1.1");

  // act
  string result;
  for (int i = 0; i < kNumTimesToCall; i++) {
    result = pathlet_internal_state.GetNextIp();
  }

  // assert
  EXPECT_STREQ(result.c_str(), kCorrectIpToGetBack.c_str());
}

TEST(GetNextIp, CallThisFunction255Time_GetCorrectIpBack) {
  // Arrange
  const int kNumTimesToCall = 255;
  const string kCorrectIpToGetBack = "192.168.1.255";

  PathletInternalState pathlet_internal_state =
      PathletInternalState("192.168.1.1");

  // act
  string result;
  for (int i = 0; i < kNumTimesToCall; i++) {
    result = pathlet_internal_state.GetNextIp();
  }

  // assert
  EXPECT_STREQ(result.c_str(), kCorrectIpToGetBack.c_str());
}

TEST(GetNextFid, CallThisFUnction1Time_GetCorrectFidBack) {
  // arrange
  const int kNumTimesToCall = 1;
  const int kCorrecdtFidBack = 1;

  PathletInternalState pathlet_internal_state("");

  // Act
  int result;
  for (int i = 0; i < kNumTimesToCall; i++) {
    result = pathlet_internal_state.GetNextFid();
  }

  // Assert
  EXPECT_EQ(result, kCorrecdtFidBack);
}

TEST(GetNextFid, CallThisFUnction255Time_GetCorrectFidBack) {
  // arrange
  const int kNumTimesToCall = 255;
  const int kCorrecdtFidBack = 255;

  PathletInternalState pathlet_internal_state("");

  // Act
  int result;
  for (int i = 0; i < kNumTimesToCall; i++) {
    result = pathlet_internal_state.GetNextFid();
  }

  // Assert
  EXPECT_EQ(result, kCorrecdtFidBack);
}

TEST(InsertPathletIntoGraph, InsertOnePathlet_MapIsCorrect) {
  // arrange
  const vector<string> kPathletsToInsert = {R"(
    fid: 1
    vnodes: 1
    vnodes: 2
    )"};
  map<int, map<int, string>> kCorrectMap = {{1,
                                             {{2,
                                               R"(
            fid: 1
            vnodes: 1
            vnodes: 2)"}}}

  };

  PathletInternalState pathlet_internal_state("");
  // Act
  for (const string& pathlet_string : kPathletsToInsert) {
    Pathlet insert_pathlet;
    google::protobuf::TextFormat::ParseFromString(pathlet_string,
                                                  &insert_pathlet);
    pathlet_internal_state.InsertPathletIntoGraph(insert_pathlet);
  }

  map<int, map<int, Pathlet>> correct_map;
  for (auto kv : kCorrectMap) {
    map<int, Pathlet> entry;
    for (auto kv2 : kv.second) {
      Pathlet correct_pathlet;
      google::protobuf::TextFormat::ParseFromString(kv2.second,
                                                    &correct_pathlet);
      entry[kv2.first] = correct_pathlet;
    }
    correct_map[kv.first] = entry;
  }

  map<int, map<int, Pathlet>> result_map =
      pathlet_internal_state.GetPathletGraph();
  map<int, map<int, string>> result_map_pathletstrings;
  for (auto kv : result_map) {
    map<int, string> entry;
    for (auto kv2 : kv.second) {
      Pathlet result_pathlet = kv2.second;
      entry[kv2.first] = result_pathlet.DebugString();
    }
    result_map_pathletstrings[kv.first] = entry;
  }

  map<int, map<int, string>> correct_map_pathletstrings;
  for (auto kv : correct_map) {
    map<int, string> entry;
    for (auto kv2 : kv.second) {
      Pathlet correct_pathlet = kv2.second;
      entry[kv2.first] = correct_pathlet.DebugString();
    }
    correct_map_pathletstrings[kv.first] = entry;
  }
  EXPECT_EQ(result_map_pathletstrings, correct_map_pathletstrings);
}

TEST(InsertPathletIntoGraph, InsertFourPathlet_MapIsCorrect) {
  // arrange
  const vector<string> kPathletsToInsert = {
      R"(
    fid: 1
    vnodes: 1
    vnodes: 2
    )",
      R"(
    fid: 2
    vnodes: 1
    vnodes: 3
    )",
      R"(
    fid: 3
    vnodes: 2
    vnodes: 4
    )",
      R"(
    fid: 4
    vnodes: 2
    vnodes: 5
    )"};
  map<int, map<int, string>> kCorrectMap = {{1,
                                             {{2,
                                               R"(
            fid: 1
            vnodes: 1
            vnodes: 2
            )"},
                                              {3,
                                               R"(
            fid: 2
            vnodes: 1
            vnodes: 3
            )"}}},
                                            {2,
                                             {{4,
                                               R"(
            fid: 3
            vnodes: 2
            vnodes: 4
            )"},
                                              {5,
                                               R"(
            fid: 4
            vnodes: 2
            vnodes: 5
            )"}}}};

  PathletInternalState pathlet_internal_state("");
  // Act
  for (const string& pathlet_string : kPathletsToInsert) {
    Pathlet insert_pathlet;
    google::protobuf::TextFormat::ParseFromString(pathlet_string,
                                                  &insert_pathlet);
    pathlet_internal_state.InsertPathletIntoGraph(insert_pathlet);
  }

  map<int, map<int, Pathlet>> correct_map;
  for (auto kv : kCorrectMap) {
    map<int, Pathlet> entry;
    for (auto kv2 : kv.second) {
      Pathlet correct_pathlet;
      google::protobuf::TextFormat::ParseFromString(kv2.second,
                                                    &correct_pathlet);
      entry[kv2.first] = correct_pathlet;
    }
    correct_map[kv.first] = entry;
  }

  map<int, map<int, Pathlet>> result_map =
      pathlet_internal_state.GetPathletGraph();
  map<int, map<int, string>> result_map_pathletstrings;
  for (auto kv : result_map) {
    map<int, string> entry;
    for (auto kv2 : kv.second) {
      Pathlet result_pathlet = kv2.second;
      entry[kv2.first] = result_pathlet.DebugString();
    }
    result_map_pathletstrings[kv.first] = entry;
  }

  map<int, map<int, string>> correct_map_pathletstrings;
  for (auto kv : correct_map) {
    map<int, string> entry;
    for (auto kv2 : kv.second) {
      Pathlet correct_pathlet = kv2.second;
      entry[kv2.first] = correct_pathlet.DebugString();
    }
    correct_map_pathletstrings[kv.first] = entry;
  }
  EXPECT_EQ(result_map_pathletstrings, correct_map_pathletstrings);
}

TEST(ConvertGraphToPathlets, Insert1Pathlet_GetCorrectPathletsBack) {
  // Arrange
  const vector<string> kInsertPathlets = {
      R"(
    fid: 1
    vnodes: 1
    vnodes: 2
    )"};

  const string kCorrectPathlets =
      R"(
    pathlets {
        fid: 1
        vnodes: 1
        vnodes: 2
    }
    )";

  PathletInternalState pathlet_internal_state("");

  Pathlets correct_pathlets;
  google::protobuf::TextFormat::ParseFromString(kCorrectPathlets,
                                                &correct_pathlets);
  // Act
  for (const string& pathlet_string : kInsertPathlets) {
    Pathlet insert_pathlet;
    google::protobuf::TextFormat::ParseFromString(pathlet_string,
                                                  &insert_pathlet);
    pathlet_internal_state.InsertPathletIntoGraph(insert_pathlet);
  }

  Pathlets result = pathlet_internal_state.ConvertGraphToPathlets();

  EXPECT_STREQ(result.DebugString().c_str(),
               correct_pathlets.DebugString().c_str());
}

TEST(ConvertGraphToPathlets, Insert4Pathlet_GetCorrectPathletsBack) {
  // Arrange
  const vector<string> kInsertPathlets = {
      R"(
    fid: 1
    vnodes: 1
    vnodes: 2
    )",
      R"(
    fid: 2
    vnodes: 1
    vnodes: 3
    )",
      R"(
    fid: 3
    vnodes: 2
    vnodes: 4
    )",
      R"(
    fid: 4
    vnodes: 2
    vnodes: 5
    )"};

  const string kCorrectPathlets =
      R"(
    pathlets {
        fid: 1
        vnodes: 1
        vnodes: 2
    }
    pathlets {
        fid: 2
        vnodes: 1
        vnodes: 3
    }
    pathlets {
        fid: 3
        vnodes: 2
        vnodes: 4
    }
    pathlets {
        fid: 4
        vnodes: 2
        vnodes: 5
    }
    )";

  PathletInternalState pathlet_internal_state("");

  Pathlets correct_pathlets;
  google::protobuf::TextFormat::ParseFromString(kCorrectPathlets,
                                                &correct_pathlets);
  // Act
  for (const string& pathlet_string : kInsertPathlets) {
    Pathlet insert_pathlet;
    google::protobuf::TextFormat::ParseFromString(pathlet_string,
                                                  &insert_pathlet);
    pathlet_internal_state.InsertPathletIntoGraph(insert_pathlet);
  }

  Pathlets result = pathlet_internal_state.ConvertGraphToPathlets();

  EXPECT_STREQ(result.DebugString().c_str(),
               correct_pathlets.DebugString().c_str());
}

TEST(GetPathletsForDestination, Insert4Pathlet_GetCorrectPathletsBack) {
  // Arrange
  const vector<string> kInsertPathlets = {
      R"(
    fid: 1
    vnodes: 1
    vnodes: 2
    )",
      R"(
    fid: 2
    vnodes: 1
    vnodes: 3
    )",
      R"(
    fid: 3
    vnodes: 2
    vnodes: 4
    destination: '192.168.1.1/24'
    )",
      R"(
    fid: 4
    vnodes: 3
    vnodes: 4
    destination: '192.168.1.1/24'
    )",
      R"(
    fid: 5
    vnodes: 5
    vnodes: 4
    destination: '192.168.1.1/24'
    )"};

  const int kStartPt = 1;
  const int kIslandId = 1;
  const string kDest = "192.168.1.1/24";

  const string kCorrectPathlets =
      R"(
    pathlets {
      fid: 3
      vnodes: 2
      vnodes: 4
      path_vector: 1
      destination: '192.168.1.1/24'
    }
    pathlets {
      fid: 1
      vnodes: 1
      vnodes: 2
      path_vector: 1
    }
    pathlets {
      fid: 4
      vnodes: 3
      vnodes: 4
      path_vector: 1
      destination: '192.168.1.1/24'
    }
    pathlets {
      fid: 2
      vnodes: 1
      vnodes: 3
      path_vector: 1
    }
    )";

  PathletInternalState pathlet_internal_state("");

  Pathlets correct_pathlets;
  google::protobuf::TextFormat::ParseFromString(kCorrectPathlets,
                                                &correct_pathlets);
  // Act
  for (const string& pathlet_string : kInsertPathlets) {
    Pathlet insert_pathlet;
    google::protobuf::TextFormat::ParseFromString(pathlet_string,
                                                  &insert_pathlet);
    pathlet_internal_state.InsertPathletIntoGraph(insert_pathlet);
  }

  Pathlets result = pathlet_internal_state.GetPathletsForDestination(
      kDest, kIslandId, kStartPt);
  // cout << "result" << endl;
  // result.PrintDebugString();
  // cout << "correct" << endl;
  // correct_pathlets.PrintDebugString();

  EXPECT_STREQ(result.DebugString().c_str(),
               correct_pathlets.DebugString().c_str());
}

TEST(GetPathletsForDestination, Insert1Pathlet_GetCorrectPathletsBack) {
  // Arrange
  const vector<string> kInsertPathlets = {
      R"(
    fid: 1
    vnodes: 1
    vnodes: 2
    destination : '192.168.1.1/24'
    )"};

  const int kStartPt = 1;
  const int kIslandId = 1;
  const string kDest = "192.168.1.1/24";

  const string kCorrectPathlets =
      R"(
    pathlets{
        fid: 1
        vnodes: 1
        vnodes: 2
        destination : '192.168.1.1/24'
        path_vector: 1
    }
    )";

  PathletInternalState pathlet_internal_state("");

  Pathlets correct_pathlets;
  google::protobuf::TextFormat::ParseFromString(kCorrectPathlets,
                                                &correct_pathlets);
  // Act
  for (const string& pathlet_string : kInsertPathlets) {
    Pathlet insert_pathlet;
    google::protobuf::TextFormat::ParseFromString(pathlet_string,
                                                  &insert_pathlet);
    pathlet_internal_state.InsertPathletIntoGraph(insert_pathlet);
  }

  Pathlets result = pathlet_internal_state.GetPathletsForDestination(
      kDest, kIslandId, kStartPt);
  // cout << "result" << endl;
  // result.PrintDebugString();
  // cout << "correct" << endl;
  // correct_pathlets.PrintDebugString();

  EXPECT_STREQ(result.DebugString().c_str(),
               correct_pathlets.DebugString().c_str());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
