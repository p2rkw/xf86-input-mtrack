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

#include "mtstate.h"
#include "trig.h"

#ifdef DEBUG_MTSTATE
# define LOG_DEBUG_MTSTATE LOG_DEBUG
#else
# define LOG_DEBUG_MTSTATE LOG_DISABLED
#endif

static int inline percentage(int dividend, int divisor)
{
	return (double)dividend / (double)divisor * 100;
}

static int inline touch_range_ratio(const struct MConfig* cfg, int value)
{
	return (double)(value - cfg->touch_min) / (double)(cfg->touch_max - cfg->touch_min) * 100;
}

static int inline pressure_range_ratio(const struct MConfig* cfg, int value)
{
	return percentage(value - cfg->pressure_min, cfg->pressure_max - cfg->pressure_min);
}

static int finger_touch_ratio(const struct MConfig* cfg, const struct FingerState* hw)
{
	switch(cfg->touch_type){
	case MCFG_SCALE:
		return percentage(hw->touch_major, hw->width_major); /* = estimated pressure */
	case MCFG_SIZE:
	case MCFG_SIZE_PRESSURE:
		return touch_range_ratio(cfg, hw->touch_major);
	case MCFG_PRESSURE_SIZE:
	case MCFG_PRESSURE:
		return pressure_range_ratio(cfg, hw->pressure);
	default:
		return 101; /* sholuld it be additional argument? or maybe it should return -1? */
	}
}

/* Check if a finger is touching the trackpad.
 */
static int is_touch(const struct MConfig* cfg,
			const struct FingerState* hw)
{
	return finger_touch_ratio(cfg, hw) > cfg->touch_down;
}

/* Check if a finger is released from the touchpad.
 */
static int is_release(const struct MConfig* cfg,
			const struct FingerState* hw)
{
	return finger_touch_ratio(cfg, hw) < cfg->touch_up;
}

static int is_thumb(const struct MConfig* cfg,
			const struct FingerState* hw)
{
	if (!cfg->touch_minor)
		return 0;

	int min = MINVAL(hw->touch_minor, hw->touch_major);
	int max = MAXVAL(hw->touch_minor, hw->touch_major);
	int pct = percentage(min, max);
	int size = touch_range_ratio(cfg, hw->touch_major);

	if (pct < cfg->thumb_ratio && size > cfg->thumb_size) {
		LOG_DEBUG_MTSTATE("is_thumb: yes %d > %d && %d > %d\n",
			pct, cfg->thumb_ratio, size, cfg->thumb_size);
		return 1;
	}
	else {
		LOG_DEBUG_MTSTATE("is_thumb: no  %d > %d && %d > %d\n",
			pct, cfg->thumb_ratio, size, cfg->thumb_size);
		return 0;
	}
}

static int is_palm(const struct MConfig* cfg,
			const struct FingerState* hw)
{
	int ratio;
	switch(cfg->touch_type){
	case MCFG_SCALE:
		ratio = percentage(hw->touch_major, hw->width_major);
		break;
	case MCFG_SIZE:
	case MCFG_SIZE_PRESSURE:
	case MCFG_PRESSURE_SIZE:
		ratio = touch_range_ratio(cfg, hw->touch_major);
		break;
	case MCFG_PRESSURE:
		ratio =  pressure_range_ratio(cfg, hw->pressure);
		break;
	default:
		return 0;
	}

	if (ratio > cfg->palm_size) {
		LOG_DEBUG_MTSTATE("is_palm: yes %d > %d\n", ratio, cfg->palm_size);
		return 1;
	}
	else {
		LOG_DEBUG_MTSTATE("is_palm: no  %d > %d\n", ratio, cfg->palm_size);
		return 0;
	}
}

/*
 * |1|   2   |3|
 * |-|-------|-|
 * |4|   5   |6|
 * |-|-------|-|
 * |7|   8   |9|
 */
int touch_which_edge(const struct MConfig* cfg, const struct Touch* t)
{
	typeof(cfg->pad_width) w = cfg->pad_width;
	typeof(cfg->pad_height) h = cfg->pad_height;

	/* Translate back origin from center of the device to top left. */
	typeof(t->x) x = t->x + w/2;
	typeof(t->y) y = t->y + h/2;

	int out = 5; // see visualization above

	// y < (cfg->edge_top_size * cfg->pad_height)/100):
	if(y * 100 < cfg->edge_top_size * h) out = 2;

	// y > h - (cfg->edge_bottom_size * h)/100) :
	if((y - h) * 100 > -cfg->edge_bottom_size * h) out = 8;

	if(x * 100 < cfg->edge_left_size * w) out -= 1; // 1,4,7

	// x > w - (cfg->edge_right_size * w)/100
	if((x - w) * 100 > -cfg->edge_right_size * w) out += 1; // 3,6,9

	LOG_INFO2(DISABLED, "x=%i y=%i out=%i w=%i h=%i\n", x,y,out,w,h);
	return out;
}

