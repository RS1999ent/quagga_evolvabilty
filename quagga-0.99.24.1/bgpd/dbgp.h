/**
 * @file dbgp.h
 * @brief Contains high-level constants and structs for D-BGP
 */

#ifndef _QUAGGA_DBGP_H
#define _QUAGGA_DBGP_H 

#include "bgpd/dbgp_lookup.h"
#include "bgpd/bgp_common.h"

typedef enum dbgp_filtered_status_s
  {
    DBGP_NOT_FILTERED = 0,
    DBGP_FILTERED = 1
  } dbgp_filtered_status_t;


/**
 * Calls' appropriate functions best-path selecton routine
 *
 * @param bgp: Information about this router
 * @param new: A new path choice
 * @param exist: The current path choice
 * @param paths_eq: 1 if the paths are equal
 *
 * @return 1 is hte ?new? path is preferable? 0 otherwise?
 */
int dbgp_info_cmp(struct bgp *bgp, struct bgp_info *new, 
		   struct bgp_info *exist, int *path_eq);


/** 
 * Call protocol-specfic code to update control information for a
 * received advertisement, before the advertisement is used for
 * best-path selection.  
 *
 * @param attr: The attributes associated with this adv
 * @param peer: Informatoin about the neighbor that sent the adv.
 */
void dbgp_update_control_info(struct attr *attr, struct peer *peer);

/**
 * Calls protocol-specific code to see if the incoming advertisement
 * should be filtered before best-path selection.
 *
 * @param attr: Attributes associated with teh advertisement
 * @param peer: The peer that sent this advertisement
 *
 * @return Whehter or not the adv should be filtered 
 */
dbgp_filtered_status_t dbgp_input_filter(struct attr *attr, struct peer *peer);

/**
 * Calls protocol-specific code to see if hte outgoing advertisement
 * should be filtered.
 * @param attr: Attributes associated withthe advertisement
 * @param peer: The peer that sent this advertisement
 *
 * @return Whether or not the advertisement should be filtered
 */
dbgp_filtered_status_t dbgp_output_filter(struct attr *attr, struct peer *peer, struct prefix* prefix);

#endif /* _QUAGGA_DBGP_H */
