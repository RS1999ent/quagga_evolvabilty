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
#define DBGP_SENTINEL_VALUE 5
/**
 * @enum Used to indicate success or failure
 */
typedef enum dbgp_result_status_s 
{
  DBGP_SUCCESS = 0,
  DBGP_FAILURE = 1
} dbgp_result_status_t; 

typedef long long int dbgp_control_info_t;
typedef uint32_t dbgp_lookup_key_t;

/**
 * Retrieves extra ctrl info from lookup service and checks to make
 * sure it is hte sentinel value.  Inserts a new lookup ke associated
 * with a new sentinal value.
 *
 * @param transit: A pre-allocated transitive attribute
 * structure. Transit->val must also be allocated.
 * @return DBGP_SUCCESS or DBGP_FAILURE
 */
dbgp_result_status_t insert_check_sentinel(struct transit *);

/**
 * Retrieves the D-BGP control info associated with this advertisement
 *
 * @param transit : Transitive attributes
 * @param control_info: Memory location where the extra ctrl info can be
 * @return The control information
 */
dbgp_result_status_t retrieve_control_info(struct transit *transit, 
					   dbgp_control_info_t *control_info);


/** 
 * Sets extra D-BGP control info associated with this advertisement 
 * 
 * @param transit: pre-allocated transitive attributes.  Transit->val
 * must also be allocated.  
 * @param control_info: Pointer to The control information; can be
 * freed after calling this fn.

 *
 * @param DBGP_SUCCESS or DBGP_FAILURe
 */
dbgp_result_status_t  set_control_info(struct transit *transit,
				       dbgp_control_info_t *control_info);

