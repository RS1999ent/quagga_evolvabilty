#include "integrated_advertisement_functions.h"
#include <iostream>
#include "integrated_advertisement.pb.h"

using namespace std;
// Retrieves a reference for a PathGroupDescriptor for a given protocol if it
// exists. If it doesn't, returns a null reference.
//
// Arguments
//   advertisement: The advertisement to search for a protocol pathgroup
//   descriptor
//   protocol: A 'Protocol' enum specifying the protocol type to look for
PathGroupDescriptor* GetProtocolPathGroupDescriptor(
    IntegratedAdvertisement* advertisement, Protocol protocol) {
  // For each pathgroup descriptor, check to see if the protocol type matches
  // the input. If it does, return the reference.
  for (int i = 0; i < advertisement->path_group_descriptors_size(); i++) {
    PathGroupDescriptor* mutable_path_group_descriptor =
        advertisement->mutable_path_group_descriptors(i);
    if (mutable_path_group_descriptor->protocol() == protocol) {
      return mutable_path_group_descriptor;
    }
  }
  // Here if there was no path group descriptor, so return NULL.
  return NULL;
}

// Retrieves a reference for a HopDescriptor for a given protocol and island if
// it exists. If it doesn't, returns a null reference.
//
// Arguments
//   advertisement: The advertisement to search for a protocol hopdescriptor
//   protocol: A 'Protocol' enum specifying the protocol type to look for
//   island_id: the island id that needs to be present along with protocol
HopDescriptor* GetProtocolHopDescriptor(IntegratedAdvertisement* advertisement,
                                        Protocol protocol, uint32_t island_id) {
  // For each hop descriptor, check to see if the protocol type matches the
  // input and the islandid. If it does, return the reference.
  for (int i = 0; i < advertisement->hop_descriptors_size(); i++) {
    HopDescriptor* mutable_hop_descriptor =
        advertisement->mutable_hop_descriptors(i);
    if (mutable_hop_descriptor->protocol() == protocol &&
        island_id == mutable_hop_descriptor->island_id()) {
      return mutable_hop_descriptor;
    }
  }
  // Here if there was no hop descriptor, so return NULL.
  return NULL;
}

// Returns a reference to a mutable Keyvalue that corresponds to the key passed
// in. If it does not exist, one will be added to the path_group_descriptor and
// a reference returned.
//
// Arguments:
//   path_group_descriptor: The path_group_descriptor to search for a given key.
//   May be mutated
//   search_key: the key to search for a keyvalue of.
//
// Returns a reference to a given KeyValue.
KeyValue* GetCreateKeyValueFromPathGroupDesc(
    PathGroupDescriptor* path_group_descriptor, const std::string& search_key) {
  // Go through each keyvalue and check if the key is equal to the search_key
  // passed in.
  for (int i = 0; i < path_group_descriptor->key_values_size(); i++) {
    KeyValue* mutable_key_value = path_group_descriptor->mutable_key_values(i);
    if (mutable_key_value->key().compare(search_key) == 0) {
      return mutable_key_value;
    }
  }
  // Here if there was no key found
  KeyValue* mutable_key_value = path_group_descriptor->add_key_values();
  mutable_key_value->set_key(search_key);
  return mutable_key_value;
}

// Returns a reference to a mutable Keyvalue that corresponds to the key passed
// in. If it does not exist, one will be added to the hop_descriptor and
// a reference returned.
//
// Arguments:
//   hop_descriptor: The hop_descriptor to search for a given key. May be
//   mutated
//   search_key: the key to search for a keyvalue of.
//
// Returns a reference to a given KeyValue.
KeyValue* GetCreateKeyValueFromHopDesc(HopDescriptor* hop_descriptor,
                                       const std::string& search_key) {
  // Go through each keyvalue and check if the key is equal to the search_key
  // passed in.
  for (int i = 0; i < hop_descriptor->key_values_size(); i++) {
    KeyValue* mutable_key_value = hop_descriptor->mutable_key_values(i);
    if (mutable_key_value->key().compare(search_key) == 0) {
      return mutable_key_value;
    }
  }
  // Here if there was no key found
  KeyValue* mutable_key_value = hop_descriptor->add_key_values();
  mutable_key_value->set_key(search_key);
  return mutable_key_value;
}

