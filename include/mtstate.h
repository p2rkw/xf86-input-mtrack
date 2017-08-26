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

#ifndef MTSTATE_H
#define MTSTATE_H

#include "common.h"
#include "mconfig.h"
#include "hwstate.h"
#include "capabilities.h"

#define MT_NEW 0
#define MT_RELEASED 1
#define MT_INVALID 2
#define MT_THUMB 3
#define MT_PALM 4
#define MT_EDGE 5
#define MT_TAP 6
#define MT_BUTTON 7

struct Touch {
	bitmask_t flags;
	double direction;
	int tracking_id;
	int x, y, dx, dy;
	int total_dx, total_dy;
	struct timeval down;
};

struct MTState {
	bitmask_t touch_used;
	struct Touch touch[DIM_TOUCHES];
};

/*
 * |1|   2   |3|
 * |-|-------|-|
 * |4|   5   |6|
 * |-|-------|-|
 * |7|   8   |9|
 */
int touch_which_edge(const struct MConfig* cfg, const struct Touch* t);

/* Initialize an MTState struct.
 */
void mtstate_init(struct MTState* ms);

/* Extract the MTState from the current hardware state.
 */
void mtstate_extract(struct MTState* ms,
			const struct MConfig* cfg,
			const struct HWState* hs,
			const struct Capabilities* caps);

#endif

