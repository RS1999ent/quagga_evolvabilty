/**
 * @file dbgp.c
 * @brief Top-level file for out-of-band implementation
 * of D-BGP
 */

#include "bgpd/dbgp.h"
#include "bgpd/bgp_debug.h"
#include "bgpd/benchmark.h"
#include "bgpd/bgp_aspath.h"
#include "bgpd/bgp_common.h"
#include "bgpd/dbgp_lookup.h"
#include "bgpd/pathlets.h"
#include "bgpd/wiser.h"
#include "bgpd/wiser_config_interface.h"
#include "bgpd/bgp_benchmark_structs.h"

/* ********************* Global vars ************************** */
extern GeneralConfigurationHandle general_configuration_;
extern BgpBenchmarkExperimentDataPtr bgp_benchmark_experiment_data_;
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
dbgp_control_info_t *GetControlInformation(struct attr *attr,
                                           struct transit **transit) {
  assert(attr != NULL);
  // special case for bgp benchmark experiment
  if(GetProtocolType(general_configuration_) == dbgp_protocol_benchmark)
    {
      // if it is an in memory experiment, then return the control information
      // in the struct, set transit to the transit (should be unused, but there
      // is an assert that requires this to be non null)
      //otherwise, retrieve the information from the lookup service (should be
      //set already)
      if(bgp_benchmark_experiment_data_->which_adhoc == IN_MEMORY)
        {
          *transit = bgp_benchmark_experiment_data_->transit;
          return bgp_benchmark_experiment_data_->control_info;
        }
      else if(bgp_benchmark_experiment_data_->which_adhoc == IN_LOOKUPSERVICE)
        {
          *transit = bgp_benchmark_experiment_data_->transit;
          return retrieve_control_info(*transit);
        }
    }
  // this was the original function!
  else {
    dbgp_control_info_t *control_info;
    // If there is a transitive attribute in there, that means we can retrieve a
    // extra control information from the lookupservice.
    if (attr->extra != NULL && attr->extra->transit != NULL &&
        attr->extra->transit->length > 0) {
      if (BGP_DEBUG (update, UPDATE_IN))  
        zlog_debug(
                   "dbgp::GetControlInformation: There was existing control information "
                   "in advert");
      struct attr_extra *extra;
      extra = attr->extra;
      *transit = extra->transit;

      if (BGP_DEBUG (update, UPDATE_IN))  
        zlog_debug("dbgp::GetControlInformation: Transit length is: %i",
                   (*transit)->length);
      control_info = retrieve_control_info(*transit);
      return control_info;
    }
    // Otherwise, there was no transitive attribute in there.  Therefore create a
    // space for it.
    if (BGP_DEBUG (update, UPDATE_IN))  
      zlog_debug(
                 "dbgp::GetControlInformation: There was no existing control information, "
                 "so create it");
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
}

/* ********************* Public functions ********************* */

void dbgp_update_control_info(struct attr *attr, struct peer *peer,
                              struct prefix *prefix) {
  if (BGP_DEBUG (update, UPDATE_IN))  
    zlog_debug("dbgp::dbgp_update_control_info: aspath of attr: %s",
             attr->aspath->str);
  dbgp_control_info_t *control_info;
  struct transit *transit;

  // Precondition asserts
  assert(peer != NULL);
  assert(attr != NULL);
  if (peer->bgp->dbgp_protocol == dbgp_protocol_baseline_sleeper ||
      peer->bgp->dbgp_protocol == dbgp_protocol_baseline ||
      peer->bgp->dbgp_protocol == dbgp_protocol_benchmark) {
    return;
  }

  control_info = GetControlInformation(attr, &transit);
  assert(transit != NULL);

  if (is_lookup_service_path(transit)) {
    return;
  }

  if (BGP_DEBUG (update, UPDATE_IN))  
    zlog_debug("dbgp::dbgp_update_control_info: protocol type: %i",
               peer->bgp->dbgp_protocol);

  switch (peer->bgp->dbgp_protocol) {
    /* Just BGP */
    case dbgp_protocol_baseline:
      return;
      break;
    case dbgp_protocol_baseline_sleeper:
      return;
      break;
    /* Critical fixes */
    case dbgp_critical_wiser:
      wiser_update_control_info(control_info, peer);
      break;
    /* case dbgp_protocol_benchmark: */
    /*   benchmark_update_control_info(control_info); */
    /*   break; */
    /* Replacement protocols */
    case dbgp_replacement_pathlets:
      new_pathlets_update_control_info(control_info, peer, attr, prefix);
      break;

    default:
      assert(0);
  }

  // Insert the updated control information into the lookup service and insert
  // the key into the the transitive attribute.
  dbgp_result_status_t success = set_control_info(transit, control_info);
  assert(success == DBGP_SUCCESS);
}

void dbgp_update_control_info_bgpstruct(struct attr *attr, struct bgp *bgp,
                                        struct prefix *prefix) {
  if (BGP_DEBUG (update, UPDATE_IN))  
    zlog_debug("dbgp::dbgp_update_control_info: aspath of attr: %s",
               attr->aspath->str);
  dbgp_control_info_t *control_info;
  struct transit *transit;

  // Precondition asserts
  assert(bgp != NULL);
  assert(attr != NULL);
  if (bgp->dbgp_protocol == dbgp_protocol_baseline_sleeper ||
      bgp->dbgp_protocol == dbgp_protocol_baseline) {
    return;
  }

  control_info = GetControlInformation(attr, &transit);
  assert(transit != NULL);

  if (is_lookup_service_path(transit)) {
    return;
  }

  if (BGP_DEBUG (update, UPDATE_IN))  
    zlog_debug("dbgp::dbgp_update_control_info: protocol type: %i",
               bgp->dbgp_protocol);
  
  switch (bgp->dbgp_protocol) {
    /* Just BGP */
    case dbgp_protocol_baseline:
      return;
      break;
    /* Replacement protocols */
    case dbgp_replacement_pathlets:
      new_pathlets_update_control_info_bgpstruct(control_info, bgp, attr,
                                                 prefix);
      break;

    default:
      assert(0);
  }

  // Insert the updated control information into the lookup service and insert
  // the key into the the transitive attribute.
  dbgp_result_status_t success = set_control_info(transit, control_info);
  assert(success == DBGP_SUCCESS);
}

int dbgp_info_cmp(struct bgp *bgp, struct bgp_info *new, struct bgp_info *exist,
                  int *path_eq) {
  int retval = 0;

  // Debug statement
  /* zlog_debug("dbgp::dbgp_info_cmp: protocol type: %i", bgp->dbgp_protocol);
   */

  // TODO: add back
  /* Always use BGP for paths connecting the lookup service */
  /* if (is_lookup_service_path(new->attr->extra->transit)) { */
  /*   return(bgp_info_cmp(bgp, new, exist, path_eq)); */
  /* } */

  switch (bgp->dbgp_protocol) {
    case dbgp_protocol_baseline:
      return (bgp_info_cmp(bgp, new, exist, path_eq));
      break;
    case dbgp_protocol_baseline_sleeper:
      return (bgp_info_cmp(bgp, new, exist, path_eq));
      break;
    case dbgp_protocol_benchmark:
      return (bgp_info_cmp(bgp, new, exist, path_eq));
      break;
    /* critical fixes */
    case dbgp_critical_wiser:
      return (wiser_info_cmp(bgp, new, exist, path_eq));
      break;
    /* repalcement protocols */
    case dbgp_replacement_pathlets:
      return (bgp_info_cmp(bgp, new, exist, path_eq));
      // pathlets_info_cmp(bgp, bgp_info, new, exists, path_eq);
      assert(0);
      break;

    default:
      assert(0);
  }

  return retval;
}

dbgp_filtered_status_t dbgp_input_filter(struct attr *attr, struct peer *peer,
                                         struct prefix *prefix) {

  if (BGP_DEBUG (update, UPDATE_IN))  
    zlog_debug("dbgp::dbgp_input_filter: aspath of attr: %s", attr->aspath->str);
  dbgp_control_info_t *control_info;
  struct attr_extra *extra;
  struct transit *transit;
  int retval = DBGP_NOT_FILTERED;
  // Precondition asserts
  assert(peer != NULL);
  assert(attr != NULL);
  if (peer->bgp->dbgp_protocol == dbgp_protocol_baseline_sleeper ||
      peer->bgp->dbgp_protocol == dbgp_protocol_baseline) {
    return DBGP_NOT_FILTERED;
  }

  control_info = GetControlInformation(attr, &transit);
  assert(transit != NULL);

  /* Don't apply any protocol-specific filters to a lookup-service path */
  if (is_lookup_service_path(transit)) {
    return DBGP_NOT_FILTERED;
  }

  switch (peer->bgp->dbgp_protocol) {
    {
      /* Just BGP */
      case dbgp_protocol_baseline:
        return DBGP_NOT_FILTERED;
        break;
      case dbgp_protocol_baseline_sleeper:
        return DBGP_NOT_FILTERED;
        break;

      case dbgp_protocol_benchmark:
        return benchmark_input_filter(control_info);
      /* Critical fixes */
      case dbgp_critical_wiser:
        return (wiser_input_filter(control_info, attr, peer));
        break;

      /* Replacement protocols */
      case dbgp_replacement_pathlets:
        return (new_pathlets_input_filter(control_info, attr, peer, prefix));
        break;

      default:
        assert(0);
    }
  }

  return retval;
}

dbgp_filtered_status_t dbgp_output_filter(struct attr *attr, struct peer *peer,
                                          struct prefix *prefix) {
  if (BGP_DEBUG (update, UPDATE_IN))  
    zlog_debug("dbgp::dbgp_output_filter: aspath of attr: %s", attr->aspath->str);
  dbgp_control_info_t *control_info;
  struct attr_extra *extra;
  struct transit *transit;
  int retval = DBGP_NOT_FILTERED;

  // debug what protocol the router thinks its running
  char *prefix_buf = malloc(256);
  prefix2str(prefix, prefix_buf, 256);
  if (BGP_DEBUG (update, UPDATE_IN))  
    zlog_debug("dbgp::dbgp_output_filter: protocol type: %i",
               peer->bgp->dbgp_protocol);
  if (BGP_DEBUG (update, UPDATE_IN))  
    zlog_debug("dbgp::dbgp_output_filter: prefix: %s", prefix_buf);
  free(prefix_buf);
  
  // If aspath is 0, then this the first thing going through, so there will be
  // no extra attributes, return not filtered.
  /* unsigned int aspath_length = aspath_size(attr->aspath); */
  /* zlog_debug("dbpg::dbpg_output_filter: aspath length: %i", aspath_length);
   */
  /* if (aspath_length == 0) */
  /*   { */
  /*     return retval; */
  /*   } */

  /* assert(attr != NULL && peer != NULL  */
  /*  && attr->extra != NULL  */
  /*        && attr->extra->transit != NULL && aspath_size(attr->aspath) > 0);
   */

  /* extra = attr->extra; */
  /* transit = extra->transit; */

  /* Don't apply any protocol-specific filters to a lookup-service path */
  /* if (is_lookup_service_path(transit)) {  */
  /*   return DBGP_NOT_FILTERED; */
  /* } */

  /* control_info = retrieve_control_info(transit); */

  // not sure why this was here 12-22-16. commenting out
  /* if(peer->bgp->as == 103) */
  /*   { */
  /*     return DBGP_FILTERED; */
  /*   } */

  switch (peer->bgp->dbgp_protocol) {
    {
      /* Just BGP */
      case dbgp_protocol_baseline:
        return DBGP_NOT_FILTERED;
        break;
      case dbgp_protocol_baseline_sleeper:
        return DBGP_NOT_FILTERED;
        break;

      /* Critical fixes */
      case dbgp_critical_wiser:
        return (wiser_output_filter(/* control_info, */ attr, peer));
        break;

      /* Replacement protocols */
      case dbgp_replacement_pathlets:
        return (pathlets_output_filter(/* control_info, */ attr, peer, prefix));
        break;
      case dbgp_protocol_benchmark:
        return benchmark_output_filter(attr);
      default:
        assert(0);
    }
  }
  return retval;
}
