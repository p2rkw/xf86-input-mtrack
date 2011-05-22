noinst_LTLIBRARIES = libmtrack.la
libmtrack_la_SOURCES = capabilities.c gestures.c hwstate.c mconfig.c mtouch.c \
	mtstate.c trig.c

AM_CPPFLAGS = -I$(top_srcdir)/include/ \
	-I/usr/include/xorg \
	-I/usr/include/pixman-1
AM_CFLAGS = $(XORG_CFLAGS)
