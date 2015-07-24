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

#define TOUCHES_MAX 4

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

	if (IS_VALID_BUTTON(button) && (button != gs->button_delayed || !IS_VALID_BUTTON(gs->button_delayed))) {
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

static void trigger_delayed_button_unsafe(struct Gestures* gs)
{
	int button;

	// clear button before timer (unless compiler decide otherwise)
	button = gs->button_delayed;

	gs->button_delayed = -1;
	timerclear(&gs->button_delayed_time);
#ifdef DEBUG_GESTURES
	xf86Msg(X_INFO, "trigger_delayed_button: %d up, timer expired\n", button);
#endif
	trigger_button_up(gs, button);
}

/*
 * If trigger_up_time is NULL or epoch time it will set timer to infinity - button up will
 * be send when user finish gesture.
 */
static void trigger_button_click(struct Gestures* gs,
			int button, struct timeval* trigger_up_time)
{
#ifdef DEBUG_GESTURES
	struct timeval delta;
#endif

	if (!IS_VALID_BUTTON(button))
		return;

	if (!IS_VALID_BUTTON(gs->button_delayed)) {
		trigger_button_down(gs, button);
		gs->button_delayed = button;
		if(trigger_up_time == NULL)
			timerclear(&gs->button_delayed_time);		// "infinite timer", wait for gesture end
		else
			timercp(&gs->button_delayed_time, trigger_up_time);	// may be also "infinite"

#ifdef DEBUG_GESTURES
		timersub(&gs->button_delayed_time, &gs->time, &delta);
		xf86Msg(X_INFO, "trigger_button_click: %d placed in delayed mode; delta: %d ms\n", button, timertoms(&delta));
#endif
	}
#ifdef DEBUG_GESTURES
	else
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
		int earliest, latest, lowest, moving = 0;
		gs->move_type = GS_NONE;
		timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);
		earliest = -1;
		latest = -1;
		lowest = -1;
		foreach_bit(i, ms->touch_used) {
			if (!cfg->button_zones && GETBIT(ms->touch[i].state, MT_INVALID))
				continue;
			if (lowest == -1 || ms->touch[i].y > ms->touch[lowest].y)
				lowest = i;
			if (cfg->button_integrated && !GETBIT(ms->touch[i].flags, GS_BUTTON))
				SETBIT(ms->touch[i].flags, GS_BUTTON);
			if (earliest == -1 || timercmp(&ms->touch[i].down, &ms->touch[earliest].down, <))
				earliest = i;
			if (latest == -1 || timercmp(&ms->touch[i].down, &ms->touch[latest].down, >))
				latest = i;
		}

		if (emulate) {
			if (cfg->button_zones && lowest >= 0) {
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
					pos = cfg->pad_width / 2 + ms->touch[lowest].x;
#ifdef DEBUG_GESTURES
					xf86Msg(X_INFO, "buttons_update: pad width %d, zones %d, zone width %f, x %d\n",
						cfg->pad_width, zones, width, pos);
#endif
					for (i = 0; i < zones; i++) {
						left = width*i;
						right = width*(i+1);
						if ((i == 0 || pos >= left) && (i == zones - 1 || pos <= right)) {
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
					if ((cfg->button_move || cfg->button_expire == 0 || timercmp(&ms->touch[latest].down, &expire, <)) &&
                            !(GETBIT(ms->touch[i].state, MT_THUMB) && cfg->ignore_thumb) &&
                            !(GETBIT(ms->touch[i].state, MT_PALM) && cfg->ignore_palm)) {
						touching++;
                    }
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
		// too much time passed by from first touch, stop waiting for incoming touches
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
		// in this branch tap was recognized as button click
		// clear tap flags from touches
		foreach_bit(i, ms->touch_used) {
			if (GETBIT(ms->touch[i].flags, GS_TAP))
				CLEARBIT(ms->touch[i].flags, GS_TAP);
		}
		// determinate which button was "tapped" by counting touches
		if (gs->tap_released == 1)
			n = cfg->tap_1touch - 1;
		else if (gs->tap_released == 2)
			n = cfg->tap_2touch - 1;
		else if (gs->tap_released == 3)
			n = cfg->tap_3touch - 1;
		else
			n = cfg->tap_4touch - 1;

		// how long button should be hold down
		timeraddms(&gs->time, cfg->tap_hold, &tv_tmp);
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
			gs->move_speed = hypot(gs->move_dx, gs->move_dy)/timertomicro(&gs->dt);
			timerclear(&gs->move_wait);
#ifdef DEBUG_GESTURES
			xf86Msg(X_INFO, "trigger_move: %d, %d (speed %f)\n",
				gs->move_dx, gs->move_dy, gs->move_speed);
#endif
		}
	}
}

static int get_swipe_dir_n(const struct Touch* touches[TOUCHES_MAX], int count)
{
	if(count > TOUCHES_MAX || count <= 0)
		return TR_NONE;

	int i, avg_dir;
	double angles[TOUCHES_MAX];

	/* Find average direction */
	for (i = 0; i < count; ++i) {
		if (touches[i]->direction == TR_NONE)
			return TR_NONE;
		angles[i] = touches[i]->direction;
	}
	avg_dir = trig_angles_avg(angles, count);

	/* Check if all touches have (almost) same direction */
	for (i = 0; i < count; ++i) {
		if (trig_angles_acute(avg_dir, touches[i]->direction) > 1.0)
			return TR_NONE;
	}
	return avg_dir;
}

static void get_swipe_avg_xy(const struct Touch* touches[TOUCHES_MAX], int count, double* out_x, double* out_y){
	double x, y;
	x = y = 0.0;
	int i;
	for (i = 0; i < count; ++i) {
		x += touches[i]->dx;
		y += touches[i]->dy;
	}
	*out_x = x/(double)count;
	*out_y = y/(double)count;
}

static struct MConfigSwipe tap_n_move_cfg = {
	.drag_sens = 1000,
	.dn_btn = 1,
	.up_btn = 1,
	.lt_btn = 1,
	.rt_btn = 1,
	.dist = 1,
	.hold = 0
};

int get_button_for_dir(const struct MConfigSwipe* cfg_swipe, int dir){
	switch (dir){
	case TR_NONE:
		if(cfg_swipe->up_btn == cfg_swipe->lt_btn && cfg_swipe->lt_btn == cfg_swipe->dn_btn && cfg_swipe->dn_btn == cfg_swipe->rt_btn)
			return cfg_swipe->up_btn - 1;
		return -1;
	case TR_DIR_UP:
		return cfg_swipe->up_btn - 1;
	case TR_DIR_DN:
		return cfg_swipe->dn_btn - 1;
	case TR_DIR_LT:
		return cfg_swipe->lt_btn - 1;
	case TR_DIR_RT:
		return cfg_swipe->rt_btn - 1;
	case (8 + TR_DIR_UP + TR_DIR_LT) / 2:
		if(cfg_swipe->up_btn != cfg_swipe->lt_btn)
			return -1;
		return cfg_swipe->up_btn - 1;
	case (TR_DIR_LT + TR_DIR_DN) / 2:
		if(cfg_swipe->lt_btn != cfg_swipe->dn_btn)
			return -1;
		return cfg_swipe->lt_btn - 1;
	case (TR_DIR_DN + TR_DIR_RT) / 2:
		if(cfg_swipe->dn_btn != cfg_swipe->rt_btn)
			return -1;
		return cfg_swipe->dn_btn - 1;
	case (TR_DIR_RT + TR_DIR_UP) / 2:
		if(cfg_swipe->rt_btn != cfg_swipe->up_btn)
			return -1;
		return cfg_swipe->rt_btn - 1;
	default:
		return -1;
	}
}

/* To avoid users' confusion accept diagonal direction only if same button was
 * bound to aligned directions.
 * If different buttons were used and it's diagonal movement we can't decide
 * which button schould be generated, so it will be ignored.
 */
static int get_unequivocal_button(const struct MConfigSwipe* cfg_swipe, int dir){
	int button;

	button = get_button_for_dir(cfg_swipe, dir);
	if (button == get_button_for_dir(cfg_swipe, trig_generalize(dir)))
		return button;
	return -1;
}

/* Return:
 *  0 - it wasn't swipe
 *  1 - it was swipe and was executed
 *  other value - it was swipe, but couldn't be executed
 */
static int trigger_swipe(struct Gestures* gs,
			const struct MConfig* cfg, const struct Touch* touches[4], int touches_count)
{
	int move_type_to_trigger, dir;
	double avg_move_x, avg_move_y, dist;
	const struct MConfigSwipe* cfg_swipe;
	struct timeval tv_tmp;
	int button;

	switch(touches_count){
	case 2:
		cfg_swipe = &cfg->scroll;
		move_type_to_trigger = GS_SCROLL;
		break;
	case 3:
		cfg_swipe = &cfg->swipe3;
		move_type_to_trigger = GS_SWIPE3;
		break;
	case 4:
		cfg_swipe = &cfg->swipe4;
		move_type_to_trigger = GS_SWIPE4;
		break;
	default:
			return 2;
	}

	dir = get_swipe_dir_n(touches, touches_count);

	if(dir == TR_NONE)
		return 0;
	button = get_unequivocal_button(cfg_swipe, dir);
	if (!IS_VALID_BUTTON(button)){
		return 3;
	}

	if (gs->move_type == move_type_to_trigger || timercmp(&gs->time, &gs->move_wait, >=)) {
		trigger_drag_stop(gs, 1);
		get_swipe_avg_xy(touches, touches_count, &avg_move_x, &avg_move_y);
		// hypot(1/n * (x0 + ... + xn); 1/n * (y0 + ... + yn)) <=> 1/n * hypot(x0 + ... + xn; y0 + ... + yn)
		dist = hypot(avg_move_x, avg_move_y);
		if(cfg_swipe->drag_sens){
			gs->move_dx = (int)(cfg->sensitivity * avg_move_x * cfg_swipe->drag_sens * 0.001);
			gs->move_dy = (int)(cfg->sensitivity * avg_move_y * cfg_swipe->drag_sens * 0.001);
		} else{
			gs->move_dx = 0;
			gs->move_dy = 0;
		}
		if (gs->move_type != move_type_to_trigger){
			trigger_delayed_button_unsafe(gs);
			gs->move_dist = 0;
		}
		else if (gs->move_dir != dir)
			gs->move_dist = 0;
		gs->move_type = move_type_to_trigger;
		gs->move_dist += (int)ABSVAL(dist);
		gs->move_dir = dir;
		gs->move_speed = dist/timertomicro(&gs->dt);
		timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);

		if (cfg_swipe->dist > 0 && gs->move_dist >= cfg_swipe->dist) {
			if(cfg_swipe->hold != 0)
				timeraddms(&gs->time, cfg_swipe->hold, &tv_tmp);
			else
				timerclear(&tv_tmp); // wait for gesture end
			gs->move_dist = MODVAL(gs->move_dist, cfg_swipe->dist);
			trigger_button_click(gs, button, &tv_tmp);
		}
#ifdef DEBUG_GESTURES
			xf86Msg(X_INFO, "trigger_swipe_button: swiping %+f in direction %d (at %d of %d) (speed %f)\n",
				dist, dir, gs->move_dist, cfg_swipe->dist, gs->move_speed);
#endif
	}
	return 1;
}

static int trigger_tap_n_swipe(struct Gestures* gs,
			const struct MConfig* cfg, const struct Touch* touches[TOUCHES_MAX], int touches_count)
{
	int dir, button, move_type_to_trigger;
	int is_tap_n_swipe;
	double avg_move_x, avg_move_y, dist;
	const struct MConfigSwipe* cfg_swipe;
	struct timeval tv_tmp;

	switch(touches_count){
		case 2:
			move_type_to_trigger = GS_TAP_N_SWIPE2;
			break;
		case 3:
			move_type_to_trigger = GS_TAP_N_SWIPE3;
			break;
		case 4:
			move_type_to_trigger = GS_TAP_N_SWIPE4;
			break;
	}

	is_tap_n_swipe = gs->move_type == move_type_to_trigger || touches[0]->direction == TR_NONE || (touches[0]->total_dx < 10 && touches[0]->total_dy < 20);

	if(is_tap_n_swipe == FALSE)
		return 0;

	/* skip first touch with dir == NONE */
	//++touches;
	//--touches_count;
	cfg_swipe = &tap_n_move_cfg;

	dir = get_swipe_dir_n(touches+1, touches_count-1);

	timeraddms(&touches[0]->down, cfg->tap_timeout * 2, &tv_tmp);
	if (timercmp(&touches[touches_count-1]->down, &tv_tmp, >=)){
		gs->move_type = move_type_to_trigger;
		xf86Msg(X_INFO, "gs->move_type = move_type_to_trigger\n");
	}
	else if (dir == TR_NONE){
		return 0;
	}

	button = get_unequivocal_button(cfg_swipe, dir);
	if (!IS_VALID_BUTTON(button)){
		return 3;
	}
	dir = trig_generalize(dir);

	if (gs->move_type == move_type_to_trigger) {
		trigger_drag_stop(gs, 1);
		get_swipe_avg_xy(touches + 1, touches_count - 1, &avg_move_x, &avg_move_y);
		// hypot(1/n * (x0 + ... + xn); 1/n * (y0 + ... + yn)) <=> 1/n * hypot(x0 + ... + xn; y0 + ... + yn)
		dist = hypot(avg_move_x, avg_move_y);
		if(cfg_swipe->drag_sens){
			gs->move_dx = (int)(cfg->sensitivity * avg_move_x * cfg_swipe->drag_sens * 0.001);
			gs->move_dy = (int)(cfg->sensitivity * avg_move_y * cfg_swipe->drag_sens * 0.001);
		} else{
			gs->move_dx = 0;
			gs->move_dy = 0;
		}
		if (gs->move_type != move_type_to_trigger){
			trigger_delayed_button_unsafe(gs);
			gs->move_dist = 0;
		}
		else if (gs->move_dir != dir)
			gs->move_dist = 0;
		gs->move_type = move_type_to_trigger;
		gs->move_dist += (int)ABSVAL(dist);
		gs->move_dir = dir;
		gs->move_speed = dist/timertomicro(&gs->dt);
		if (cfg_swipe->dist > 0 && gs->move_dist >= cfg_swipe->dist) {
			if(cfg_swipe->hold != 0)
				timeraddms(&gs->time, cfg_swipe->hold, &tv_tmp);
			else
				timerclear(&tv_tmp); // wait for gesture end
			gs->move_dist = MODVAL(gs->move_dist, cfg_swipe->dist);
			trigger_button_click(gs, button, &tv_tmp);
		}
	}
	return 1;
}

static void trigger_scale(struct Gestures* gs,
			const struct MConfig* cfg,
			double dist, int dir)
{
	if (gs->move_type == GS_SCALE || !timercmp(&gs->time, &gs->move_wait, <)) {
		struct timeval tv_tmp;
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
		if (gs->move_dist >= cfg->scale_dist) {
			gs->move_dist = MODVAL(gs->move_dist, cfg->scale_dist);
			timeraddms(&gs->time, cfg->gesture_hold, &tv_tmp);
			if (dir == TR_DIR_UP)
				trigger_button_click(gs, cfg->scale_up_btn - 1, &tv_tmp);
			else if (dir == TR_DIR_DN)
				trigger_button_click(gs, cfg->scale_dn_btn - 1, &tv_tmp);
		}
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_scale: scaling %+f in direction %d (at %d of %d) (speed %f)\n",
			dist, dir, gs->move_dist, cfg->scale_dist, gs->move_speed);
#endif
	}
}

