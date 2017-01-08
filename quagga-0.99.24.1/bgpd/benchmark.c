

#include "bgpd/benchmark.h"
#include "bgpd/bgp_aspath.h"
#include "bgpd/bgp_debug.h"
#include "bgpd/bgp_common.h"
#include "bgpd/bgp_route.h"
#include "bgpd/dbgp.h"
#include "bgpd/dbgp_lookup.h"
#include "bgpd/wiser_config_interface.h"
#include "bgpd/bgp_benchmark_structs.h"

/* ********************* Global vars ************************** */
extern GeneralConfigurationHandle general_configuration_;
extern BgpBenchmarkStatsPtr bgp_benchmark_stats;
char* SetBenchmarkIABytes(char* serialized_advert, int advert_size,
                          int num_bytes_to_set, int* modified_advert_size);
int GetBenchmarkIABytesSize(char* serialized_advert, int advert_size);
char* SerializedAdverToString(char* serialized_advert, int advert_size);


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
dbgp_control_info_t *GetControlInformation2(struct attr *attr,
                                           struct transit **transit) {
  assert(attr != NULL);
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


/* ********************* Public functions ********************* */


dbgp_filtered_status_t benchmark_input_filter(
    dbgp_control_info_t* control_info) {
  // DBGP BENCHMARK Deserialization step, start timer for deserialziation
  // latency. End processing timer for deserialziation
  if(bgp_benchmark_stats != NULL) {
    /* clock_gettime(CLOCK_REALTIME, &bgp_benchmark_stats->deserialization_latency.bgp_deserialization_beagle_timer.start_time); */
    StartContiguousStatsDuration(&bgp_benchmark_stats->deserialization_latency.bgp_deserialization_beagle_stats);
    UpdateContiguousStatsDuration(&bgp_benchmark_stats->processing_latency.bgp_nlri_parse_stats);
  }

  // see if there are bytes in there already
  int num_bytes =
      GetBenchmarkIABytesSize(control_info->integrated_advertisement,
                              control_info->integrated_advertisement_size);
  // end deserialization step update deserialization_latency.current_duration
  if(bgp_benchmark_stats != NULL) {

    UpdateContiguousStatsDuration(&bgp_benchmark_stats->deserialization_latency.bgp_deserialization_beagle_stats);
    EndContiguousStatsMeasurement(&bgp_benchmark_stats->deserialization_latency.bgp_deserialization_beagle_stats);
    /* int64_t nanosec_duration = GetNanoSecDuration(bgp_benchmark_stats->deserialization_latency.bgp_deserialization_beagle_timer.start_time); */
    /* bgp_benchmark_stats->deserialization_latency.total_durations_bgp_deserialization_beagle += nanosec_duration; */
    /* bgp_benchmark_stats->deserialization_latency.num_measurements_bgp_deserialization_beagle++; */
    // start processing timer again
    StartContiguousStatsDuration(&bgp_benchmark_stats->processing_latency.bgp_nlri_parse_stats);
    if (BGP_DEBUG (update, UPDATE_IN))  
      zlog_debug("benchmark_input_filter: current time for bgp_nlri_parse (ms) %f", bgp_benchmark_stats->processing_latency.bgp_nlri_parse_stats.current_duration / 1000000.0);
    /* clock_gettime(CLOCK_REALTIME, &bgp_benchmark_stats->processing_latency.bgp_update_main_timer.start_time); */
    
    /* UpdateDeserializationCurrentDuration(&bgp_benchmark_stats->deserialization_latency); */
  }

  if (BGP_DEBUG (update, UPDATE_IN))
    zlog_debug("benchmark_input_filter: num bytes found in advert: %d",
             num_bytes);
  /* zlog_debug( */
  /*     "benchmark_update_control_info: advert received: %s", */
  /*     SerializedAdverToString(control_info->integrated_advertisement, */
  /*                             control_info->integrated_advertisement_size)); */
  return DBGP_NOT_FILTERED;
}

void benchmark_update_control_info(dbgp_control_info_t* control_info) {
  // see if there are bytes in there already
  int num_bytes =
      GetBenchmarkIABytesSize(control_info->integrated_advertisement,
                              control_info->integrated_advertisement_size);
  if (BGP_DEBUG (update, UPDATE_IN))  
    zlog_debug("benchmark_update_control_info: num bytes found in advert: %d",
               num_bytes);
  if (BGP_DEBUG (update, UPDATE_IN))  
    zlog_debug(
               "benchmark_update_control_info: advert received: %s",
               SerializedAdverToString(control_info->integrated_advertisement,
                                       control_info->integrated_advertisement_size));
  // if there are no bytes in advert, then we are originating advert, set it.
  if (num_bytes == -1) {
    // set the benchmark bytes in control information
    {
      uint32_t num_bytes_to_set = GetBenchmarkBytes(general_configuration_);
      if (BGP_DEBUG (update, UPDATE_IN))  
        zlog_debug("benchmark_update_control_info: setting %d bytes",
                   num_bytes_to_set);
      char* old_integrated_advertisement =
        control_info->integrated_advertisement;
      int old_integrated_advertisement_size =
          control_info->integrated_advertisement_size;
      int new_size;
      char* new_integrated_advertisement_info = SetBenchmarkIABytes(
          old_integrated_advertisement, old_integrated_advertisement_size,
          num_bytes_to_set, &new_size);
      free(old_integrated_advertisement);
      control_info->integrated_advertisement =
          new_integrated_advertisement_info;
      control_info->integrated_advertisement_size = new_size;
      if (BGP_DEBUG (update, UPDATE_IN))  
        zlog_debug(
                   "benchmark_update_control_info: new advertisement: %s",
                   SerializedAdverToString(control_info->integrated_advertisement,
                                           control_info->integrated_advertisement_size));
    }
  }
}

dbgp_filtered_status_t benchmark_output_filter(struct attr* attr) {
  // if either the transit or extra is null, then we haven't inserted benchmark
  // information into advert (we are announcing it), therefore insert it
  if (attr->extra == NULL || attr->extra->transit == NULL) {
    if (BGP_DEBUG (update, UPDATE_IN))  
      zlog_debug(
                 "benchmark_output_filter: no transit or extra, means we are announcing "
                 "it. Update control info to put benchmark info in");
    if (BGP_DEBUG (update, UPDATE_IN))  
      zlog_debug("benchmark_output_filter: aspath of attr: %s",
                 attr->aspath->str);
    uint32_t num_bytes_to_set = GetBenchmarkBytes(general_configuration_);
    if(num_bytes_to_set > 0) {
      
    dbgp_control_info_t* control_info;
    struct transit* transit;

    // Precondition asserts
    assert(attr != NULL);
    control_info = GetControlInformation2(attr, &transit);
    assert(transit != NULL);
    if (BGP_DEBUG (update, UPDATE_IN))  
      zlog_debug("benchmark_output_filter: setting %d bytes",
                 num_bytes_to_set);
    char* old_integrated_advertisement = control_info->integrated_advertisement;
    int old_integrated_advertisement_size =
        control_info->integrated_advertisement_size;
    int new_size;
    char* new_integrated_advertisement_info = SetBenchmarkIABytes(
        old_integrated_advertisement, old_integrated_advertisement_size,
        num_bytes_to_set, &new_size);
    free(old_integrated_advertisement);
    control_info->integrated_advertisement = new_integrated_advertisement_info;
    control_info->integrated_advertisement_size = new_size;
    if (BGP_DEBUG (update, UPDATE_IN))  
      zlog_debug(
                 "benchmark_output_filter: new advertisement: %s",
                 SerializedAdverToString(control_info->integrated_advertisement,
                                         control_info->integrated_advertisement_size));
    // Insert the updated control information into the lookup service and insert
    // the key into the the transitive attribute.
    dbgp_result_status_t success = set_control_info(transit, control_info);
    assert(success == DBGP_SUCCESS);
    }
  }
  return DBGP_NOT_FILTERED;
}
