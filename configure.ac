# Process this file with autoconf to create configure.

AC_INIT([cryptkeeper], [0.8.666], [t-morton@blueyonder.co.uk])
AC_PREREQ(2.59)
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.9 foreign])

AC_PROG_CC
AC_GNU_SOURCE
AC_PROG_CXX
AC_C_CONST
AC_STDC_HEADERS

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.16])

PKG_CHECK_MODULES(GCONF, gconf-2.0)
PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.8 gconf-2.0)

#AC_SUBST(GTK_CFLAGS)
#AC_SUBST(GTK_LIBS)

CFLAGS="-Wall -O2 $GTK_CFLAGS"
CXXFLAGS="$CFLAGS"
LIBS="$GTK_LIBS"

#AC_PROG_LIBTOOL

AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in])
AC_OUTPUT
