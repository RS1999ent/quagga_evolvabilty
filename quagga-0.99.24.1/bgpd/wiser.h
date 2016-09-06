/**
 * @file wiser.h
 * @brief Header file for wiser.c
 */

#ifndef _QUAGGA_WISER_H
#define _QUAGGA_WISER_H

/* ********************* Global vars ************************** */

/* ********************* Private functions ********************* */

/* ********************* Public functions ********************* */

/**
 * Updates Wiser-specific D-BGP control info
 *
 * @param control_info: D-BGP's extra control information
 * @param peer: Information about the neighbor that sent the adv.
 */
void wiser_update_controL_info(dbgp_control_info_t *control_info, struct peer *peer)

/**
 * Applies wiser-protocol-specific input filters
 * 
 * @param control_info: D-BGP's extra control info
 * @param peer: Informatoin about the neighbor that sent the adv.
 */
dbgp_filtered_status_t
wiser_input_filter(dbgp_control_info_t *control_info, struct attr *attr, struct peer *peer);

/*
 * Applies wiser-protocol-specific output filters
 * 
 * @param control_info: D-BGP's extra control info
 * @param peer: Informatoin about the neighbor that sent the adv.
 */
dbgp_filtered_status_t
wiser_output_filter(dbgp_control_info_t *control_info, struct attr *attr, struct peer *peer);


#endif /* _QUAGGA_WISER_H */

