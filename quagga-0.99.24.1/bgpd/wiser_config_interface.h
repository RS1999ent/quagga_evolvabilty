#ifndef BGPD_WISER_CONFIG_INTERFACE_H
#define BGPD_WISER_CONFIG_INTERFACE_H
#include "shared_enum_definitions.h"
#include "stdint.h"

struct WiserConfig;
struct GeneralConfiguration;
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct WiserConfig * WiserConfigHandle;
  void    free_WiserConfig(WiserConfigHandle);
  int GetLinkCost(WiserConfigHandle, const char * ip1, const char * ip2);
  const char* LinkCostsToString(WiserConfigHandle wiser_config_handle);

  /* Generalconfiguration class methods below here */
  // currently creates a wiser config from the string of a file name
  typedef struct GeneralConfiguration* GeneralConfigurationHandle;

  GeneralConfigurationHandle CreateGeneralConfig(char const * filename);
  void FreeGeneralConfig(GeneralConfigurationHandle general_config_handle);
  dbgp_protocol_t GetProtocolType(GeneralConfigurationHandle general_config_handle);
  uint32_t GetIslandId(GeneralConfigurationHandle general_config_handle);
  int IsRemoteAsAnIslandMember(GeneralConfigurationHandle general_config_handle, int remote_as);
  WiserConfigHandle GetWiserConfig(GeneralConfigurationHandle general_config_handle);


#ifdef __cplusplus
}
#endif

#endif

