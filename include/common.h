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

#define DIM_FINGER 16
#define DIM_TOUCHES 16

/* year-proof millisecond event time */
typedef __u64 mstime_t;

/* all bit masks have this type */
typedef unsigned int bitmask_t;

/**
 * m - bit set (integer)
 * x - modified bit
 * b - new value
 * @{
 */
#define BITMASK(x) (1U << (x))
#define BITONES(x) (BITMASK(x) - 1U)
#define GETBIT(m, x) (((m) >> (x)) & 1U)
#define SETBIT(m, x) (m |= BITMASK(x))
#define CLEARBIT(m, x) (m &= ~BITMASK(x))
#define MODBIT(m, x, b) ((b) ? SETBIT(m, x) : CLEARBIT(m, x))
/** }@ */

#define ABSVAL(x) ((x) < 0 ? -1*(x) : (x))
#define MINVAL(x, y) ((x) < (y) ? (x) : (y))
#define MAXVAL(x, y) ((x) > (y) ? (x) : (y))
#define MODVAL(x, y) ((x) - ((int)((x) / (y))) * (y))
#define SQRVAL(x) ((x) * (x))
#define CLAMPVAL(x, min, max) MAXVAL(MINVAL(x, max), min)
#define SGNVAL (x) ((x) < 0 ? -1 : (((x) > 0) ? 1 : 0))

#define LOG_ERROR(...) \
	do{ \
		xf86Msg(X_ERROR, "mtrack[%i] %s:%i: ", get_next_log_number(), __FILE__, __LINE__); \
		xf86Msg(X_ERROR, __VA_ARGS__); \
	}while(0)

#define LOG_WARNING(...) \
	do{ \
		xf86Msg(X_WARNING, "mtrack[%i] %s:%i: ", get_next_log_number(), __FILE__, __LINE__); \
		xf86Msg(X_WARNING, __VA_ARGS__); \
	}while(0)

#define LOG_INFO(...) \
	do{ \
		xf86Msg(X_INFO, "mtrack[%i] %s:%i: ", get_next_log_number(), __FILE__, __LINE__); \
		xf86Msg(X_INFO, __VA_ARGS__); \
	}while(0)
#define LOG_INFO_CONT(...) xf86Msg(X_INFO, "mtrack[...]: " __VA_ARGS__)

#define LOG_DISABLED(...) do { } while(0)

#define LOG_INFO_ENABLED(...) LOG_INFO(__VA_ARGS__)
#define LOG_INFO_DISABLED(...)

#define LOG_INFO2(ENABLED_or_DISABLED, ...) LOG_INFO_##ENABLED_or_DISABLED(__VA_ARGS__)

#if defined(DEBUG_DRIVER) && (DEBUG_DRIVER != 0)
# define LOG_DEBUG LOG_INFO
#else
# define LOG_DEBUG LOG_DISABLED
#endif

static inline int get_next_log_number(){
	static int last = 0;
	return ++last;
}

/* Retrieve the current time and place it in tv.
 */
static inline void microtime(struct timeval* tv)
{
	gettimeofday(tv, NULL);
}

/* Copy one time value to another.
 */
static inline void timercp(struct timeval* dest, const struct timeval* src)
{
	memcpy(dest, src, sizeof(struct timeval));
}

/* Convert a timeval to milliseconds since the epoch. Truncates additional
 * timer resolution effectively rounding down.
 */
static inline mstime_t timertoms(const struct timeval* tv)
{
	return (mstime_t)(tv->tv_sec*1000) + (mstime_t)(tv->tv_usec/1000);
}

/* Convert a value in milliseconds to a timeval and place the value in tv.
 */
static inline void timerfromms(struct timeval* tv, const mstime_t ms)
{
	tv->tv_sec = (time_t)(ms/1000);
	tv->tv_usec = (suseconds_t)((ms%1000)*1000);
}

/* Convert a timeval to microseconds.
 */
static inline suseconds_t timertomicro(const struct timeval* tv)
{
	return tv->tv_sec * 1000000 + tv->tv_usec;
}

/* Add milliseconds to a timeval and place the resulting value in dest.
 */
static inline void timeraddms(const struct timeval* a, const mstime_t b, struct timeval* dest)
{
	struct timeval tv;
	timerfromms(&tv, b);
	timeradd(a, &tv, dest);
}

/* Check if given timeval a is set to epoch time.
 */
static inline int isepochtime(const struct timeval* a)
{
	struct timeval epoch;
	timerclear(&epoch);

	return timercmp(a, &epoch, ==);
}

/* Clamp value to 15 bits.
 */
static inline int clamp15(int x)
{
	return x < -32767 ? -32767 : x > 32767 ? 32767 : x;
}

/* Absolute scale is assumed to fit in 15 bits.
 */
static inline int dist2(int dx, int dy)
{
	dx = clamp15(dx);
	dy = clamp15(dy);
	return dx * dx + dy * dy;
}



/* Count number of bits (Sean Eron Andersson's Bit Hacks).
 */
static inline int bitcount(unsigned v)
{
	v -= ((v>>1) & 0x55555555);
	v = (v&0x33333333) + ((v>>2) & 0x33333333);
	return (((v + (v>>4)) & 0xF0F0F0F) * 0x1010101) >> 24;
}

/* Return index of first bit [0-31], -1 on zero\
 */
#define firstbit(v) (__builtin_ffs(v) - 1)

/* Boost-style foreach bit.
 */
#define foreach_bit(i, m)						\
	for (i = firstbit(m); i >= 0; i = firstbit((m) & (~0U << (i + 1))))

/* Robust system ioctl calls.
 */
#define SYSCALL(call) while (((call) == -1) && (errno == EINTR))

/**
 * \defgroup Gestures.c Functions from gestures.c
 *
 *   These functions are implemented in gestures.c
 *
 * @{
 */

struct Gestures;

/**
 *  Compute x*x+y*y and compare it with value*value.
 * It's equal to following comparision: sqrt(x*x+y*y) [?] value.
 * Implementation in gestures.c
 *
 * @param x
 * @param y
 * @param value
 * @return -1 when lhs is less than rhs, 0 when equal, 1 when greater
 */
int hypot_cmpf(double x, double y, double value);

/**
 *  It's called 'uncond' because caller have to check that all conditions required to
 * trigger delayed button were met.
 *
 * @return triggered button
 */
int trigger_delayed_button_uncond(struct Gestures* gs);

/** }@ */

#endif
