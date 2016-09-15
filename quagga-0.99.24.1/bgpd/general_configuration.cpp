#include "general_configuration.h"



GeneralConfiguration::GeneralConfiguration(Configuration general_configuration) {
  general_configuration_ = general_configuration;
}


int GeneralConfiguration::GetIslandId() {
  return general_configuration_.island_id();
}

dbgp_protocol_t GeneralConfiguration::GetProtocolType()
{
  // get the protocol type
  ProtocolType config_protocol_type = general_configuration_.protocol_type();

  // switch on it
  switch (config_protocol_type){
  case PT_WISER:
    return dbgp_critical_wiser;
    break;
  case PT_PATHLETS:
    return dbgp_replacement_pathlets;
    break;

  default :
    return dbgp_protocol_baseline;
    break;
  }
}


int GeneralConfiguration::IsRemoteAsAnIslandMember(int remote_as) {
  for(const int& island_member : general_configuration_.island_member_ases())
    {
      if(island_member == remote_as)
        {
          return 1;
        }
    }
  return 0;
}

WiserConfig* GeneralConfiguration::GetWiserConfig() {
  WiserConfig *return_wiser_config = NULL;
  // if we have a wiserconfiguration, then return a WiserConfig reference.
  // Otherwise return null
  if(general_configuration_.has_wiser_protocol_config())
    {
      WiserProtocolConfig general_config_copy = general_configuration_.wiser_protocol_config();
      return_wiser_config = new WiserConfig(general_config_copy);

    }

  return return_wiser_config;
}

PathletConfig* GeneralConfiguration::GetPathletConfig(){
  PathletConfig* return_pathlet_config = NULL;

  if(general_configuration_.has_pathlet_config())
    {
      PathletProtoConfig general_config_copy = general_configuration_.pathlet_config();
      return_pathlet_config = new PathletConfig(general_config_copy);
    }
  return return_pathlet_config;

}