// Returns a reference to a mutable Keyvalue that corresponds to the key passed
// in. If it does not exist, it will return null
//
// Arguments:
//   hop_descriptor: The hop_descriptor to search for a given key. May be
//   mutated
//   search_key: the key to search for a keyvalue of.
//
// Returns a reference to a given KeyValue. Null if it doesn't exist.
KeyValue* GetHopDescriptorKeyValue(HopDescriptor* hop_descriptor,
                                   const std::string& search_key) {
  // Go through each keyvalue and check if the key is equal to the search_key
  // passed in.
  for (int i = 0; i < hop_descriptor->key_values_size(); i++) {
    KeyValue* mutable_key_value = hop_descriptor->mutable_key_values(i);
    if (mutable_key_value->key().compare(search_key) == 0) {
      return mutable_key_value;
    }
  }
  return NULL;
}

// Returns a reference to a mutable Keyvalue that corresponds to the key passed
// in. If it does not exist, it will return null
//
// Arguments:
//   path_group_descriptor: The path_group_descriptor to search for a given key.
//   May be mutated
//   search_key: the key to search for a keyvalue of.
//
// Returns a reference to a given KeyValue. Null if it doesn't exist.
KeyValue* GetPathGroupKeyValue(PathGroupDescriptor* path_group_descriptor,
                               const std::string& search_key) {
  // Go through each keyvalue and check if the key is equal to the search_key
  // passed in.
  for (int i = 0; i < path_group_descriptor->key_values_size(); i++) {
    KeyValue* mutable_key_value = path_group_descriptor->mutable_key_values(i);
    if (mutable_key_value->key().compare(search_key) == 0) {
      return mutable_key_value;
    }
  }
  return NULL;
}

char* SetWiserControlInfo(char* serialized_advert, int advert_size,
                          int additive_path_cost, int* modified_advert_size,
                          float normalization) {
  // Parse the existing advertisement if it exists
  IntegratedAdvertisement working_advertisement;
  if (advert_size != 0) {
    working_advertisement.ParseFromArray(serialized_advert, advert_size);
  }

  // Get a wiser path group descriptor if it exists

  PathGroupDescriptor* wiser_path_group_descriptor =
      GetProtocolPathGroupDescriptor(&working_advertisement, Protocol::P_WISER);

  // If there was no existing path group descriptor, then add it
  if (wiser_path_group_descriptor == NULL) {
    wiser_path_group_descriptor =
        working_advertisement.add_path_group_descriptors();
    wiser_path_group_descriptor->set_protocol(Protocol::P_WISER);
  }

  // Get the keyvalue corresponding to wiser path cost.
  KeyValue* mutable_key_value = GetCreateKeyValueFromPathGroupDesc(
      wiser_path_group_descriptor, "PathCost");

  // add onto the path cost and reserialize it into the value of the key value.
  // Normalize the current cost first
  PathCost path_cost;
  path_cost.ParseFromString(mutable_key_value->value());
  int current_cost = path_cost.path_cost();
  int new_cost = current_cost * normalization + additive_path_cost;
  printf(
      "integrated_advertisement_functions::SetWiserControlInfo: cost %i added "
      "to entering control info with cost %i giving total %i",
      additive_path_cost, current_cost, new_cost);
  path_cost.set_path_cost(new_cost);
  mutable_key_value->set_value(path_cost.SerializeAsString());

  // working_advertisement.PrintDebugString();

  *modified_advert_size = working_advertisement.ByteSize();
  char* modified_advert = (char*)malloc(*modified_advert_size);

  working_advertisement.SerializeToArray(modified_advert,
                                         *modified_advert_size);

  return modified_advert;
}

