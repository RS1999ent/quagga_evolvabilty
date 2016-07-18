#ifndef BGPD_WISER_CONFIG_H
#define BGPD_WISER_CONFIG_H
#include "quagga_config.pb.h"

/* using quagga_config.pb.h::Configuration; */
class WiserConfig {

  WiserConfig();

  Configuration config_;
};

#endif // BGPD_WISER_CONFIG_H
