/**
 * @file dbgp.c 
 * @brief Top-level file for out-of-band implementation
 * of D-BGP
 */

#include "bgpd/dbgp.h"
#include "bgpd/dbgp_lookup.h"
#include "bgpd/wiser.h"
#include "bgpd/bgp_common.h"
#include "bgpd/bgp_aspath.h"

/* ********************* Global vars ************************** */

/* ********************* Private functions ********************* */
/* Function that gets control information to be updated. If it already exists in
   the lookup service (as a key in the transitive attribute), then it will
   retrieve it. If it does not exist, this function creates a spot for the
   transitive attribute in 'attr' and returns a blank control info.

   Arguments:
      @param attr: A bgp attributes to get control information from or add a
      transit to it if there is no extra control information. May be mutated
      @param transit: Will be mutated to point to a transitive attribute in the
      attr (where a lookup key will be placed). This is a ptr to a ptr in order
      to change where the transitive attribute pointer points to.

   Return: A reference to a piece of bgp extra control information.
*/
dbgp_control_info_t* GetControlInformation(struct attr* attr, struct transit** transit) {

  assert(attr != NULL);
  dbgp_control_info_t *control_info;
  // If there is a transitive attribute in there, that means we can retrieve a
  // extra control information from the lookupservice.
  if(attr->extra != NULL && attr->extra->transit != NULL){
    zlog_debug("dbgp::GetControlInformation: There was existing control information in advert");
    struct attr_extra *extra;
    extra = attr->extra;
    *transit = extra->transit;
    control_info = retrieve_control_info(*transit);
    return control_info;
  }
  // Otherwise, there was no transitive attribute in there.  Therefore create a space for it.
  zlog_debug("dbgp::GetControlInformation: There was no existing control information, so create it");
  bgp_attr_extra_transit_get(attr, sizeof(dbgp_lookup_key_t));
  control_info = malloc(sizeof(dbgp_protocol_t));
  // Create serialized empty integrated_advertisement and set the appropriate
  // fields in the control information. This is effectively setting the size to
  // be 0 at the moment and the integrated_advertisement being NULL
  control_info->integrated_advertisement = NULL;
  control_info->integrated_advertisement_size = 0;

  /* control_info->sentinel = 0; */
  *transit = attr->extra->transit;
  assert(*transit != NULL);
  return control_info;

}

/* ********************* Public functions ********************* */


void dbgp_update_control_info(struct attr *attr, struct peer *peer) 
{
  dbgp_control_info_t *control_info;
  struct transit *transit;

  // Precondition asserts
  assert(peer != NULL);
  assert(attr != NULL);

  control_info = GetControlInformation(attr, &transit);
  assert(transit != NULL);

  if (is_lookup_service_path(transit)) {
    return;
  }

  zlog_debug("dbgp::dbgp_update_control_info: protocol type: %i", peer->bgp->dbgp_protocol);

  switch(peer->bgp->dbgp_protocol) 
    { 
      /* Just BGP */
    case dbgp_protocol_baseline: 
      return;
      break;
      /* Critical fixes */
    case dbgp_critical_wiser: 
      wiser_update_control_info(control_info, peer);
      break; 

      /* Replacement protocols */
    case dbgp_replacement_pathlets: 
      // pathlets_update_control_info(control_info, peer);
      assert(0);
      break;

    default:
      assert(0);
    }

  // Insert the updated control information into the lookup service and insert
  // the key into the the transitive attribute.
  dbgp_result_status_t success = set_control_info(transit, control_info);
  assert(success == DBGP_SUCCESS);
}

int dbgp_info_cmp(struct bgp *bgp, struct bgp_info *new, 
		   struct bgp_info *exist, int *path_eq)
{
  int retval = 0;

  // Debug statement
  zlog_debug("dbgp::dbgp_info_cmp: protocol type: %i", bgp->dbgp_protocol);

  //TODO: add back
  /* Always use BGP for paths connecting the lookup service */
  /* if (is_lookup_service_path(new->attr->extra->transit)) { */
  /*   return(bgp_info_cmp(bgp, new, exist, path_eq)); */
  /* } */

  switch(bgp->dbgp_protocol) {

  case dbgp_protocol_baseline: 
    return(bgp_info_cmp(bgp, new, exist, path_eq));
    break;

    /* critical fixes */
  case dbgp_critical_wiser: 
    return(wiser_info_cmp(bgp, new, exist, path_eq));
    break;
    /* repalcement protocols */
  case dbgp_replacement_pathlets:
    //pathlets_info_cmp(bgp, bgp_info, new, exists, path_eq);
    assert(0);
    break;

  default:
    assert(0);
  }

  return retval;
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

  /* Don't apply any protocol-specific filters to a lookup-service path */
  if (is_lookup_service_path(transit)) { 
    return DBGP_NOT_FILTERED;
  }
  
  control_info = retrieve_control_info(transit);

  switch (peer->bgp->dbgp_protocol) {
    { 
      /* Just BGP */
    case dbgp_protocol_baseline: 
      return DBGP_NOT_FILTERED;
      break;

      /* Critical fixes */
    case dbgp_critical_wiser: 
      return(wiser_input_filter(control_info, attr, peer));
      break; 

      /* Replacement protocols */
    case dbgp_replacement_pathlets: 
      //return(pathlets_output_fitler(control_info, attr, peer));
      assert(0);
      break;

    default:
      assert(0);
    }
  }

  return retval;
}

dbgp_filtered_status_t dbgp_output_filter(struct attr *attr, struct peer *peer) 
{
  dbgp_control_info_t *control_info;
  struct attr_extra *extra;
  struct transit *transit;
  int retval = DBGP_NOT_FILTERED;
  
  // debug what protocol the router thinks its running
  zlog_debug("dbgp::dbgp_output_filter: protocol type: %i", peer->bgp->dbgp_protocol);

  // If aspath is 0, then this the first thing going through, so there will be no extra attributes, return not filtered.
  unsigned int aspath_length = aspath_size(attr->aspath);
  zlog_debug("dbpg::dbpg_output_filter: aspath length: %i", aspath_length);
  zlog_debug("dbpg::dbpg_output_filter: aspath: %s", attr->aspath->str);
  /* zlog_debug("dbpg::dbpg_output_filter: aspath length: %i", attr->aspath->segments->length); */
  if (aspath_length == 0)
    {
      return retval;
    }


  assert(attr != NULL && peer != NULL 
	 && attr->extra != NULL 
         && attr->extra->transit != NULL && aspath_size(attr->aspath) > 0);

  extra = attr->extra;
  transit = extra->transit;
  
  /* Don't apply any protocol-specific filters to a lookup-service path */
  if (is_lookup_service_path(transit)) { 
    return DBGP_NOT_FILTERED;
  }
  
  control_info = retrieve_control_info(transit);

  switch (peer->bgp->dbgp_protocol) {
    { 
      /* Just BGP */
    case dbgp_protocol_baseline: 
      return DBGP_NOT_FILTERED;
      break;

      /* Critical fixes */
    case dbgp_critical_wiser: 
      return(wiser_output_filter(control_info, attr, peer));
      break; 

      /* Replacement protocols */
    case dbgp_replacement_pathlets: 
      //return(pathlets_output_filter(control_info, attr, peer));
      assert(0);
      break;

    default:
      assert(0);
    }
  }
  return retval;
} 
