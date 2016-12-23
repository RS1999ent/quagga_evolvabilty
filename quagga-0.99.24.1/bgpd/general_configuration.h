#ifndef BGPD_GENERAL_CONFIGURATION_H
#define BGPD_GENERAL_CONFIGURATION_H
#include "pathlet_config.h"
#include "quagga_config.pb.h"
#include "shared_enum_definitions.h"
#include "wiser_config.h"

enum kCProtocolType { CPT_UNKNOWN, CPT_WISER };

struct GeneralConfiguration {
 public:
  GeneralConfiguration() {}
  GeneralConfiguration(Configuration general_configuration);

  // Gets the protocol being used on this router from the configuration in an
  // ENUM that C code can work with.
  //
  // Returns: The enum corresponding to the protocol type.
  dbgp_protocol_t GetProtocolType();

  /* Gets the 'island_id' from the general configuration protobuf.

     Returns: The integer corresponding to the island id. untested
   */
  int GetIslandId();

  /* Checks to see if the 'remote_as' is a part of the protocol island that this
     router is a part of.

     Arguments:
        remote_as: the remote as to check if it is a part of our island.

     Returns: 1 if it is and 0 if it isn't.
  */
  int IsRemoteAsAnIslandMember(int remote_as);

  /* GetBenchmarkBytes returns the number of bytes to write for the
     BenchmarkProtocolConfig. We assume that, if called, the configuration
     exists.

     Returns: the number of bytes to serialize.
  */
  int GetBenchmarkBytes();

  /* Creates and returns a reference to the WiserConfig class (that is the */
  /* object that works with the wiserconfig protobuf). */

  /* Returns a reference to a Wiserconfig (if it exists). Null if it doesn't */
  /* exist. */
  WiserConfig* GetWiserConfig();

  /* Creates and returns a reference to the PathletConfig class (that is the */
  /* object that works with the wiserconfig protobuf). */

  /* Returns a reference to a PathletConfig (if it exists). Null if it doesn't
   */
  /* exist. */
  PathletConfig* GetPathletConfig();

 private:
  Configuration general_configuration_;
};

#endif
