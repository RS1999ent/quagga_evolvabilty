#include "general_configuration.h"



GeneralConfiguration::GeneralConfiguration(Configuration general_configuration) {
  general_configuration_ = general_configuration;
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

  default :
    return dbgp_protocol_baseline;
    break;
  }
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
