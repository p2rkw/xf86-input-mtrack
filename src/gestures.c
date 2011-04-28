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
#include "trig.h"
#include <poll.h>

#define IS_VALID_BUTTON(x) (x >= 0 && x <= 31)

static void trigger_button_up(struct Gestures* gs, int button)
{
	if (IS_VALID_BUTTON(button)) {
		if (button == 0 && gs->button_emulate > 0) {
			button = gs->button_emulate;
			gs->button_emulate = 0;
		}
		CLEARBIT(gs->buttons, button);
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_button_up: %d up\n", button);
#endif
	}
}

static void trigger_button_down(struct Gestures* gs, int button)
{
	if (IS_VALID_BUTTON(button) && (button != gs->button_delayed || gs->button_delayed_time == 0)) {
		SETBIT(gs->buttons, button);
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_button_down: %d down\n", button);
#endif
	}
#if DEBUG_GESTURES
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
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_button_emulation: %d emulated\n", button);
#endif
	}
}

static void trigger_button_click(struct Gestures* gs,
			int button, mstime_t trigger_up_time)
{
	if (IS_VALID_BUTTON(button) && gs->button_delayed_time == 0) {
		trigger_button_down(gs, button);
		gs->button_delayed = button;
		gs->button_delayed_ms = 0;
		gs->button_delayed_time = trigger_up_time;
#if DEBUG_GESTRUES
		xf86Msg(X_INFO, "trigger_button_click: %d placed in delayed mode\n");
#endif
	}
#if DEBUG_GESTURES
	else if (IS_VALID_BUTTON(button))
		xf86Msg(X_INFO, "trigger_button_click: %d ignored, in delayed mode\n", button);
#endif
}

static void trigger_drag_ready(struct Gestures* gs,
			mstime_t expire_time)
{
	gs->move_drag = GS_DRAG_READY;
	gs->move_drag_expire = expire_time;
#if DEBUG_GESTURES
	xf86Msg(X_INFO, "trigger_drag_ready: drag is ready\n");
#endif
}

static void trigger_drag_start(struct Gestures* gs)
{
	if (gs->move_drag == GS_DRAG_READY) {
		gs->button_delayed = 0;
		gs->button_delayed_ms = 0;
		gs->button_delayed_time = 0;
		gs->move_drag = GS_DRAG_ACTIVE;
		gs->move_drag_expire = 0;
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_drag_start: drag is active\n");
#endif
	}
}

static void trigger_drag_stop(struct Gestures* gs, int force)
{
	if (gs->move_drag == GS_DRAG_READY && force) {
		gs->move_drag = GS_NONE;
		gs->move_drag_expire = 0;
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_drag_stop: drag canceled\n");
#endif
	}
	else if (gs->move_drag == GS_DRAG_ACTIVE) {
		gs->move_drag = GS_NONE;
		gs->move_drag_expire = 0;
		trigger_button_up(gs, 0);
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_drag_stop: drag stopped\n");
#endif
	}
}

static void buttons_update(struct Gestures* gs,
			const struct MConfig* cfg,
			const struct HWState* hs,
			struct MTState* ms)
{
	if (!cfg->button_enable)
		return;

	static bitmask_t button_prev = 0U;
	int i, n, down, emulate;
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
		gs->move_type = GS_NONE;
		gs->move_wait = hs->evtime + cfg->gesture_wait;
		n = 0;
		foreach_bit(i, ms->touch_used) {
			if (GETBIT(ms->touch[i].state, MT_INVALID))
				continue;
			if (cfg->button_integrated && !GETBIT(ms->touch[i].flags, GS_BUTTON))
				SETBIT(ms->touch[i].flags, GS_BUTTON);
			n++;
		}

		if (emulate) {
			if (cfg->button_integrated)
				n--;

			if (n == 1 && cfg->button_1touch > 0)
				trigger_button_emulation(gs, cfg->button_1touch);
			else if (n == 2 && cfg->button_2touch > 0)
				trigger_button_emulation(gs, cfg->button_2touch);
		}
	}
}

static void tapping_update(struct Gestures* gs,
			const struct MConfig* cfg,
			const struct HWState* hs,
			struct MTState* ms)
{
	if (cfg->tap_1touch < 0 && cfg->tap_2touch < 0 && cfg->tap_3touch < 0)
		return;

	int i, n, dist, released_max;
	released_max = MAXVAL(cfg->tap_1touch, MAXVAL(cfg->tap_2touch, cfg->tap_3touch));

