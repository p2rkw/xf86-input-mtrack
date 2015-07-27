SOURCES_COMMON = \
	src/capabilities.c \
	src/gestures.c \
	src/hwstate.c \
	src/mconfig.c \
	src/mtouch.c \
	src/mtstate.c \
	src/trig.c

@DRIVER_NAME@_drv_la_LTLIBRARIES = @DRIVER_NAME@_drv.la
@DRIVER_NAME@_drv_la_LDFLAGS = -module -avoid-version
@DRIVER_NAME@_drv_la_SOURCES = $(SOURCES_COMMON) \
	driver/mtrack.c \
	driver/mprops.c
@DRIVER_NAME@_drv_ladir = @inputdir@

noinst_PROGRAMS = mtrack-test
mtrack_test_SOURCES = $(SOURCES_COMMON) \
	tools/mtrack-test.c
mtrack_test_CFLAGS = $(AM_CFLAGS)

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/include/ \
	-I/usr/include/xorg \
	-I/usr/include/pixman-1
AM_CFLAGS = $(XORG_CFLAGS)

.PHONY: ChangeLog INSTALL

INSTALL:
	$(INSTALL_CMD)

ChangeLog:
	$(CHANGELOG_CMD)

dist-hook: ChangeLog INSTALL
