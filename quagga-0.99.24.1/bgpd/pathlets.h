#ifndef _BGPD_PATHLETS_H_
#define _BGPD_PATHLETS_H_

/* #include "bgpd/bgp_route.h" */
#include "bgpd/dbgp.h"

/* ********************* Global vars ************************** */

/* ********************* Private functions ********************* */

/* ********************* Public functions ********************* */
/**
 * Updates pathlet-specific D-BGP control info
 *
 * @param control_info: D-BGP's extra control information
 * @param peer: Information about the neighbor that sent the adv.
 */
void pathlets_update_control_info(dbgp_control_info_t *control_info, struct peer *peer);

/**
 * Applies pathlets-protocol-specific input filters
 * 
 * @param control_info: D-BGP's extra control info
 * @param peer: Informatoin about the neighbor that sent the adv.
 */
dbgp_filtered_status_t
pathlets_input_filter(dbgp_control_info_t *control_info, struct attr *attr, struct peer *peer);

/**
 * Applies pathlets-protocol-specific output filters
 *
 * @param control_info: D-BGP's extra control info
 * @param peer: Informatoin about the neighbor that sent the adv.
 */
dbgp_filtered_status_t
pathlets_output_filter(dbgp_control_info_t *control_info, struct attr *attr, struct peer *peer);

/* /\** */
/*  * Applies pathlets's best-path selection algorithm */
/*  *\/ */
/* void pathlets_best_selection(struct bgp *bgp, struct bgp_node *rn,  */
/* 			  struct bgp_maxpaths_cfg *mpath_cfg,  */
/* 			  struct bgp_info_pair *result); */

/**
 * Applies pathlet's path selection code specifically */

int pathlets_info_cmp (struct bgp *bgp, struct bgp_info *new, struct bgp_info *exist,
		    int *paths_eq);
#endif
