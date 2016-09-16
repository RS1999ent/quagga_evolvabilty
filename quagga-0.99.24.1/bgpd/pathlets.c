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
extern int vtysh_execute(const char *line);
/* extern struct vty* vty; */
int HasPathletInformation(char* serialized_advert, int advert_size, int island_id);

void AnounceStaticRoute(char* ip_and_prefix, struct bgp* bgp){
  struct bgp_static *bgp_static;
  struct prefix p;
  str2prefix (ip_and_prefix, &p);
  bgp_static = XCALLOC (MTYPE_BGP_STATIC, sizeof (struct bgp_static));
  bgp_static->backdoor = 0;
  bgp_static->valid = 0;
  bgp_static->igpmetric = 0;
  bgp_static->igpnexthop.s_addr = 0;

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

void pathlets_update_control_info(dbgp_control_info_t* control_info, struct peer* peer) {
  
}

dbgp_filtered_status_t pathlets_input_filter(dbgp_control_info_t* control_info, struct attr* attr, struct peer* peer) {

  // if an incoming advertisement has control info
  int has_pathlet_info = HasPathletInformation(control_info->integrated_advertisement, control_info->integrated_advertisement_size, peer->bgp->island_id);

  // if there is pathlet info in it, we don't want to do any filtering, let it
  // pass onto the pathlets_update_control_info
  if(has_pathlet_info == 1){
    zlog_debug("pathlets:pathlets_input_filter: Advert has no control info. aspath: %s", attr->aspath->str);
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
  
  // Create the vty command to announce a new prefix
  /* char* command = malloc(256); */
  /* memset(command, 0, 256); */

  /* snprintf(command, 256, "conf t -c \"router bgp %i\" -c \"network %s/%d\"", peer->bgp->as, new_ip, 32); */
  /* vtysh_execute(command); */
  /* vty_out(vty, command); */

  AnounceStaticRoute(new_ip, peer->bgp);

  /* bgp_static_set (vty, vty->index, new_ip, AFI_IP, SAFI_UNICAST, */
                         /* NULL, 1); */
  return DBGP_NOT_FILTERED;
}

dbgp_filtered_status_t pathlets_output_filter(dbgp_control_info_t* control_info, struct attr* attr, struct peer* peer) {

  return DBGP_NOT_FILTERED;
}

/* int pathlets_info_cmp (struct bgp *bgp, struct bgp_info *new, struct bgp_info *exist, */
/*                        int *paths_eq){ */
/*   return 0; */
/* } */
