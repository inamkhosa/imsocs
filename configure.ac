dnl --------------------------------
dnl Initialization macros.
dnl --------------------------------

AC_INIT([onlinecharging],[1.0.0])
AC_PREREQ(2.59)

TOPLEVEL_CONFIGURE_ARGUMENTS=""
AC_SUBST(TOPLEVEL_CONFIGURE_ARGUMENTS)

PACKAGE=onlinecharging

MAJOR_VERSION=1
MINOR_VERSION=0
MICRO_VERSION=0

VERSION=$MAJOR_VERSION.$MINOR_VERSION.$MICRO_VERSION
RELEASE=$MAJOR_VERSION.$MINOR_VERSION

PACKAGE_VERSION=$VERSION

#shared library versioning
LIBRARY_VERSION=$MAJOR_VERSION:$MINOR_VERSION:$MICRO_VERSION
#                       | | |
#                +------+ | +---+
#                |        |     |
#             current:revision:age
#                |        |     |
#                |        |     +- increment if interfaces have been added
#                |        |        set to zero if interfaces have been removed
#                                  or changed
#                |        +- increment if source code has changed
#                |           set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed


AC_SUBST(LIBRARY_VERSION)
AC_SUBST(PACKAGE)
AC_SUBST(PACKAGE_VERSION)
AC_SUBST(VERSION)
AC_SUBST(RELEASE)

AC_CONFIG_SRCDIR([src/onlinecharging.c])
AM_CONFIG_HEADER([config.h])

dnl -----------------------------------------------
dnl Package name and version number (user defined)
dnl -----------------------------------------------

AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define)
AC_LANG_CPLUSPLUS

dnl -----------------------------------------------
dnl Checks for programs.
dnl -----------------------------------------------

AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AM_SANITY_CHECK

dnl -----------------------------------------------
dnl Checks for libraries.
dnl -----------------------------------------------

PKG_CHECK_MODULES(DIAMETER_DEPS, libdiameter-1.0 >= 1.0.0)
AC_SUBST(DIAMETER_DEPS_CFLAGS)
AC_SUBST(DIAMETER_DEPS_LIBS)

PKG_CHECK_MODULES(DIAMAPPCOMMON_DEPS, libdiamappcommon-1.0 >= 1.0.0)
AC_SUBST(DIAMAPPCOMMON_DEPS_CFLAGS)
AC_SUBST(DIAMAPPCOMMON_DEPS_LIBS)

dnl -----------------------------------------------
dnl Checks for header files.
dnl -----------------------------------------------

AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([unistd.h])


dnl -----------------------------------------------
dnl Checks for typedefs, structures, and compiler characteristics.
dnl -----------------------------------------------


AC_C_CONST


dnl -----------------------------------------------
dnl Checks for functions.
dnl -----------------------------------------------

AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero])
AC_CHECK_FUNCS([getcwd])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([strtol])

dnl -----------------------------------------------
dnl Generates Makefiles.
dnl -----------------------------------------------

AC_CONFIG_FILES(Makefile \
		src/Makefile \
		src/ocf/Makefile \
		src/ratingfunc/Makefile \
		src/db_module/Makefile 
)

dnl -----------------------------------------------
dnl Generates Makefiles.
dnl -----------------------------------------------

AC_OUTPUT

