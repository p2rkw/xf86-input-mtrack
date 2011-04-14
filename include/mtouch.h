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

#ifndef MTOUCH_H
#define MTOUCH_H

#include "common.h"
#include "capabilities.h"
#include "hwstate.h"
#include "mtstate.h"
#include "mconfig.h"
#include "gestures.h"

struct MTouch {
	struct mtdev dev;
	struct Capabilities caps;
	struct HWState hs;
	struct MTState state;
	struct MConfig cfg;
	struct Gestures gs;
};

int mtouch_configure(struct MTouch *mt, int fd);
int mtouch_open(struct MTouch *mt, int fd);
int mtouch_close(struct MTouch *mt, int fd);

int read_packet(struct MTouch *mt, int fd);
int has_delayed(struct MTouch *mt, int fd);

#endif
