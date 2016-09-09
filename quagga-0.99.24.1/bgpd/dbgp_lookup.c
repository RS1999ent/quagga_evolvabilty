/**
 * @file dbgp_lookup.c
 * @brief Implements set/retrieve functions for D-BGP's lookup service (currenlty redis)
 */

#include <time.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <pthread.h>

#include "lib/zebra.h"
#include "lib/log.h"
#include "lib/prefix.h"
#include "lib/vty.h"
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


/**
 * Unpacks a reply from the lookup service and inserts it in a
 * dbgp_control_info_t struct.
 *
 * @param char* reply: The reply from the lookup service
 * @param len: The length of the reply
 * 
 * @return: Pointer to a malloc'd dbgp_control_info_t struct
 */
static dbgp_control_info_t *unpack_redis_reply(char *reply, int len) 
{
  dbgp_control_info_t *control_info;

  assert(reply != NULL);

  control_info = calloc(1, sizeof(dbgp_control_info_t));

  // Set the length of the serialized protobuf (should be the length of the
  // value retrieved)
  control_info->integrated_advertisement_size = len;

  // Create memory for the integrated advertisement and copy the data to it.
  control_info->integrated_advertisement = malloc(len);
  memcpy(control_info->integrated_advertisement, reply, len);

  /* Currently just unpacking sentinel value */
  /* control_info->sentinel = atoll(reply); */

  return(control_info);
}


/**
 * Packs data in a dbgp_control_inf_t struct into a string for
 * insertion into the lookup service
 *
 * @param control_info: A pointer to a dbgp_control_info_t struct
 * @param packed: A pre-allocated char * in which the packed data will be placed
 * @return a malloc'd char *pointer to the encoded string, which is
 * null terminated.
 */
static dbgp_result_status_t pack_dbgp_control_info(dbgp_control_info_t *control_info, 
				    char *packed_val)
{

  assert(control_info != NULL && packed_val != NULL);

  // copy the serialized integrated advertisement into packed_val
  memcpy(packed_val, control_info->integrated_advertisement,
         control_info->integrated_advertisement_size);

  /* Currently just packing sentinel value */
  /* snprintf(packed_val, DBGP_PACKED_VAL_LEN, "%"PRIu64"", control_info->sentinel); */

  return(DBGP_SUCCESS);
}


/* ********************* Public functions ********************* */

int is_lookup_service_path(struct transit *transit)
{
  assert(transit != NULL);
  assert(transit->val != NULL);

  if (*(dbgp_lookup_key_t *)transit->val == DBGP_LOOKUP_SVC_PATH) {
    return 1;
  }

  return 0;
}

dbgp_result_status_t insert_check_sentinel(struct transit *transit)
{
  dbgp_control_info_t *control_info;
  dbgp_result_status_t retval;

  assert(transit != NULL && transit->val != NULL);

/** 
   * @note: (rajas) I am using this as a hook to attch to the bgpd
   * process for debugging 
   */
  //sleep(50);

  control_info = retrieve_control_info(transit);
  assert(control_info != NULL);
  
  /* Check sentinal value */
  assert(control_info->sentinel == DBGP_SENTINEL_VALUE);
  
  /* Insert a new lookup key with the same sentinel value */
  retval = set_control_info(transit, control_info);
  assert(retval == DBGP_SUCCESS);
  
  free(control_info);

  return(DBGP_SUCCESS);
}

dbgp_result_status_t insert_sentinel(struct transit *transit) 
{ 
  dbgp_control_info_t new_control_info;

  assert(transit != NULL && transit->val != NULL);

  /** 
   * @note: (rajas) I am using this as a hook to attch to the bgpd
   * process for debugging 
   */
  //sleep(50);

  new_control_info.sentinel = DBGP_SENTINEL_VALUE;
  set_control_info(transit, &new_control_info); 

  return (DBGP_SUCCESS);
}
  

dbgp_control_info_t *retrieve_control_info(struct transit * transit)
{
  redisContext *c;
  redisReply* reply;
  dbgp_control_info_t *control_info;

  /* Input sanity checks */
  assert(transit != NULL && transit->val != NULL);

  /** 
   * @note: (rajas) I am using this as a hook to attch to the bgpd
   * process for debugging 
   */
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

  control_info = unpack_redis_reply(reply->str, reply->len);

  free(reply);
  free(c);

  return(control_info);
}


dbgp_result_status_t set_control_info(struct transit *transit, 
				      dbgp_control_info_t *control_info)
{
  uint32_t *key = NULL;
  redisContext *c;
  redisReply* reply;
  char packed_val[control_info->integrated_advertisement_size];

  /* Input sanity checks */
  assert(transit != NULL && transit->val != NULL && control_info != NULL);

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
  do {
    *key = rand();
  } while (*key == DBGP_LOOKUP_SVC_PATH);

  assert(pack_dbgp_control_info(control_info, packed_val) == DBGP_SUCCESS);

  /* Store control info in lookup service */
  c = connect_to_redis();
  reply = redisCommand(c, "SET %"PRIu32" %s", *key, packed_val);

  if (reply->type == REDIS_REPLY_ERROR) {
    zlog_err("%s: failed to store control info.  Key=%"PRIu32", control info=%s", 
	     __func__, *key, packed_val);
    assert(0);
  }

  /* Add key to advertisement */
  transit->length = sizeof(dbgp_lookup_key_t);
  transit->val = (u_char *)key;

  free(reply); 
  free(c);

  return(DBGP_SUCCESS);
}

int has_dbgp_control_info(struct transit *transit)
{
  if (transit == NULL || transit->val == NULL)
  {
    return 0;
  }
  else if ( transit->length == sizeof(dbgp_lookup_key_t))
    {
      return 1;
    }
  return 0;
}
   
  
