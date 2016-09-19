#include "pathlet_internal_state.h"
#include <arpa/inet.h>
#include <unordered_set>
#include <iostream>
using namespace std;

void PathletInternalState::InsertPathletToSend(string associated_ip,
                                               Pathlet pathlet_to_send) {
  ip_to_pathlet_to_send[associated_ip] = pathlet_to_send;
  InsertPathletIntoGraph(pathlet_to_send);
}


// untested
Pathlet PathletInternalState::GetPathletToSend(string associated_ip) {
  Pathlet return_pathlet;
  return_pathlet = ip_to_pathlet_to_send[associated_ip];
  // ip_to_pathlet_to_send.erase(associated_ip);
  return return_pathlet;
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

Pathlets PathletInternalState::GetPathletsForDestination(string destination,
                                                         int island_id,
                                                         int as_num) {
  // create predecessor list, find the node that has the pathlet that has the
  // destination we are looking for
  map<int, unordered_set<int>> predecessors = GetPredecessors(as_num);

  int dest_node;
  for(auto primary_and_map : pathlet_graph_)
    {
      for(auto adjacent_and_pathlet : primary_and_map.second){
        if(destination.compare(adjacent_and_pathlet.second.destination()) == 0)
          {
            dest_node = adjacent_and_pathlet.first;
            break;
          }
      }
    }

  vector<Pathlet> pathlet_vector = GetAllPrecedingPathlets(dest_node, predecessors);
  Pathlets return_pathlets;
  for(Pathlet pathlet : pathlet_vector){
    pathlet.add_path_vector(island_id);
    Pathlet *insert_pathlet = return_pathlets.add_pathlets();
    *insert_pathlet = pathlet;
  }
  return return_pathlets;
}
// private methods under here
map<int, unordered_set<int>> PathletInternalState::GetPredecessors(int as_num){

  // Mark all the vertices as not visited and initialized all predecessors
  map<int, bool> visited;
  map<int, unordered_set<int>> predecessors;
  for (auto primary_and_map : pathlet_graph_) {
    int primary = primary_and_map.first;
    visited[primary] = false;
    predecessors[primary] = unordered_set<int>();
    for (auto adjacent_and_pathlet : primary_and_map.second) {
      visited[adjacent_and_pathlet.first] = false;
      predecessors[adjacent_and_pathlet.first] = unordered_set<int>();
    }
  }

  // Create a stack for BFS
  vector<int> stack;

  // Mark the current node as visited and push it
  visited[as_num] = true;
  stack.push_back(as_num);

  while (!stack.empty()) {
    // pop a vertex from stack
    int working_as = stack.back();
    stack.pop_back();
    // Get all adjacent vertices of the dequeued vertex working_as
    // If a adjacent has not been visited, then mark it visited
    // and enqueue it
    for (auto adjacent_and_pathlet : pathlet_graph_[working_as]) {
      int adjacent_node = adjacent_and_pathlet.first;
      if (!visited[adjacent_node]) {
        visited[adjacent_node] = true;
        stack.push_back(adjacent_node);
      }
      // we don't want the root as having any predcessors otherwise we'll run
      // into an infinite loop
      if(predecessors[adjacent_node].count(working_as) == 0 && adjacent_node != as_num){
        predecessors[adjacent_node].insert(working_as);
      }
    }
  }
  return predecessors;
}

vector<Pathlet> PathletInternalState::GetAllPrecedingPathlets(int starting_pt, map<int, unordered_set<int>> predecessors)
{
  vector<Pathlet> return_vector;
  for(int predecessor : predecessors[starting_pt]){
    Pathlet predecessor_pathlet = pathlet_graph_[predecessor][starting_pt];
    return_vector.push_back(predecessor_pathlet);
    vector<Pathlet> append_pathlet = GetAllPrecedingPathlets(predecessor, predecessors);
    for(Pathlet pathlet : append_pathlet){
      return_vector.push_back(pathlet);
    }
  }
  return return_vector;
}


