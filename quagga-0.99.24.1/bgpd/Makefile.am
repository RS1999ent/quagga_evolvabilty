## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS =  $(PICFLAGS) 
#One can also put -lprotobuf and -lgtest to link in google deps if your system is setup properly
#-lstdc++ to get the wiser_config_interface wrapper to link properly
AM_LDFLAGS = $(PILDFLAGS) -lprotobuf

noinst_LIBRARIES = libbgp.a
# sbin_PROGRAMS = bgpd unittests
sbin_PROGRAMS = bgpd

libbgp_a_SOURCES = bgpd.c bgp_fsm.c bgp_aspath.c bgp_community.c bgp_attr.c		\
	bgp_debug.c bgp_route.c bgp_zebra.c bgp_open.c bgp_routemap.c bgp_packet.c	\
	bgp_network.c bgp_filter.c bgp_regex.c bgp_clist.c bgp_dump.c bgp_snmp.c		\
	bgp_ecommunity.c bgp_mplsvpn.c bgp_nexthop.c bgp_damp.c bgp_table.c					\
	bgp_advertise.c bgp_vty.c bgp_mpath.c quagga_config.pb.cc wiser_config.cc		\
	wiser_config_interface.cpp

noinst_HEADERS = bgp_aspath.h bgp_attr.h bgp_community.h bgp_debug.h bgp_fsm.h	\
	bgp_network.h bgp_open.h bgp_packet.h bgp_regex.h bgp_route.h bgpd.h					\
	bgp_filter.h bgp_clist.h bgp_dump.h bgp_zebra.h bgp_ecommunity.h							\
	bgp_mplsvpn.h bgp_nexthop.h bgp_damp.h bgp_table.h bgp_advertise.h bgp_snmp.h	\
	bgp_vty.h bgp_mpath.h quagga_config.pb.h wiser_config.h												\
	wiser_config_interface.h

bgpd_SOURCES = bgp_main.c
# bgpd_LDADD = libbgp.a ../lib/libzebra.la @LIBCAP@ @LIBM@ /usr/local/lib/libprotobuf.so /usr/local/lib/libprotobuf-lite.so
bgpd_LDADD = libbgp.a ../lib/libzebra.la @LIBCAP@ @LIBM@ /home/david/third-party/apps/protobuf-2.6.0/src/libprotobuf.la /home/david/third-party/apps/protobuf-2.6.0/src/libprotobuf-lite.la

# unittests_SOURCES = wiser_config_test.cc
# unittests_LDADD = libbgp.a ../lib/libzebra.la @LIBCAP@ @LIBM@ ~/third-party/libs/googletest/googletest
# unittests_LDADD = libbgp.a /usr/lib/libgtest.so /usr/local/lib/libprotobuf.so /usr/local/lib/libprotobuf-lite.so

examplesdir = $(exampledir)
dist_examples_DATA = bgpd.conf.sample bgpd.conf.sample2

EXTRA_DIST = BGP4-MIB.txt

