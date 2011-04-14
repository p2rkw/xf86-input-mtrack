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

#ifndef CAPABILITIES_H
#define CAPABILITIES_H

#include "common.h"
#include "button.h"

struct Capabilities {
	struct input_id devid;
	char devname[32];
	int has_left, has_middle, has_right;
	int has_mtdata, has_ibt;
	int has_slot;
	int has_abs[MT_ABS_SIZE];
	struct input_absinfo slot;
	struct input_absinfo abs[MT_ABS_SIZE];
};

int read_capabilities(struct Capabilities *cap, int fd);
int get_cap_xsize(const struct Capabilities *cap);
int get_cap_ysize(const struct Capabilities *cap);
int get_cap_wsize(const struct Capabilities *cap);

int get_cap_xmid(const struct Capabilities *cap);
int get_cap_ymid(const struct Capabilities *cap);

void output_capabilities(const struct Capabilities *cap);

#endif