// untested
int GetWiserPathCost(char* serialized_advert, int advert_size) {
  // Parse the advertisement
  IntegratedAdvertisement proto_advert;
  proto_advert.ParseFromArray(serialized_advert, advert_size);

  // Get the wiser descriptor (we are assuming that it is always there)
  PathGroupDescriptor* wiser_path_group_descriptor =
      GetProtocolPathGroupDescriptor(&proto_advert, Protocol::P_WISER);

  if (wiser_path_group_descriptor == NULL) {
    return -1;
  }

  // Get the path cost from the assumed existent keyvalue
  KeyValue* path_cost_kv =
      GetPathGroupKeyValue(wiser_path_group_descriptor, "PathCost");
  PathCost path_cost;
  path_cost.ParseFromString(path_cost_kv->value());
  return path_cost.path_cost();
}

int GetLastWiserNode(char* serialized_advert, int advert_size) {
  // unserialize advert
  IntegratedAdvertisement input_ia;
  input_ia.ParseFromArray(serialized_advert, advert_size);

  PathGroupDescriptor* wiser_path_group_descriptor =
      GetProtocolPathGroupDescriptor(&input_ia, Protocol::P_WISER);

  // if wiser_path_group_descriptor is null, then there is no last wiser node.
  // return -1
  if (wiser_path_group_descriptor == NULL) {
    return -1;
  }

  // if the key value for 'LastWiserNode' doesn't exist, return -1
  KeyValue* last_wiser =
      GetPathGroupKeyValue(wiser_path_group_descriptor, "LastWiserNode");
  if (last_wiser == NULL) {
    return -1;
  }

  return std::stoi(last_wiser->value());
}

char* SetLastWiserNode(char* serialized_advert, int advert_size,
                       int new_last_node, int* return_advert_size) {
  // unserialize advert
  IntegratedAdvertisement working_advertisement;
  working_advertisement.ParseFromArray(serialized_advert, advert_size);

  // Get a wiser path group descriptor if it exists
  PathGroupDescriptor* wiser_path_group_descriptor =
      GetProtocolPathGroupDescriptor(&working_advertisement, Protocol::P_WISER);

  // If there was no existing path group descriptor, then add it
  if (wiser_path_group_descriptor == NULL) {
    wiser_path_group_descriptor =
        working_advertisement.add_path_group_descriptors();
    wiser_path_group_descriptor->set_protocol(Protocol::P_WISER);
  }

  // Get or create the key vlaue
  KeyValue* mutable_key_value = GetCreateKeyValueFromPathGroupDesc(
      wiser_path_group_descriptor, "LastWiserNode");
  mutable_key_value->set_value(std::to_string(new_last_node));

  *return_advert_size = working_advertisement.ByteSize();
  char* return_advert = (char*)malloc(*return_advert_size);
  working_advertisement.SerializeToArray(return_advert, *return_advert_size);
  return return_advert;
}

char* SerializedAdverToString(char* serialized_advert, int advert_size) {
  IntegratedAdvertisement parsed_advert;
  parsed_advert.ParseFromArray(serialized_advert, advert_size);
  int num_chars = parsed_advert.DebugString().size();
  char* return_buffer = (char*)malloc(num_chars + 1);
  parsed_advert.DebugString().copy(return_buffer, num_chars, 0);
  return return_buffer;
}

