SUBDIRS= ocf ratingfunc db_module 

bin_PROGRAMS = onlinecharging

onlinecharging_SOURCES = onlinecharging.h onlinecharging.c 

INCLUDES = $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS) $(LIBXML_DEPS_CFLAGS) $(LIBAMPS_DEPS_CFLAGS)

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) $(LIBXML_DEPS_LIBS) $(LIBAMPS_DEPS_LIBS)

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

onlinechargingextradir = $(datadir)/@PACKAGE@
onlinechargingextra_DATA = $(top_srcdir)/config/*
