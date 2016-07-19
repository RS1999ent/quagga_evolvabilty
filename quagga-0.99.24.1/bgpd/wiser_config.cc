
#include "wiser_config.h"
namespace quagga_bgpd{

WiserConfig::WiserConfig(WiserProtocolConfig wiser_config)
{
  wiser_config_ = wiser_config;
}


int GetLinkCost(string ip1, string ip2)
{
  return 1; 
}
} // namespace quagga_bgpd
