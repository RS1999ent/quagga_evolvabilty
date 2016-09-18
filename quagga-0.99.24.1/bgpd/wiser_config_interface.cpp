#include "wiser_config_interface.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include "general_configuration.h"
#include "integrated_advertisement.pb.h"
#include "pathlet_config.h"
#include "pathlet_internal_state.h"
#include "wiser_config.h"

#include "quagga_config.pb.h"

extern "C" {
void free_WiserConfig(WiserConfigHandle p) { delete p; }
int GetLinkCost(WiserConfigHandle wiser_handle, const char* ip1,
                const char* ip2) {
  std::string string_ip1 = std::string(ip1);
  std::string string_ip2 = std::string(ip2);

  return wiser_handle->GetLinkCost(string_ip1, string_ip2);
}

const char* LinkCostsToString(WiserConfigHandle wiser_config_handle) {
  return wiser_config_handle->LinkCostsToString();
}
// Generalconfiguration class methods below here

GeneralConfigurationHandle CreateGeneralConfig(char const* filename) {
  // COPY PASTE JOB FROM
  // http://stackoverflow.com/questions/10842066/parse-in-text-file-for-google-protocol-buffer

  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  Configuration general_config;  // My protobuf object

  bool retValue = false;

  int fileDescriptor = open(filename, O_RDONLY);

  if (fileDescriptor < 0) {
    std::cerr << " Error opening the file " << std::endl;
    return nullptr;
  }

  google::protobuf::io::FileInputStream fileInput(fileDescriptor);
  fileInput.SetCloseOnDelete(true);

  if (!google::protobuf::TextFormat::Parse(&fileInput, &general_config)) {
    std::cerr << std::endl << "Failed to parse file!" << std::endl;
    return nullptr;
  } else {
    std::cerr << "Read Input File - " << filename << std::endl;
    return new GeneralConfiguration(general_config);
  }

  std::cerr << "error" << std::endl;
  return new GeneralConfiguration();
}

dbgp_protocol_t GetProtocolType(
    GeneralConfigurationHandle general_config_handle) {
  return general_config_handle->GetProtocolType();
}

uint32_t GetIslandId(GeneralConfigurationHandle general_config_handle) {
  return general_config_handle->GetIslandId();
}

int IsRemoteAsAnIslandMember(GeneralConfigurationHandle general_config_handle,
                             int remote_as) {
  return general_config_handle->IsRemoteAsAnIslandMember(remote_as);
}

void FreeGeneralConfig(GeneralConfigurationHandle general_config_handle) {
  delete general_config_handle;
}

WiserConfigHandle GetWiserConfig(
    GeneralConfigurationHandle general_config_handle) {
  return general_config_handle->GetWiserConfig();
}

std::mutex internal_state_mutex;
char* ConvertGraphToPathlets(PathletInternalStateHandle pathlet_internal_state,
                             int* size) {
  internal_state_mutex.lock();
  Pathlets return_pathlets;
  return_pathlets = pathlet_internal_state->ConvertGraphToPathlets();
  *size = return_pathlets.ByteSize();
  char* return_serialized = (char*)malloc(*size);
  return_pathlets.SerializeToArray(return_serialized, *size);
  internal_state_mutex.unlock();
  return return_serialized;
}

void InsertPathletIntoGraph(PathletInternalStateHandle pathlet_internal_state,
                            char* pathlet_serialized, int size) {
  internal_state_mutex.lock();
  // parse pathlet_serialized
  Pathlet pathlet;
  pathlet.ParseFromArray(pathlet_serialized, size);

  pathlet_internal_state->InsertPathletIntoGraph(pathlet);
  internal_state_mutex.unlock();
}

void InsertPathletToSend(PathletInternalStateHandle pathlet_internal_state,
                         char* associated_ip, int fid, int as1, int as2) {
  internal_state_mutex.lock();
  // parse serialized
  Pathlet pathlet;
  pathlet.set_fid(fid);
  pathlet.add_vnodes(as1);
  pathlet.add_vnodes(as2);
  pathlet_internal_state->InsertPathletToSend(string(associated_ip), pathlet);
  internal_state_mutex.unlock();
}

char* GetPathletAssociatedWithIp(
    PathletInternalStateHandle pathlet_internal_state,
    const char* associated_ip, int* return_size) {
  Pathlet gotten_pathlet;
  char* return_serialized;
  internal_state_mutex.lock();

  gotten_pathlet =
      pathlet_internal_state->GetPathletToSend(string(associated_ip));
  *return_size = gotten_pathlet.ByteSize();
  return_serialized = (char*)malloc(*return_size);
  gotten_pathlet.SerializeToArray(return_serialized, *return_size);
  internal_state_mutex.unlock();
  return return_serialized;
}
}

char* GetNextIp(PathletInternalStateHandle pathlet_internal_state) {
  internal_state_mutex.lock();
  string next_ip = pathlet_internal_state->GetNextIp();
  char* return_buffer = (char*)malloc(next_ip.size() + 1);
  strcpy(return_buffer, next_ip.c_str());
  internal_state_mutex.unlock();
  return return_buffer;
}
int GetNextFid(PathletInternalStateHandle pathlet_internal_state) {
  internal_state_mutex.lock();
  int next_fid = pathlet_internal_state->GetNextFid();
  internal_state_mutex.unlock();
  return next_fid;
}

PathletConfigHandle CreatePathletConfig(
    GeneralConfigurationHandle general_config) {
  return general_config->GetPathletConfig();
}
PathletInternalStateHandle CreatePathletInternalState(
    PathletConfigHandle pathlet_config) {
  return pathlet_config->CreatePathletInternalState();
}
int IsIslandBorderRouter(PathletConfigHandle pathlet_config) {
  return pathlet_config->IsIslandBorderRouter();
}

char* GetPrivateIp(PathletConfigHandle pathlet_config) {
  char* return_buffer = (char*)malloc(INET_ADDRSTRLEN);
  strcpy(return_buffer, pathlet_config->GetPrivateIp().c_str());
  return return_buffer;
}

char* GetPathletGraphString(PathletInternalStateHandle pathlet_internal_state) {
  string graph_string = pathlet_internal_state->GraphToString();
  char* return_buffer = (char*)malloc(graph_string.size() + 1);
  strcpy(return_buffer, graph_string.c_str());
  return return_buffer;
}

char* GetPathletsToSendString(PathletInternalStateHandle pathlet_internal_state) {
  string pathlets_to_send_string = pathlet_internal_state->PathletsToSendToString();
  char* return_buffer = (char*)malloc(pathlets_to_send_string.size() + 1);
  strcpy(return_buffer, pathlets_to_send_string.c_str());
  return return_buffer;
}
