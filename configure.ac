# Process this file with autoconf to create configure.

AC_INIT([cryptkeeper], [0.9.3], [t-morton@blueyonder.co.uk])
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

#PKG_CHECK_MODULES(GNOME_KEYRING, gnome-keyring-1, CFLAGS="$CFLAGS -DUSE_GNOME_KEYRING", echo "Gnome Keyring support disabled")
PKG_CHECK_MODULES(DEP, gtk+-2.0 >= 2.8 gconf-2.0)

#AC_SUBST(DEP_CFLAGS)
#AC_SUBST(DEP_LIBS)

CFLAGS="$CFLAGS $DEP_CFLAGS $GNOME_KEYRING_CFLAGS"
CXXFLAGS="$CFLAGS"
LIBS="$DEP_LIBS $GNOME_KEYRING_LIBS"

#AC_PROG_LIBTOOL

AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in])
AC_OUTPUT
