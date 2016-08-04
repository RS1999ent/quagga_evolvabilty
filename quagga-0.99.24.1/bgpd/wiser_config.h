#ifndef BGPD_WISER_CONFIG_H
#define BGPD_WISER_CONFIG_H
#include "quagga_config.pb.h"

#include <unordered_map>

/* namespace quagga_bgpd { */
  using std::string;
  // Purpose of class is to handle the specifics of interacting with
  // configuration for wiser.
  struct WiserConfig {

  public:
    WiserConfig(WiserProtocolConfig wiser_config);

    // Gets the link cost between two interfaces in a topology. Interfaces are
    // assumed to be unique and symmetric.
    //
    // Arguments:
    //   ip1 : The ip address of an interface at one end of the link.
    //   ip2 : The ip addreess of an interface at the other end of the link.
    //
    // Returns: The link cost of the link. If it doesn't exist, returns -1.
    int GetLinkCost(string ip1, string ip2);

  private:

    // Processes the wiser_config_ class member into wiser_link_costs_. Called
    // once when wiser_link_costs_ is empty.
    void ProcessWiserTopology();

    // Holds the protobuf representation of the wiserconfig
    WiserProtocolConfig wiser_config_;

    // Holds the link cost of links. First key in first map is the ip address of
    // one end of a link. The first key on the second map is the ip address of
    // the other end, with the value being the link cost.
    // Visal Example in python syntax:
    //   'a1' : {'b1' : 500}, shows that the link a1-b1 has a cost of 500
    std::unordered_map<string, std::unordered_map<string, uint64_t>> wiser_link_costs_;
  };
/* } // namespace quagga_bgpd */

#endif // BGPD_WISER_CONFIG_H
