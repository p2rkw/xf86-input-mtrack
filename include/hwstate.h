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

#ifndef HWSTATE_H
#define HWSTATE_H

#include "common.h"
#include "capabilities.h"

struct FingerState {
	/* The size of the contact area */
	int touch_major, touch_minor;
	/* The size of the approaching tool */
	int width_major, width_minor;
	int orientation, pressure;
	int position_x, position_y;
	int tracking_id;
};

struct HWState {
	struct FingerState data[DIM_FINGER];
	bitmask_t used;
	bitmask_t slot;
	bitmask_t button;
	int max_x, max_y;
	struct timeval evtime;
};

void hwstate_init(struct HWState *s,
			const struct Capabilities *caps);
int hwstate_modify(struct HWState *s,
			struct mtdev *dev, int fd,
			const struct Capabilities *caps);
void hwstate_output(const struct HWState *s);

int find_finger(const struct HWState *s, int tracking_id);

#endif
