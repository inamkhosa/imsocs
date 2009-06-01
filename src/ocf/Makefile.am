

h_sources = ocf_ebcf.h ocf_msgextractor.h ocf_session.h ocf.h ocf_sbcf.h ocf_statemachine.h

c_sources = ocf.c ocf_msgextractor.c ocf_session.c ocf_ebcf.c ocf_sbcf.c ocf_statemachine.c

INCLUDES = -I. -I$(top_srcdir)/src -I$(top_srcdir)/include/amps -I$(top_srcdir)/include/libxml2 $(LIBXML_DEPS_CFLAGS) $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS)

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) -L/usr/local/lib -lxml2 -lamps

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

lib_LTLIBRARIES= libocf.la
libocf_la_SOURCES= $(h_sources) $(c_sources)
libocf_la_LDFLAGS= -version-info $(LIBRARY_VERSION) -release $(RELEASE)
