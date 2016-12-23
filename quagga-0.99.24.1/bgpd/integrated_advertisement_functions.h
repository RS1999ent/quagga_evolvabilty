#include "wiser_config_interface.h"
/* Creates a empty integrated advertisement and returns a reference to its
   serialized form.

   Arguments:
      size: ptr to an int that will be mutated with the length of the serialized
      instance.

   Returns: A ptr to the serialized empty protobuf.
*/
extern "C" char* CreateEmptyIntegratedAdvertisement(int* size);

/* Adds a wiser link cost to the integrated advertisement. If the advertisement
   doesn't exist (i.e. has a size of 0) then this function creates it and puts
   the cost for wiser in it.

   Arguments;
      serialized_advert: ptr to the serialized advertisement. Ownership is
      retained by caller.
      advert_size: the size of the serialized_advert. Expected >=0
      additive_path_cost: the cost to add to the advertisement.
      additive_path_cost: the cost to add on to the 'PathCost' attribute of
   wiser.
      modified_advert_size: The size of the advertisement after the cost has
      been added.
      normalization: How much the existing cost should be normalized before
      adding our own cost.

   Returns: A ptr to a new serialized advertisement. Ownership is passed to
   caller.

*/
extern "C" char* SetWiserControlInfo(char* serialized_advert, int advert_size,
                                     int additive_path_cost,
                                     int* modified_advert_size,
                                     float normalization);

/* Gets the wiser path cost associated with this serialized advert.

   @Note: Assumption: Wiser nodes are making the assumption that there is always
   a path cost if there is a pathgroupattributes (which there will be because
   they update the link cost on the incoming link). Note for if this ever
   changes.

   Arguments:
      serialized_advert: the serialized advert that is a serialized version of a
      protobuf.
      advert_size: the size of this serialized protobuf.

   Returns: the path coste associated with this advertisement. -1 if it doesn't
   exist
 */
extern "C" int GetWiserPathCost(char* serialized_advert, int advert_size);

/* Returns the last wiser node that changed the path cost.

   Arguments:
      serialized_advert: the IA proto in serialized form

   Returns: -1 if there was no previous wiser node that touched the node.
   Otherwise it will be the as that touched it last

 */
extern "C" int GetLastWiserNode(char* serialized_advert, int advert_size);

/* Sets a new LastWiserNode key value.  If it doesn't exist, it creates one and
   sets it

   Arguments:
      serialized_advert: The IA proto in serialized form that we are updateing.
      advet_size: the size of this advert in bytes
      new_last_node: the new last wiser node we are setting
      return_advert_size: The size of the returned new serialized advert

   Returns: A serialized advert with the new information set
 */
extern "C" char* SetLastWiserNode(char* serialized_advert, int advert_size,
                                  int new_last_node, int* return_advert_size);

extern "C" char* SerializedAdverToString(char* serialized_advert,
                                         int advert_size);

extern "C" char* PrintPathletsFromSerializedAdvert(char* serialized_advert,
                                                   int advert_size,
                                                   int island_id);

////////////////////////////////////
/// Pathlet specific functions/////
//////////////////////////////////

// converts adjacency graph to hop descriptors and creates a new IA from it.
// (adding to the existing IA)
extern "C" char* GenerateExternalPathletControlInfo(
    PathletInternalStateHandle pathlet_internal_state, int island_id,
    char* serialized_advert, int advert_size, int* new_advert_size);

/* Returns an integrated advertisement with the pathlets for a speicfic subgraph
   based on the destination ip. */
extern "C" char* GenerateExternalPathletDestinationControlInfo(
    PathletInternalStateHandle pathlet_internal_state, const char* destination,
    int island_id, int as_num, char* serialized_advert, int advert_size,
    int* new_advert_size);

// Given an serialized advert, will return 1 if it has pathlet control
// information in it.
extern "C" int HasPathletInformation(char* serialized_advert, int advert_size,
                                     int island_id);

// Given a serialized advert, will merge any pathlet information into the
// internal pathlet state maintained by PathletInternalState
extern "C" void MergePathletInformationIntoGraph(
    PathletInternalStateHandle pathlet_internal_state, char* serialized_advert,
    int advert_size, int island_id);

// Given an ip address, will get the pathlet information associated with it
// (that is in the map of ipaddress_to_pathlet information) and return a new
// advertisement with the pathlet information in it.
//
// Returns: The new serialzied advert, NULL if there was no pathlet associated
// with the ip address
extern "C" char* GenerateInternalPathletControlInfo(
    PathletInternalStateHandle pathlet_internal_state, char* serialized_advert,
    int advert_size, const char* ip_address, int* new_size, int island_id);

// given IA, island id, path vector, modifiy pathlets, create and add a cross
// gulf pathlet, add them to pathlets to send, mutates a list of ips. Assume
// that announceips has enough space
extern "C" void CreatePathletsFromIA(
    PathletInternalStateHandle pathlet_internal_state, char* serialized_advert,
    int advert_size, uint32_t* aspath, int aspath_size, int island_id,
    int as_num, char* announce_ips[], int* num_ips);

// Gets an array of the pathvector assocaited with a pathlet ip that will be
// announced. This is used for creatting a quagga path vector. Assuming that
// there is a pathlet assoicated with ip otherwise undefined behavior.
extern "C" uint32_t* GetPathletPathVectorForAssociatedIp(
    PathletInternalStateHandle pathlet_internal_state, const char* ip_address,
    uint32_t* path_vector_size);


///////////////////////////////
// Benchmark specific funcs //
/////////////////////////////

/* SetBenchmarkIABytes takes in a serialized advert and size and fills up a
   BenchmarkProtocol message with num_bytes_to_set

   Arguments:
      serialized_advert: ptr to the serialized advertisement. Ownership is
      retained by caller advert_size: the size of the serialized_advert.
      Expected >=0
      num_bytes_to_set: the number of dummy bytes to fill the advertisement up
      with
      modified_advert_size: The size of the advertisement after the benchmark
      bytes were added

   Returns: A ptr to a new serialized advertisement. Ownership is passed to
      caller.
*/
extern "C" char* SetBenchmarkIABytes(char* serialized_advert, int advert_size, int num_bytes_to_set, int* modified_advert_size);


/* GetBenchmarkIABytesSize returns the size of the bytes that are stored for the
   benchmark protocol. Mainly used for deserializing the packet to get this
   value.

   Arguments:
      serialized_advert: the advert to try and get the bytes from
      advert_size: the size of this serialized advert

   Returns: the size of raw bytes stored by the benchmark protocol
*/
extern "C" int GetBenchmarkIABytesSize(char* serialized_advert, int advert_size);
