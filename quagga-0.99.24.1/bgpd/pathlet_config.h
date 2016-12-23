#ifndef BGPD_PATHLET_CONFIG_H
#define BGPD_PATHLET_CONFIG_H

#include "pathlet_internal_state.h"
#include "quagga_config.pb.h"

struct PathletConfig {
 public:
  PathletConfig(PathletProtoConfig pathlet_config) {
    pathlet_config_ = pathlet_config;
  }

  /* Returns whether this particular router is a island border router. 0 if it
     isn't, 1 if it is */
  int IsIslandBorderRouter() {
    if (pathlet_config_.is_island_border_router() == 1) {
      return 1;
    } else {
      return 0;
    }
  }

  /* Creates a pathlet internal state from the island private slash24_ip*/
  PathletInternalState* CreatePathletInternalState() {
    return new PathletInternalState(pathlet_config_.private_slash24_ip());
  }

  // Gets the private slash 24 associated with this ip
  string GetPrivateIp() { return pathlet_config_.private_slash24_ip(); }

  ManualPathlet GetManualTwoHop(string one_hop_ip, int* found1) {
    *found1 = 0;
    for (Filter filter : pathlet_config_.filters()) {
      string filter_ip = filter.one_hop_ip();
      if (one_hop_ip.compare(filter_ip) == 0) {
        *found1 = 1;
        return filter.pathlet_to_advertise();
      }
    }
    ManualPathlet man_pathlet;
    return man_pathlet;
  }

 private:
  PathletProtoConfig pathlet_config_;
};

#endif
