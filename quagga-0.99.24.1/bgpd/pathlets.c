#include "pathlets.h"
#include "bgpd/bgp_aspath.h"
#include "bgpd/bgp_route.h"
#include "lib/memory.h"
#include "lib/prefix.h"
#include "lib/vty.h"
#include "wiser_config_interface.h"
/* #include "bgpd/bgp_vty.h" */
/* #include "bgpd/dbgp_lookup.h" */
/* #include "bgpd/bgp_common.h" */
/* #include "bgpd/bgp_route.h" */
/* #include "bgpd/wiser_config_interface.h" */
/* #include <arpa/inet.h> */

/* ********************* Global vars ************************** */
#define CLASS_B_NETMASK ntohl(0xFFFF0000)
#define CLASS_C_NETMASK ntohl(0xFFFFFF00)
extern GeneralConfigurationHandle general_configuration_;
extern PathletInternalStateHandle pathlet_internal_state_;
extern PathletConfigHandle pathlet_config_;
/* extern struct vty* vty; */
int HasPathletInformation(char* serialized_advert, int advert_size,
                          int island_id);
char* GenerateInternalPathletControlInfo(
    PathletInternalStateHandle pathlet_internal_state, char* serialized_advert,
    int advert_size, const char* ip_address, int* new_size, int island_id);
void MergePathletInformationIntoGraph(
    PathletInternalStateHandle pathlet_internal_state, char* serialized_advert,
    int advert_size, int island_id);
char* SerializedAdverToString(char* serialized_advert, int advert_size);
char* GetPathletGraphString(PathletInternalStateHandle pathlet_internal_state);
char* PrintPathletsFromSerializedAdvert(char* serialized_advert,
                                        int advert_size, int island_id);
char* GetPathletsToSendString(
    PathletInternalStateHandle pathlet_internal_state);

char* GenerateExternalPathletDestinationControlInfo(
    PathletInternalStateHandle pathlet_internal_state, const char* destination,
    int island_id, int as_num, char* serialized_advert, int advert_size,
    int* new_advert_size);
char* GenerateExternalPathletControlInfo(
    PathletInternalStateHandle pathlet_internal_state, int island_id,
    char* serialized_advert, int advert_size, int* new_advert_size);
/* Determines if two ips are on the same subnet. It is assumed that both ips
   are
   valid and allocated */
int IpInSameSubnet(char* ip1_str, char* ip2_str, uint32_t netmask) {
  assert(ip1_str != NULL);
  assert(ip2_str != NULL);
  struct sockaddr_in sa_ip1, sa_ip2;

  // store this IP address in sa:
  inet_pton(AF_INET, ip1_str, &(sa_ip1.sin_addr));
  inet_pton(AF_INET, ip2_str, &(sa_ip2.sin_addr));
  uint32_t ip1, ip2;
  ip1 = sa_ip1.sin_addr.s_addr;
  ip2 = sa_ip2.sin_addr.s_addr;
  if ((ip1 & netmask) == (ip2 & netmask)) {
    return 1;
  } else {
    return 0;
  }
}

unsigned int aspath_size_custom(struct aspath* aspath) {
  int size = 0;
  struct assegment* seg = aspath->segments;

  while (seg) {
    size += seg->length;
    seg = seg->next;
  }
  return size;
}

// Same function as above, except that one of the ips is a saddr int
int IpInSameSubnetOneIsInt(uint32_t ip1, char* ip2_str, uint32_t netmask) {
  assert(ip2_str != NULL);
  struct sockaddr_in sa_ip2;

  // store this IP address in sa:
  inet_pton(AF_INET, ip2_str, &(sa_ip2.sin_addr));
  uint32_t ip2;
  ip2 = sa_ip2.sin_addr.s_addr;
  if ((ip1 & netmask) == (ip2 & netmask)) {
    return 1;
  } else {
    return 0;
  }
}

