

h_sources = ocsdb.h

c_sources = ocsdb.c

INCLUDES = -I. -I$(top_srcdir)/src -I$(top_srcdir)/include/amps -I$(top_srcdir)/include/libxml2 $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS)

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) -L/usr/local/lib -lxml2 -lamps

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

lib_LTLIBRARIES= libonlinecharg_db.la
libonlinecharg_db_la_SOURCES= $(h_sources) $(c_sources)
libonlinecharg_db_la_LDFLAGS= -version-info $(LIBRARY_VERSION) -release $(RELEASE)
