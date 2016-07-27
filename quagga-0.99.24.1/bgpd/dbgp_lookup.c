/**
 * @file dbgp_lookup.c
 * @brief Implements set/retrieve functions for D-BGP's lookup service (currenlty redis)
 */

#include <time.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <inttypes.h>

#include "lib/zebra.h"
#include "lib/log.h"
#include "bgpd/bgpd.h"
#include "bgpd/bgp_attr.h"
#include "bgpd/dbgp_lookup.h"
#include "hiredis/hiredis.h"


/* ********************* Global vars ************************** */
static int g_rand_init = 0;

/** Timeout for connecting to redis */
static struct timeval g_timeout = { 5, 500000}; // 5.5 seconds

/* ********************* Private functions ********************* */

/** 
 * Creates a random seed by mixing three values
 * 
 * @param a: first value to be mixed in
 * @param b: second value to be mixed in
 * @param c: third value to be mixed in
 *
 * @return: a random number
 */
static unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}
/**
 * Connects to redis service.  Asserts on failure.
 */
static redisContext *connect_to_redis() {
  redisContext *c = NULL;

  c = redisConnectWithTimeout(REDIS_IP, REDIS_PORT, g_timeout);
  if (c == NULL) {
    zlog_err("%s:, failed to connect to redis", __func__);
    assert(0);
  }

  return(c);
}


/* ********************* Public functions ********************* */

dbgp_result_status_t insert_check_sentinel(struct transit *transit)
{
  dbgp_control_info_t control_info;
  dbgp_result_status_t retval;

  assert(transit != NULL && transit->val != NULL);

  retval = retrieve_control_info(transit, &control_info);
  assert(retval == DBGP_SUCCESS);
  
  /* Check sentinal value */
  assert(control_info == DBGP_SENTINEL_VALUE);
  
  /* Insert a new lookup key with the same sentinel value */
  retval = set_control_info(transit, &control_info);
  assert(retval == DBGP_SUCCESS);
  
  return(DBGP_SUCCESS);
}

dbgp_result_status_t insert_sentinel(struct transit *transit) 
{ 
  dbgp_control_info_t new_control_info;

  /** @note: (rajas) I am using this as a hook to attch to the bgpd
   * process for debugging */
  sleep(50);

  new_control_info = DBGP_SENTINEL_VALUE;
  set_control_info(transit, &new_control_info); 

  return (DBGP_SUCCESS);
}
  

dbgp_result_status_t retrieve_control_info(struct transit * transit,
					   dbgp_control_info_t * control_info)
{
  redisContext *c;
  redisReply* reply;

  /* Input sanity checks */
  assert(transit != NULL && control_info != NULL);

  /** @note: (rajas) I am using this as a hook to attch to the bgpd
   * process for debugging */
  //sleep(50);

  /* First retrieve the lookup key */
  assert(transit->length == sizeof(dbgp_lookup_key_t));

  /* Get D-BGP control info from lookup service */
  c = connect_to_redis();
  reply = redisCommand(c, "GET %"PRIu32"", *(dbgp_lookup_key_t *)transit->val);

  if(reply->type == REDIS_REPLY_ERROR) {
    zlog_err("%s:, failed to retrieve D-BGP control info. Key=%"PRIu32"",
	     __func__, *(dbgp_lookup_key_t *)transit->val);
    assert(0);
  }

  *control_info = (dbgp_control_info_t)(atoll(reply->str));
  free(reply);
  free(c);

  return(DBGP_SUCCESS);
}


dbgp_result_status_t set_control_info(struct transit *transit, 
				      dbgp_control_info_t *control_info)
{
  uint32_t *key = NULL;
  redisContext *c;
  redisReply* reply;

  /* Input sanity checks */
  assert(transit != NULL && control_info != NULL);

  /** @note: (rajas) I am using this as a hook to attch to the bgpd
   * process for debugging */
  //sleep(50);

  /* Get a number that will serve as the new lookup key */
  if (g_rand_init == 0) {
    /** @bug: rajas - sleep to guarantee two instances
     * of Quagga started by MiniNExT don't use same seed
     */
    sleep(2); 
    srand(mix(clock(), time(NULL), pthread_self()));
    g_rand_init = 1;
  }
  key = (dbgp_lookup_key_t *)malloc(sizeof(dbgp_lookup_key_t));
  *key = rand();

  /* Store control info in lookup service */
  c = connect_to_redis();
  reply = redisCommand(c, "SET %"PRIu32" %"PRIu64"", *key, *control_info);

  if (reply->type == REDIS_REPLY_ERROR) {
    zlog_err("%s: failed to store control info.  Key=%"PRIu32", control info=%"PRIu64"",
	     __func__, *key, *control_info);
    assert(0);
  }

  /* Create new transitive attribute structure */
  assert(transit->val != NULL);

  /* Add key to advertisement */
  transit->length = sizeof(dbgp_lookup_key_t);
  /* Make sure to convert from host to network byte order*/
  transit->val = key;

  free(reply); 
  free(c);

  return(DBGP_SUCCESS);
}
   
  
