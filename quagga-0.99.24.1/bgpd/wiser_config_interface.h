#ifndef BGPD_WISER_CONFIG_INTERFACE_H
#define BGPD_WISER_CONFIG_INTERFACE_H
#include "shared_enum_definitions.h"
#include "stdint.h"

struct WiserConfig;
struct GeneralConfiguration;
struct PathletInternalState;
struct PathletConfig;
#ifdef __cplusplus
extern "C" {
#endif

typedef struct WiserConfig* WiserConfigHandle;
void free_WiserConfig(WiserConfigHandle);
int GetLinkCost(WiserConfigHandle, const char* ip1, const char* ip2);
const char* LinkCostsToString(WiserConfigHandle wiser_config_handle);

/* Generalconfiguration class methods below here */
// currently creates a wiser config from the string of a file name
typedef struct GeneralConfiguration* GeneralConfigurationHandle;

GeneralConfigurationHandle CreateGeneralConfig(char const* filename);
void FreeGeneralConfig(GeneralConfigurationHandle general_config_handle);
dbgp_protocol_t GetProtocolType(
    GeneralConfigurationHandle general_config_handle);
uint32_t GetIslandId(GeneralConfigurationHandle general_config_handle);
int IsRemoteAsAnIslandMember(GeneralConfigurationHandle general_config_handle,
                             int remote_as);
WiserConfigHandle GetWiserConfig(
    GeneralConfigurationHandle general_config_handle);

uint32_t GetBenchmarkBytes(GeneralConfigurationHandle general_config_handle);

// pathletInternalState handle creator
typedef struct PathletInternalState* PathletInternalStateHandle;
char* ConvertGraphToPathlets(PathletInternalStateHandle pathlet_internal_state,
                             int* size);
void InsertPathletIntoGraph(PathletInternalStateHandle pathlet_internal_state,
                            char* pathlet, int size);
// INSERT PATHLET, takes 3 ints, and an ip address.
void InsertPathletToSend(PathletInternalStateHandle pathlet_internal_state,
                         char* associated_ip, int fid, int as1, int as2,
                         char* dest_ip, int is_two_hop);
void InsertRawPathletToSend(PathletInternalStateHandle pathlet_internal_state,
                            char* assocaited_ip, char* serialized_pathlet,
                            int pathlet_size);
char* GetPathletAssociatedWithIp(
    PathletInternalStateHandle pathlet_internal_state,
    const char* associated_ip, int* return_size);
char* GetNextIp(PathletInternalStateHandle pathlet_internal_state);
int GetNextFid(PathletInternalStateHandle pathlet_internal_state);
char* GetPathletsForDestination(
    PathletInternalStateHandle pathlet_internal_state, const char* destination,
    int island_id, int as_num, int* pathlets_size);
char* GetPathletGraphString(PathletInternalStateHandle pathlet_internal_state);
char* GetPathletsToSendString(
    PathletInternalStateHandle pathlet_internal_state);

// pathlet config functions
typedef struct PathletConfig* PathletConfigHandle;
PathletConfigHandle CreatePathletConfig(
    GeneralConfigurationHandle general_config);
PathletInternalStateHandle CreatePathletInternalState(
    PathletConfigHandle pathlet_config);
int IsIslandBorderRouter(PathletConfigHandle pathlet_config);
char* GetPrivateIp(PathletConfigHandle pathlet_config);
char* GetManualTwoHop(PathletConfigHandle pathlet_config, char* one_hop_ip,
                      int* vnode1, int* vnode2);

#ifdef __cplusplus
}
#endif

#endif