char* PrintPathletsFromSerializedAdvert(char* serialized_advert,
                                        int advert_size, int island_id) {
  IntegratedAdvertisement parsed_advert;
  parsed_advert.ParseFromArray(serialized_advert, advert_size);

  HopDescriptor* hop_descriptor =
      GetProtocolHopDescriptor(&parsed_advert, Protocol::P_PATHLETS, island_id);
  // if null then there are no pathlets in the advert
  if (hop_descriptor == NULL) {
    string no_pathlets = "NO PATHLETS";
    char* return_char = (char*)malloc(no_pathlets.size() + 1);
    strcpy(return_char, no_pathlets.c_str());
    return return_char;
  }

  KeyValue* pathlets_kv =
      GetHopDescriptorKeyValue(hop_descriptor, "PathletGraph");
  // if null there are no pathelts graph in the advert
  if (pathlets_kv == NULL) {
    string no_pathlets = "NO PATHLETS GRAPH";
    char* return_char = (char*)malloc(no_pathlets.size() + 1);
    strcpy(return_char, no_pathlets.c_str());
    return return_char;
  }

  Pathlets pathlets;
  pathlets.ParseFromString(pathlets_kv->value());

  int num_chars = parsed_advert.DebugString().size() + 256;
  char* return_buffer = (char*)malloc(num_chars);
  strcpy(return_buffer, pathlets.DebugString().c_str());
  return return_buffer;
}

char* GenerateExternalPathletControlInfo(
    PathletInternalStateHandle pathlet_internal_state, int island_id,
    char* serialized_advert, int advert_size, int* new_advert_size) {
  // parse advert
  IntegratedAdvertisement parsed_advert;
  parsed_advert.ParseFromArray(serialized_advert, advert_size);

  // Get a pathlets graph.
  int size;
  char* pathlet_graph_serialized =
      ConvertGraphToPathlets(pathlet_internal_state, &size);
  Pathlets pathlet_graph;
  pathlet_graph.ParseFromArray(pathlet_graph_serialized, size);

  HopDescriptor* pathlet_hop_descriptor =
      GetProtocolHopDescriptor(&parsed_advert, Protocol::P_PATHLETS, island_id);
  // if pathlethop descriptor null, that means we need to create one.
  if (pathlet_hop_descriptor == NULL) {
    pathlet_hop_descriptor = parsed_advert.add_hop_descriptors();
    pathlet_hop_descriptor->set_protocol(Protocol::P_PATHLETS);
    pathlet_hop_descriptor->set_island_id(island_id);
  }
  // add the pathlet graph as the value to a keyuvalue of "PathletGraph"
  KeyValue* pathlet_kv =
      GetCreateKeyValueFromHopDesc(pathlet_hop_descriptor, "PathletGraph");

  pathlet_kv->set_value(pathlet_graph.SerializeAsString());
  parsed_advert.PrintDebugString();

  *new_advert_size = parsed_advert.ByteSize();
  char* return_advert = (char*)malloc(*new_advert_size);
  parsed_advert.SerializeToArray(return_advert, *new_advert_size);

  return return_advert;
}

char* GenerateExternalPathletDestinationControlInfo(
    PathletInternalStateHandle pathlet_internal_state, const char* destination,
    int island_id, int as_num, char* serialized_advert, int advert_size,
    int* new_advert_size) {
  // parse advert
  IntegratedAdvertisement parsed_advert;
  parsed_advert.ParseFromArray(serialized_advert, advert_size);

  // Get a pathlets graph.
  int size;
  char* pathlet_graph_serialized = GetPathletsForDestination(
      pathlet_internal_state, destination, island_id, as_num, &size);
  Pathlets pathlet_graph;
  pathlet_graph.ParseFromArray(pathlet_graph_serialized, size);

  HopDescriptor* pathlet_hop_descriptor =
      GetProtocolHopDescriptor(&parsed_advert, Protocol::P_PATHLETS, island_id);
  // if pathlethop descriptor null, that means we need to create one.
  if (pathlet_hop_descriptor == NULL) {
    pathlet_hop_descriptor = parsed_advert.add_hop_descriptors();
    pathlet_hop_descriptor->set_protocol(Protocol::P_PATHLETS);
    pathlet_hop_descriptor->set_island_id(island_id);
  }
  // add the pathlet graph as the value to a keyuvalue of "PathletGraph"
  KeyValue* pathlet_kv =
      GetCreateKeyValueFromHopDesc(pathlet_hop_descriptor, "PathletGraph");

  pathlet_kv->set_value(pathlet_graph.SerializeAsString());
  // pathlet_graph.PrintDebugString();
  // parsed_advert.PrintDebugString();

  *new_advert_size = parsed_advert.ByteSize();
  char* return_advert = (char*)malloc(*new_advert_size);
  parsed_advert.SerializeToArray(return_advert, *new_advert_size);

  return return_advert;
}

