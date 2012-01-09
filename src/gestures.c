/***************************************************************************
 *
 * Multitouch X driver
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
 * Copyright (C) 2011 Ryan Bourgeois <bluedragonx@gmail.com>
 *
 * Gestures
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
 * Copyright (C) 2010 Arturo Castro <mail@arturocastro.net>
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

#include "gestures.h"
#include "mtouch.h"
#include "trig.h"

#define IS_VALID_BUTTON(x) (x >= 0 && x <= 31)

static void trigger_button_up(struct Gestures* gs, int button)
{
	if (IS_VALID_BUTTON(button)) {
		if (button == 0 && gs->button_emulate > 0) {
			button = gs->button_emulate;
			gs->button_emulate = 0;
		}
		CLEARBIT(gs->buttons, button);
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_button_up: %d up\n", button);
#endif
	}
}

static void trigger_button_down(struct Gestures* gs, int button)
{
	struct timeval epoch;
	timerclear(&epoch);

	if (IS_VALID_BUTTON(button) && (button != gs->button_delayed || timercmp(&gs->button_delayed_time, &epoch, ==))) {
		SETBIT(gs->buttons, button);
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_button_down: %d down\n", button);
#endif
	}
#ifdef DEBUG_GESTURES
	else if (IS_VALID_BUTTON(button))
		xf86Msg(X_INFO, "trigger_button_down: %d down ignored, in delayed mode\n", button);
#endif
}

static void trigger_button_emulation(struct Gestures* gs, int button)
{
	if (IS_VALID_BUTTON(button) && GETBIT(gs->buttons, 0)) {
		CLEARBIT(gs->buttons, 0);
		SETBIT(gs->buttons, button);
		gs->button_emulate = button;
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_button_emulation: %d emulated\n", button);
#endif
	}
}

static void trigger_button_click(struct Gestures* gs,
			int button, struct timeval* trigger_up_time)
{
	struct timeval epoch;
	timerclear(&epoch);

	if (IS_VALID_BUTTON(button) && timercmp(&gs->button_delayed_time, &epoch, ==)) {
		trigger_button_down(gs, button);
		gs->button_delayed = button;
		timercp(&gs->button_delayed_time, trigger_up_time);
		timerclear(&gs->button_delayed_delta);
#ifdef DEBUG_GESTRUES
		xf86Msg(X_INFO, "trigger_button_click: %d placed in delayed mode\n");
#endif
	}
#ifdef DEBUG_GESTURES
	else if (IS_VALID_BUTTON(button))
		xf86Msg(X_INFO, "trigger_button_click: %d ignored, in delayed mode\n", button);
#endif
}

static void trigger_drag_ready(struct Gestures* gs,
			const struct MConfig* cfg)
{
	gs->move_drag = GS_DRAG_READY;
	timeraddms(&gs->time, cfg->drag_timeout, &gs->move_drag_expire);
#ifdef DEBUG_GESTURES
	xf86Msg(X_INFO, "trigger_drag_ready: drag is ready\n");
#endif
}

static int trigger_drag_start(struct Gestures* gs,
			const struct MConfig* cfg,
			int dx, int dy)
{
	if (gs->move_drag == GS_DRAG_READY) {
		timerclear(&gs->move_drag_expire);
		if (cfg->drag_wait == 0) {
 			gs->move_drag = GS_DRAG_ACTIVE;
			trigger_button_down(gs, 0);
#ifdef DEBUG_GESTURES
			xf86Msg(X_INFO, "trigger_drag_start: drag is active\n");
#endif
		}
		else {
			gs->move_drag = GS_DRAG_WAIT;
			gs->move_drag_dx = dx;
			gs->move_drag_dy = dy;
			timeraddms(&gs->time, cfg->drag_wait, &gs->move_drag_wait);
#ifdef DEBUG_GESTURES
			xf86Msg(X_INFO, "trigger_drag_start: drag in wait\n");
#endif
		}
	}
	else if (gs->move_drag == GS_DRAG_WAIT) {
		gs->move_drag_dx += dx;
		gs->move_drag_dy += dy;
		if (!timercmp(&gs->time, &gs->move_drag_wait, <)) {
			gs->move_drag = GS_DRAG_ACTIVE;
			trigger_button_down(gs, 0);
#ifdef DEBUG_GESTURES
			xf86Msg(X_INFO, "trigger_drag_start: drag is active\n");
#endif
		}
		else if (dist2(gs->move_drag_dx, gs->move_drag_dy) > SQRVAL(cfg->drag_dist)) {
			gs->move_drag = GS_NONE;
#ifdef DEBUG_GESTURES
			xf86Msg(X_INFO, "trigger_drag_start: drag canceled, moved too far\n");
#endif
		}
	}
	return gs->move_drag != GS_DRAG_WAIT;
}

static void trigger_drag_stop(struct Gestures* gs, int force)
{
	if (gs->move_drag == GS_DRAG_READY && force) {
		gs->move_drag = GS_NONE;
		timerclear(&gs->move_drag_expire);
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_drag_stop: drag canceled\n");
#endif
	}
	else if (gs->move_drag == GS_DRAG_ACTIVE) {
		gs->move_drag = GS_NONE;
		timerclear(&gs->move_drag_expire);
		trigger_button_up(gs, 0);
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_drag_stop: drag stopped\n");
#endif
	}
}

static void buttons_update(struct Gestures* gs,
			const struct MConfig* cfg,
			const struct HWState* hs,
			struct MTState* ms)
{
	if (!cfg->button_enable || cfg->trackpad_disable >= 3)
		return;

	static bitmask_t button_prev = 0U;
	int i, down, emulate, touching;
	down = 0;
	emulate = GETBIT(hs->button, 0) && !GETBIT(button_prev, 0);

	for (i = 0; i < 32; i++) {
		if (GETBIT(hs->button, i) == GETBIT(button_prev, i))
			continue;
		if (GETBIT(hs->button, i)) {
			down++;
			trigger_button_down(gs, i);
		}
		else
			trigger_button_up(gs, i);
	}
	button_prev = hs->button;

	if (down) {
		int earliest, latest, moving = 0;
		gs->move_type = GS_NONE;
		timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);
		earliest = -1;
		latest = -1;
		foreach_bit(i, ms->touch_used) {
			if (GETBIT(ms->touch[i].state, MT_INVALID))
				continue;
			if (cfg->button_integrated && !GETBIT(ms->touch[i].flags, GS_BUTTON))
				SETBIT(ms->touch[i].flags, GS_BUTTON);
			if (earliest == -1 || timercmp(&ms->touch[i].down, &ms->touch[earliest].down, <))
				earliest = i;
			if (latest == -1 || timercmp(&ms->touch[i].down, &ms->touch[latest].down, >))
				latest = i;
		}

		if (emulate) {
			if (cfg->button_zones && earliest >= 0) {
				int zones, left, right, pos;
				double width;

				zones = 0;
				if (cfg->button_1touch > 0)
					zones++;
				if (cfg->button_2touch > 0)
					zones++;
				if (cfg->button_3touch > 0)
					zones++;

				if (zones > 0) {
					width = ((double)cfg->pad_width)/((double)zones);
					pos = cfg->pad_width / 2 + ms->touch[earliest].x;
#ifdef DEBUG_GESTURES
					xf86Msg(X_INFO, "buttons_update: pad width %d, zones %d, zone width %f, x %d\n",
						cfg->pad_width, zones, width, pos);
#endif
					for (i = 0; i < zones; i++) {
						left = width*i;
						right = width*(i+1);
						if (pos >= left && pos <= right) {
#ifdef DEBUG_GESTURES
							xf86Msg(X_INFO, "buttons_update: button %d, left %d, right %d (found)\n", i, left, right);
#endif
							break;
						}
#ifdef DEBUG_GESTURES
						else
							xf86Msg(X_INFO, "buttons_update: button %d, left %d, right %d\n", i, left, right);
#endif
					}

					if (i == 0)
						trigger_button_emulation(gs, cfg->button_1touch - 1);
					else if (i == 1)
						trigger_button_emulation(gs, cfg->button_2touch - 1);
					else
						trigger_button_emulation(gs, cfg->button_3touch - 1);
				}
			}
			else if (latest >= 0) {
				touching = 0;
				struct timeval expire;
				foreach_bit(i, ms->touch_used) {
					timeraddms(&ms->touch[i].down, cfg->button_expire, &expire);
					if (cfg->button_move || cfg->button_expire == 0 || timercmp(&ms->touch[latest].down, &expire, <))
						touching++;
				}

				if (cfg->button_integrated)
					touching--;

				if (touching == 1 && cfg->button_1touch > 0)
					trigger_button_emulation(gs, cfg->button_1touch - 1);
				else if (touching == 2 && cfg->button_2touch > 0)
					trigger_button_emulation(gs, cfg->button_2touch - 1);
				else if (touching == 3 && cfg->button_3touch > 0)
					trigger_button_emulation(gs, cfg->button_3touch - 1);
			}
		}
	}
}

static void tapping_update(struct Gestures* gs,
			const struct MConfig* cfg,
			struct MTState* ms)
{
	int i, n, dist, released_max;
	struct timeval tv_tmp;
	struct timeval epoch;

	if (cfg->trackpad_disable >= 1)
		return;

	if (cfg->tap_4touch > 0)
		released_max = 4;
	else if (cfg->tap_3touch > 0)
		released_max = 3;
	else if (cfg->tap_2touch > 0)
		released_max = 2;
	else if (cfg->tap_1touch > 0)
		released_max = 1;
	else
		return;

	timerclear(&epoch);
	timeraddms(&gs->tap_time_down, cfg->tap_timeout, &tv_tmp);
	if (!timercmp(&gs->tap_time_down, &epoch, ==) && !timercmp(&gs->time, &tv_tmp, <)) {
		gs->tap_touching = 0;
		gs->tap_released = 0;
		timerclear(&gs->tap_time_down);

		foreach_bit(i, ms->touch_used) {
			if (GETBIT(ms->touch[i].flags, GS_TAP))
				CLEARBIT(ms->touch[i].flags, GS_TAP);
		}
	}
	else {
		foreach_bit(i, ms->touch_used) {
			if (GETBIT(ms->touch[i].state, MT_INVALID) || GETBIT(ms->touch[i].flags, GS_BUTTON)) {
				if (GETBIT(ms->touch[i].flags, GS_TAP)) {
					CLEARBIT(ms->touch[i].flags, GS_TAP);
					gs->tap_touching--;
#ifdef DEBUG_GESTURES
					xf86Msg(X_INFO, "tapping_update: tap_touching-- (%d): invalid or button\n", gs->tap_touching);
#endif
				}
			}
			else {
				if (GETBIT(ms->touch[i].state, MT_NEW)) {
					SETBIT(ms->touch[i].flags, GS_TAP);
					gs->tap_touching++;
#ifdef DEBUG_GESTURES
					xf86Msg(X_INFO, "tapping_update: tap_touching++ (%d): new touch\n", gs->tap_touching);
#endif
					timerclear(&tv_tmp);
					if (timercmp(&gs->tap_time_down, &epoch, ==))
						timercp(&gs->tap_time_down, &gs->time);
				}

				if (GETBIT(ms->touch[i].flags, GS_TAP)) {
					dist = dist2(ms->touch[i].total_dx, ms->touch[i].total_dy);
					if (dist >= SQRVAL(cfg->tap_dist)) {
						CLEARBIT(ms->touch[i].flags, GS_TAP);
						gs->tap_touching--;
#ifdef DEBUG_GESTURES
					xf86Msg(X_INFO, "tapping_update: tap_touching-- (%d): moved too far\n", gs->tap_touching);
#endif
					}
					else if (GETBIT(ms->touch[i].state, MT_RELEASED)) {
						gs->tap_touching--;
						gs->tap_released++;
#ifdef DEBUG_GESTURES
					xf86Msg(X_INFO, "tapping_update: tap_touching-- (%d): released\n", gs->tap_touching);
					xf86Msg(X_INFO, "tapping_update: tap_released++ (%d) (max %d): released\n", gs->tap_released, released_max);
#endif
					}
				}
			}
		}
	}

	if ((gs->tap_touching == 0 && gs->tap_released > 0) || gs->tap_released >= released_max) {
		foreach_bit(i, ms->touch_used) {
			if (GETBIT(ms->touch[i].flags, GS_TAP))
				CLEARBIT(ms->touch[i].flags, GS_TAP);
		}

		if (gs->tap_released == 1)
			n = cfg->tap_1touch - 1;
		else if (gs->tap_released == 2)
			n = cfg->tap_2touch - 1;
		else if (gs->tap_released == 3)
			n = cfg->tap_3touch - 1;
		else
			n = cfg->tap_4touch - 1;

		trigger_button_click(gs, n, &tv_tmp);
		if (cfg->drag_enable && n == 0)
			trigger_drag_ready(gs, cfg);

		gs->move_type = GS_NONE;
		timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);

		gs->tap_touching = 0;
		gs->tap_released = 0;
		timerclear(&gs->tap_time_down);
	}
}

static void trigger_move(struct Gestures* gs,
			const struct MConfig* cfg,
			int dx, int dy)
{
	if ((gs->move_type == GS_MOVE || !timercmp(&gs->time, &gs->move_wait, <)) && (dx != 0 || dy != 0)) {
		if (trigger_drag_start(gs, cfg, dx, dy)) {
			gs->move_dx = (int)(dx*cfg->sensitivity);
			gs->move_dy = (int)(dy*cfg->sensitivity);
			gs->move_type = GS_MOVE;
			gs->move_dist = 0;
			gs->move_dir = TR_NONE;
			gs->move_speed = trig_pythagorean(gs->move_dx, gs->move_dy)/timertomicro(&gs->dt);
			timerclear(&gs->move_wait);
#ifdef DEBUG_GESTURES
			xf86Msg(X_INFO, "trigger_move: %d, %d\n", dx, dy);
#endif
		}
	}
}

static void trigger_scroll(struct Gestures* gs,
			const struct MConfig* cfg,
			double dist, int dir)
{
	if (gs->move_type == GS_SCROLL || !timercmp(&gs->time, &gs->move_wait, <)) {
		struct timeval tv_tmp;
		trigger_drag_stop(gs, 1);
		if (gs->move_type != GS_SCROLL || gs->move_dir != dir)
			gs->move_dist = 0;
		gs->move_dx = 0;
		gs->move_dy = 0;
		gs->move_type = GS_SCROLL;
		gs->move_dist += (int)ABSVAL(dist);
		gs->move_dir = dir;
		gs->move_speed = dist/timertomicro(&gs->dt);
		timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);

		if (gs->move_dist >= cfg->scroll_dist) {
			gs->move_dist = MODVAL(gs->move_dist, cfg->scroll_dist);
			timeraddms(&gs->time, cfg->gesture_hold, &tv_tmp);
			if (dir == TR_DIR_UP)
				trigger_button_click(gs, cfg->scroll_up_btn - 1, &tv_tmp);
			else if (dir == TR_DIR_DN)
				trigger_button_click(gs, cfg->scroll_dn_btn - 1, &tv_tmp);
			else if (dir == TR_DIR_LT)
				trigger_button_click(gs, cfg->scroll_lt_btn - 1, &tv_tmp);
			else if (dir == TR_DIR_RT)
				trigger_button_click(gs, cfg->scroll_rt_btn - 1, &tv_tmp);
		}
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_scroll: scrolling %+f in direction %d (at %d of %d)\n", dist, dir, gs->move_dist, cfg->scroll_dist);
#endif
	}
}

static void trigger_swipe(struct Gestures* gs,
			const struct MConfig* cfg,
			double dist, int dir, int isfour)
{
	if (gs->move_type == GS_SWIPE || !timercmp(&gs->time, &gs->move_wait, <)) {
		struct timeval tv_tmp;
		trigger_drag_stop(gs, 1);
		if (gs->move_type != GS_SWIPE || gs->move_dir != dir)
			gs->move_dist = 0;
		gs->move_dx = 0;
		gs->move_dy = 0;
		gs->move_type = GS_SWIPE;
		gs->move_dist += (int)ABSVAL(dist);
		gs->move_dir = dir;
		gs->move_speed = dist/timertomicro(&gs->dt);
		timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);
		timeraddms(&gs->time, cfg->gesture_hold, &tv_tmp);

		if (isfour) {
			if (cfg->swipe4_dist > 0 && gs->move_dist >= cfg->swipe4_dist) {
				gs->move_dist = MODVAL(gs->move_dist, cfg->swipe4_dist);
				if (dir == TR_DIR_UP)
					trigger_button_click(gs, cfg->swipe4_up_btn - 1, &tv_tmp);
				else if (dir == TR_DIR_DN)
					trigger_button_click(gs, cfg->swipe4_dn_btn - 1, &tv_tmp);
				else if (dir == TR_DIR_LT)
					trigger_button_click(gs, cfg->swipe4_lt_btn - 1, &tv_tmp);
				else if (dir == TR_DIR_RT)
					trigger_button_click(gs, cfg->swipe4_rt_btn - 1, &tv_tmp);
			}
#ifdef DEBUG_GESTURES
			xf86Msg(X_INFO, "trigger_swipe4: swiping %+f in direction %d (at %d of %d)\n", dist, dir, gs->move_dist, cfg->swipe_dist);
#endif
		}
		else {
			if (cfg->swipe_dist > 0 && gs->move_dist >= cfg->swipe_dist) {
				gs->move_dist = MODVAL(gs->move_dist, cfg->swipe_dist);
				if (dir == TR_DIR_UP)
					trigger_button_click(gs, cfg->swipe_up_btn - 1, &tv_tmp);
				else if (dir == TR_DIR_DN)
					trigger_button_click(gs, cfg->swipe_dn_btn - 1, &tv_tmp);
				else if (dir == TR_DIR_LT)
					trigger_button_click(gs, cfg->swipe_lt_btn - 1, &tv_tmp);
				else if (dir == TR_DIR_RT)
					trigger_button_click(gs, cfg->swipe_rt_btn - 1, &tv_tmp);
			}
#ifdef DEBUG_GESTURES
			xf86Msg(X_INFO, "trigger_swipe: swiping %+f in direction %d (at %d of %d)\n", dist, dir, gs->move_dist, cfg->swipe_dist);
#endif
		}
	}
}

static void trigger_scale(struct Gestures* gs,
			const struct MConfig* cfg,
			double dist, int dir)
{
	if (gs->move_type == GS_SCALE || !timercmp(&gs->time, &gs->move_wait, <)) {
		struct timeval tv_tmp;
		int scale_dist_sqr = SQRVAL(cfg->scale_dist);
		trigger_drag_stop(gs, 1);
		if (gs->move_type != GS_SCALE || gs->move_dir != dir)
			gs->move_dist = 0;
		gs->move_dx = 0;
		gs->move_dy = 0;
		gs->move_type = GS_SCALE;
		gs->move_dist += (int)ABSVAL(dist);
		gs->move_dir = dir;
		gs->move_speed = dist/timertomicro(&gs->dt);
		timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);
		if (gs->move_dist >= scale_dist_sqr) {
			gs->move_dist = MODVAL(gs->move_dist, scale_dist_sqr);
			timeraddms(&gs->time, cfg->gesture_hold, &tv_tmp);
			if (dir == TR_DIR_UP)
				trigger_button_click(gs, cfg->scale_up_btn - 1, &tv_tmp);
			else if (dir == TR_DIR_DN)
				trigger_button_click(gs, cfg->scale_dn_btn - 1, &tv_tmp);
		}
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_scale: scaling %+f in direction %d (at %d of %d)\n", dist, dir, gs->move_dist, scale_dist_sqr);
#endif
	}
}

static void trigger_rotate(struct Gestures* gs,
			const struct MConfig* cfg,
			double dist, int dir)
{
	if (gs->move_type == GS_ROTATE || !timercmp(&gs->time, &gs->move_wait, <)) {
		struct timeval tv_tmp;
		int rotate_dist_sqr = SQRVAL(cfg->rotate_dist);
		trigger_drag_stop(gs, 1);
		if (gs->move_type != GS_ROTATE || gs->move_dir != dir)
			gs->move_dist = 0;
		gs->move_dx = 0;
		gs->move_dy = 0;
		gs->move_type = GS_ROTATE;
		gs->move_dist += (int)ABSVAL(dist);
		gs->move_dir = dir;
		gs->move_speed = dist/timertomicro(&gs->dt);
		timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);
		if (gs->move_dist >= rotate_dist_sqr) {
			gs->move_dist = MODVAL(gs->move_dist, rotate_dist_sqr);
			timeraddms(&gs->time, cfg->gesture_hold, &tv_tmp);
			if (dir == TR_DIR_LT)
				trigger_button_click(gs, cfg->rotate_lt_btn - 1, &tv_tmp);
			else if (dir == TR_DIR_RT)
				trigger_button_click(gs, cfg->rotate_rt_btn - 1, &tv_tmp);
		}
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_rotate: rotating %+f in direction %d (at %d of %d)\n", dist, dir, gs->move_dist, rotate_dist_sqr);
#endif
	}
}

static void trigger_reset(struct Gestures* gs)
{
	trigger_drag_stop(gs, 0);
	gs->move_dx = 0;
	gs->move_dy = 0;
	gs->move_type = GS_NONE;
	gs->move_dist = 0;
	gs->move_dir = TR_NONE;
	timerclear(&gs->move_wait);
}

static int get_scroll_dir(const struct Touch* t1,
			const struct Touch* t2)
{
	if (trig_angles_acute(t1->direction, t2->direction) < 2.0) {
		double angles[2];
		angles[0] = t1->direction;
		angles[1] = t2->direction;
		return trig_generalize(trig_angles_avg(angles, 2));
	}
	return TR_NONE;
}

static int get_rotate_dir(const struct Touch* t1,
			const struct Touch* t2)
{
	double v, d1, d2;
	v = trig_direction(t2->x - t1->x, t2->y - t1->y);
	d1 = trig_angles_add(v, 2);
	d2 = trig_angles_sub(v, 2);
	if (trig_angles_acute(t1->direction, d1) < 2 && trig_angles_acute(t2->direction, d2) < 2)
		return TR_DIR_RT;
	else if (trig_angles_acute(t1->direction, d2) < 2 && trig_angles_acute(t2->direction, d1) < 2)
		return TR_DIR_LT;
	return TR_NONE;
}

static int get_scale_dir(const struct Touch* t1,
			const struct Touch* t2)
{
	double v;
	if (trig_angles_acute(t1->direction, t2->direction) >= 2) {
		v = trig_direction(t2->x - t1->x, t2->y - t1->y);
		if (trig_angles_acute(v, t1->direction) < 2)
			return TR_DIR_DN;
		else
			return TR_DIR_UP;
	}
	return TR_NONE;
}

static int get_swipe_dir(const struct Touch* t1,
			const struct Touch* t2,
			const struct Touch* t3)
{
	double angles[3];
	angles[0] = t1->direction;
	angles[1] = t2->direction;
	angles[2] = t3->direction;
	return trig_generalize(trig_angles_avg(angles, 3));
}

static int get_swipe4_dir(const struct Touch* t1,
			const struct Touch* t2,
			const struct Touch* t3,
			const struct Touch* t4)
{
	double angles[4];
	angles[0] = t1->direction;
	angles[1] = t2->direction;
	angles[2] = t3->direction;
	angles[3] = t4->direction;
	return trig_generalize(trig_angles_avg(angles, 4));
}

static void moving_update(struct Gestures* gs,
			const struct MConfig* cfg,
			struct MTState* ms)
{
	int i, count, btn_count, dx, dy, dir;
	double dist;
	struct Touch* touches[4];
	count = btn_count = 0;
	dx = dy = 0;
	dir = 0;

	// Reset movement.
	gs->move_dx = 0;
	gs->move_dy = 0;

	// Count touches and aggregate touch movements.
	foreach_bit(i, ms->touch_used) {
		if (GETBIT(ms->touch[i].state, MT_INVALID))
			continue;
		else if (GETBIT(ms->touch[i].flags, GS_BUTTON)) {
			btn_count++;
			dx += ms->touch[i].dx;
			dy += ms->touch[i].dy;
		}
		else if (!GETBIT(ms->touch[i].flags, GS_TAP)) {
			if (count < 4)
				touches[count++] = &ms->touch[i];
		}
	}

	// Determine gesture type.
	if (count == 0) {
		if (btn_count >= 1 && cfg->trackpad_disable < 2)
			trigger_move(gs, cfg, dx, dy);
		else if (btn_count < 1)
			trigger_reset(gs);
	}
	else if (count == 1 && cfg->trackpad_disable < 2) {
		dx += touches[0]->dx;
		dy += touches[0]->dy;
		trigger_move(gs, cfg, dx, dy);
	}
	else if (count == 2 && cfg->trackpad_disable < 1) {
		// scroll, scale, or rotate
		if ((dir = get_scroll_dir(touches[0], touches[1])) != TR_NONE) {
			dist = trig_pythagorean(
				touches[0]->dx + touches[1]->dx,
				touches[0]->dy + touches[1]->dy);
			trigger_scroll(gs, cfg, dist/2, dir);
		}
		else if ((dir = get_rotate_dir(touches[0], touches[1])) != TR_NONE) {
			dist = dist2(touches[0]->dx, touches[0]->dy) + dist2(touches[1]->dx, touches[1]->dy);
			trigger_rotate(gs, cfg, dist/2, dir);
		}
		else if ((dir = get_scale_dir(touches[0], touches[1])) != TR_NONE) {
			dist = dist2(touches[0]->dx, touches[0]->dy) + dist2(touches[1]->dx, touches[1]->dy);
			trigger_scale(gs, cfg, dist/2, dir);
		}
	}
	else if (count == 3 && cfg->trackpad_disable < 1) {
		if ((dir = get_swipe_dir(touches[0], touches[1], touches[2])) != TR_NONE) {
			dist = trig_pythagorean(
				touches[0]->dx + touches[1]->dx + touches[2]->dx,
				touches[0]->dy + touches[1]->dy + touches[2]->dy);
			trigger_swipe(gs, cfg, dist/3, dir, 0);
		}
	}
	else if (count == 4 && cfg->trackpad_disable < 1) {
		if ((dir = get_swipe4_dir(touches[0], touches[1], touches[2], touches[3])) != TR_NONE) {
			dist = trig_pythagorean(
				touches[0]->dx + touches[1]->dx + touches[2]->dx + touches[3]->dx,
				touches[0]->dy + touches[1]->dy + touches[2]->dy + touches[3]->dy);
			trigger_swipe(gs, cfg, dist/4, dir, 1);
		}
	}
}

static void dragging_update(struct Gestures* gs)
{
	if (gs->move_drag == GS_DRAG_READY && timercmp(&gs->time, &gs->move_drag_expire, >)) {
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "dragging_update: drag expired\n");
#endif
		trigger_drag_stop(gs, 1);
	}
}

static void delayed_update(struct Gestures* gs)
{
	struct timeval epoch;
	timerclear(&epoch);

	if (timercmp(&gs->button_delayed_time, &epoch, ==))
		return;

	if (!timercmp(&gs->time, &gs->button_delayed_time, <)) {
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "delayed_update: %d delay expired, triggering up\n", gs->button_delayed);
#endif
		trigger_button_up(gs, gs->button_delayed);
		gs->button_delayed = 0;
		timerclear(&gs->button_delayed_time);
		timerclear(&gs->button_delayed_delta);
	}
	else {
		timersub(&gs->button_delayed_time, &gs->time, &gs->button_delayed_delta);
	}
}

void gestures_init(struct MTouch* mt)
{
	memset(&mt->gs, 0, sizeof(struct Gestures));
}

void gestures_extract(struct MTouch* mt)
{
	timersub(&mt->hs.evtime, &mt->gs.time, &mt->gs.dt);
	timercp(&mt->gs.time, &mt->hs.evtime);

	dragging_update(&mt->gs);
	buttons_update(&mt->gs, &mt->cfg, &mt->hs, &mt->state);
	tapping_update(&mt->gs, &mt->cfg, &mt->state);
	moving_update(&mt->gs, &mt->cfg, &mt->state);
	delayed_update(&mt->gs);
}

static int gestures_sleep(struct MTouch* mt, const struct timeval* sleep)
{
	if (mtdev_empty(&mt->dev)) {
		struct timeval now;
		mtdev_idle(&mt->dev, mt->fd, timertoms(sleep));
		microtime(&now);
		timersub(&now, &mt->gs.time, &mt->gs.dt);
		timercp(&mt->gs.time, &now);
		return 1;
	}
	return 0;
}

int gestures_delayed(struct MTouch* mt)
{
	struct Gestures* gs = &mt->gs;
	struct timeval epoch;
	timerclear(&epoch);

	if (timercmp(&gs->button_delayed_time, &epoch, >)) {
		if (gestures_sleep(mt, &gs->button_delayed_delta)) {
#ifdef DEBUG_GESTURES
			xf86Msg(X_INFO, "gestures_delayed: %d up, timer expired\n", gs->button_delayed);
#endif
			trigger_button_up(gs, gs->button_delayed);
			gs->move_dx = 0;
			gs->move_dy = 0;
			gs->button_delayed = 0;
			timerclear(&gs->button_delayed_time);
			timerclear(&gs->button_delayed_delta);
			return 1;
		}
	}
	return 0;
}

