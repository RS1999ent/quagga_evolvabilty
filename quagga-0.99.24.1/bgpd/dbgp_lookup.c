/**
 * @file dbgp_lookup.c
 * @brief Implements set/retrieve functions for D-BGP's lookup service (currenlty redis)
 */

#include <time.h>
#include <stdlib.h>

#include "lib/zebra.h"
#include "bgpd/bgpd.h"
#include "bgpd/bgp_attr.h"
#include "bgpd/bgp_route.h" 
#include "bgpd/dbgp_lookup.h"
#include "hiredis/hiredis.h"



/* ********************* Global vars ************************** */
static int rand_init = 0;


/* ********************* Public functions ********************* */

dbgp_result_status_t retrieve_control_info(struct attr *attr, dbgp_control_info_t *control_info)
{
  struct transit* transit;
  unsigned int key;
  redisContext *c;
  redisReply* reply;
  char redis_cmd[256];

  /* First retrieve the lookup key, which is stored in transitive
   * community attributes
   */
  transit = attr->extra->transit; 
  assert(transit->length == sizeof(unsigned int));
  key = (unsigned int)*transit->val;

  c = redisConnect("REDIS_IP", REDIS_PORT);
  sprintf(redis_cmd, "GET %ud", key);
  reply = redisCommand(c, redis_cmd); 
  assert(reply->type != REDIS_REPLY_ERROR || REDIS_REPLY_ARRAY);
  *control_info = reply->integer; 
  free(reply);

  return(DBGP_SUCCESS);
}


dbgp_result_status_t set_control_info(struct attr* attr, dbgp_control_info_t* control_info)
{
  unsigned int key;
  redisContext *c;
  redisReply* reply;
  char redis_cmd [256];

  /* Get a number that will serve as the new lookup key */
  if (rand_init == 0) {
    srand(time(NULL));
    rand_init = 1;
  }
  key = rand();

  /* Store control info in lookup service */
  c = redisConnect(REDIS_IP, REDIS_PORT);
  sprintf(redis_cmd, "SET %ud %lld", key, *control_info);
  reply = redisCommand(c, redis_cmd); 
  assert(reply->type != REDIS_REPLY_ERROR);
  free(reply); 

  return(DBGP_SUCCESS);
}
   
  
