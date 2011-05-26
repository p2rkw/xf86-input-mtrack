SOURCES_COMMON = \
	$(srcdir)/src/capabilities.c \
	$(srcdir)/src/gestures.c \
	$(srcdir)/src/hwstate.c \
	$(srcdir)/src/mconfig.c \
	$(srcdir)/src/mtouch.c \
	$(srcdir)/src/mtstate.c \
	$(srcdir)/src/trig.c

@DRIVER_NAME@_drv_la_LTLIBRARIES = @DRIVER_NAME@_drv.la
@DRIVER_NAME@_drv_la_LDFLAGS = -module -avoid-version
@DRIVER_NAME@_drv_la_SOURCES = $(SOURCES_COMMON) \
	$(srcdir)/driver/mtrack.c \
	$(srcdir)/driver/mprops.c
@DRIVER_NAME@_drv_ladir = @inputdir@

bin_PROGRAMS = mtrack-test
mtrack_test_SOURCES = $(SOURCES_COMMON) \
	$(srcdir)/tools/mtrack-test.c
mtrack_test_CFLAGS = $(AM_CFLAGS)

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
