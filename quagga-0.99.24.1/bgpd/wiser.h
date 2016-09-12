/**
 * @file wiser.h
 * @brief Header file for wiser.c
 */

#ifndef _QUAGGA_WISER_H
#define _QUAGGA_WISER_H

#include "bgpd/bgp_route.h"



/* ********************* Global vars ************************** */

/* ********************* Private functions ********************* */

/* Called from update_wiser_control_info.  Adds a new link cost to the integrated advertisement and frees the memory of the old one

   Arguments:
   additive_link_cost: The linkcost to add to the possibly existing link cost
   in control_info->integrated_advertisement
   control_info: The dbgp_control_info that contains the control information we are interested in.
   normalization: the normalization factor to scale existing cost by.

*/
void AddLinkCostToIntegratedAdvertisement(int additive_link_cost, float normalization, dbgp_control_info_t* control_info);

/* ********************* Public functions ********************* */

/**
 * Updates Wiser-specific D-BGP control info
 *
 * @param control_info: D-BGP's extra control information
 * @param peer: Information about the neighbor that sent the adv.
 */
void wiser_update_control_info(dbgp_control_info_t *control_info, struct peer *peer);

/**
 * Applies wiser-protocol-specific input filters
 * 
 * @param control_info: D-BGP's extra control info
 * @param peer: Informatoin about the neighbor that sent the adv.
 */
dbgp_filtered_status_t
wiser_input_filter(dbgp_control_info_t *control_info, struct attr *attr, struct peer *peer);

/**
 * Applies wiser-protocol-specific output filters
 * 
 * @param control_info: D-BGP's extra control info
 * @param peer: Informatoin about the neighbor that sent the adv.
 */
dbgp_filtered_status_t
wiser_output_filter(dbgp_control_info_t *control_info, struct attr *attr, struct peer *peer);

/**
 * Applies wiser's best-path selection algorithm
 */
void wiser_best_selection(struct bgp *bgp, struct bgp_node *rn, 
			  struct bgp_maxpaths_cfg *mpath_cfg, 
			  struct bgp_info_pair *result);

/**
 * Applies Wiser's path selection code specifically */

int wiser_info_cmp (struct bgp *bgp, struct bgp_info *new, struct bgp_info *exist,
		    int *paths_eq);

#endif /* _QUAGGA_WISER_H */

