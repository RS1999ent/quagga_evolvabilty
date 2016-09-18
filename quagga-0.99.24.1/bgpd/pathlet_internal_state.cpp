#include "pathlet_internal_state.h"
#include <arpa/inet.h>

void PathletInternalState::InsertPathletToSend(string associated_ip,
                                               Pathlet pathlet_to_send) {
  ip_to_pathlet_to_send[associated_ip] = pathlet_to_send;
}

// untested
Pathlet PathletInternalState::GetPathletToSend(string associated_ip) {
  Pathlet return_pathlet;

  return ip_to_pathlet_to_send[associated_ip];
}

Pathlets PathletInternalState::ConvertGraphToPathlets() {
  // for each entry in each map, create a pathlet and add it to the return
  // pathlets
  Pathlets return_pathlets;
  for (auto primarynode_and_map : pathlet_graph_) {
    int primarynode = primarynode_and_map.first;
    for (auto adjacent_and_pathlet : primarynode_and_map.second) {
      // create a pathlet and insert it into return_pathlets
      Pathlet *new_pathlet = return_pathlets.add_pathlets();
      *new_pathlet = adjacent_and_pathlet.second;
      // int adjacent_node = adjacent_and_pathlet.first;
      // Pathlet adjcent_pathlet = adjacent_and_pathlet.second;
      // int fid = adjacent_pathlet.;
      // new_pathlet->set_fid(fid);
      // new_pathlet->add_vnodes(primarynode);
      // new_pathlet->add_vnodes(adjacent_node);
    }
  }
  return return_pathlets;
}

void PathletInternalState::InsertPathletIntoGraph(Pathlet pathlet) {
  // Get the fields to describe the adjacency entry
  // int fid = pathlet.fid();
  int primary_node = pathlet.vnodes(0);
  int adjacent_node = pathlet.vnodes(1);

  // Create primary node entry if it doesn't exist
  if (pathlet_graph_.count(primary_node) != 1) {
    pathlet_graph_[primary_node] = map<int, Pathlet>();
  }

  // Simply insert it into the graph
  pathlet_graph_[primary_node][adjacent_node] = pathlet;
}

int PathletInternalState::GetNextFid() {
  int return_fid = next_fid_;
  next_fid_++;
  return return_fid;
}

string PathletInternalState::GetNextIp() {
  uint32_t nextip = next_ip_;
  struct sockaddr_in sa;
  // Put the integer ip in an s_addr so that inet_ntop can work its magic
  sa.sin_addr.s_addr = htonl(nextip);
  char str[INET_ADDRSTRLEN];

  // Get the presentation format
  inet_ntop(AF_INET, &(sa.sin_addr), str, INET_ADDRSTRLEN);

  // incremetn the ip so the next time this is called a fresh ip is given
  next_ip_++;

  // return a string
  return string(str);
}

PathletInternalState::PathletInternalState(string ip_slash_24) {
  // Convert the presentation format to an int32
  struct sockaddr_in sa;
  inet_pton(AF_INET, ip_slash_24.c_str(), &sa.sin_addr);
  next_ip_ = ntohl(sa.sin_addr.s_addr);

  // Start at fid 1
  next_fid_ = 1;
}

const map<int, map<int, Pathlet>> PathletInternalState::GetPathletGraph() {
  return pathlet_graph_;
}

string PathletInternalState::GraphToString() {
  string return_string;

  for (auto node_and_adjnodeAndPathlet : pathlet_graph_) {
    int primary_node = node_and_adjnodeAndPathlet.first;
    for (auto adjnode_and_pathlet : node_and_adjnodeAndPathlet.second) {
      int adjacent_node = adjnode_and_pathlet.first;
      int fid = adjnode_and_pathlet.second.fid();
      return_string += std::to_string(primary_node) + " " +
                       std::to_string(adjacent_node) + " " +
                       std::to_string(fid) + "\n";
    }
  }
  return return_string;
}

string PathletInternalState::PathletsToSendToString() {
  string return_string;

  for (auto ip_and_pathlet : ip_to_pathlet_to_send) {
    string ip = ip_and_pathlet.first;
    string pathlet = ip_and_pathlet.second.DebugString();
    return_string += ip + " pathlet {" + pathlet + "}\n";
  }
  return return_string;

}
