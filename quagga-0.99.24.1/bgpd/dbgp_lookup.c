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

  retval = retrieve_control_info(transit, &control_info);
  assert(retval == DBGP_SUCCESS);
  
  /* Check sentinal value */
  assert(control_info == DBGP_SENTINEL_VALUE);
  
  /* Insert a new lookup key with the same sentinel value */
  retval = set_control_info(transit, &control_info);
  assert(retval == DBGP_SUCCESS);
  
  return(DBGP_SUCCESS);
}


dbgp_result_status_t retrieve_control_info(struct transit * transit,
					   dbgp_control_info_t * control_info)
{
  redisContext *c;
  redisReply* reply;
  char redis_cmd[256];

  /* Input sanity checks */
  assert(transit != NULL && control_info != NULL);

  /** @bug: (rajas) I am using this as a hook to attch to the bgpd
   * process for debugging */
  //sleep(50);

  /* First retrieve the lookup key */
  assert(transit->length == sizeof(dbgp_lookup_key_t));

  /* Get D-BGP control info from lookup service */
  c = connect_to_redis();
  sprintf(redis_cmd, "GET %"PRIu32"", *(dbgp_lookup_key_t *)transit->val);
  reply = redisCommand(c, redis_cmd); 
  if(reply->type != REDIS_REPLY_INTEGER) {
    zlog_err("%s:, failed to retrieve D-BGP control info. Key=%"PRIu32"",
	     __func__, *(dbgp_lookup_key_t *)transit->val);
    assert(0);
  }

  *control_info = reply->integer; 
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
  char redis_cmd [256];

  /* Input sanity checks */
  assert(transit != NULL && control_info != NULL);

  /** @bug: (rajas) I am using this as a hook to attch to the bgpd
   * process for debugging */
  //sleep(50);

  /* Get a number that will serve as the new lookup key */
  if (g_rand_init == 0) {
    srand(time(NULL));
    g_rand_init = 1;
  }
  key = (dbgp_lookup_key_t *)malloc(sizeof(dbgp_lookup_key_t));
  *key = rand();

  /* Store control info in lookup service */
  c = connect_to_redis();
  sprintf(redis_cmd, "SET %"PRIu32" %"PRIu64"", *key, *control_info);
  reply = redisCommand(c, redis_cmd); 
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
   
  