void AddAssociatedPathlet(char* associated_ip, int island_id,
                          dbgp_control_info_t* control_info) {
  char* old_integrated_advertisement = control_info->integrated_advertisement;
  int old_integrated_advertisement_size =
      control_info->integrated_advertisement_size;
  int new_size;
  char* new_integrated_advertisement_info = GenerateInternalPathletControlInfo(
      pathlet_internal_state_, old_integrated_advertisement,
      old_integrated_advertisement_size, associated_ip, &new_size, island_id);
  assert(new_integrated_advertisement_info != NULL);

  zlog_debug(
      "pathlets::AddAssociatedPathlet: Pathlets in outgoing advert:\n %s",
      PrintPathletsFromSerializedAdvert(new_integrated_advertisement_info,
                                        new_size, island_id));

  free(old_integrated_advertisement);
  control_info->integrated_advertisement = new_integrated_advertisement_info;
  control_info->integrated_advertisement_size = new_size;
}

void AddExternalPathletControlInfoForDest(struct prefix* prefix, int island_id,
                                          int as_num,
                                          dbgp_control_info_t* control_info) {
  char* old_integrated_advertisement = control_info->integrated_advertisement;
  int old_integrated_advertisement_size =
      control_info->integrated_advertisement_size;
  int new_size;
  char* prefix_string = malloc(256);
  prefix2str(prefix, prefix_string, 256);
  char* new_integrated_advertisement_info =
      GenerateExternalPathletDestinationControlInfo(
          pathlet_internal_state_, prefix_string, island_id, as_num,
          old_integrated_advertisement, old_integrated_advertisement_size,
          &new_size);
  assert(new_integrated_advertisement_info != NULL);

  zlog_debug(
      "pathlets::AddExternalPathletControlInfoForDest: Pathlets in outgoing "
      "advert:\n %s",
      PrintPathletsFromSerializedAdvert(new_integrated_advertisement_info,
                                        new_size, island_id));

  free(old_integrated_advertisement);
  control_info->integrated_advertisement = new_integrated_advertisement_info;
  control_info->integrated_advertisement_size = new_size;
}

void AddExternalPathletControlInfoForAll(int island_id,
                                         dbgp_control_info_t* control_info) {
  char* old_integrated_advertisement = control_info->integrated_advertisement;
  int old_integrated_advertisement_size =
      control_info->integrated_advertisement_size;
  int new_size;
  char* new_integrated_advertisement_info = GenerateExternalPathletControlInfo(
      pathlet_internal_state_, island_id, old_integrated_advertisement,
      old_integrated_advertisement_size, &new_size);
  assert(new_integrated_advertisement_info != NULL);

  zlog_debug(
      "pathlets::AddExternalPathletControlInfoForAll: Pathlets in outgoing "
      "advert:\n %s",
      PrintPathletsFromSerializedAdvert(new_integrated_advertisement_info,
                                        new_size, island_id));

  free(old_integrated_advertisement);
  control_info->integrated_advertisement = new_integrated_advertisement_info;
  control_info->integrated_advertisement_size = new_size;
}

void AnounceStaticRoute(char* ip_and_prefix, struct bgp* bgp) {
  struct bgp_static* bgp_static;
  struct prefix p;
  str2prefix(ip_and_prefix, &p);
  bgp_static = XCALLOC(MTYPE_BGP_STATIC, sizeof(struct bgp_static));
  bgp_static->backdoor = 0;
  bgp_static->valid = 0;
  bgp_static->igpmetric = 0;
  bgp_static->igpnexthop.s_addr = 0;

  zlog_debug("pathlets::AnnounceStaticRoute: announcing ip: %s", ip_and_prefix);
  bgp_static_update(bgp, &p, bgp_static, AFI_IP, SAFI_UNICAST);
}

