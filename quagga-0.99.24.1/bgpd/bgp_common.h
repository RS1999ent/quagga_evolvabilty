/**
 * @file bgp_common.h
 * @brief: Contains functions and variables pulled out of bgp_route.c to
 * allow D-BGP code to access them
 */

#ifndef _BGP_COMMON_H
#define _BGP_COMMON_H

#include "bgpd/bgpd.h"

/* BGP_info struct from bgp_route.h */
struct bgp_info
{
  /* For linked list. */
  struct bgp_info *next;
  struct bgp_info *prev;
  
  /* Peer structure.  */
  struct peer *peer;

  /* Attribute structure.  */
  struct attr *attr;
  
  /* Extra information */
  struct bgp_info_extra *extra;
  

  /* Multipath information */
  struct bgp_info_mpath *mpath;

  /* Uptime.  */
  time_t uptime;

  /* reference count */
  int lock;
  
  /* BGP information status.  */
  u_int16_t flags;
#define BGP_INFO_IGP_CHANGED    (1 << 0)
#define BGP_INFO_DAMPED         (1 << 1)
#define BGP_INFO_HISTORY        (1 << 2)
#define BGP_INFO_SELECTED       (1 << 3)
#define BGP_INFO_VALID          (1 << 4)
#define BGP_INFO_ATTR_CHANGED   (1 << 5)
#define BGP_INFO_DMED_CHECK     (1 << 6)
#define BGP_INFO_DMED_SELECTED  (1 << 7)
#define BGP_INFO_STALE          (1 << 8)
#define BGP_INFO_REMOVED        (1 << 9)
#define BGP_INFO_COUNTED	(1 << 10)
#define BGP_INFO_MULTIPATH      (1 << 11)
#define BGP_INFO_MULTIPATH_CHG  (1 << 12)

  /* BGP route type.  This can be static, RIP, OSPF, BGP etc.  */
  u_char type;

  /* When above type is BGP.  This sub type specify BGP sub type
     information.  */
  u_char sub_type;
#define BGP_ROUTE_NORMAL       0
#define BGP_ROUTE_STATIC       1
#define BGP_ROUTE_AGGREGATE    2
#define BGP_ROUTE_REDISTRIBUTE 3 
};


/* Get MED value.  If MED value is missing and "bgp bestpath
   missing-as-worst" is specified, treat it as the worst value. */
u_int32_t
bgp_med_value (struct attr *attr, struct bgp *bgp);

/* Compare two bgp route entity.  br is preferable then return 1. */
int
bgp_info_cmp (struct bgp *bgp, struct bgp_info *new, struct bgp_info *exist,
	      int *paths_eq);

#endif /* _BGP_COMMON_H */
