AC_PREREQ([2.60])
AC_INIT([xf86-input-mtrack], [0.2.0])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_INSTALL
AC_CHECK_LIB([mtdev], [mtdev_get])
AC_CONFIG_FILES([Makefile src/Makefile driver/Makefile tools/Makefile])
AC_OUTPUT
