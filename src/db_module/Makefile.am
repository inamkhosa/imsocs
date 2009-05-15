

h_sources = ocsdb.h

c_sources = ocsdb.c

INCLUDES = -I. -I$(top_srcdir)/src $(LIBAMPS_DEPS_CFLAGS) $(top_srcdir)/src $(LIBXML_DEPS_CFLAGS) $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS)

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) $(LIBXML_DEPS_LIBS)

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

lib_LTLIBRARIES= libonlinecharg_db.la
libonlinecharg_db_la_SOURCES= $(h_sources) $(c_sources)
libonlinecharg_db_la_LDFLAGS= -version-info $(LIBRARY_VERSION) -release $(RELEASE)
