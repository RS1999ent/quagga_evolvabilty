/**
 * @file wiser.c
 * @brief Implements the Wiser protocol for D-BGP
 */

#include "bgpd/dbgp.h"
#include "bgpd/dbgp_lookup.h"
#include "bgpd/bgp_aspath.h"
#include "bgpd/wiser.h"
#include "bgpd/bgp_common.h"
#include "bgpd/bgp_route.h"
#include "bgpd/wiser_config_interface.h"
#include <arpa/inet.h>

/* ********************* Global vars ************************** */
extern WiserConfigHandle  wiser_config_;
char* SetWiserControlInfo(char* serialized_advert, int advert_size, int additive_path_cost, int* modified_advert_size,
                          float normalization);
int GetWiserPathCost(char* serialized_advert, int advert_size);
int GetLastWiserNode(char* serialized_advert, int advert_size);
char* SetLastWiserNode(char* serialized_advert, int advert_size, int new_last_node, int *return_advert_size);
int GetvirtualNeighbor(char* serialized_advert, int advert_size);
char* SerializedAdverToString(char* serialized_advert, int advert_size);

/* ********************* Private functions ********************* */

void AddLinkCostToIntegratedAdvertisement(int additive_link_cost, float normalization, dbgp_control_info_t* control_info) {
  char* old_integrated_advertisement = control_info->integrated_advertisement;
  int old_integrated_advertisement_size = control_info->integrated_advertisement_size;
  int new_size;
  char* new_integrated_advertisement_info = SetWiserControlInfo(old_integrated_advertisement,
                                                                old_integrated_advertisement_size,
                                                                additive_link_cost, &new_size, normalization);
  free(old_integrated_advertisement);
  control_info->integrated_advertisement = new_integrated_advertisement_info;
  control_info->integrated_advertisement_size = new_size;

}

void SetLastWiserNodeInIntegratedAdvertisement(int last_wiser_node, dbgp_control_info_t* control_info) {
  char* old_integrated_advertisement = control_info->integrated_advertisement;
  int old_integrated_advertisement_size = control_info->integrated_advertisement_size;
  int new_size;
  char* new_integrated_advertisement_info = SetLastWiserNode(old_integrated_advertisement,
                                                                old_integrated_advertisement_size,
                                                                last_wiser_node, &new_size);
  free(old_integrated_advertisement);
  control_info->integrated_advertisement = new_integrated_advertisement_info;
  control_info->integrated_advertisement_size = new_size;

}

float ComputeNormalizationForAdvert(dbgp_control_info_t *control_info, int local_as) {
  // get virtual neighbor
  int virtual_neighbor = GetLastWiserNode(control_info->integrated_advertisement, control_info->integrated_advertisement_size);
  // if it's -1, then there is no partner to compute normalization, return 1.
  if(virtual_neighbor == -1){
    return 1;
  }

  
  int cost_received_from_neighbor = RetrieveWiserCosts(local_as, virtual_neighbor);
  int cost_neighbor_received_from_me = RetrieveWiserCosts(virtual_neighbor, local_as);
  zlog_debug("wiser::ComputeNormalizationForAdvert: cost recieved from neighbor %i: %i", virtual_neighbor, cost_received_from_neighbor);
  zlog_debug("wiser::ComputeNormalizationForAdvert: cost neighbor %i received from me: %i",
             virtual_neighbor, cost_neighbor_received_from_me);

  // if either aren't in teh lookup service, return 1
  if(cost_neighbor_received_from_me == -1 || cost_received_from_neighbor == -1){
    return 1;
  }

  // return normalization
  // reverse TODO
  /* return (float) cost_received_from_neighbor / cost_neighbor_received_from_me; */
  float normalization = (float) cost_neighbor_received_from_me / cost_received_from_neighbor ;
  zlog_debug("wiser::ComputeNormalizationForAdvert: Normalization for advert is: %f", normalization);
  assert(normalization > 0);  // normalization should alwyas be greater than 0
  return normalization;
}

