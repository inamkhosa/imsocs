

h_sources = ratingfunc.h

c_sources = ratingfunc.c

INCLUDES = -I. -I$(top_srcdir)/src -I$(top_srcdir)/include/amps -I$(top_srcdir)/include/libxml2 $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS)

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) -L/usr/local/lib -lxml2 -lamps

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

lib_LTLIBRARIES= libratingfunc.la
libratingfunc_la_SOURCES= $(h_sources) $(c_sources)
libratingfunc_la_LDFLAGS= -version-info $(LIBRARY_VERSION) -release $(RELEASE)
