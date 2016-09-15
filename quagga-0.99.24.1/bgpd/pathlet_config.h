#ifndef BGPD_PATHLET_CONFIG_H
#define BGPD_PATHLET_CONFIG_H

#include "quagga_config.pb.h"
#include "pathlet_internal_state.h"

struct PathletConfig{

 public:
  PathletConfig(PathletProtoConfig pathlet_config){
    pathlet_config_ = pathlet_config;
  }

  /* Returns whether this particular router is a island border router. 0 if it
     isn't, 1 if it is */
  int IsIslandBorderRouter(){
    if(pathlet_config_.is_island_border_router() == 1){
      return 1;
    }
    else{
      return 0;
    }
  }

  /* Creates a pathlet internal state from the island private slash24_ip*/
  PathletInternalState* CreatePathletInternalState(){
    return new PathletInternalState(pathlet_config_.private_slash24_ip());
  }

private:

  PathletProtoConfig pathlet_config_;

};

#endif
