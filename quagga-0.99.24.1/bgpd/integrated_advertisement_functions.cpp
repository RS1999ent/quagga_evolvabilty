#include "integrated_advertisement_functions.h"
#include "integrated_advertisement.pb.h"
#include <iostream>

// Retrieves a reference for a PathGroupDescriptor for a given protocol if it
// exists. If it doesn't, returns a null reference.
//
// Arguments
//   advertisement: The advertisement to search for a protocol pathgroup
//   descriptor
//   protocol: A 'Protocol' enum specifying the protocol type to look for
PathGroupDescriptor* GetProtocolPathGroupDescriptor(IntegratedAdvertisement *advertisement, Protocol protocol)
{
  // For each pathgroup descriptor, check to see if the protocol type matches the input. If it does, return the reference.
  for(int i = 0; i < advertisement->path_group_descriptors_size(); i++){
    PathGroupDescriptor *mutable_path_group_descriptor =
      advertisement->mutable_path_group_descriptors(i);
    if (mutable_path_group_descriptor->protocol() == protocol) {
      return mutable_path_group_descriptor;
    }
  }
  // Here if there was no path group descriptor, so return NULL.
  return NULL;
}

// Returns a reference to a mutable Keyvalue that corresponds to the key passed
// in. If it does not exist, one will be added to the path_group_descriptor and
// a reference returned.
//
// Arguments:
//   path_group_descriptor: The path_group_descriptor to search for a given key. May be mutated
//   search_key: the key to search for a keyvalue of.
//
// Returns a reference to a given KeyValue.
KeyValue* GetCreateKeyValue(PathGroupDescriptor* path_group_descriptor,
                            const std::string& search_key){
  // Go through each keyvalue and check if the key is equal to the search_key passed in.
  for(int i = 0; i < path_group_descriptor->key_values_size(); i++){
    KeyValue* mutable_key_value = path_group_descriptor->mutable_key_values(i);
    if(mutable_key_value->key().compare(search_key) == 0)
      {
        return mutable_key_value;
      }
  }
  // Here if there was no key found
  KeyValue *mutable_key_value = path_group_descriptor->add_key_values();
  mutable_key_value->set_key(search_key);
  return mutable_key_value;
}

// Returns a reference to a mutable Keyvalue that corresponds to the key passed
// in. If it does not exist, it will return null
//
// Arguments:
//   path_group_descriptor: The path_group_descriptor to search for a given key. May be mutated
//   search_key: the key to search for a keyvalue of.
//
// Returns a reference to a given KeyValue. Null if it doesn't exist.
KeyValue* GetKeyValue(PathGroupDescriptor* path_group_descriptor,
                            const std::string& search_key){
  // Go through each keyvalue and check if the key is equal to the search_key passed in.
  for(int i = 0; i < path_group_descriptor->key_values_size(); i++){
    KeyValue* mutable_key_value = path_group_descriptor->mutable_key_values(i);
    if(mutable_key_value->key().compare(search_key) == 0)
      {
        return mutable_key_value;
      }
  }
  return NULL;
}


char* SetWiserControlInfo(char* serialized_advert, int advert_size, int additive_path_cost, int* modified_advert_size) {

  // Parse the existing advertisement if it exists
  IntegratedAdvertisement working_advertisement;
  if(advert_size != 0){
    assert(working_advertisement.ParseFromArray(serialized_advert, advert_size) == true);
  }

  // Get a wiser path group descriptor if it exists

  PathGroupDescriptor *wiser_path_group_descriptor =
    GetProtocolPathGroupDescriptor(&working_advertisement,
                                   Protocol::P_WISER);

  // If there was no existing path group descriptor, then add it
  if(wiser_path_group_descriptor == NULL){
    wiser_path_group_descriptor = working_advertisement.add_path_group_descriptors();
    wiser_path_group_descriptor->set_protocol(Protocol::P_WISER);
  }

  // Get the keyvalue corresponding to wiser path cost.
  KeyValue *mutable_key_value = GetCreateKeyValue(wiser_path_group_descriptor, "PathCost");

  // add onto the path cost and reserialize it into the value of the key value
  PathCost path_cost;
  path_cost.ParseFromString(mutable_key_value->value());
  int current_cost = path_cost.path_cost();
  int new_cost = current_cost + additive_path_cost;
  printf("integrated_advertisement_functions::SetWiserControlInfo: cost %i added to entering control info with cost %i giving total %i", additive_path_cost, current_cost, new_cost);
  path_cost.set_path_cost(new_cost);
  mutable_key_value->set_value(path_cost.SerializeAsString());

  // working_advertisement.PrintDebugString();

  *modified_advert_size = working_advertisement.ByteSize();
  char *modified_advert = new char[*modified_advert_size];

  working_advertisement.SerializeToArray(modified_advert, *modified_advert_size);

  return modified_advert;
}

//untested
int GetWiserPathCost(char* serialized_advert, int advert_size)
{
  // Parse the advertisement
  IntegratedAdvertisement proto_advert;
  assert(proto_advert.ParseFromArray(serialized_advert, advert_size) == 1);

  // Get the wiser descriptor (we are assuming that it is always there)
  PathGroupDescriptor *wiser_path_group_descriptor =
    GetProtocolPathGroupDescriptor(&proto_advert, 
                                   Protocol::P_WISER);

  // Get the path cost from the assumed existent keyvalue
  KeyValue *path_cost_kv = GetKeyValue(wiser_path_group_descriptor, "PathCost");
  PathCost path_cost;
  path_cost.ParseFromString(path_cost_kv->value());
  return path_cost.path_cost();

}



