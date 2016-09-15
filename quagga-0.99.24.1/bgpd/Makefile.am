## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib -I /home/david/third-party/googletest-release-1.8.0/googletest/include
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(PICFLAGS) $(PROTOBUF_CFLAGS) -g -O0
AM_LDFLAGS = $(PILDFLAGS)

noinst_LIBRARIES = libbgp.a
sbin_PROGRAMS = bgpd unittests

libbgp_a_SOURCES = bgpd.c bgp_fsm.c bgp_aspath.c bgp_community.c bgp_attr.c		\
	bgp_debug.c bgp_route.c bgp_zebra.c bgp_open.c bgp_routemap.c bgp_packet.c	\
	bgp_network.c bgp_filter.c bgp_regex.c bgp_clist.c bgp_dump.c bgp_snmp.c		\
	bgp_ecommunity.c bgp_mplsvpn.c bgp_nexthop.c bgp_damp.c bgp_table.c					\
	bgp_advertise.c bgp_vty.c bgp_mpath.c dbgp_lookup.c dbgp.c									\
	quagga_config.pb.cc wiser_config.cc wiser_config_interface.cpp wiser.c			\
	bgp_common.c general_configuration.cpp integrated_advertisement.pb.cc				\
	integrated_advertisement_functions.cpp pathlets.cpp

noinst_HEADERS = bgp_aspath.h bgp_attr.h bgp_community.h bgp_debug.h bgp_fsm.h	\
	bgp_network.h bgp_open.h bgp_packet.h bgp_regex.h bgp_route.h bgpd.h					\
	bgp_filter.h bgp_clist.h bgp_dump.h bgp_zebra.h bgp_ecommunity.h							\
	bgp_mplsvpn.h bgp_nexthop.h bgp_damp.h bgp_table.h bgp_advertise.h bgp_snmp.h	\
	bgp_vty.h bgp_mpath.h dbgp_lookup.h dbgp.h quagga_config.pb.h wiser_config.h	\
	wiser_config_interface.h wiser.h bgp_common.h general_configuration.h					\
	shared_enum_definitions.h integrated_advertisement.pb.cc											\
	integrated_advertisement_functions.h pathlets.h pathlet_config.h

bgpd_SOURCES = bgp_main.c
bgpd_LDADD = libbgp.a ../lib/libzebra.la @LIBCAP@ @LIBM@ @LIBHIREDIS@ @PROTOBUF_LIBS@ @libstdcxx_LIBS@ 

unittests_SOURCES = wiser_config_test.cc
unittests_LDADD = libbgp.a /home/david/third-party/googletest-release-1.8.0/googletest/libgtest.so @PROTOBUF_LIBS@ @libstdcxx_LIBS@

examplesdir = $(exampledir)
dist_examples_DATA = bgpd.conf.sample bgpd.conf.sample2

EXTRA_DIST = BGP4-MIB.txt