int HasPathletInformation(char* serialized_advert, int advert_size,
                          int island_id) {
  // parse advert
  IntegratedAdvertisement parsed_advert;
  parsed_advert.ParseFromArray(serialized_advert, advert_size);

  HopDescriptor* pathlet_hop_descriptor =
      GetProtocolHopDescriptor(&parsed_advert, Protocol::P_PATHLETS, island_id);

  // if it is null, it doesn't have pathlet info, return 0.
  if (pathlet_hop_descriptor == NULL) {
    return 0;
  } else {
    return 1;
  }
}

void MergePathletInformationIntoGraph(
    PathletInternalStateHandle pathlet_internal_state, char* serialized_advert,
    int advert_size, int island_id) {
  // parse advert
  IntegratedAdvertisement parsed_advert;
  parsed_advert.ParseFromArray(serialized_advert, advert_size);

  HopDescriptor* pathlet_hop_descriptor =
      GetProtocolHopDescriptor(&parsed_advert, Protocol::P_PATHLETS, island_id);
  // if there is no pathlet information, there is nothing to do.
  if (pathlet_hop_descriptor == NULL) {
    return;
  }

  Pathlets pathlets;
  KeyValue* pathlet_info =
      GetHopDescriptorKeyValue(pathlet_hop_descriptor, "PathletGraph");

  pathlets.ParseFromString(pathlet_info->value());

  // For each pathlet, merge into datastruct
  for (const Pathlet& pathlet : pathlets.pathlets()) {
    int pathlet_size = pathlet.ByteSize();
    char* serialized_pathlet = (char*)malloc(pathlet_size);
    pathlet.SerializeToArray(serialized_pathlet, pathlet_size);
    pathlet.PrintDebugString();
    InsertPathletIntoGraph(pathlet_internal_state, serialized_pathlet,
                           pathlet_size);
    free(serialized_pathlet);
  }
}

// int* GetPathVectorForOutgoingPathlet(
//     PathletInternalStateHandle pathlet_internal_state, char*
//     serialized_advert,
//     int advert_size, const char* ip_address, int* path_vector-size) {}

char* GenerateInternalPathletControlInfo(
    PathletInternalStateHandle pathlet_internal_state, char* serialized_advert,
    int advert_size, const char* ip_address, int* new_size, int island_id) {
  // parse advert
  IntegratedAdvertisement parsed_advert;
  parsed_advert.ParseFromArray(serialized_advert, advert_size);

  int pathlet_size;
  char* serialzied_pathlet = GetPathletAssociatedWithIp(
      pathlet_internal_state, ip_address, &pathlet_size);

  // you should always get something back if this function is called
  if (pathlet_size == 0) {
    return NULL;
  }
  // parse the gottne pathlet
  Pathlet gotten_pathlet;
  gotten_pathlet.ParseFromArray(serialzied_pathlet, pathlet_size);

  HopDescriptor* pathlet_hop_descriptor =
      GetProtocolHopDescriptor(&parsed_advert, Protocol::P_PATHLETS, island_id);
  // if null, there is no existing pathlet hop descriptor, generate one
  if (pathlet_hop_descriptor == NULL) {
    pathlet_hop_descriptor = parsed_advert.add_hop_descriptors();
    pathlet_hop_descriptor->set_protocol(Protocol::P_PATHLETS);
    pathlet_hop_descriptor->set_island_id(island_id);
  }

  // create pathlets message to place the gotten pathlet in and to place in the
  // keyvalues
  Pathlets pathlets;
  pathlets.add_pathlets()->CopyFrom(gotten_pathlet);
  KeyValue* pathlet_kv =
      GetCreateKeyValueFromHopDesc(pathlet_hop_descriptor, "PathletGraph");
  pathlet_kv->set_value(pathlets.SerializeAsString());

  // We added the pathlet, serialized the advert.
  *new_size = parsed_advert.ByteSize();
  char* return_serialized = (char*)malloc(*new_size);
  parsed_advert.SerializeToArray(return_serialized, *new_size);
  gotten_pathlet.PrintDebugString();
  pathlets.PrintDebugString();
  parsed_advert.PrintDebugString();

  return return_serialized;
}

