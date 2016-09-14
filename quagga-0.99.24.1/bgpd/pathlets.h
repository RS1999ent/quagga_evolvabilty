#ifndef BGPD_PATHLETS_H
#define BGPD_PATHLETS_H

#include <map>
#include <vector>
#include <tuple>

#include "integrated_advertisement.pb.h"

using namespace std;
class PathletInternalState{

 public:

  PathletInternalState(string ip_slash_24);

  string GetNextIp();

  int GetNextFid();

  void InsertPathletIntoGraph(Pathlet pathlet);

  Pathlets ConvertGraphToPathlets();

  Pathlet GetPathletToSend(string associated_ip);

 private:

  map<int, map<int, int>> pathlet_graph_;

  int next_fid_;

  uint32_t next_ip_;

  map<string, Pathlet> pathlets_to_send_;
 



};

#endif
