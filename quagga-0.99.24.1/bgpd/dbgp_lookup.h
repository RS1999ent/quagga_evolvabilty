/** 
 * @file dbgp_lookup.h
 * @brief Contains set/retrieve functions for obtaining info from
 * D-BGP's lookup service (redis).
 */

#include "lib/zebra.h"
#include "bgpd/bgpd.h"
#include "bgpd/bgp_attr.h" 
#include "hiredis/hiredis.h"

#define REDIS_IP "172.0.5.1"
#define REDIS_PORT 6379

/**
 * @enum Used to indicate success or failure
 */
typedef enum dbgp_result_status_s 
{
  DBGP_SUCCESS = 0,
  DBGP_FAILURE = 1
} dbgp_result_status_t; 

typedef long long int dbgp_control_info_t;


/**
 * Retrieves the D-BGP control info associated with this advertisement
 *
 * @param attr: BGP attributes for the advertisement @param
 * @param control_info: Memory location where the extra ctrl info can be
 * @return The control information
 */
dbgp_result_status_t retrieve_control_info(struct attr const * const attr, dbgp_control_info_t * const control_info);


/** 
 * Sets extra D-BGP control info associated with this advertisement 
 * 
 * @param attr: attributes for the advertisement
 * @param control_info: Pointer to THe control information; can be
 * freed after calling this fn

 *
 * @param DBGP_SUCCESS or DBGP_FAILURe
 */
dbgp_result_status_t  set_control_info(struct attr * const attr, dbgp_control_info_t const * const control_info);

