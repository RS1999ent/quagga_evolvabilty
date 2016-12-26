

#include "bgpd/benchmark.h"
#include "bgpd/dbgp_lookup.h"
#include "bgpd/wiser_config_interface.h"

/* ********************* Global vars ************************** */
extern GeneralConfigurationHandle general_configuration_;
char* SetBenchmarkIABytes(char* serialized_advert, int advert_size,
                          int num_bytes_to_set, int* modified_advert_size);
int GetBenchmarkIABytesSize(char* serialized_advert, int advert_size);
char* SerializedAdverToString(char* serialized_advert, int advert_size);

/* ********************* Public functions ********************* */

dbgp_filtered_status_t benchmark_input_filter(dbgp_control_info_t *control_info){
  // see if there are bytes in there already
  int num_bytes =
    GetBenchmarkIABytesSize(control_info->integrated_advertisement,
                            control_info->integrated_advertisement_size);
  zlog_debug("benchmark_update_control_info: num bytes found in advert: %d",
             num_bytes);
  zlog_debug(
             "benchmark_update_control_info: advert received: %s",
             SerializedAdverToString(control_info->integrated_advertisement,
                                     control_info->integrated_advertisement_size));
  return DBGP_NOT_FILTERED;
}

/* void benchmark_update_control_info(dbgp_control_info_t* control_info) { */
/*   // see if there are bytes in there already */
/*   int num_bytes = */
/*       GetBenchmarkIABytesSize(control_info->integrated_advertisement, */
/*                               control_info->integrated_advertisement_size); */
/*   zlog_debug("benchmark_update_control_info: num bytes found in advert: %d", */
/*              num_bytes); */
/*   zlog_debug( */
/*       "benchmark_update_control_info: advert received: %s", */
/*       SerializedAdverToString(control_info->integrated_advertisement, */
/*                               control_info->integrated_advertisement_size)); */
/*   // if there are no bytes in advert, then we are originating advert, set it. */
/*   if (num_bytes == -1) { */
/*     // set the benchmark bytes in control information */
/*     { */
/*       uint32_t num_bytes_to_set = GetBenchmarkBytes(general_configuration_); */
/*       zlog_debug("benchmark_update_control_info: setting %d bytes", */
/*                  num_bytes_to_set); */
/*       char* old_integrated_advertisement = */
/*           control_info->integrated_advertisement; */
/*       int old_integrated_advertisement_size = */
/*           control_info->integrated_advertisement_size; */
/*       int new_size; */
/*       char* new_integrated_advertisement_info = SetBenchmarkIABytes( */
/*           old_integrated_advertisement, old_integrated_advertisement_size, */
/*           num_bytes_to_set, &new_size); */
/*       free(old_integrated_advertisement); */
/*       control_info->integrated_advertisement = */
/*           new_integrated_advertisement_info; */
/*       control_info->integrated_advertisement_size = new_size; */
/*       zlog_debug( */
/*           "benchmark_update_control_info: new advertisement: %s", */
/*           SerializedAdverToString(control_info->integrated_advertisement, */
/*                                   control_info->integrated_advertisement_size)); */
/*     } */
/*   } */
/* } */

dbgp_filtered_status_t benchmark_output_filter(struct attr* attr) {
  // if either the transit or extra is null, then we haven't inserted benchmark
  // information into advert (we are announcing it), therefore insert it
  if (attr->extra == NULL || attr->extra->transit == NULL) {
    zlog_debug(
        "benchmark_output_filter: no transit or extra, means we are announcing "
        "it. Update control info to put benchmark info in");
    // Create control information to hold the benchmark information, get and set
    // the benchmark information in the IA and put it in the lookup service
    {
      dbgp_control_info_t* control_info;
      struct transit* transit;
      control_info = GetControlInformation(attr, &transit);
      assert(transit != NULL);
      // insert the bytes to send for the benchmark
      {
        uint32_t num_bytes_to_set = GetBenchmarkBytes(general_configuration_);
        zlog_debug("benchmark_outputfilter: setting %d bytes",
                   num_bytes_to_set);
        if (num_bytes_to_set > 0) {
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
        zlog_debug("benchmark_outputfilter: new advertisement: %s",
                   SerializedAdverToString(
                       control_info->integrated_advertisement,
                       control_info->integrated_advertisement_size));
          }
      }
      // Insert the updated control information into the lookup service and
      // insert
      // the key into the the transitive attribute.
      dbgp_result_status_t success = set_control_info(transit, control_info);
      assert(success == DBGP_SUCCESS);
    }
  }
  return DBGP_NOT_FILTERED;
}
