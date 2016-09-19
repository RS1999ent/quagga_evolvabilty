#ifndef BGPD_SHARED_ENUM_DEFINITIONS_H
#define BGPD_SHARED_ENUM_DEFINITIONS_H

/* File used for enums that are shared between c++ and c code. */


/**
 * @enum D-BGP - Used to indicate what critical fix protocol this
 * router supports.  Routers that support multi-path protocol (e.g.,
 * SCION) must also pick a critical fix protocol (or BGP).  This is
 * necessary because multiple paths cannot be selected at island
 * border routers.
 */
typedef enum dbgp_protocols_s {
  /* Just BGP */
  dbgp_protocol_baseline = 0,

  /* Critical fix protocols */
  dbgp_critical_wiser = 1,

  // runs baseline, but sleeps (this is for use for gulfs to wake up later so
  // pathlets can advertise through them).
  dbgp_protocol_baseline_sleeper = 2,

  /* Replacement protocols */
  dbgp_replacement_none = 1000,
  dbgp_replacement_scion = 1001,
  dbgp_replacement_pathlets = 1002
} dbgp_protocol_t;

#endif
