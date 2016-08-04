
#include "wiser_config.h"
namespace quagga_bgpd{

  WiserConfig::WiserConfig(WiserProtocolConfig wiser_config)
  {
    wiser_config_ = wiser_config;
  }


  int WiserConfig::GetLinkCost(string ip1, string ip2)
  {
    if(wiser_link_costs_.empty())
      {
        ProcessWiserTopology();
      }
    // if there is an entyr for ip1, then check for a corresponding vlaue of ip2
    // in the value of the key of ip1.
    if(wiser_link_costs_.count(ip1) == 1){
      std::unordered_map<string, uint64_t> ip1_value = wiser_link_costs_.at(ip1);
      // check if the other end of the link is attached
      if(ip1_value.count(ip2) == 1)
        {
          return ip1_value.at(ip2);
        }
    }

    // here if there is no link in either direction, return erro result.
    return -1; 
  }

  // private methods under here

  void WiserConfig::ProcessWiserTopology(){
    if (!wiser_config_.has_topology()){
      return;
    }

    // for every link in every node, insert it into wiser link costs where there
    // is a primary key entry in the map for both ends.
    for (const NodeLink& node_link : wiser_config_.topology().node_links())
      {
        // ip of the primary node
        string ip1;
        ip1 = node_link.primary_node().interface_ip();
        for(const Link& link : node_link.links()){
          // ip of an adjacent ip to ip1
          string ip2;
          ip2 = link.adjacent_node().interface_ip();
          uint64_t link_cost = link.link_cost();
          // Add entry to map both forward and reverse
          std::unordered_map<string, uint64_t> tmp_map_ip_to_link_cost;
          tmp_map_ip_to_link_cost[ip2] = link_cost;
          wiser_link_costs_[ip1] = tmp_map_ip_to_link_cost;
          // clear the tmp map
          tmp_map_ip_to_link_cost.clear();
          tmp_map_ip_to_link_cost[ip1] = link_cost;
          wiser_link_costs_[ip2] = tmp_map_ip_to_link_cost;
        }
      }
  }
} // namespace quagga_bgpd
