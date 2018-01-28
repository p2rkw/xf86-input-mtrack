/***************************************************************************
 *
 * Multitouch X driver
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
 * Copyright (C) 2011 Ryan Bourgeois <bluedragonx@gmail.com>
 * Copyright (C) 2015-2018 Paweł Turkowski <p2rkw0@gmail.com>
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

#ifndef MTOUCH_H
#define MTOUCH_H

#include "common.h"
#include "capabilities.h"
#include "hwstate.h"
#include "mtstate.h"
#include "mconfig.h"
#include "gestures.h"
#include "os.h" /* xorg/os.h for timers */

struct MTouch {
	int fd;
	struct mtdev dev;
	struct Capabilities caps;
	struct HWState hs;
	struct MTState state;
	struct MConfig cfg;
	struct Gestures gs;
	int timer_kind;   // to avoid setting timer multiple times
	/*
	 * Timers documentation:
	 * http://www.x.org/releases/X11R7.7/doc/xorg-server/Xserver-spec.html#id2536042
	 */
	OsTimerPtr timer;
	int absolute_mode;		// Should absolute mode be enabled ? 0 or 1
	ValuatorMask* valuator_mask;
	DeviceIntPtr local_dev;
};

int mtouch_configure(struct MTouch* mt, int fd);
int mtouch_open(struct MTouch* mt, int fd);
int mtouch_close(struct MTouch* mt);

int mtouch_read(struct MTouch* mt);
int mtouch_delayed(struct MTouch* mt);


/**
 * \defgroup Timers Perform asynchronous tasks
 *
 *  Timers are defined by labels (IDs). To add new timer create new ID, and implement
 * appropriate section in mt_timer_start, mt_timer_stop, and mt_timer_callback.
 *
 *  All timers share same X resource, so generally you can't have more than one timer
 * ticking at time. For now it's fine.
 *
 *  Functions are implemented in mtrack.c
 *
 * @{
 */

/** Will cancel any timer */
#define MT_TIMER_ANY            -1
#define MT_TIMER_NONE           0
/* #define -MT_TIMER_ANY         1 - reserved */
#define MT_TIMER_DELAYED_BUTTON 2
#define MT_TIMER_COASTING       3

void mt_timer_start(struct MTouch *mt, int kind);
/**
 *  Executed on early stop.
 * For example when one timer is replaced by naother.
 * @param mt
 */
void mt_timer_stop(struct MTouch *mt);
CARD32 mt_timer_callback(OsTimerPtr timer, CARD32 time, void *arg);
void mt_timer_stop_if(struct MTouch *mt, int kind);
/**
 *  Restart timer with same kind.
 * Does not call start nor stop callbacks.
 * @param mt
 */
void mt_timer_continue(struct MTouch *mt, mstime_t timeout);
/**
 *  Kind > 0 starts timer, kind < 0 stops timer with ID == abs(kind).
 * MT_TIMER_ANY stops whatever timer is running.
 * @param mt
 * @param kind
 */
void mt_timer_start_or_stop(struct MTouch *mt, int kind);

/** }@ */

#endif
