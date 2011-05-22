noinst_PROGRAMS = mtrack-test
mtrack_test_SOURCES = mtrack-test.c
mtrack_test_LDADD = $(abs_top_srcdir)/src/libmtrack.la

AM_CPPFLAGS = -I$(top_srcdir)/include/ \
	-I/usr/include/xorg \
	-I/usr/include/pixman-1
AM_CFLAGS = $(XORG_CFLAGS)
