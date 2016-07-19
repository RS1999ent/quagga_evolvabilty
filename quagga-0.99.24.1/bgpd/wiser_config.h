#ifndef BGPD_WISER_CONFIG_H
#define BGPD_WISER_CONFIG_H
#include "quagga_config.pb.h"

namespace quagga_bgpd {
using std::string;
class WiserConfig {

 public:
  WiserConfig(WiserProtocolConfig wiser_config);

  int GetLinkCost(string ip1, string ip2);

 private:

  WiserProtocolConfig wiser_config_;
};
} // namespace quagga_bgpd

#endif // BGPD_WISER_CONFIG_H
