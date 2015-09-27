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

#ifndef GESTURES_H
#define GESTURES_H

#include "common.h"
#include "mconfig.h"
#include "hwstate.h"
#include "mtstate.h"

struct MTouch;

#define GS_TAP 0
#define GS_BUTTON 1

#define GS_NONE 0
#define GS_MOVE 1
#define GS_SCROLL 2
#define GS_SWIPE2 GS_SCROLL
#define GS_SWIPE3 3
#define GS_SWIPE4 4
#define GS_SCALE 5
#define GS_ROTATE 6
#define GS_DRAG_READY 7
#define GS_DRAG_WAIT 8
#define GS_DRAG_ACTIVE 9
#define GS_HOLD1_MOVE1 10
#define GS_HOLD1_MOVE2 11
#define GS_HOLD1_MOVE3 12

struct Gestures {
	/* Taps, physical buttons, and gestures will trigger
	 * button events. If a bit is set, the button is down.
	 * If a bit is not set, the button is up.
	 * Bit 0 is button 1.
	 */
	bitmask_t buttons;

	/* Pointer movement is tracked here.
	 */
	int move_dx, move_dy;

	/* Current time and time delta. Updated after each event and after sleeping.
	 */
	struct timeval time;
	struct timeval dt;

	/* Internal state tracking. Not for direct access.
	 */
	int button_emulate;
	/* Invalid button_delayed means that there's no delayed button.
	 */
	int button_delayed;
	/* If equals to epoch time then button is delayed till gesture end.
	 */
	struct timeval button_delayed_time;

	int tap_touching;
	int tap_released;
	struct timeval tap_timeout;

	int move_type;
	int move_dist;
	int move_dir;
	int move_drag;
	int move_drag_dx;
	int move_drag_dy;

	struct timeval move_wait;
	struct timeval move_drag_wait;
	struct timeval move_drag_expire;
	struct timeval move_start;

	/* Scroll vertical, horizontal.
	 */
	float scroll_speed_x, scroll_speed_y;

	/* Number of scroll coasting frame */
	int scroll_coast_tick_no;
};


void gestures_init(struct MTouch* mt);
void gestures_extract(struct MTouch* mt);
int gestures_delayed(struct MTouch* mt);

#endif