/* Given two extra attributes (this is where any extra control info is stored at
   where a lookup key would be) compute whether one is better than the other
   based on wiser infomation alone.

   Arguments:
      @param newattre: The extra attributes associated with a new path.
      @param existattre: The extra attributes associated with an existing path.

   @return 1 if newattre is better than existattre. 0 if existattre is better
   than newattre. -1 if neither and we need to move on in the decision process.
*/
int ComputeWiserDecision(const struct attr_extra* newattre, const struct attr_extra* existattre, int local_as)
{
  struct transit *newtransit, *existtransit;
  newtransit = newattre->transit;
  existtransit = existattre->transit;
  if(!has_dbgp_control_info(newtransit) || !has_dbgp_control_info(existtransit))
    {
      // debug
      zlog_debug("wiser::ComputeWiserDecision: neither control info had wiser extra info (i.e. a lookupkey)");
      return -1;
    }

  // Here if there is some control information in it. Get it. and get the path
  // cost from it.
  dbgp_control_info_t *new_control_info = retrieve_control_info(newtransit);
  dbgp_control_info_t *exist_control_info = retrieve_control_info(existtransit);

  int path_cost_from_new = GetWiserPathCost(new_control_info->integrated_advertisement,
                                            new_control_info->integrated_advertisement_size);
  int path_cost_from_exist = GetWiserPathCost(exist_control_info->integrated_advertisement,
                                             exist_control_info->integrated_advertisement_size);

  // normalized cost is: normalizaiton * cost in advert + our cost.
  /* int normalization_for_new = ComputeNormalizationForAdvert(new_control_info, local_as); */
  /* int normalization_for_exist = ComputeNormalizationForAdvert(exist_control_info, local_as); */

  /* float path_cost_from_exist = path_cost_from_exist * normalization_for_exist; */
  /* float path_cost_from_new = path_cost_from_new * normalization_for_new; */



  // Return 1 if new is better than exist, 0  if exist is better than old, -1 if they are equal.
  if (path_cost_from_new < path_cost_from_exist){
    zlog_debug("wiser::ComputeWiserDecision: New (%i) is better than exist (%i)", path_cost_from_new, path_cost_from_exist);
    return 1;
  }
  if (path_cost_from_new > path_cost_from_exist){
    zlog_debug("wiser::ComputeWiserDecision: New (%i) is worse than exist (%i)", path_cost_from_new, path_cost_from_exist);
    return 0;
  }
  // here if they are the same
  zlog_debug("wiser::ComputeWiserDecision: New (%i) is the same than exist (%i)", path_cost_from_new, path_cost_from_exist);
  return -1;
}

/* ********************* Public functions ********************* */

/*
Mutates 'control_info' to reflect its previous value incremented by the cost of
the link it came in on.

Arguments:
   @param control_info: The control info to be updated (TODO: Make into an IA).
   Will be mutated.
   @param peer: The peer that gave us this advertisement. Will use the remote_id
   and local_id in incrementing link costs
 */

