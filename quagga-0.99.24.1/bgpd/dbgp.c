/**
 * @file dbgp.c 
 * @brief Top-level file for out-of-band implementation
 * of D-BGP
 */

#include "bgpd/dbgp.h"
#include "bgpd/dbgp_lookup.h"
#include "bgpd/wiser.h"

/* ********************* Global vars ************************** */

/* ********************* Private functions ********************* */

/* ********************* Public functions ********************* */

void dbgp_update_control_info(struct attr *attr, struct peer *peer) 
{
  dbgp_control_info_t *control_info;
  struct attr_extra *extra;
  struct transit *transit;

  assert(attr != NULL && peer != NULL 
	 && attr->extra != NULL 
	 && attr->extra->transit != NULL);

  extra = attr->extra;
  transit = extra->transit;

  control_info = retrieve_control_info(transit);

  switch(peer->bgp->dbgp_protocol) 
    { 
      /* Critical fixes */
    case dbgp_critical_wiser: 
      wiser_update_control_info(control_info, peer);
      break; 

      /* Replacement protocols */
    case dbgp_replacement_pathlets: 
      // pathlets_update_control_info(control_info, peer);
      break;
    default:
      assert(0);
    }
}

dbgp_filtered_status_t dbgp_input_filter(struct attr *attr, struct peer *peer)
{
  dbgp_control_info_t *control_info;
  struct attr_extra *extra;
  struct transit *transit;
  int retval = DBGP_NOT_FILTERED;

  assert(attr != NULL && peer != NULL 
	 && attr->extra != NULL 
	 && attr->extra->transit != NULL);
  
  extra = attr->extra;
  transit = extra->transit;
  
  control_info = retrieve_control_info(transit);

  switch (peer->bgp->dbgp_protocol) {
    { 
      /* Critical fixes */
    case dbgp_critical_wiser: 
      //retval = wiser_input_filter(control_info, attr, peer);
      break; 

      /* Replacement protocols */
    case dbgp_replacement_pathlets: 
      // retval = pathlets_output_fitler(control_info, attr, peer);
      break;
    default:
      retval = DBGP_FILTERED;
      assert(0);
    }
  }
}

dbgp_filtered_status_t dbgp_output_filter(struct attr *attr, struct peer *peer) 
{
  dbgp_control_info_t *control_info;
  struct attr_extra *extra;
  struct transit *transit;
  int retval = DBGP_NOT_FILTERED;

  assert(attr != NULL && peer != NULL 
	 && attr->extra != NULL 
	 && attr->extra->transit != NULL);
  
  extra = attr->extra;
  transit = extra->transit;
  
  control_info = retrieve_control_info(transit);

  switch (peer->bgp->dbgp_protocol) {
    { 
      /* Critical fixes */
    case dbgp_critical_wiser: 
      //retval = wiser_output_filter(control_info, attr, peer);
      break; 

      /* Replacement protocols */
    case dbgp_replacement_pathlets: 
      // retval = pathlets_output_filter(control_info, attr, peer);
      break;
    default:
      retval = DBGP_FILTERED;
      assert(0);
    }
  }
} 