static void trigger_rotate(struct Gestures* gs,
			const struct MConfig* cfg,
			double dist, int dir)
{
	if (gs->move_type == GS_ROTATE || !timercmp(&gs->time, &gs->move_wait, <)) {
		struct timeval tv_tmp;
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
		if (gs->move_dist >= cfg->rotate_dist) {
			gs->move_dist = MODVAL(gs->move_dist, cfg->rotate_dist);
			timeraddms(&gs->time, cfg->gesture_hold, &tv_tmp);
			if (dir == TR_DIR_LT)
				trigger_button_click(gs, cfg->rotate_lt_btn - 1, &tv_tmp);
			else if (dir == TR_DIR_RT)
				trigger_button_click(gs, cfg->rotate_rt_btn - 1, &tv_tmp);
		}
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "trigger_rotate: rotating %+f in direction %d (at %d of %d) (speed %f)\n",
			dist, dir, gs->move_dist, cfg->rotate_dist, gs->move_speed);
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

static void moving_update(struct Gestures* gs,
			const struct MConfig* cfg,
			struct MTState* ms)
{
	int i, count, btn_count, dx, dy, dir;
	double dist;
	const struct Touch* touches[TOUCHES_MAX];
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
			if (count < TOUCHES_MAX)
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
		if(gs->move_type == GS_TAP_N_SWIPE2 && touches[0]->dx < 10 && touches[0]->dy < 10){
			/* nothing */
		}
		else{
			dx += touches[0]->dx;
			dy += touches[0]->dy;
			trigger_move(gs, cfg, dx, dy);
		}
	}
	else if (count == 2 && cfg->trackpad_disable < 1) {
		// scroll, scale, or rotate
		if (trigger_tap_n_swipe(gs, cfg, touches, count)){
			/* nothing to do */
		}
		else if (trigger_swipe(gs, cfg, touches, count)) {
			/* nothing to do */
		}
		else if ((dir = get_rotate_dir(touches[0], touches[1])) != TR_NONE) {
			dist = ABSVAL(hypot(touches[0]->dx, touches[0]->dy)) +
				ABSVAL(hypot(touches[1]->dx, touches[1]->dy));
			trigger_rotate(gs, cfg, dist/2, dir);
		}
		else if ((dir = get_scale_dir(touches[0], touches[1])) != TR_NONE) {
			dist = ABSVAL(hypot(touches[0]->dx, touches[0]->dy)) +
				ABSVAL(hypot(touches[1]->dx, touches[1]->dy));
			trigger_scale(gs, cfg, dist/2, dir);
		}
	}
	else if ((count == 3 || count == 4) && cfg->trackpad_disable < 1) {
		if (trigger_tap_n_swipe(gs, cfg, touches, count)){
			/* nothing to do */
		}
		else if (trigger_swipe(gs, cfg, touches, count)) {
			/* nothing to do */
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

static int is_timer_infinite(struct Gestures* gs){
	return isepochtime(&gs->button_delayed_time);
}

static void delayed_update(struct Gestures* gs)
{
	// if there's no delayed button - return
	if(!IS_VALID_BUTTON(gs->button_delayed))
		return;

	if (!is_timer_infinite(gs) && timercmp(&gs->time, &gs->button_delayed_time, >=)) {
#ifdef DEBUG_GESTURES
		xf86Msg(X_INFO, "delayed_update: %d delay expired, triggering up\n", gs->button_delayed);
#endif
		trigger_delayed_button_unsafe(gs);
	}
	else {
#ifdef DEBUG_GESTURES
		struct timeval delta;
		timersub(&gs->button_delayed_time, &gs->time, &delta);
		xf86Msg(X_INFO, "delayed_update: %d still waiting, new delta %d ms\n", gs->button_delayed, timertoms(&delta));
#endif
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

/*
 * Executed every input time frame, at least once. First time from 'read_input' to check if
 * timer is needed.
 * Return value 1 means that next timer should be installed with this function as a
 * callabck and gs->button_delayed_delta as delay time.
 *
 * Return vale meaning:
 *  - 0 - no delay to handle, don't install timer, do nothing
 *  - 1 - only install timer
 *  - 2 - state was changed, so caller have to cancel timer and handle state change
 *        by calling TimerCancel() and handle_gestures()
 */
int gestures_delayed(struct MTouch* mt)
{
	struct Gestures* gs = &mt->gs;
	struct MTState* ms = &mt->state;
	struct timeval now, delta;
	int i, taps_released;

	taps_released = 0;

	// if there's no delayed button - return
	if(!IS_VALID_BUTTON(gs->button_delayed))
		return 0;

	// count released fingers
	foreach_bit(i, ms->touch_used) {
		if (GETBIT(ms->touch[i].state, MT_RELEASED))
			++taps_released;
	}

	if(taps_released != 0){
		// if one of fingers were released it means that gesture was finished so
		// send "button up" event immediately without checking for delivery time
		// for tap&swipe gesture user have to release first finger to end gesture
		if (gs->move_type != GS_TAP_N_SWIPE2 || GETBIT(ms->touch[0].state, MT_RELEASED)){
			trigger_delayed_button_unsafe(gs);
			gs->move_dx = gs->move_dy = 0;
			gs->move_type = GS_NONE;
			return 2;
		}
	}

	if(is_timer_infinite(gs))
		return 0;

	microtime(&now);
	timersub(&now, &mt->gs.time, &mt->gs.dt);
	timercp(&mt->gs.time, &now);

	if(timercmp(&gs->button_delayed_time, &now, >)){
		 timersub(&gs->button_delayed_time, &now, &delta);
		 // That second check may seem unnecessary, but it is not.
		 // Even if button delayed time is > than now time, timertoms may still return 0
		 // because it truncates time to miliseconds. It's important because truncated time
		 // is used to setup timer.
		 if(timertoms(&delta) > 1){
#ifdef DEBUG_GESTURES
			xf86Msg(X_INFO, "gestures_delayed: %d delayed, new delta: %d ms\n", gs->button_delayed, timertoms(&delta));
#endif
			return 1; // install timer
		 }
	}

	trigger_delayed_button_unsafe(gs);
	gs->move_dx = gs->move_dy = 0;
	return 2;
}

