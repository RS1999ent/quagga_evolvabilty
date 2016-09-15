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
  
}

int PathletInternalState::GetNextFid() {
  int return_fid = next_fid_;
  next_fid_++;
  return return_fid;
}

string PathletInternalState::GetNextIp() {

  uint32_t nextip = next_ip_;
  struct sockaddr_in sa;
  sa.sin_addr.s_addr = htonl(nextip);
  char str[INET_ADDRSTRLEN];

  inet_ntop(AF_INET, &(sa.sin_addr), str, INET_ADDRSTRLEN);

  next_ip_++;

  return string(str);
}

PathletInternalState::PathletInternalState(string ip_slash_24) {
  struct sockaddr_in sa;
  inet_pton(AF_INET, ip_slash_24.c_str(), &sa.sin_addr);
  next_ip_ = ntohl(sa.sin_addr.s_addr);

  next_fid_ = 1;
}