	if (gs->tap_time_down != 0 && hs->evtime >= gs->tap_time_down + cfg->tap_timeout) {
		gs->tap_time_down = 0;
		gs->tap_touching = 0;
		gs->tap_released = 0;
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
				}
			}
			else {
				if (GETBIT(ms->touch[i].state, MT_NEW)) {
					SETBIT(ms->touch[i].flags, GS_TAP);
					gs->tap_touching++;
					if (gs->tap_time_down == 0)
						gs->tap_time_down = hs->evtime;
				}

				if (GETBIT(ms->touch[i].flags, GS_TAP)) {
					dist = dist2(ms->touch[i].total_dx, ms->touch[i].total_dy);
					if (dist >= cfg->tap_dist) {
						CLEARBIT(ms->touch[i].flags, GS_TAP);
						gs->tap_touching--;
					}
					else if (GETBIT(ms->touch[i].state, MT_RELEASED)) {
						gs->tap_touching--;
						gs->tap_released++;
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
			n = cfg->tap_1touch;
		else if (gs->tap_released == 2)
			n = cfg->tap_2touch;
		else
			n = cfg->tap_3touch;

		trigger_button_click(gs, n, hs->evtime + cfg->tap_hold);
		if (cfg->drag_enable && n == 0)
			trigger_drag_ready(gs, hs->evtime + cfg->drag_timeout);

		gs->move_type = GS_NONE;
		gs->move_wait = hs->evtime + cfg->gesture_wait;

		gs->tap_time_down = 0;
		gs->tap_touching = 0;
		gs->tap_released = 0;
	}
}

static void trigger_move(struct Gestures* gs,
			const struct MConfig* cfg,
			const struct HWState* hs,
			int dx, int dy)
{
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_move: move_type = %d, evtime = %llu, move_wait = %llu, dx = %d, dy = %d\n", gs->move_type, hs->evtime, gs->move_wait, dx, dy);
#endif
	if ((gs->move_type == GS_MOVE || hs->evtime >= gs->move_wait) && (dx != 0 || dy != 0)) {
		trigger_drag_start(gs);
		gs->move_dx = (int)(dx*cfg->sensitivity);
		gs->move_dy = (int)(dy*cfg->sensitivity);
		gs->move_type = GS_MOVE;
		gs->move_wait = 0;
		gs->move_dist = 0;
		gs->move_dir = TR_NONE;
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_move: %d, %d\n", dx, dy);
#endif
	}
}

static void trigger_scroll(struct Gestures* gs,
			const struct MConfig* cfg,
			const struct HWState* hs,
			int dist, int dir)
{
	if (gs->move_type == GS_SCROLL || hs->evtime >= gs->move_wait) {
		trigger_drag_stop(gs, 1);
		if (gs->move_type != GS_SCROLL || gs->move_dir != dir)
			gs->move_dist = 0;
		gs->move_dx = 0;
		gs->move_dy = 0;
		gs->move_type = GS_SCROLL;
		gs->move_wait = hs->evtime + cfg->gesture_wait;
		gs->move_dist += ABSVAL(dist);
		gs->move_dir = dir;
		if (gs->move_dist >= cfg->scroll_dist) {
			gs->move_dist = MODVAL(gs->move_dist, cfg->scroll_dist);
			if (dir == TR_DIR_UP)
				trigger_button_click(gs, cfg->scroll_up_btn, hs->evtime + cfg->gesture_hold);
			else if (dir == TR_DIR_DN)
				trigger_button_click(gs, cfg->scroll_dn_btn, hs->evtime + cfg->gesture_hold);
			else if (dir == TR_DIR_LT)
				trigger_button_click(gs, cfg->scroll_lt_btn, hs->evtime + cfg->gesture_hold);
			else if (dir == TR_DIR_RT)
				trigger_button_click(gs, cfg->scroll_rt_btn, hs->evtime + cfg->gesture_hold);
		}
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_scroll: scrolling %+d in direction %d (at %d of %d)\n", dist, dir, gs->move_dist, cfg->scroll_dist);
#endif
	}
}

static void trigger_swipe(struct Gestures* gs,
			const struct MConfig* cfg,
			const struct HWState* hs,
			int dist, int dir)
{
	if (gs->move_type == GS_SWIPE || hs->evtime >= gs->move_wait) {
		trigger_drag_stop(gs, 1);
		if (gs->move_type != GS_SWIPE || gs->move_dir != dir)
			gs->move_dist = 0;
		gs->move_dx = 0;
		gs->move_dy = 0;
		gs->move_type = GS_SWIPE;
		gs->move_wait = hs->evtime + cfg->gesture_wait;
		gs->move_dist += ABSVAL(dist);
		gs->move_dir = dir;
		if (gs->move_dist >= cfg->swipe_dist) {
			gs->move_dist = MODVAL(gs->move_dist, cfg->swipe_dist);
			if (dir == TR_DIR_UP)
				trigger_button_click(gs, cfg->swipe_up_btn, hs->evtime + cfg->gesture_hold);
			else if (dir == TR_DIR_DN)
				trigger_button_click(gs, cfg->swipe_dn_btn, hs->evtime + cfg->gesture_hold);
			else if (dir == TR_DIR_LT)
				trigger_button_click(gs, cfg->swipe_lt_btn, hs->evtime + cfg->gesture_hold);
			else if (dir == TR_DIR_RT)
				trigger_button_click(gs, cfg->swipe_rt_btn, hs->evtime + cfg->gesture_hold);
		}
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_swipe: swiping %+d in direction %d (at %d of %d)\n", dist, dir, gs->move_dist, cfg->swipe_dist);
#endif
	}
}

static void trigger_scale(struct Gestures* gs,
			const struct MConfig* cfg,
			const struct HWState* hs,
			int dist, int dir)
{
	if (gs->move_type == GS_SCALE || hs->evtime >= gs->move_wait) {
		trigger_drag_stop(gs, 1);
		if (gs->move_type != GS_SCALE || gs->move_dir != dir)
			gs->move_dist = 0;
		gs->move_dx = 0;
		gs->move_dy = 0;
		gs->move_type = GS_SCALE;
		gs->move_wait = hs->evtime + cfg->gesture_wait;
		gs->move_dist += ABSVAL(dist);
		gs->move_dir = dir;
		if (gs->move_dist >= cfg->scale_dist) {
			gs->move_dist = MODVAL(gs->move_dist, cfg->scale_dist);
			if (dir == TR_DIR_UP)
				trigger_button_click(gs, cfg->scale_up_btn, hs->evtime + cfg->gesture_hold);
			else if (dir == TR_DIR_DN)
				trigger_button_click(gs, cfg->scale_dn_btn, hs->evtime + cfg->gesture_hold);
		}
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_scale: scaling %+d in direction %d (at %d of %d)\n", dist, dir, gs->move_dist, cfg->scale_dist);
#endif
	}
}

static void trigger_rotate(struct Gestures* gs,
			const struct MConfig* cfg,
			const struct HWState* hs,
			int dist, int dir)
{
	if (gs->move_type == GS_ROTATE || hs->evtime >= gs->move_wait) {
		trigger_drag_stop(gs, 1);
		if (gs->move_type != GS_ROTATE || gs->move_dir != dir)
			gs->move_dist = 0;
		gs->move_dx = 0;
		gs->move_dy = 0;
		gs->move_type = GS_ROTATE;
		gs->move_wait = hs->evtime + cfg->gesture_wait;
		gs->move_dist += ABSVAL(dist);
		gs->move_dir = dir;
		if (gs->move_dist >= cfg->scale_dist) {
			gs->move_dist = MODVAL(gs->move_dist, cfg->scale_dist);
			if (dir == TR_DIR_LT)
				trigger_button_click(gs, cfg->rotate_lt_btn, hs->evtime + cfg->gesture_hold);
			else if (dir == TR_DIR_RT)
				trigger_button_click(gs, cfg->rotate_rt_btn, hs->evtime + cfg->gesture_hold);
		}
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_rotate: rotating %+d in direction %d (at %d of %d)\n", dist, dir, gs->move_dist, cfg->rotate_dist);
#endif
	}
}

static void trigger_reset(struct Gestures* gs)
{
	trigger_drag_stop(gs, 0);
	gs->move_dx = 0;
	gs->move_dy = 0;
	gs->move_type = GS_NONE;
	gs->move_wait = 0;
	gs->move_dist = 0;
	gs->move_dir = TR_NONE;
}

static int get_scroll_dir(const struct Touch* t1,
			const struct Touch* t2)
{
	if (trig_angles_acute(t1->direction, t2->direction) < 2.0)
		return trig_generalize(t1->direction);
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
	double d1, d2;
	d1 = MINVAL(t1->direction, MINVAL(t2->direction, t3->direction));
	d2 = MAXVAL(t1->direction, MAXVAL(t2->direction, t3->direction));
	if (trig_angles_acute(d1, d2) < 2)
		return trig_generalize(t1->direction);
	return TR_NONE;
}

static void moving_update(struct Gestures* gs,
			const struct MConfig* cfg,
			const struct HWState* hs,
			struct MTState* ms)
{
	int i, count, btn_count, dx, dy, dist, dir;
	struct Touch* touches[3];
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
			if (count < 3)
				touches[count++] = &ms->touch[i];
		}
	}

