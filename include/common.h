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

#ifndef COMMON_H
#define COMMON_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <xorg-server.h>
#include <xf86.h>
#include <xf86_OSproc.h>
#include <xf86Xinput.h>
#include <errno.h>
#include <mtdev-mapping.h>
#include <stdint.h>
#include <sys/time.h>

#define DIM_FINGER 32
#define DIM_TOUCHES 32

/* year-proof millisecond event time */
typedef __u64 mstime_t;

/* all bit masks have this type */
typedef unsigned int bitmask_t;

#define BITMASK(x) (1U << (x))
#define BITONES(x) (BITMASK(x) - 1U)
#define GETBIT(m, x) (((m) >> (x)) & 1U)
#define SETBIT(m, x) (m |= BITMASK(x))
#define CLEARBIT(m, x) (m &= ~BITMASK(x))
#define MODBIT(m, x, b) ((b) ? SETBIT(m, x) : CLEARBIT(m, x))

#define ABSVAL(x) ((x) < 0 ? -1*(x) : (x))
#define MINVAL(x, y) ((x) < (y) ? (x) : (y)) 
#define MAXVAL(x, y) ((x) > (y) ? (x) : (y))
#define MODVAL(x, y) ((x) - ((int)((x) / (y))) * (y))
#define SQRVAL(x) ((x) * (x))
#define CLAMPVAL(x, min, max) MAXVAL(MINVAL(x, max), min)

static inline void microtime(struct timeval* tv)
{
	gettimeofday(tv, NULL);
}

static inline void timercp(struct timeval* dest, struct timeval* src)
{
	memcpy(dest, src, sizeof(timeval));
}

static inline void timeraddms(struct timeval* tv, mstime_t ms)
{
	struct timeval src;
	src.tv_sec = (time_t)(ms/1000);
	src.tv_usec = (suseconds_t)((ms%1000)*1000);
	timeradd(&src, tv, tv);
}

static inline int clamp15(int x)
{
	return x < -32767 ? -32767 : x > 32767 ? 32767 : x;
}

/* absolute scale is assumed to fit in 15 bits */
static inline int dist2(int dx, int dy)
{
	dx = clamp15(dx);
	dy = clamp15(dy);
	return dx * dx + dy * dy;
}

/* Count number of bits (Sean Eron Andersson's Bit Hacks) */
static inline int bitcount(unsigned v)
{
	v -= ((v>>1) & 0x55555555);
	v = (v&0x33333333) + ((v>>2) & 0x33333333);
	return (((v + (v>>4)) & 0xF0F0F0F) * 0x1010101) >> 24;
}

/* Return index of first bit [0-31], -1 on zero */
#define firstbit(v) (__builtin_ffs(v) - 1)

/* boost-style foreach bit */
#define foreach_bit(i, m)						\
	for (i = firstbit(m); i >= 0; i = firstbit((m) & (~0U << i + 1)))

/* robust system ioctl calls */
#define SYSCALL(call) while (((call) == -1) && (errno == EINTR))

#endif