int aspath_get_rightmost(struct aspath* aspath) {
  struct assegment* seg;
  int return_as = -1;

  if ((aspath == NULL) || (aspath->segments == NULL))
    return return_as;  // return -1 if there is no aspath

  seg = aspath->segments;

  while (seg) {
    int i;

    for (i = 0; i < seg->length; i++) return_as = seg->as[i];

    seg = seg->next;
  }
  // Here, return AS should be the rightmost segment
  return return_as;
}

char* Prefix2Str(struct prefix* prefix) {
  char* prefix_buf = malloc(256);
  prefix2str(prefix, prefix_buf, 256);
  return prefix_buf;
}

int IsPublicPrefix(struct prefix* prefix) {
  // if it is a private ip, then we don't want to do anything with it.
  char* private_ip = GetPrivateIp(pathlet_config_);
  char* prefixstring = Prefix2Str(prefix);
  zlog_debug("pathlets::IsPublicPrefix: prefix is %s", prefixstring);
  free(prefixstring);
  // if it isn't in the same subnet as the private ip, then it is a public ip.
  if (!IpInSameSubnetOneIsInt(prefix->u.prefix4.s_addr, private_ip,
                              CLASS_B_NETMASK)) {
    zlog_debug("pathlets::IsPublicPrefix: It is");
    return 1;
  }

  // here if it is a public prefix
  return 0;
}

void HandlePublicPrefixForExternal(dbgp_control_info_t* control_info,
                                   struct peer* peer, struct attr* attr,
                                   struct prefix* prefix) {
  /* // if it is a private ip, then we don't want to do anything with it. */
  /* char* private_ip = GetPrivateIp(pathlet_config_); */
  /* zlog_debug("pathlets::HandlePublicPrefixForExternal: Peer as is %i", */
  /*            peer->as); */
  /* if (IpInSameSubnetOneIsInt(prefix->u.prefix4.s_addr, private_ip, */
  /*                            CLASS_B_NETMASK)) { */
  /*   return 0; */
  /* } */
  /* // here if it is a public ip */
  /* // if the peer is an internal peer, return 1 (we don't want to mess with */
  /* // control info) */
  /* if (IsRemoteAsAnIslandMember(general_configuration_, peer->as)) { */
  /*   return 1; */
  /* } */

  char* prefix_buf = malloc(256);
  prefix2str(prefix, prefix_buf, 256);
  /* zlog_debug( */
  /*     "pathlets::HandlePublicPrefixForExternal: Going to advertise into the
   * gulf, " */
  /*     "sleeping, ip %s", */
  /*     prefix_buf); */
  /* sleep(20); */
  /* zlog_debug( */
  /*     "pathlets::HandlePublicPrefixForExternal: Going to advertise into the
   * gulf,
   * waking, " */
  /*     "ip %s", */
  /*     prefix_buf); */
  zlog_debug(
      "pathlets::HandlePublicPrefixForExternal: Going to advertise into the "
      "gulf: "
      "ip %s",
      prefix_buf);

  AddExternalPathletControlInfoForAll(peer->bgp->island_id, control_info);
  char* new_aspathstring = malloc(256);
  memset(new_aspathstring, 0, 256);
  snprintf(new_aspathstring, 256, "%d", peer->bgp->island_id);
  attr->aspath = aspath_str2aspath(new_aspathstring);
  zlog_debug("pathlets::HandlePublicPrefixForExternal: set aspath to %s",
             attr->aspath->str);

  free(prefix_buf);

  /* AddExternalPathletControlInfoForDest(prefix, peer->bgp->island_id, */
  /*                                      peer->bgp->as, control_info); */

  /* return 1; */
}