#if DEBUG_GESTURES
		xf86Msg(X_INFO, "moving_update: count = %d, btn_count = %d\n", count, btn_count);
#endif

	// Determine gesture type.
	if (count == 0) {
		if (btn_count > 0)
			trigger_move(gs, cfg, hs, dx, dy);
		else
			trigger_reset(gs);
	}
	else if (count == 1) {
		dx += touches[0]->dx;
		dy += touches[0]->dy;
		trigger_move(gs, cfg, hs, dx, dy);
	}
	else if (count == 2) {
		// scroll, scale, or rotate
		if ((dir = get_scroll_dir(touches[0], touches[1])) != TR_NONE) {
			if (dir == TR_DIR_LT || dir == TR_DIR_RT)
				dist = touches[0]->dx + touches[1]->dx;
			else
				dist = touches[0]->dy + touches[1]->dy;
			trigger_scroll(gs, cfg, hs, dist/2, dir);
		}
		else if ((dir = get_rotate_dir(touches[0], touches[1])) != TR_NONE) {
			dist = dist2(touches[0]->dx, touches[0]->dy) + dist2(touches[1]->dx, touches[1]->dy);
			trigger_rotate(gs, cfg, hs, dist/2, dir);
		}
		else if ((dir = get_scale_dir(touches[0], touches[1])) != TR_NONE) {
			dist = dist2(touches[0]->dx, touches[0]->dy) + dist2(touches[1]->dx, touches[1]->dy);
			trigger_scale(gs, cfg, hs, dist/2, dir);
		}
	}
	else if (count == 3) {
		if ((dir = get_swipe_dir(touches[0], touches[1], touches[2])) != TR_NONE) {
			if (dir == TR_DIR_LT || dir == TR_DIR_RT)
				dist = touches[0]->dx + touches[1]->dx + touches[2]->dx;
			else
				dist = touches[0]->dy + touches[1]->dy + touches[2]->dy;
			trigger_swipe(gs, cfg, hs, dist/3, dir);
		}
	}
}

