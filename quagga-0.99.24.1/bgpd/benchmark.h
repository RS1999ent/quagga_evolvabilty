
#ifndef _QUAGGA_BENCHMARK_H
#define _QUAGGA_BENCHMARK_H
#include "bgpd/dbgp_lookup.h"
#include "bgpd/dbgp.h"

/* ********************* Global vars ************************** */

/* ********************* Private functions ********************* */

/* ********************* Public functions ********************* */

/*
  benchmark_input_filter just prints out the number of bytes received for the
  benchmark protocol. Is already deserialized by dbgp_input_filter

  Arguments:
     control_info: the control information that points to the serialized IA

  Returns: DBGP_NOT_FILTERED
 */
dbgp_filtered_status_t benchmark_input_filter(dbgp_control_info_t *control_info);

/**
 * Updates benchmarkp proto-specific D-BGP control info
 *
 * @param control_info: D-BGP's extra control information
 */
/* void benchmark_update_control_info(dbgp_control_info_t* control_info); */

/**
 * Applies benchmark-protocol-specific output filters
 *
 * @param attr: the attribute of the bgp advertisement
 * returns: not filtered
 */
dbgp_filtered_status_t benchmark_output_filter(struct attr* attr);


#endif /* _QUAGGA_BENCHMARK_H */