static int is_edge(const struct MConfig* cfg, const struct Touch* t)
{
	return touch_which_edge(cfg, t) != 5;
}

/* Find a touch by its tracking ID.  Return -1 if not found.
 */
static int find_touch(struct MTState* ms,
			int tracking_id)
{
	int i;
	foreach_bit(i, ms->touch_used) {
		if (ms->touch[i].tracking_id == tracking_id)
			return i;
	}
	return -1;
}

/* Add a touch to the MTState.  Return the new index of the touch.
 */
static int touch_append(struct MTState* ms,
			const struct MConfig* cfg,
			const struct Capabilities* caps,
			const struct HWState* hs,
			int fn)
{
	int x, y;
	int n = firstbit(~ms->touch_used);
	const struct FingerState* fs = &hs->data[fn];
	if (n < 0)
		LOG_WARNING("Too many touches to track. Ignoring touch %d.\n", fs->tracking_id);
	else {
		/* Set origin (0,0) of 'struct Touch' coordinate system to central point of the device */
		x = translate_cap_x(caps, fs->position_x);
		y = translate_cap_y(caps, fs->position_y);
		x = cfg->axis_x_invert ? -x : x;
		y = cfg->axis_y_invert ? -y : y;
		ms->touch[n].flags = 0U;
		timercp(&ms->touch[n].down, &hs->evtime);
		ms->touch[n].direction = TR_NONE;
		ms->touch[n].tracking_id = fs->tracking_id;
		ms->touch[n].x = x;
		ms->touch[n].y = y;
		ms->touch[n].dx = 0;
		ms->touch[n].dy = 0;
		ms->touch[n].total_dx = 0;
		ms->touch[n].total_dy = 0;
		SETBIT(ms->touch[n].flags, MT_NEW);
		SETBIT(ms->touch_used, n);
	}
	return n;
}

/* Update a touch.
 */
static void touch_update(struct MTState* ms,
			const struct MConfig* cfg,
			const struct Capabilities* caps,
			const struct FingerState* fs,
			int touch)
{
	int x, y;
	/* Translate origin of struct Touch coordinate system to middle point of device */
	x = translate_cap_x(caps, fs->position_x);
	y = translate_cap_y(caps, fs->position_y);

	x = cfg->axis_x_invert ? -x : x;
	y = cfg->axis_y_invert ? -y : y;
	ms->touch[touch].dx = x - ms->touch[touch].x;
	ms->touch[touch].dy = y - ms->touch[touch].y;
	ms->touch[touch].total_dx += ms->touch[touch].dx;
	ms->touch[touch].total_dy += ms->touch[touch].dy;
	ms->touch[touch].x = x;
	ms->touch[touch].y = y;
	if (ms->touch[touch].dx != 0 || ms->touch[touch].dy != 0) {
		ms->touch[touch].direction = trig_direction(ms->touch[touch].dx, ms->touch[touch].dy);
	}
	CLEARBIT(ms->touch[touch].flags, MT_NEW);
}

/* Release a touch.
 */
static void touch_release(struct MTState* ms,
			int touch)
{
	ms->touch[touch].dx = 0;
	ms->touch[touch].dy = 0;
	ms->touch[touch].direction = TR_NONE;
	CLEARBIT(ms->touch[touch].flags, MT_NEW);
	SETBIT(ms->touch[touch].flags, MT_RELEASED);
}

/* Invalidate all touches.
 */
static void touches_invalidate(struct MTState* ms)
{
	int i;
	foreach_bit(i, ms->touch_used)
		SETBIT(ms->touch[i].flags, MT_INVALID);
}

/* Update all touches.
 */
