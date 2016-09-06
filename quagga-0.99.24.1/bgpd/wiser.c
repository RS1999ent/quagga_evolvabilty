/**
 * @file wiser.c
 * @brief Implements the Wiser protocol for D-BGP
 */

#include "bgpd/dbgp.h"
#include "bgpd/dbgp_lookup.h"

/* ********************* Global vars ************************** */

/* ********************* Private functions ********************* */

/* ********************* Public functions ********************* */

void wiser_update_cotroL_info(dbgp_control_info_t *control_info, struct peer *peer)
{
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

