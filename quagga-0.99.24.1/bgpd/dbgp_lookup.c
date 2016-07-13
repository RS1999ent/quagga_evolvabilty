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

dbgp_result_status_t retrieve_control_info(struct attr * attr, 
					   dbgp_control_info_t * control_info)
{
  struct transit* transit;
  uint32_t key;
  redisContext *c;
  redisReply* reply;
  char redis_cmd[256];

  /** @bug: (rajas) I am using this as a hook to attch to the bgpd
   * process for debugging */
  //sleep(50);

  /* First retrieve the lookup key, which is stored in transitive
   * community attributes
   */
  transit = bgp_attr_extra_get(attr)->transit;
  assert(transit->length == sizeof(uint32_t));

  /* Convert from network to host byte order */
  key = ntohl(*(uint32_t *)transit->val);

  /* Get D-BGP control info from lookup service */
  sprintf(redis_cmd, "GET %"PRIu32"", key);
  reply = redisCommand(c, redis_cmd); 
  if(reply->type != REDIS_REPLY_INTEGER) {
    zlog_err("%s:, failed to retrieve D-BGP control info. Key=%"PRIu32"",
	     __func__, key);
    assert(0);
  }

  *control_info = reply->integer; 
  free(reply);
  free(c);

  return(DBGP_SUCCESS);
}


dbgp_result_status_t set_control_info(struct attr * attr, 
				      dbgp_control_info_t *control_info)
{
  struct transit* transit;
  int *key;
  redisContext *c;
  redisReply* reply;
  char redis_cmd [256];

  /** @bug: (rajas) I am using this as a hook to attch to the bgpd
   * process for debugging */
  //sleep(50);

  /* Input sanity checks */
  assert(attr != NULL && control_info != NULL);

  /* Get a number that will serve as the new lookup key */
  if (g_rand_init == 0) {
    srand(time(NULL));
    g_rand_init = 1;
  }
  key = (int *)malloc(sizeof(int));
  *key = rand();

  c = connect_to_redis();

  /* Store control info in lookup service */
  sprintf(redis_cmd, "SET %"PRIu32" %"PRIu64"", *key, *control_info);
  reply = redisCommand(c, redis_cmd); 
  if (reply->type == REDIS_REPLY_ERROR) {
    zlog_err("%s: failed to store control info.  Key=%"PRIu32", control info=%"PRIu64"",
	     __func__, *key, *control_info);
    assert(0);
  }

  /* Add key to advertisement */
  /* If there is a previous transitive attribute, et rid of it */
  transit = bgp_attr_extra_get(attr)->transit;
  if (attr->extra->transit != NULL) {
    transit_unintern(transit);
  }

  attr->extra->transit = (struct transit *)malloc(sizeof(struct transit));
  transit = attr->extra->transit;

  transit->length = sizeof(uint32_t);
  /* Make sure to convert from host to network byte order*/
  *key = htonl(*key);
  transit->val = (u_char *)key;

  /* Intern new transitive attribute */
  transit_intern(transit);

  free(reply); 
  free(c);

  return(DBGP_SUCCESS);
}
   
  