static void dragging_update(struct Gestures* gs,
			const struct HWState* hs)
{
	if (gs->move_drag == GS_DRAG_READY && hs->evtime > gs->move_drag_expire) {
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "dragging_update: drag expired\n");
#endif
		trigger_drag_stop(gs, 1);
	}
}

static void delayed_update(struct Gestures* gs,
			const struct HWState* hs)
{
	if (gs->button_delayed_time == 0)
		return;

	if (hs->evtime >= gs->button_delayed_time) {
#if DEBUG_GESTURES
		xf86Msg(X_INFO, "delayed_update: %d delay expired, triggering up\n", gs->button_delayed);
#endif
		trigger_button_up(gs, gs->button_delayed);
		gs->button_delayed_time = 0;
		gs->button_delayed_ms = 0;
		gs->button_delayed = 0;
	}
	else
		gs->button_delayed_ms = gs->button_delayed_time - hs->evtime;
}

void gestures_init(struct Gestures* gs)
{
	memset(gs, 0, sizeof(struct Gestures));
}

void gestures_extract(struct Gestures* gs,
			const struct MConfig* cfg,
			const struct HWState* hs,
			struct MTState* ms)
{
	dragging_update(gs, hs);
	buttons_update(gs, cfg, hs, ms);
	tapping_update(gs, cfg, hs, ms);
	moving_update(gs, cfg, hs, ms);
	delayed_update(gs, hs);
}

int gestures_delayed(struct Gestures* gs,
			struct mtdev* dev, int fd)
{
	if (gs->button_delayed_time > 0) {
		if (mtdev_empty(dev) && mtdev_idle(dev, fd, gs->button_delayed_ms)) {
#if DEBUG_GESTURES
			xf86Msg(X_INFO, "gestures_delayed: %d up, timer expired\n", gs->button_delayed);
#endif
			trigger_button_up(gs, gs->button_delayed);
			gs->move_dx = 0;
			gs->move_dy = 0;
			gs->button_delayed_time = 0;
			gs->button_delayed_ms = 0;
			gs->button_delayed = 0;
			return 1;
		}
	}
	return 0;
}

