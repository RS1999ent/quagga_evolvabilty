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
#define DBGP_PACKED_VAL_LEN 256

/**
 * @enum Used to indicate success or failure
 */
typedef enum dbgp_result_status_s 
{
  DBGP_SUCCESS = 0,
  DBGP_FAILURE = 1
} dbgp_result_status_t; 

typedef struct dbgp_control_info_s { 
  unsigned long long sentinel;
  /**
   * @bug: D-BGP - Add more stuff here for different protocols
   */
} dbgp_control_info_t;

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
dbgp_result_status_t insert_check_sentinel(struct transit *transit);

/**
 * Inserts a new lookup key associated with a new sentinel value.
 *
 * @param transit: A pre-allocated transiive attribute structure.
 * Transit->val must also be allocated.
 * @return DBGP_SUCCESS or DBGP_FAILURE
 */
dbgp_result_status_t insert_sentinel(struct transit *transit);

/**
 * Retrieves the D-BGP control info associated with this advertisement
 *
 * @param transit : Transitive attributes
 * @return A pointer to the control information
 */
dbgp_control_info_t *retrieve_control_info(struct transit *transit);
		
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

