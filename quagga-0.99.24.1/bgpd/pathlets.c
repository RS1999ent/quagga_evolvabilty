#include "pathlets.h"
#include "bgpd/bgp_aspath.h"
#include "wiser_config_interface.h"
#include "lib/vty.h"
#include "lib/memory.h"
#include "bgpd/bgp_route.h"
/* #include "bgpd/bgp_vty.h" */
/* #include "bgpd/dbgp_lookup.h" */
/* #include "bgpd/bgp_common.h" */
/* #include "bgpd/bgp_route.h" */
/* #include "bgpd/wiser_config_interface.h" */
/* #include <arpa/inet.h> */

/* ********************* Global vars ************************** */
extern GeneralConfigurationHandle general_configuration_;
extern PathletInternalStateHandle pathlet_internal_state_;
/* extern struct vty* vty; */
int HasPathletInformation(char* serialized_advert, int advert_size, int island_id);
char* GenerateInternalPathletControlInfo(PathletInternalStateHandle pathlet_internal_state, char *serialized_advert, int advert_size, const char* ip_address, int* new_size, int island_id);
void MergePathletInformationIntoGraph(PathletInternalStateHandle pathlet_internal_state, char *serialized_advert, int advert_size, int island_id);

void AddAssociatedPathlet(char* associated_ip, int island_id, dbgp_control_info_t* control_info) {
  char* old_integrated_advertisement = control_info->integrated_advertisement;
  int old_integrated_advertisement_size = control_info->integrated_advertisement_size;
  int new_size;
  char* new_integrated_advertisement_info = GenerateInternalPathletControlInfo(pathlet_internal_state_, old_integrated_advertisement, old_integrated_advertisement_size, associated_ip, &new_size, island_id);

  free(old_integrated_advertisement);
  control_info->integrated_advertisement = new_integrated_advertisement_info;
  control_info->integrated_advertisement_size = new_size;

}

void AnounceStaticRoute(char* ip_and_prefix, struct bgp* bgp){
  struct bgp_static *bgp_static;
  struct prefix p;
  str2prefix (ip_and_prefix, &p);
  bgp_static = XCALLOC (MTYPE_BGP_STATIC, sizeof (struct bgp_static));
  bgp_static->backdoor = 0;
  bgp_static->valid = 0;
  bgp_static->igpmetric = 0;
  bgp_static->igpnexthop.s_addr = 0;

  zlog_debug("pathlets::AnnounceStaticRoute: announcing ip: %s", ip_and_prefix);
	bgp_static_update (bgp, &p, bgp_static, AFI_IP, SAFI_UNICAST);
}

int
aspath_get_rightmost (struct aspath *aspath)
{
  struct assegment *seg;
  int return_as = -1;

  if ( (aspath == NULL) || (aspath->segments == NULL) )
    return return_as; // return -1 if there is no aspath
  
  seg = aspath->segments;
  
  while (seg)
    {
      int i;
      
      for (i = 0; i < seg->length; i++)
        return_as = seg->as[i];
      
      seg = seg->next;
    }
  // Here, return AS should be the rightmost segment
  return return_as;
}
/* ********************* Public functions ********************* */

void pathlets_update_control_info(dbgp_control_info_t* control_info, struct peer* peer, struct attr* attr) {
  int originating_as = aspath_get_rightmost(attr->aspath);
  // Is the originating as an island member
  int is_originating_island_member = IsRemoteAsAnIslandMember(general_configuration_, originating_as);
  int has_pathlet_info = HasPathletInformation(control_info->integrated_advertisement, control_info->integrated_advertisement_size, peer->bgp->island_id);

  //case where it has pathlet control info from our island.
  if(has_pathlet_info){
    MergePathletInformationIntoGraph(pathlet_internal_state_, control_info->integrated_advertisement, control_info->integrated_advertisement_size, peer->bgp->island_id);
    // return because we don't want to modify, only learn from this advert
    return;
  }
  // here if there is no pathlet info from our island
  // case where it is external and has pathlet info, merge into adjacency graph associated with island id
  // TODO

  // If it is the case that the advert originated from our island and has no
  // pathlet info, then this is one we originated (we would have filtered it
  // otherwise in input filter) and we should have some pathlet information for
  // it.
  if(is_originating_island_member){
    char *string_ip = malloc(INET_ADDRSTRLEN);
    inet_ntop (AF_INET, &attr->nexthop, string_ip, INET_ADDRSTRLEN);
    zlog_debug("pathlets::pathlets_update_control_info: Next for advert: %s", string_ip);
    AddAssociatedPathlet(string_ip, peer->bgp->island_id, control_info);
  }
}

dbgp_filtered_status_t pathlets_input_filter(dbgp_control_info_t* control_info, struct attr* attr, struct peer* peer) {

  // if an incoming advertisement has control info
  int has_pathlet_info = HasPathletInformation(control_info->integrated_advertisement, control_info->integrated_advertisement_size, peer->bgp->island_id);

  // if there is pathlet info in it, we don't want to do any filtering, let it
  // pass onto the pathlets_update_control_info
  if(has_pathlet_info == 1){
    zlog_debug("pathlets:pathlets_input_filter: Advert has control info. aspath: %s", attr->aspath->str);
    return DBGP_NOT_FILTERED;
  }
  // here if the advert had no pathlet control info.
  // Get the originating as and determine if 
  int originating_as = aspath_get_rightmost(attr->aspath);
  // If the as is not a part of our island, then there is no specific pathlet
  // stuff we need to do with it.
  if(IsRemoteAsAnIslandMember(general_configuration_, originating_as) == 0){
    return DBGP_NOT_FILTERED;
  }
  // Here if there is no pathlet info and the originating as was a island
  // member. This means that this as is advertising a 1 hop location. We need to
  // create a pathlet and originate a new prefix with this pathlet.
  char *new_ip = GetNextIp(pathlet_internal_state_);
  int free_fid = GetNextFid(pathlet_internal_state_);
  int as1 = peer->bgp->as;
  int as2 = peer->as;
  zlog_debug("pathlets::pathlets_input_filter: creating pathlet for ip (%s) fid (%i) as1(%i) as2(%i)", new_ip, free_fid, as1, as2);
  InsertPathletToSend(pathlet_internal_state_, new_ip, free_fid, as1, as2);

  // create ip to announce (/32)
  char *announce_ip = malloc(256);
  snprintf(announce_ip, 256, "%s/%d", new_ip, 32);
  
  // announce it
  AnounceStaticRoute(announce_ip, peer->bgp);

  free (new_ip);
  free(announce_ip);
  // filter it because it is the first instance of a 1 hop pathlet
  return DBGP_FILTERED;
}

dbgp_filtered_status_t pathlets_output_filter(dbgp_control_info_t* control_info, struct attr* attr, struct peer* peer) {

  int peer_apart_of_island = IsRemoteAsAnIslandMember(general_configuration_, peer->as);
  int originated_in_island = IsRemoteAsAnIslandMember(general_configuration_, aspath_get_rightmost(attr->aspath));

  if(!peer_apart_of_island && originated_in_island)
    {
      zlog("pathlets::pathlets_output_filter: Peer %i not in island and aspath %s originated in island filtered", peer->as, attr->aspath->str);
      return DBGP_FILTERED;
    }
  dbgp_update_control_info(attr, peer);
  return DBGP_NOT_FILTERED;
}

/* int pathlets_info_cmp (struct bgp *bgp, struct bgp_info *new, struct bgp_info *exist, */
/*                        int *paths_eq){ */
/*   return 0; */
/* } */