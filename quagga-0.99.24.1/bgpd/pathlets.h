#ifndef BGPD_PATHLETS_H
#define BGPD_PATHLETS_H

#include <map>
#include <vector>
#include <tuple>

#include "integrated_advertisement.pb.h"

using namespace std;
/* Keeps track of the internal state necessary to implement pathlets in
   quagga.

   The main components is the internal representation of the FID graph where
   nodes are vnodes and edges are FIDs. The second component is state that
   allows for this to work in quagga where it holds an ip address and the
   pathlet associated with it.

*/
class PathletInternalState{

 public:

  /* Constructs the class with the only initial input state is the /24 range
     where it can announce its individual pathlets. */
  PathletInternalState(string ip_slash_24);

  /* From the /24 range, gets a /32 that hasn't been used to announce an FID.
     Undefined behavior if you call this function more than 255 times. This
     function simply increments returns 'next_ip_' and increments it by one. So
     doing this more than 255 times will go beyond its allocation.

     Returns: An ip address (in string form) that hasn't been associated with a
     pathlet.
  */
  string GetNextIp();

  /* Gets the next FID that hasn't locally been used.

     Returns: The next FID that hasn't been locally used.
   */
  int GetNextFid();

  /* Given a pathlet (which consists of a FID and a pair of vnodes), inserts an
     entry into the pathlet_graph.

     Arguments:
        pathlet: the Pathlet message proto.
  */
  void InsertPathletIntoGraph(Pathlet pathlet);

  /* Converts the pathlet_graph_ into a series of pathlets in the form of a
     'Pathlets' message. This is for placing into a hop descriptor.

     TODO: make it so it places the proper multihop pathlet if avaialble.

     Returns: a Pathlets message that is then placed into a KeyValue in the
     integrated advertisement associated with a Hop descriptor.
  */
  Pathlets ConvertGraphToPathlets();

  /* Given an 'associated_ip', get the pathlet that is associated with the ip.
     In quagga, we are having unique ips identify pathlets in order for them to
     be disseminated everywhere. This will throw an assertion if this is called
     with an ip that is not in the 'pathlets_to_send' as it should always be
     there.

     Arguments:
        associated_ip: the ip address to get the pathlet for.

     Returns: The value in the 'ip_to_pathlet_to_send' associated with the passed
     value
  */
  Pathlet GetPathletToSend(string associated_ip);

 private:

  /* The pathlet graph where the key is the primary vnode mapping to an adjacent
     vnode mapping with the value being the FID to traverse that vnode */
  map<int, map<int, int>> pathlet_graph_;

  /* The next fid that hasn't been used for edges out of this node */
  int next_fid_;

  /* The next ip that hasn't been used for pathlets being advertised by this
     node. */
  uint32_t next_ip_;

  /* Maps an association between an ip address and a pathlet. This identifies
     the control information that needs to be attached to an advertisement with
     this ip address. */
  map<string, Pathlet> ip_to_pathlet_to_send;
 



};

#endif
