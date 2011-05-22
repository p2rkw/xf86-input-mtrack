@DRIVER_NAME@_drv_la_LTLIBRARIES = @DRIVER_NAME@_drv.la
@DRIVER_NAME@_drv_la_LDFLAGS = -module -avoid-version
@DRIVER_NAME@_drv_la_SOURCES = @DRIVER_NAME@.c mprops.c
@DRIVER_NAME@_drv_la_LIBADD = $(abs_top_srcdir)/src/libmtrack.la
@DRIVER_NAME@_drv_ladir = @inputdir@

AM_CPPFLAGS = -I$(top_srcdir)/include/ \
	-I/usr/include/xorg \
	-I/usr/include/pixman-1
AM_CFLAGS = $(XORG_CFLAGS)