void wiser_update_control_info(dbgp_control_info_t *control_info, struct peer *peer)
{
  // Convert peer->remote_id and peer->local_id to human readable ips.
  struct in_addr remote_id = peer->remote_id;
  struct in_addr local_id = peer->local_id;

  char string_remote_id[INET_ADDRSTRLEN] ;
  char string_local_id[INET_ADDRSTRLEN];
  memset(&string_remote_id, 0, INET_ADDRSTRLEN);
  memset(&string_local_id, 0, INET_ADDRSTRLEN);

  inet_ntop(AF_INET, &remote_id, string_remote_id, INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &local_id, string_local_id, INET_ADDRSTRLEN);

  // Get the link cost
  zlog_debug("wiser::wiser_update_control_info: Attempting to get linkcost between %s, %s", string_local_id, string_remote_id);
  /* zlog_debug("wiser::wiser_update_control_info: printing linkcosts:\n %s", LinkCostsToString(wiser_config_)); */
  int additive_link_cost = GetLinkCost(wiser_config_, string_local_id, string_remote_id);
  assert(additive_link_cost != -1); // should not be -1. If it is, something is wrong.
  zlog_debug("HERE 1");
  zlog_debug("advert_tostring: %s", SerializedAdverToString(control_info->integrated_advertisement, control_info->integrated_advertisement_size));
  // Get cost and last wiser node that touched advertisement
  int current_cost = GetWiserPathCost(control_info->integrated_advertisement, control_info->integrated_advertisement_size);
  zlog_debug("HERE 2");
  // if current_cost is -1, then there was no cost from a virtual neighbor.
  if(current_cost != -1)
    {
      zlog_debug("HERE 3");
      // here if we need to update the cost in the lookupservice.
      int last_wiser_node = GetLastWiserNode(control_info->integrated_advertisement, control_info->integrated_advertisement_size);
      zlog_debug("HERE 4");
      assert(last_wiser_node != -1);
      zlog_debug("HERE 5");
      // update the lookupservice
      IncrementWiserCosts(peer->bgp->as, last_wiser_node, current_cost);
      zlog_debug("HERE 6");
      zlog_debug("wiser::wiser_update_control_info: Advertisement has existing cost in it for key %i-%i. Incrementing lookup service by cost %i", peer->bgp->as, last_wiser_node, current_cost);
    }
  else{
    // set to -1 so we know whether there was wiser info originally in advert.
    // This is for wiser_info_cmp
    zlog_debug("wiser::wiser_update_control_info: There was no virtual wiser neighbor" );
  }

  // compute normalization before overriding setlasstwisernode in order to get
  // the costs arriving at both ends.
  zlog_debug("HERE 7");
  float normalization = ComputeNormalizationForAdvert(control_info, peer->bgp->as);

  //set us as the last wiser node
  zlog_debug("HERE 8");
  SetLastWiserNodeInIntegratedAdvertisement(peer->bgp->as, control_info);
  zlog_debug("HERE 9");

  //mutate the control_info
  zlog_debug("HERE 10");
  AddLinkCostToIntegratedAdvertisement(additive_link_cost, normalization, control_info);
  int normalized_cost = GetWiserPathCost(control_info->integrated_advertisement, control_info->integrated_advertisement_size) - additive_link_cost;
  zlog_debug("wiser::wiser_update_control_info: updated control info -  %f (normalization) * %i (old cost) + %i (additive) = %i (truncated))", normalization, current_cost, additive_link_cost, normalized_cost + additive_link_cost);
  zlog_debug("HERE 11");

  //Debug info
  /* zlog_debug("wiser::wiser_update_control_info: Link %s, %s with cost %i added to entering control info with cost %lld giving total %lld", string_local_id, string_remote_id, additive_link_cost, old_control_info_value , control_info->sentinel); */

  return;
}

dbgp_filtered_status_t 
wiser_input_filter(dbgp_control_info_t *control_info, struct attr *attr, struct peer *peer)
{
  return DBGP_NOT_FILTERED;
}

dbgp_filtered_status_t 
wiser_output_filter(dbgp_control_info_t *control_info, struct attr *attr, struct peer *peer)
{
  return DBGP_NOT_FILTERED;
}

void wiser_best_selection(struct bgp *bgp, struct bgp_node *rn, 
			  struct bgp_maxpaths_cfg *mpath_cfg, 
			  struct bgp_info_pair *result)
{
  return;
}

