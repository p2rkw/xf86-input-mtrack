# Initialize Autoconf
AC_PREREQ([2.60])
AC_INIT([xf86-input-mtrack],
	[0.2.0],
	[https://bugs.freedesktop.org/enter_bug.cgi?product=xorg],
	[xf86-input-mtrack])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR(.)

# Initialize Automake
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE

# Initialize libtool
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# Initialize X.Org macros 1.8 or later for MAN_SUBSTS set by XORG_MANPAGE_SECTIONS
m4_ifndef([XORG_MACROS_VERSION],
          [m4_fatal([must install xorg-macros 1.8 or later before running autoconf/autogen])])
XORG_MACROS_VERSION(1.8)
XORG_DEFAULT_OPTIONS

# Checks for libraries.
AC_CHECK_LIB([mtdev], [mtdev_open])
AC_CHECK_LIB([m], [atan2])

# Obtain compiler/linker options for the mtrack driver dependencies
PKG_CHECK_MODULES(XORG, [xorg-server >= 1.7] xproto inputproto $REQUIRED_MODULES)

# Set driver name
DRIVER_NAME=mtrack
AC_SUBST([DRIVER_NAME])

# configure option for module install directory
AC_ARG_WITH(xorg-module-dir, AC_HELP_STRING([--with-xorg-module-dir=DIR],
	[Default xorg module directory [[default=$libdir/xorg/modules]]]),
	[moduledir="$withval"],
	[moduledir="$libdir/xorg/modules"])
inputdir=${moduledir}/input
AC_SUBST(inputdir)

# configure option to build extra tools
AC_ARG_ENABLE(tools, AC_HELP_STRING([--enable-tools],
	[Build extra tools (default: disabled)]),
	[ENABLE_TOOLS=yes],
	[ENABLE_TOOLS=no])
AM_CONDITIONAL([BUILD_TOOLS], [test "x$ENABLE_TOOLS" = xyes])

# configure option to enable gesture ate debugging
AC_ARG_ENABLE(debug-gestures, AS_HELP_STRING([--enable-debug-gestures],
                                    [Enable gesture debugging (default: disabled)]),
                                    [DEBUG_GESTURES=yes],
									[DEBUG_GESTURES=no])
if test "x$DEBUG_GESTURES" = xyes; then
   AC_DEFINE(DEBUG_GESTURES, 1, [Enable gesture debugging.])
fi

# configure option to enable multitouch state debugging
AC_ARG_ENABLE(debug-mtstate, AS_HELP_STRING([--enable-debug-mtstate],
                                    [Enable multitouch state debugging (default: disabled)]),
                                    [DEBUG_MTSTATE=yes],
									[DEBUG_MTSTATE=no])
if test "x$DEBUG_MTSTATE" = xyes; then
   AC_DEFINE(DEBUG_MTSTATE, 1, [Enable multitouch state debugging.])
fi

# configure option to enable property debugging
AC_ARG_ENABLE(debug-props, AS_HELP_STRING([--enable-debug-props],
                                    [Enable property debugging (default: disabled)]),
                                    [DEBUG_PROPS=yes],
									[DEBUG_PROPS=no])
if test "x$DEBUG_PROPS" = xyes; then
   AC_DEFINE(DEBUG_PROPS, 1, [Enable property debugging.])
fi

# configure option to enable driver debugging
AC_ARG_ENABLE(debug-driver, AS_HELP_STRING([--enable-debug-driver],
                                    [Enable property debugging (default: disabled)]),
                                    [DEBUG_DRIVER=yes],
									[DEBUG_DRIVER=no])
if test "x$DEBUG_DRIVER" = xyes; then
   AC_DEFINE(DEBUG_DRIVER, 1, [Enable property debugging.])
fi


# configure option to enable all debugging
AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug],
                                    [Enable all debugging (default: disabled)]),
                                    [DEBUG_ALL=$enableval],
									[DEBUG_ALL=no])
if test "x$DEBUG_ALL" = xyes; then
   AC_DEFINE(DEBUG_GESTURES, 1, [Enable gesture debugging.])
   AC_DEFINE(DEBUG_MTSTATE, 1, [Enable multitouch state debugging.])
   AC_DEFINE(DEBUG_PROPS, 1, [Enable property debugging.])
   AC_DEFINE(DEBUG_DRIVER, 1, [Enable driver debugging.])
fi

# Everything else
AC_PROG_CC
AC_PROG_INSTALL
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

