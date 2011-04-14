/***************************************************************************
 *
 * Multitouch X driver
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
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

#include "capabilities.h"

#define SETABS(c, x, map, key, fd)					\
	(c->has_##x = getbit(map, key) && getabs(&c->x, key, fd))

#define ADDCAP(s, c, x) strcat(s, c->has_##x ? " " #x : "")

static const int SN_COORD = 250;	/* coordinate signal-to-noise ratio */
static const int SN_WIDTH = 100;	/* width signal-to-noise ratio */
static const int SN_ORIENT = 10;	/* orientation signal-to-noise ratio */

static const int bits_per_long = 8 * sizeof(long);

static inline int nlongs(int nbit)
{
	return (nbit + bits_per_long - 1) / bits_per_long;
}

static inline int getbit(const unsigned long *map, int key)
{
	return (map[key / bits_per_long] >> (key % bits_per_long)) & 0x01;
}

static int getabs(struct input_absinfo *abs, int key, int fd)
{
	int rc;
	SYSCALL(rc = ioctl(fd, EVIOCGABS(key), abs));
	return rc >= 0;
}

static int has_mt_data(const struct Capabilities *cap)
{
	return cap->has_abs[MTDEV_POSITION_X] && cap->has_abs[MTDEV_POSITION_Y];
}

static int has_integrated_button(const struct Capabilities *cap)
{
	static const int bcm5974_vmask_ibt = 1;
	/* magic trackpad */
	if (cap->devid.vendor == 0x05ac && cap->devid.product == 0x030e)
		return 1;
	/* macbooks */
	if (strcmp(cap->devname, "bcm5974"))
		return 0;
	return cap->devid.version & bcm5974_vmask_ibt;
}

static void default_fuzz(struct Capabilities *cap, unsigned int code, int sn)
{
	int bit = mtdev_abs2mt(code);
	if (cap->has_abs[bit] && cap->abs[bit].fuzz == 0)
		cap->abs[bit].fuzz =
			(cap->abs[bit].maximum - cap->abs[bit].minimum) / sn;
}

int read_capabilities(struct Capabilities *cap, int fd)
{
	unsigned long evbits[nlongs(EV_MAX)];
	unsigned long absbits[nlongs(ABS_MAX)];
	unsigned long keybits[nlongs(KEY_MAX)];
	int rc, i;

	memset(cap, 0, sizeof(struct Capabilities));

	SYSCALL(rc = ioctl(fd, EVIOCGID, &cap->devid));
	if (rc < 0)
		return rc;
	SYSCALL(rc = ioctl(fd, EVIOCGNAME(sizeof(cap->devname)), cap->devname));
	if (rc < 0)
		return rc;
	SYSCALL(rc = ioctl(fd, EVIOCGBIT(EV_SYN, sizeof(evbits)), evbits));
	if (rc < 0)
		return rc;
	SYSCALL(rc = ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybits)), keybits));
	if (rc < 0)
		return rc;
	SYSCALL(rc = ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absbits)), absbits));
	if (rc < 0)
		return rc;

	cap->has_left = getbit(keybits, BTN_LEFT);
	cap->has_middle = getbit(keybits, BTN_MIDDLE);
	cap->has_right = getbit(keybits, BTN_RIGHT);

	SETABS(cap, slot, absbits, ABS_MT_SLOT, fd);
	for (i = 0; i < MT_ABS_SIZE; i++)
		SETABS(cap, abs[i], absbits, mtdev_mt2abs(i), fd);

	cap->has_mtdata = has_mt_data(cap);
	cap->has_ibt = has_integrated_button(cap);

	default_fuzz(cap, ABS_MT_POSITION_X, SN_COORD);
	default_fuzz(cap, ABS_MT_POSITION_Y, SN_COORD);
	default_fuzz(cap, ABS_MT_TOUCH_MAJOR, SN_WIDTH);
	default_fuzz(cap, ABS_MT_TOUCH_MINOR, SN_WIDTH);
	default_fuzz(cap, ABS_MT_WIDTH_MAJOR, SN_WIDTH);
	default_fuzz(cap, ABS_MT_WIDTH_MINOR, SN_WIDTH);
	default_fuzz(cap, ABS_MT_ORIENTATION, SN_ORIENT);

	return 0;
}

int get_cap_xsize(const struct Capabilities *cap)
{
	const struct input_absinfo *x = &cap->abs[MTDEV_POSITION_X];
	return x->maximum - x->minimum;
}

int get_cap_ysize(const struct Capabilities *cap)
{
	const struct input_absinfo *y = &cap->abs[MTDEV_POSITION_Y];
	return y->maximum - y->minimum;
}

int get_cap_wsize(const struct Capabilities *cap)
{
	const struct input_absinfo *w = &cap->abs[MTDEV_TOUCH_MAJOR];
	return w->maximum - w->minimum;
}

int get_cap_xmid(const struct Capabilities *cap)
{
	const struct input_absinfo *x = &cap->abs[MTDEV_POSITION_X];
	return (x->maximum + x->minimum) >> 1;
}

int get_cap_ymid(const struct Capabilities *cap)
{
	const struct input_absinfo *y = &cap->abs[MTDEV_POSITION_Y];
	return (y->maximum + y->minimum) >> 1;
}

void output_capabilities(const struct Capabilities *cap)
{
	char line[1024];
	int i;
	memset(line, 0, sizeof(line));
	ADDCAP(line, cap, left);
	ADDCAP(line, cap, middle);
	ADDCAP(line, cap, right);
	ADDCAP(line, cap, mtdata);
	ADDCAP(line, cap, ibt);
	xf86Msg(X_INFO, "multitouch: devname: %s\n", cap->devname);
	xf86Msg(X_INFO, "multitouch: devid: %x %x %x\n",
		cap->devid.vendor, cap->devid.product, cap->devid.version);
	xf86Msg(X_INFO, "multitouch: caps:%s\n", line);
	for (i = 0; i < MT_ABS_SIZE; i++) {
		if (cap->has_abs[i])
			xf86Msg(X_INFO, "multitouch: %d: min: %d max: %d\n",
				i,
				cap->abs[i].minimum,
				cap->abs[i].maximum);
	}
}