int wiser_info_cmp (struct bgp *bgp, struct bgp_info *new, struct bgp_info *exist,
		    int *paths_eq)
{
  struct attr *newattr, *existattr;
  struct attr_extra *newattre, *existattre;
  bgp_peer_sort_t new_sort;
  bgp_peer_sort_t exist_sort;
  u_int32_t new_pref;
  u_int32_t exist_pref;
  u_int32_t new_med;
  u_int32_t exist_med;
  u_int32_t new_weight;
  u_int32_t exist_weight;
  uint32_t newm, existm;
  struct in_addr new_id;
  struct in_addr exist_id;
  int new_cluster;
  int exist_cluster;
  int internal_as_route;
  int confed_as_route;
  int ret;

  *paths_eq = 0;

  /* 0. Null check. */
  if (new == NULL)
    return 0;
  if (exist == NULL)
    return 1;

  // Debug statement: make sure we enter this function
  zlog_debug("wiser::wiser_info_cmp:: entered function and passed null check");

  newattr = new->attr;
  existattr = exist->attr;
  newattre = newattr->extra;
  existattre = existattr->extra;

  /* 1. Weight check. */
  new_weight = exist_weight = 0;

  if (newattre)
    new_weight = newattre->weight;
  if (existattre)
    exist_weight = existattre->weight;

  if (new_weight > exist_weight)
    return 1;
  if (new_weight < exist_weight)
    return 0;

  /* 2. Local preference check. */
  new_pref = exist_pref = bgp->default_local_pref;

  if (newattr->flag & ATTR_FLAG_BIT (BGP_ATTR_LOCAL_PREF))
    new_pref = newattr->local_pref;
  if (existattr->flag & ATTR_FLAG_BIT (BGP_ATTR_LOCAL_PREF))
    exist_pref = existattr->local_pref;

  if (new_pref > exist_pref)
    return 1;
  if (new_pref < exist_pref)
    return 0;

  /* 3. Wiser computation. Will prefer the route with the lower cost. Both must
     have some cost in the advert otherwise the default decision process will
     take place */
  int wiser_decision = ComputeWiserDecision(newattre, existattre, bgp->as);
  // If it is not -1, then that means we have a decision. Return it. Otherwise
  // use other decisions.
  if(wiser_decision != -1){
    return wiser_decision;
  }

  /* 3. Local route check. We prefer:
   *  - BGP_ROUTE_STATIC
   *  - BGP_ROUTE_AGGREGATE
   *  - BGP_ROUTE_REDISTRIBUTE
   */
  /* if (! (new->sub_type == BGP_ROUTE_NORMAL)) */
  /*    return 1; */
  /* if (! (exist->sub_type == BGP_ROUTE_NORMAL)) */
  /*    return 0; */

  /* 4. AS path length check. */
  if (! bgp_flag_check (bgp, BGP_FLAG_ASPATH_IGNORE))
    {
      int exist_hops = aspath_count_hops (existattr->aspath);
      int exist_confeds = aspath_count_confeds (existattr->aspath);
      
      if (bgp_flag_check (bgp, BGP_FLAG_ASPATH_CONFED))
	{
	  int aspath_hops;
	  
	  aspath_hops = aspath_count_hops (newattr->aspath);
          aspath_hops += aspath_count_confeds (newattr->aspath);
          
	  if ( aspath_hops < (exist_hops + exist_confeds))
	    return 1;
	  if ( aspath_hops > (exist_hops + exist_confeds))
	    return 0;
	}
      else
	{
	  int newhops = aspath_count_hops (newattr->aspath);
	  
	  if (newhops < exist_hops)
	    return 1;
          if (newhops > exist_hops)
	    return 0;
	}
    }

  /* 5. Origin check. */
  if (newattr->origin < existattr->origin)
    return 1;
  if (newattr->origin > existattr->origin)
    return 0;

  /* 6. MED check. */
  internal_as_route = (aspath_count_hops (newattr->aspath) == 0
		      && aspath_count_hops (existattr->aspath) == 0);
  confed_as_route = (aspath_count_confeds (newattr->aspath) > 0
		    && aspath_count_confeds (existattr->aspath) > 0
		    && aspath_count_hops (newattr->aspath) == 0
		    && aspath_count_hops (existattr->aspath) == 0);
  
  if (bgp_flag_check (bgp, BGP_FLAG_ALWAYS_COMPARE_MED)
      || (bgp_flag_check (bgp, BGP_FLAG_MED_CONFED)
	 && confed_as_route)
      || aspath_cmp_left (newattr->aspath, existattr->aspath)
      || aspath_cmp_left_confed (newattr->aspath, existattr->aspath)
      || internal_as_route)
    {
      new_med = bgp_med_value (new->attr, bgp);
      exist_med = bgp_med_value (exist->attr, bgp);

      if (new_med < exist_med)
	return 1;
      if (new_med > exist_med)
	return 0;
    }

  /* 7. Peer type check. */
  new_sort = new->peer->sort;
  exist_sort = exist->peer->sort;

  if (new_sort == BGP_PEER_EBGP
      && (exist_sort == BGP_PEER_IBGP || exist_sort == BGP_PEER_CONFED))
    return 1;
  if (exist_sort == BGP_PEER_EBGP
      && (new_sort == BGP_PEER_IBGP || new_sort == BGP_PEER_CONFED))
    return 0;

  /* 8. IGP metric check. */
  newm = existm = 0;

  if (new->extra)
    newm = new->extra->igpmetric;
  if (exist->extra)
    existm = exist->extra->igpmetric;

  if (newm < existm)
    ret = 1;
  if (newm > existm)
    ret = 0;

  /* 9. Maximum path check. */
  if (newm == existm)
    {
      if (bgp_flag_check(bgp, BGP_FLAG_ASPATH_MULTIPATH_RELAX))
        {

	  /*
	   * For the two paths, all comparison steps till IGP metric
	   * have succeeded - including AS_PATH hop count. Since 'bgp
	   * bestpath as-path multipath-relax' knob is on, we don't need
	   * an exact match of AS_PATH. Thus, mark the paths are equal.
	   * That will trigger both these paths to get into the multipath
	   * array.
	   */
	  *paths_eq = 1;
        }
      else if (new->peer->sort == BGP_PEER_IBGP)
	{
	  if (aspath_cmp (new->attr->aspath, exist->attr->aspath))
	    *paths_eq = 1;
	}
      else if (new->peer->as == exist->peer->as)
	*paths_eq = 1;
    }
  else
    {
      /*
       * TODO: If unequal cost ibgp multipath is enabled we can
       * mark the paths as equal here instead of returning
       */
      return ret;
    }

  /* 10. If both paths are external, prefer the path that was received
     first (the oldest one).  This step minimizes route-flap, since a
     newer path won't displace an older one, even if it was the
     preferred route based on the additional decision criteria below.  */
  if (! bgp_flag_check (bgp, BGP_FLAG_COMPARE_ROUTER_ID)
      && new_sort == BGP_PEER_EBGP
      && exist_sort == BGP_PEER_EBGP)
    {
      if (CHECK_FLAG (new->flags, BGP_INFO_SELECTED))
	return 1;
      if (CHECK_FLAG (exist->flags, BGP_INFO_SELECTED))
	return 0;
    }

  /* 11. Rourter-ID comparision. */
  if (newattr->flag & ATTR_FLAG_BIT(BGP_ATTR_ORIGINATOR_ID))
    new_id.s_addr = newattre->originator_id.s_addr;
  else
    new_id.s_addr = new->peer->remote_id.s_addr;
  if (existattr->flag & ATTR_FLAG_BIT(BGP_ATTR_ORIGINATOR_ID))
    exist_id.s_addr = existattre->originator_id.s_addr;
  else
    exist_id.s_addr = exist->peer->remote_id.s_addr;

  if (ntohl (new_id.s_addr) < ntohl (exist_id.s_addr))
    return 1;
  if (ntohl (new_id.s_addr) > ntohl (exist_id.s_addr))
    return 0;

  /* 12. Cluster length comparision. */
  new_cluster = exist_cluster = 0;

  if (newattr->flag & ATTR_FLAG_BIT(BGP_ATTR_CLUSTER_LIST))
    new_cluster = newattre->cluster->length;
  if (existattr->flag & ATTR_FLAG_BIT(BGP_ATTR_CLUSTER_LIST))
    exist_cluster = existattre->cluster->length;

  if (new_cluster < exist_cluster)
    return 1;
  if (new_cluster > exist_cluster)
    return 0;

  /* 13. Neighbor address comparision. */
  ret = sockunion_cmp (new->peer->su_remote, exist->peer->su_remote);

  if (ret == 1)
    return 0;
  if (ret == -1)
    return 1;

  return 1;
}
  
			  
     