static void touches_update(struct MTState* ms,
			const struct MConfig* cfg,
			const struct HWState* hs,
			const struct Capabilities* caps)
{
	int i, n, disable = 0;
	// Release missing touches.
	foreach_bit(i, ms->touch_used) {
		if (find_finger(hs, ms->touch[i].tracking_id) == -1)
			touch_release(ms, i);
	}

	// Add and update touches.
	foreach_bit(i, hs->used) {
		n = find_touch(ms, hs->data[i].tracking_id);
		if (n >= 0) {
			if (is_release(cfg, &hs->data[i]))
				touch_release(ms, n);
			else
				touch_update(ms, cfg, caps, &hs->data[i], n);
		}
		else if (is_touch(cfg, &hs->data[i]))
			n = touch_append(ms, cfg, caps, hs, i);

		if (n >= 0) {
			// Track and invalidate thumb, palm, and edge touches.
			if (is_thumb(cfg, &hs->data[i]))
				SETBIT(ms->touch[n].flags, MT_THUMB);
			else
				CLEARBIT(ms->touch[n].flags, MT_THUMB);

			if (is_palm(cfg, &hs->data[i]))
				SETBIT(ms->touch[n].flags, MT_PALM);
			else
				CLEARBIT(ms->touch[n].flags, MT_PALM);

			if (is_edge(cfg, &ms->touch[n])) {
				if (GETBIT(ms->touch[n].flags, MT_NEW))
					SETBIT(ms->touch[n].flags, MT_EDGE);
			}
			else
				CLEARBIT(ms->touch[n].flags, MT_EDGE);

			MODBIT(ms->touch[n].flags, MT_INVALID,
				(GETBIT(ms->touch[n].flags, MT_THUMB) && cfg->ignore_thumb) ||
				(GETBIT(ms->touch[n].flags, MT_PALM) && cfg->ignore_palm) ||
				GETBIT(ms->touch[n].flags, MT_EDGE));

			disable |= cfg->disable_on_thumb && GETBIT(ms->touch[n].flags, MT_THUMB);
			disable |= cfg->disable_on_palm && GETBIT(ms->touch[n].flags, MT_PALM);
		}
	}

	if (disable)
		touches_invalidate(ms);
}

/* Remove released touches.
 */
static void touches_clean(struct MTState* ms)
{
	int i, used;
	used = ms->touch_used;
	foreach_bit(i, used) {
		if (GETBIT(ms->touch[i].flags, MT_RELEASED))
			CLEARBIT(ms->touch_used, i);
	}
}

#if DEBUG_MTSTATE
static void mtstate_output(const struct MTState* ms,
			const struct HWState* hs)
{
	int i, n;
	struct timeval tv;

	n = bitcount(ms->touch_used);
	if (bitcount(ms->touch_used) > 0) {
		microtime(&tv);
		LOG_INFO("mtstate: %d touches at event time %llu (rt %llu)\n",
			n, timertoms(&hs->evtime), timertoms(&tv));
	}
	foreach_bit(i, ms->touch_used) {
		if (GETBIT(ms->touch[i].flags, MT_RELEASED)) {
			timersub(&hs->evtime, &ms->touch[i].down, &tv);
			LOG_INFO("  released p(%d, %d) d(%+d, %+d) dir(%f) down(%llu) time(%lld)\n",
						ms->touch[i].x, ms->touch[i].y, ms->touch[i].dx, ms->touch[i].dy,
						ms->touch[i].direction, timertoms(&ms->touch[i].down), timertoms(&tv));
		}
		else if (GETBIT(ms->touch[i].flags, MT_NEW)) {
			LOG_INFO("  new      p(%d, %d) d(%+d, %+d) dir(%f) down(%llu)\n",
						ms->touch[i].x, ms->touch[i].y, ms->touch[i].dx, ms->touch[i].dy,
						ms->touch[i].direction, timertoms(&ms->touch[i].down));
		}
		else if (GETBIT(ms->touch[i].flags, MT_INVALID)) {
			timersub(&hs->evtime, &ms->touch[i].down, &tv);
			LOG_INFO("  invalid  p(%d, %d) d(%+d, %+d) dir(%f) down(%llu) time(%lld)\n",
						ms->touch[i].x, ms->touch[i].y, ms->touch[i].dx, ms->touch[i].dy,
						ms->touch[i].direction, timertoms(&ms->touch[i].down), timertoms(&tv));
		}
		else {
			LOG_INFO("  touching p(%d, %d) d(%+d, %+d) dir(%f) down(%llu)\n",
						ms->touch[i].x, ms->touch[i].y, ms->touch[i].dx, ms->touch[i].dy,
						ms->touch[i].direction, timertoms(&ms->touch[i].down));
		}
	}
}
#endif

void mtstate_init(struct MTState* ms)
{
	memset(ms, 0, sizeof(struct MTState));
}

// Process changes in touch state.
void mtstate_extract(struct MTState* ms,
			const struct MConfig* cfg,
			const struct HWState* hs,
			const struct Capabilities* caps)
{
	touches_clean(ms);
	touches_update(ms, cfg, hs, caps);

#if DEBUG_MTSTATE
	mtstate_output(ms, hs);
#endif
}

