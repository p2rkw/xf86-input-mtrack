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

static const int use_grab = 0;

int mtouch_configure(struct MTouch *mt, int fd)
{
	int rc = read_capabilities(&mt->caps, fd);
	if (rc < 0)
		return rc;
	output_capabilities(&mt->caps);
	return 0;
}


int mtouch_open(struct MTouch *mt, int fd)
{
	int ret;
	ret = mtdev_open(&mt->dev, fd);
	if (ret)
		goto error;
	mconfig_init(&mt->cfg, &mt->caps);
	hwstate_init(&mt->hs, &mt->caps);
	mtstate_init(&mt->state);
	gestures_init(&mt->gs);
	if (use_grab) {
		SYSCALL(ret = ioctl(fd, EVIOCGRAB, 1));
		if (ret)
			goto close;
	}
	return 0;
 close:
	mtdev_close(&mt->dev);
 error:
	return ret;
}


int mtouch_close(struct MTouch *mt, int fd)
{
	int ret;
	if (use_grab) {
		SYSCALL(ret = ioctl(fd, EVIOCGRAB, 0));
		if (ret)
			xf86Msg(X_WARNING, "mtouch: ungrab failed\n");
	}
	mtdev_close(&mt->dev);
	return 0;
}

int read_packet(struct MTouch *mt, int fd)
{
	int ret = hwstate_modify(&mt->hs, &mt->dev, fd, &mt->caps);
	if (ret <= 0)
		return ret;
	mtstate_extract(&mt->state, &mt->cfg, &mt->hs, &mt->caps);
	gestures_extract(&mt->gs, &mt->cfg, &mt->hs, &mt->state);
	return 1;
}

int has_delayed(struct MTouch *mt, int fd)
{
	return gestures_delayed(&mt->gs, &mt->dev, fd);
}

