

h_sources = ratingfunc.h

c_sources = ratingfunc.c

INCLUDES = -I. -I$(top_srcdir)/src $(LIBAMPS_DEPS_CFLAGS) $(top_srcdir)/src $(LIBXML_DEPS_CFLAGS) $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS)

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) $(LIBXML_DEPS_LIBS)

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

lib_LTLIBRARIES= libratingfunc.la
libratingfunc_la_SOURCES= $(h_sources) $(c_sources)
libratingfunc_la_LDFLAGS= -version-info $(LIBRARY_VERSION) -release $(RELEASE)