// handles the input for outside the island. This will involve searching for
// pathlet information
void HandleExternalIslandInput(dbgp_control_info_t* control_info,
                               struct peer* peer, struct attr* attr,
                               struct prefix* prefix) {
  // go through each number in aspath and see if that has pathlet info.
  struct assegment* segments = attr->aspath->segments;
  zlog_debug("pathlets::HandleExternalIslandInput: aspath: %s",
             attr->aspath->str);
  zlog_debug(
      "pathlets::HandleExternalIslandInput: External island input tostring %s",
      SerializedAdverToString(control_info->integrated_advertisement,
                              control_info->integrated_advertisement_size));
  while (segments) {
    for (int i = 0; i < segments->length; i++) {
      zlog_debug(
          "pathlets::HandleExternalIslandInput: Check if island_id/as %i has "
          "pathlet info",
          segments->as[i]);
      int has_pathlet_info = HasPathletInformation(
          control_info->integrated_advertisement,
          control_info->integrated_advertisement_size, segments->as[i]);
      if (has_pathlet_info) {
        zlog_debug(
            "pathlets:HandleExternalIslandInput: Advert has control info for "
            "island %i.",
            segments->as[i]);
        /* zlog_debug( */
        /*     "pathlets:HandleExternalIslandInput: Advert has control info. "
         */
        /*     "prefix: %s", */
        /*     prefix_buf); */
        /* zlog_debug( */
        /*     "pathlets::HandleExternalIslandInput: IA control info: \n %s", */
        /*     SerializedAdverToString( */
        /*         control_info->integrated_advertisement, */
        /*         control_info->integrated_advertisement_size)); */
        zlog_debug(
            "pathlets::HandleExternalIslandInput: IA control info: \n %s",
            PrintPathletsFromSerializedAdvert(
                control_info->integrated_advertisement,
                control_info->integrated_advertisement_size,
                segments->as[i]));
      }
    }
    segments = segments->next;
  }
}

/* ********************* Public functions ********************* */

void pathlets_update_control_info(dbgp_control_info_t* control_info,
                                  struct peer* peer, struct attr* attr,
                                  struct prefix* prefix) {
  int originating_as = aspath_get_rightmost(attr->aspath);
  // Is the originating as an island member
  int is_originating_island_member =
      IsRemoteAsAnIslandMember(general_configuration_, originating_as);
  int has_pathlet_info = HasPathletInformation(
      control_info->integrated_advertisement,
      control_info->integrated_advertisement_size, peer->bgp->island_id);

  // handle prefix, and if it is a public one (meant for ases outside the
  // island), return. If aspath is greater than 1, then we just want to
  // propagate it throughout the island.
  if (IsPublicPrefix(prefix)) {
    // if the peer is an island member, then don't do anything to this public
    // prefix
    if (!IsRemoteAsAnIslandMember(general_configuration_, peer->as)) {
      // the prefix is public and the peer is external, handle it (i.e. add
      // control info) And it originated within our island or ourselves.
      if(is_originating_island_member || originating_as == peer->bgp->as){
        HandlePublicPrefixForExternal(control_info, peer, attr, prefix);
      }
      return;
    }
  }

  // case where it has pathlet control info from our island. if it has control
  // info and we didn't originate it, then read the control info and move on
  if (has_pathlet_info && aspath_size_custom(attr->aspath) != 0) {
    zlog_debug(
        "pathlets:update_control_info: This has pathlet information to merge "
        "and we didnt' originate it: "
        "%s",
        PrintPathletsFromSerializedAdvert(
            control_info->integrated_advertisement,
            control_info->integrated_advertisement_size, peer->bgp->island_id));
    MergePathletInformationIntoGraph(
        pathlet_internal_state_, control_info->integrated_advertisement,
        control_info->integrated_advertisement_size, peer->bgp->island_id);
    char* pathlet_graph = GetPathletGraphString(pathlet_internal_state_);
    zlog_debug(
        "pathlets:update_control_info: Pathlet graph:\n %s <endpathletgraph>",
        pathlet_graph);
    free(pathlet_graph);
    // return because we don't want to modify, only learn from this advert
    return;
  }
  // here if there is no pathlet info from our island
  // case where it is external and has pathlet info, merge into adjacency
  // graph
  // associated with island id
  // TODO

  // If we originated the pathlet, then we have pathlet information (we would
  // have filtered it otherwise)
  if (aspath_size_custom(attr->aspath) == 0) {
    char* string_ip = malloc(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &prefix->u.prefix4, string_ip, INET_ADDRSTRLEN);
    zlog_debug("pathlets::pathlets_update_control_info: Next for advert: %s",
               string_ip);
    char* private_ip = GetPrivateIp(pathlet_config_);
    if (!IpInSameSubnet(string_ip, private_ip, CLASS_C_NETMASK)) {
      zlog_debug(
          "pathlets::pathlets_update_control_info: We're announcing the 0 "
          "hop "
          "pathlet, don't add any information to it. %s",
          string_ip);
      free(private_ip);
      free(string_ip);
      return;
    }
    AddAssociatedPathlet(string_ip, peer->bgp->island_id, control_info);
    // Add associated pathlet into the graph
    MergePathletInformationIntoGraph(
        pathlet_internal_state_, control_info->integrated_advertisement,
        control_info->integrated_advertisement_size, peer->bgp->island_id);
    zlog_debug(
        "pathlets::pathlets_update_control_info: advertisement: %s",
        SerializedAdverToString(control_info->integrated_advertisement,
                                control_info->integrated_advertisement_size));
    free(string_ip);
    free(private_ip);
  }
}

