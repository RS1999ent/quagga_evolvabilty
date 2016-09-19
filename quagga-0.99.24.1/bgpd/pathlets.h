#ifndef _BGPD_PATHLETS_H_
#define _BGPD_PATHLETS_H_

/* #include "bgpd/bgp_route.h" */
#include "bgpd/dbgp.h"

/* ********************* Global vars ************************** */

/* ********************* Private functions ********************* */

/* Handle a public facing prefix (a prefix that isn't in the /16 private
   subnet)

   Returns: 1 if this was a public facing prefix. There is special logic we want
   to do for these as opposed to the private prefixes
*/
int HandlePublicPrefix(dbgp_control_info_t* control_info,
                       struct peer* peer, struct attr* attr,
                       struct prefix* prefix);

/* ********************* Public functions ********************* */
/**
 * Updates pathlet-specific D-BGP control info
 *
 * @param control_info: D-BGP's extra control information
 * @param peer: Information about the neighbor that sent the adv.
 * @param attr: the attribute information (used for determining if this is an
 * internal advert0)
 */
void pathlets_update_control_info(dbgp_control_info_t *control_info,
                                  struct peer *peer, struct attr *attr,
                                  struct prefix *prefix);


/**
 * Updates pathlet-specific D-BGP control info without needing a peer struct
 *
 * @param control_info: D-BGP's extra control information
 * @param bgp: info about ourselfs
 * @param attr: the attribute information (used for determining if this is an
 * internal advert0)
 */
void pathlets_update_control_info_bgpstruct(dbgp_control_info_t* control_info,
                                  struct bgp* bgp, struct attr* attr,
                                  struct prefix* prefix);

/* Announces a static route prefix

   ip_and_prefix: the ip address in cidr notation to announce.
   bgp: the router bgp information
 */
void AnounceStaticRoute(char *ip_and_prefix, struct bgp *bgp);

/**
 * Applies pathlets-protocol-specific input filters
 *
 * @param control_info: D-BGP's extra control info
 * @param peer: Informatoin about the neighbor that sent the adv.
 */
dbgp_filtered_status_t pathlets_input_filter(dbgp_control_info_t *control_info,
                                             struct attr *attr,
                                             struct peer *peer, struct prefix* prefix);

/**
 * Applies pathlets-protocol-specific output filters
 *
 * @param control_info: D-BGP's extra control info
 * @param peer: Informatoin about the neighbor that sent the adv.
 */
dbgp_filtered_status_t pathlets_output_filter(
    /* dbgp_control_info_t *control_info, */ struct attr *attr,
    struct peer *peer, struct prefix *prefix);

/* /\** */
/*  * Applies pathlets's best-path selection algorithm */
/*  *\/ */
/* void pathlets_best_selection(struct bgp *bgp, struct bgp_node *rn,  */
/* 			  struct bgp_maxpaths_cfg *mpath_cfg,  */
/* 			  struct bgp_info_pair *result); */

/**
 * Applies pathlet's path selection code specifically */

int pathlets_info_cmp(struct bgp *bgp, struct bgp_info *new,
                      struct bgp_info *exist, int *paths_eq);
#endif
