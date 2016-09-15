#include "pathlets.h"
#include <arpa/inet.h>


Pathlet PathletInternalState::GetPathletToSend(string associated_ip) {
  Pathlet b;

  return b;
}

Pathlets PathletInternalState::ConvertGraphToPathlets() {
  Pathlets b;

  return b;
}

void PathletInternalState::InsertPathletIntoGraph(Pathlet pathlet) {
  // Get the fields to describe the adjacency entry
  int fid = pathlet.fid();
  int primary_node = pathlet.vnodes(0);
  int adjacent_node = pathlet.vnodes(1);

  //Create primary node entry if it doesn't exist
  if(pathlet_graph_.count(primary_node) != 1){
    pathlet_graph_[primary_node] = map<int,int>();
  }

  // Simply insert it into the graph
  pathlet_graph_[primary_node][adjacent_node] = fid;

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


const map<int,map<int, int>> PathletInternalState::GetPathletGraph() {
  return pathlet_graph_;
}
