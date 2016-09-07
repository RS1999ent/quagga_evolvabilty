#ifndef BGPD_GENERAL_CONFIGURATION_H
#define BGPD_GENERAL_CONFIGURATION_H
#include "quagga_config.pb.h"
#include "wiser_config.h"
#include "shared_enum_definitions.h"

enum kCProtocolType{
  CPT_UNKNOWN,
  CPT_WISER
};

struct GeneralConfiguration {

public:
  GeneralConfiguration(Configuration general_configuration);

  // Gets the protocol being used on this router from the configuration in an
  // ENUM that C code can work with.
  //
  // Returns: The enum corresponding to the protocol type.
  dbgp_protocol_t GetProtocolType();

  /* Creates and returns a reference to the WiserConfig class (that is the */
  /* object that works with the wiserconfig protobuf). */
  
  /* Returns a reference to a Wiserconfig (if it exists). Null if it doesn't */
  /* exist. */
  WiserConfig* GetWiserConfig();


private:

  Configuration general_configuration_;

};

#endif
