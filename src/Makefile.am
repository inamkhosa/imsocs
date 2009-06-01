SUBDIRS= ocf ratingfunc db_module 

bin_PROGRAMS = onlinecharging

onlinecharging_SOURCES = onlinecharging.h onlinecharging.c 

INCLUDES = $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS) -I$(top_srcdir)/include/amps -I$(top_srcdir)/include/libxml2

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) -L/usr/local/lib -lxml2 -lamps

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

onlinechargingextradir = $(datadir)/@PACKAGE@
onlinechargingextra_DATA = $(top_srcdir)/config/*.xml