void pathlets_update_control_info_bgpstruct(dbgp_control_info_t* control_info,
                                            struct bgp* bgp, struct attr* attr,
                                            struct prefix* prefix) {
  int originating_as = aspath_get_rightmost(attr->aspath);
  // Is the originating as an island member
  int is_originating_island_member =
      IsRemoteAsAnIslandMember(general_configuration_, originating_as);
  int has_pathlet_info = HasPathletInformation(
      control_info->integrated_advertisement,
      control_info->integrated_advertisement_size, bgp->island_id);

  char* string_ip = malloc(INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &prefix->u.prefix4, string_ip, INET_ADDRSTRLEN);
  zlog_debug(
      "pathlets::pathlets_update_control_info_bgp_struct: Next for advert: "
      "%s",
      string_ip);
  char* private_ip = GetPrivateIp(pathlet_config_);
  if (!IpInSameSubnet(string_ip, private_ip, CLASS_C_NETMASK)) {
    zlog_debug(
        "pathlets::pathlets_update_control_info_bgp_struct: This is a public "
        "ip, don't do anything to it"
        "pathlet, don't add any information to it. %s",
        string_ip);
    free(private_ip);
    free(string_ip);
    return;
  }

  // case where it has pathlet control info from our island. if it has control
  // info and we didn't originate it, then read the control info and move on
  if (has_pathlet_info && aspath_size_custom(attr->aspath) != 0) {
    zlog_debug(
        "pathlets:update_control_info: This has pathlet information to merge "
        "and we didnt' originate it: "
        "%s",
        PrintPathletsFromSerializedAdvert(
            control_info->integrated_advertisement,
            control_info->integrated_advertisement_size, bgp->island_id));
    MergePathletInformationIntoGraph(
        pathlet_internal_state_, control_info->integrated_advertisement,
        control_info->integrated_advertisement_size, bgp->island_id);
    char* pathlet_graph = GetPathletGraphString(pathlet_internal_state_);
    zlog_debug(
        "pathlets:update_control_info_bgp_struct: Pathlet graph:\n %s "
        "<endpathletgraph>",
        pathlet_graph);
    free(pathlet_graph);
    // return because we don't want to modify, only learn from this advert
    return;
  }
  // here if there is no pathlet info from our island
  // case where it is external and has pathlet info, merge into adjacency
  // graph
  // associated with island id
  // TODO

  // If we originated the pathlet, then we have pathlet information (we would
  // have filtered it otherwise)
  if (aspath_size_custom(attr->aspath) == 0) {
    AddAssociatedPathlet(string_ip, bgp->island_id, control_info);
    // Add associated pathlet into the graph
    MergePathletInformationIntoGraph(
        pathlet_internal_state_, control_info->integrated_advertisement,
        control_info->integrated_advertisement_size, bgp->island_id);
    zlog_debug(
        "pathlets::pathlets_update_control_info_bgp_struct: advertisement: "
        "%s",
        SerializedAdverToString(control_info->integrated_advertisement,
                                control_info->integrated_advertisement_size));
    free(string_ip);
    free(private_ip);
  }
}

