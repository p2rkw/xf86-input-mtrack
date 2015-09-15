/***************************************************************************
 *
 * Multitouch X driver
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
 * Copyright (C) 2011 Ryan Bourgeois <bluedragonx@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 **************************************************************************/

#include "mtouch.h"
#include <fcntl.h>
#include <stdarg.h>

void xf86Msg(int type, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) <= 13
typedef XF86OptionPtr pointer;
#endif

int xf86SetIntOption(XF86OptionPtr opts, const char *name, int deflt)
{
	return deflt;
}

int xf86SetBoolOption(XF86OptionPtr opts, const char *name, int deflt)
{
	return deflt;
}

double xf86SetRealOption(XF86OptionPtr opts, const char *name, double deflt)
{
	return deflt;
}

static void print_gestures(const struct Gestures* gs)
{
	int i;
	static bitmask_t buttons_prev = 0U;
	for (i = 0; i < 32; i++) {
		if (GETBIT(gs->buttons, i) == GETBIT(buttons_prev, i))
			continue;

		if (GETBIT(gs->buttons, i))
			printf("button %d down\n", i+1);
		else
			printf("button %d up\n", i+1);
	}

	if (gs->move_dx != 0 || gs->move_dy != 0)
		printf("moving (%+4d, %+4d)\n", gs->move_dx, gs->move_dy);

	buttons_prev = gs->buttons;
}

static void loop_device(int fd)
{
	struct MTouch mt;
	if (mtouch_configure(&mt, fd)) {
		fprintf(stderr, "error: could not configure device\n");
		return;
	}
	if (mtouch_open(&mt, fd)) {
		fprintf(stderr, "error: could not open device\n");
		return;
	}

	mconfig_defaults(&mt.cfg);
	printf("width:  %d\n", mt.hs.max_x);
	printf("height: %d\n", mt.hs.max_y);

	//while (!mtdev_idle(&mt.dev, fd, 5000)) {
	while (1) {
		while (mtouch_read(&mt) > 0)
			print_gestures(&mt.gs);
		if (mtouch_delayed(&mt))
			print_gestures(&mt.gs);
	}
	mtouch_close(&mt);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: test <mtdev>\n");
		return -1;
	}
	int fd = open(argv[1], O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr, "error: could not open file\n");
		return -1;
	}
	loop_device(fd);
	close(fd);
	return 0;
}