// untested
void CreatePathletsFromIA(PathletInternalStateHandle pathlet_internal_state,
                          char* serialized_advert, int advert_size,
                          uint32_t* aspath, int aspath_size, int island_id,
                          int as_num, char* announce_ips[], int* num_ips) {
  *num_ips = 0;
  int array_position = 0;  // current position in announce_ips where we can
                           // assign an ip
  // parse advert
  IntegratedAdvertisement parsed_advert;
  parsed_advert.ParseFromArray(serialized_advert, advert_size);

  HopDescriptor* pathlet_hop_descriptor =
      GetProtocolHopDescriptor(&parsed_advert, Protocol::P_PATHLETS, island_id);
  // if null, there is no existing pathlet hop descriptor, generate one
  if (pathlet_hop_descriptor == NULL) {
    return;
  }

  // get the pathlets that came from this advert
  Pathlets pathlets;
  KeyValue* pathlet_kv =
      GetHopDescriptorKeyValue(pathlet_hop_descriptor, "PathletGraph");
  if (pathlet_kv == NULL) {
    return;
  }

  pathlets.ParseFromString(pathlet_kv->value());
  for (Pathlet a_pathlet : pathlets.pathlets()) {
    // append pathvector
    for (int i = 0; i < aspath_size; i++) {
      a_pathlet.add_path_vector(aspath[i]);
    }
    char* new_ip = GetNextIp(pathlet_internal_state);
    int pathlet_size = a_pathlet.ByteSize();
    char* serialized_pathlet = (char*)malloc(pathlet_size);
    a_pathlet.SerializeToArray(serialized_pathlet, pathlet_size);
    InsertRawPathletToSend(pathlet_internal_state, new_ip, serialized_pathlet,
                           pathlet_size);

    char* announce_ip = (char*)malloc(256);
    snprintf(announce_ip, 256, "%s/%d", new_ip, 32);
    announce_ips[array_position] = announce_ip;
    array_position++;
    *num_ips = *num_ips + 1;
  }
  // create a pathlet to the as that originated it.
  Pathlet cross_gulf_pathlet;
  int new_fid = GetNextFid(pathlet_internal_state);
  char* new_ip = GetNextIp(pathlet_internal_state);
  cross_gulf_pathlet.add_vnodes(as_num);
  // the as that sent this to us will be in the second to last position
  cross_gulf_pathlet.add_vnodes(aspath[aspath_size - 2]);
  cross_gulf_pathlet.set_fid(new_fid);
  for (int i = 0; i < aspath_size; i++) {
    cross_gulf_pathlet.add_path_vector(aspath[i]);
  }
  int pathlet_size = cross_gulf_pathlet.ByteSize();
  char* serialized_pathlet = (char*)malloc(pathlet_size);
  cross_gulf_pathlet.SerializeToArray(serialized_pathlet, pathlet_size);
  InsertRawPathletToSend(pathlet_internal_state, new_ip, serialized_pathlet,
                         pathlet_size);
  char* announce_ip = (char*)malloc(256);
  snprintf(announce_ip, 256, "%s/%d", new_ip, 32);
  announce_ips[array_position] = announce_ip;
  *num_ips = *num_ips + 1;
}