dbgp_filtered_status_t pathlets_input_filter(dbgp_control_info_t* control_info,
                                             struct attr* attr,
                                             struct peer* peer,
                                             struct prefix* prefix) {
  zlog_debug("pathlets::pathlets_input_filter: in pathlets input filter");
  zlog_debug("pathlets::pathlets_input_filter: Aspath size: %i",
             aspath_size_custom(attr->aspath));
  // if an incoming advertisement has control info
  int has_pathlet_info = HasPathletInformation(
      control_info->integrated_advertisement,
      control_info->integrated_advertisement_size, peer->bgp->island_id);

  // if there is pathlet info in it, we don't want to do any filtering, let it
  // pass onto the pathlets_update_control_info
  char* prefix_buf = malloc(256);
  prefix2str(prefix, prefix_buf, 256);
  zlog_debug("pathlets::pathlets_input_filter: Prefix gotten: %s", prefix_buf);
  if (has_pathlet_info == 1) {
    zlog_debug(
        "pathlets:pathlets_input_filter: Advert has control info. aspath: %s",
        attr->aspath->str);
    zlog_debug(
        "pathlets:pathlets_input_filter: Advert has control info. prefix: %s",
        prefix_buf);
    zlog_debug(
        "pathlets::pathlets_input_filter: IA control info: \n %s",
        SerializedAdverToString(control_info->integrated_advertisement,
                                control_info->integrated_advertisement_size));
    zlog_debug(
        "pathlets::pathlets_input_filter: IA control info: \n %s",
        PrintPathletsFromSerializedAdvert(
            control_info->integrated_advertisement,
            control_info->integrated_advertisement_size, peer->bgp->island_id));

    return DBGP_NOT_FILTERED;
  }
  // here if the advert had no pathlet control info.
  // Get the originating as and determine if
  int originating_as = aspath_get_rightmost(attr->aspath);
  // If the as is not a part of our island, then there is no specific pathlet
  // stuff we need to do with it.
  if (IsRemoteAsAnIslandMember(general_configuration_, originating_as) == 0) {
    zlog_debug(
        "pathlets::pathlets_input_filter: Control info from outside the "
        "island");
    HandleExternalIslandInput(control_info, peer, attr, prefix);
    // filter because we don't want this stuff to go further into island,
    // handleexternlaislandinput is going to create and announce the proper
    // pathlets
    return DBGP_NOT_FILTERED; //there is a problem when filtering this WHY?
  }
  // If it is a public IP and the aspath is > 1, that means this is a public
  // ip
  // that just neeeds to propagate around because the 1 hop for it has already
  // been announced. This is so it gets to the edge of the island to be pushed
  // out into the gulf
  if (IsPublicPrefix(prefix) && aspath_size_custom(attr->aspath) > 1) {
    zlog_debug(
        "pathlets::pathlets_input_filter: This is a public IP, don't create "
        "another pathlet: prefix: %s",
        prefix_buf);
    return DBGP_NOT_FILTERED;
  }
  // Here if there is no pathlet info and the originating as was a island
  // member
  // and the aspath is < 1. This means that this as is advertising a 1 hop
  // location. We need to create a pathlet and originate a new prefix with
  // this
  // pathlet.
  char* new_ip = GetNextIp(pathlet_internal_state_);
  int free_fid = GetNextFid(pathlet_internal_state_);
  int as1 = peer->bgp->as;
  int as2 = peer->as;
  zlog_debug(
      "pathlets::pathlets_input_filter: creating pathlet for ip (%s) fid "
      "(%i) "
      "as1(%i) as2(%i) with dest (%s)",
      new_ip, free_fid, as1, as2, prefix_buf);
  InsertPathletToSend(pathlet_internal_state_, new_ip, free_fid, as1, as2,
                      prefix_buf);
  zlog_debug("pathlets::pathlets_input_filter: ip_to_pathlets_to_send:\n %s",
             GetPathletsToSendString(pathlet_internal_state_));

  // create ip to announce (/32)
  char* announce_ip = malloc(256);
  snprintf(announce_ip, 256, "%s/%d", new_ip, 32);

  // announce it
  AnounceStaticRoute(announce_ip, peer->bgp);
  /* zlog_debug("pathlets::pathlets_input_filter: sleep after announcing"); */
  /* sleep(5); */

  free(new_ip);
  free(announce_ip);
  // filter it because it is the first instance of a 1 hop pathlet
  /* return DBGP_FILTERED; */
  // not filtering anymore, we just won't do anything with it until it reaches
  // a
  // border router
  return DBGP_NOT_FILTERED;
}

