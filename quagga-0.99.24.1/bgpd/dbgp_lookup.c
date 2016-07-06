/**
 * @file dbgp_lookup.c
 * @brief Implements set/retrieve functions for D-BGP's lookup service (currenlty redis)
 */

#include <time.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <inttypes.h>

#include "lib/zebra.h"
#include "bgpd/bgpd.h"
#include "bgpd/bgp_attr.h"
//#include "bgpd/bgp_route.h" 
#include "bgpd/dbgp_lookup.h"
#include "hiredis/hiredis.h"


/* ********************* Global vars ************************** */
static int rand_init = 0;


/* ********************* Public functions ********************* */

dbgp_result_status_t retrieve_control_info(struct attr const * const attr, dbgp_control_info_t * const control_info)
{
  struct transit* transit;
  uint32_t key;
  redisContext *c = NULL;
  redisReply* reply;
  char redis_cmd[256];

  /* First retrieve the lookup key, which is stored in transitive
   * community attributes
   */
  transit = attr->extra->transit; 
  assert(transit->length == sizeof(uint32_t));
  /* Convert from network to host byte order */
  key = ntohl(*(uint32_t *)transit->val);

  c = redisConnect(REDIS_IP, REDIS_PORT);
  assert (c != NULL);
  sprintf(redis_cmd, "GET %"PRIu32"", key);
  reply = redisCommand(c, redis_cmd); 
  assert(reply->type != REDIS_REPLY_ERROR || REDIS_REPLY_ARRAY);
  *control_info = reply->integer; 
  free(reply);

  return(DBGP_SUCCESS);
}


dbgp_result_status_t set_control_info(struct attr * const attr, dbgp_control_info_t const * const control_info)
{
  struct transit* transit;
  int *key;
  redisContext *c = NULL;
  redisReply* reply;
  char redis_cmd [256];

  /* Get a number that will serve as the new lookup key */
  if (rand_init == 0) {
    srand(time(NULL));
    rand_init = 1;
  }
  key = (int *)malloc(sizeof(int));
  *key = rand();

  /* Store control info in lookup service */
  c = redisConnect(REDIS_IP, REDIS_PORT);
  assert(c != NULL);
  sprintf(redis_cmd, "SET %"PRIu32" %"PRIu64"", *key, *control_info);
  reply = redisCommand(c, redis_cmd); 
  assert(reply->type != REDIS_REPLY_ERROR);
  free(reply); 

  /* Add key to advertisement */
  /* transit should have already been allocated in bgp_attr.c */
  assert(attr->extra->transit != NULL);
  transit = attr->extra->transit;

  transit->length = sizeof(uint32_t);
  /* Make sure to convert from host to network byte order*/
  *key = htonl(*key);
  /* transit->val should be de-allocated in bgp_attr.c eventually */
  transit->val = (u_char *)key;
						    
  return(DBGP_SUCCESS);
}
   
  