uint32_t* GetPathletPathVectorForAssociatedIp(
    PathletInternalStateHandle pathlet_internal_state, const char* ip_address,
    uint32_t* path_vector_size) {
  int pathlet_size;
  char* serialzied_pathlet = GetPathletAssociatedWithIp(
      pathlet_internal_state, ip_address, &pathlet_size);

  Pathlet pathlet;
  pathlet.ParseFromArray(serialzied_pathlet, pathlet_size);
  *path_vector_size = pathlet.path_vector_size();
  uint32_t* return_int_arr =
      (uint32_t*)malloc(*path_vector_size * sizeof(uint32_t));
  int pos = 0;
  for (uint32_t as : pathlet.path_vector()) {
    return_int_arr[pos] = as;
    pos++;
  }
  return return_int_arr;
}

// benchmarking functions below here

char* SetBenchmarkIABytes(char* serialized_advert, int advert_size,
                          int num_bytes_to_set, int* modified_advert_size) {
  IntegratedAdvertisement working_advertisement;
  if (advert_size != 0) {
    working_advertisement.ParseFromArray(serialized_advert, advert_size);
  }

  // Get a benchmark path group descriptor if it exists

  PathGroupDescriptor* benchmark_path_group_descriptor =
      GetProtocolPathGroupDescriptor(&working_advertisement,
                                     Protocol::P_BENCHMARK);

  // If there was no existing path group descriptor, then add it
  if (benchmark_path_group_descriptor == NULL) {
    benchmark_path_group_descriptor =
        working_advertisement.add_path_group_descriptors();
    benchmark_path_group_descriptor->set_protocol(Protocol::P_BENCHMARK);
  }

  // Get the keyvalue corresponding to benchmark bytes
  KeyValue* mutable_key_value = GetCreateKeyValueFromPathGroupDesc(
      benchmark_path_group_descriptor, "Bytes");

  BenchmarkProtocol benchmark_protocol;
  // Fill the benchmark protocol with bytes. Takes in the benchmark protocol and
  // the number of bytes to fill. Currently uses the repeated int32 field for this
  {
    // old way setting the number of bytes directly
    // char* byte_buffer = new char[num_bytes_to_set];
    // // go through and put a 1 in the byte
    // for(int i = 0; i < num_bytes_to_set; i++){
    //   byte_buffer[i] = 1;
    // }
    // benchmark_protocol.set_some_bytes(byte_buffer);
    // // delete[] byte_buffer;

    // new way using rpeated int32 field. Therefore, num ints corresponds to
    // num_bytes_to_set / 4
    for(int i = 0; i < num_bytes_to_set / 4; i++){
      benchmark_protocol.add_repeated_ints(2147483647);
    }
  }

  mutable_key_value->set_value(benchmark_protocol.SerializeAsString());

  // working_advertisement.PrintDebugString();

  *modified_advert_size = working_advertisement.ByteSize();
  char* modified_advert = (char*)malloc(*modified_advert_size);

  working_advertisement.SerializeToArray(modified_advert,
                                         *modified_advert_size);

  return modified_advert;
}

int GetBenchmarkIABytesSize(char* serialized_advert, int advert_size) {


  // Parse the advertisement
  IntegratedAdvertisement proto_advert;
  proto_advert.ParseFromArray(serialized_advert, advert_size);

  // Get the benchmark descriptor (we are assuming that it is always there)
  PathGroupDescriptor* benchmark_path_group_descriptor =
      GetProtocolPathGroupDescriptor(&proto_advert, Protocol::P_BENCHMARK);

  if (benchmark_path_group_descriptor == NULL) {
    return -1;
  }

  // Get the path cost from the assumed existent keyvalue
  KeyValue* benchmark_bytes_kv =
      GetPathGroupKeyValue(benchmark_path_group_descriptor, "Bytes");
  BenchmarkProtocol benchmark_protocol;
  benchmark_protocol.ParseFromString(benchmark_bytes_kv->value());
  return benchmark_protocol.google::protobuf::Message::ByteSize();
}