dbgp_filtered_status_t pathlets_output_filter(
    /* dbgp_control_info_t* control_info, */ struct attr* attr,
    struct peer* peer, struct prefix* prefix) {
  int peer_apart_of_island =
      IsRemoteAsAnIslandMember(general_configuration_, peer->as);
  int originated_in_island = IsRemoteAsAnIslandMember(
      general_configuration_, aspath_get_rightmost(attr->aspath));
  // NOTE: ADDED this type of check in update control info.
  // If the ip is not in the /24 of our private pathlet space, then we are
  // either originating it or it comes from outside. Do nothing in that case.
  /* char* private_ip = GetPrivateIp(pathlet_config_); */
  /* char* compare_ip = malloc(INET_ADDRSTRLEN); */
  /* inet_ntop(AF_INET, &prefix->u.prefix4, compare_ip, INET_ADDRSTRLEN); */
  /* zlog_debug( */
  /*     "pathlets::pathlets_output_filter: comparing private %s, with compare
   * %s", */
  /*     private_ip, compare_ip); */
  /* if (!IpInSameSubnet(private_ip, compare_ip, CLASS_C_NETMASK)) { */
  /*   zlog_debug( */
  /*       "pathlets::pathlets_output_filter: don't do anything to an advert
   * we
   * " */
  /*       "originated"); */
  /*   // clean up the memory */
  /*   free(private_ip); */
  /*   free(compare_ip); */
  /*   return DBGP_NOT_FILTERED; */
  /* } */
  // clean up in case they are a part of the private address space
  /* free(private_ip); */
  /* free(compare_ip); */

  // We want to filter outgoing only if the peer is external to island and ip
  // advertising is in the private class b range of the island.
  char* private_ip;
  private_ip = GetPrivateIp(pathlet_config_);
  int is_private_ip = IpInSameSubnetOneIsInt(prefix->u.prefix4.s_addr,
                                             private_ip, CLASS_B_NETMASK);
  free(private_ip);
  if (!peer_apart_of_island && is_private_ip) {
    char* prefix_buf = malloc(256);
    prefix2str(prefix, prefix_buf, 256);
    zlog_debug(
        "pathlets::pathlets_output_filter: Peer %i not in island and aspath "
        "%s is a private ip %s",
        peer->as, attr->aspath->str, prefix_buf);
    /* char* private_prefix = GetPrivateIp(pathlet_config_); */

    return DBGP_FILTERED;
  }
  dbgp_update_control_info(attr, peer, prefix);
  if (!peer_apart_of_island) {
    zlog_debug("pathlets::pathlets_output_filter: sleeping");
    sleep(20);
  }

  return DBGP_NOT_FILTERED;
}

/* int pathlets_info_cmp (struct bgp *bgp, struct bgp_info *new, struct bgp_info
 * *exist, */
/*                        int *paths_eq){ */
/*   return 0; */
/* } */
