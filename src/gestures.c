/***************************************************************************
 *
 * Multitouch X driver
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
 * Copyright (C) 2011 Ryan Bourgeois <bluedragonx@gmail.com>
 * Copyright (C) 2015-2018 Paweł Turkowski <p2rkw0@gmail.com>
 *
 * Gestures
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
 * Copyright (C) 2010 Arturo Castro <mail@arturocastro.net>
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

#include "gestures.h"
#include "mtouch.h"
#include "trig.h"
//#define DEBUG_GESTURES 1
#ifdef DEBUG_GESTURES
# define LOG_DEBUG_GESTURES LOG_DEBUG
# define LOG_EMULATED LOG_INFO
#else
# define LOG_DEBUG_GESTURES LOG_DISABLED
# define LOG_EMULATED LOG_DISABLED
#endif


#define IS_VALID_BUTTON(x) (x >= 0 && x <= 31)

static void break_coasting(struct Gestures* gs){
	gs->scroll_speed_x = gs->scroll_speed_y = 0.0f;
}

static void trigger_button_up(struct Gestures* gs, int button)
{
	if (IS_VALID_BUTTON(button)) {
		if (button == 0 && gs->button_emulate > 0) {
			button = gs->button_emulate;
			gs->button_emulate = 0;
		}
		CLEARBIT(gs->buttons, button);
		LOG_DEBUG_GESTURES("trigger_button_up: %d up\n", button);
	}
}

static void trigger_button_down(struct Gestures* gs, int button)
{
	struct timeval epoch;
	timerclear(&epoch);

	if (IS_VALID_BUTTON(button) &&
			(button != gs->button_delayed || !IS_VALID_BUTTON(gs->button_delayed))
	) {
		SETBIT(gs->buttons, button);

		LOG_DEBUG_GESTURES("trigger_button_down: %d down\n", button);
	}
	else if (IS_VALID_BUTTON(button))
		LOG_DEBUG_GESTURES("trigger_button_down: %d down ignored, in delayed mode\n", button);
}

static void trigger_button_emulation(struct Gestures* gs, int button)
{
#if 0
	if (IS_VALID_BUTTON(button) && GETBIT(gs->buttons, 0)) {
		CLEARBIT(gs->buttons, 0);
		SETBIT(gs->buttons, button);
		gs->button_emulate = button;
		LOG_DEBUG_GESTURES("trigger_button_emulation: %d emulated\n", button);
	}
#else
	trigger_button_down(gs, button);
	gs->integrated_emulated_button = button + 1;
#endif
}

static void trigger_button_emulation_end(struct Gestures* gs)
{
	if(gs->integrated_emulated_button > 0){
		trigger_button_up(gs, gs->integrated_emulated_button - 1);
		gs->integrated_emulated_button = 0;
	}
}

int trigger_delayed_button_uncond(struct Gestures* gs)
{
	int button;

	// clear button before timer (unless compiler decide otherwise)
	button = gs->button_delayed;

	gs->button_delayed = -1;
	timerclear(&gs->button_delayed_time);
	gs->move_dist = 0; /* don't count movement from delayed button phase in next stroke */

	LOG_DEBUG_GESTURES("trigger_delayed_button: %d up, timer expired\n", button);
	trigger_button_up(gs, button);

	return button;
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
		LOG_DEBUG_GESTURES("trigger_button_click: %d placed in delayed mode; delta: %lld ms\n", button, timertoms(&delta));
#endif
	}
#ifdef DEBUG_GESTURES
	else
		LOG_DEBUG_GESTURES("trigger_button_click: %d ignored, in delayed mode\n", button);
#endif
}

static void reset_drag(struct Gestures* gs){
	if (gs->drag_state == GS_DRAG_ACTIVE) {
		gs->drag_state = GS_NONE;
		timerclear(&gs->move_drag_expire);
		trigger_button_up(gs, 0);
		LOG_DEBUG_GESTURES("reset_drag: drag stopped\n");
	}
}

static void trigger_drag_ready(struct Gestures* gs,
			const struct MConfig* cfg)
{
	gs->drag_state = GS_DRAG_READY;
	timeraddms(&gs->time, cfg->drag_timeout, &gs->move_drag_expire);
	LOG_DEBUG_GESTURES("trigger_drag_ready: drag is ready\n");
	/* Break coasting */
	break_coasting(gs);
}

static int trigger_drag_start(struct Gestures* gs,
			const struct MConfig* cfg,
			int dx, int dy)
{
	if (gs->drag_state == GS_DRAG_READY) {
		timerclear(&gs->move_drag_expire);
		if (cfg->drag_wait == 0) {
 			gs->drag_state = GS_DRAG_ACTIVE;
			trigger_button_down(gs, 0);
			LOG_DEBUG_GESTURES("trigger_drag_start: drag is active\n");
		}
		else {
			gs->drag_state = GS_DRAG_WAIT;
			gs->move_drag_dx = dx;
			gs->move_drag_dy = dy;
			timeraddms(&gs->time, cfg->drag_wait, &gs->move_drag_wait);
			LOG_DEBUG_GESTURES("trigger_drag_start: drag in wait\n");
		}
	}
	else if (gs->drag_state == GS_DRAG_WAIT) {
		gs->move_drag_dx += dx;
		gs->move_drag_dy += dy;
		if (!timercmp(&gs->time, &gs->move_drag_wait, <)) {
			gs->drag_state = GS_DRAG_ACTIVE;
			trigger_button_down(gs, 0);
			LOG_DEBUG_GESTURES("trigger_drag_start: drag is active\n");
		}
		else if (dist2(gs->move_drag_dx, gs->move_drag_dy) > SQRVAL(cfg->drag_dist)) {
			gs->drag_state = GS_NONE;
			LOG_DEBUG_GESTURES("trigger_drag_start: drag canceled, moved too far\n");
		}
	}
	return gs->drag_state != GS_DRAG_WAIT;
}

static void trigger_drag_stop(struct Gestures* gs, const struct MConfig* cfg)
{
	int lock_timeout;

	lock_timeout = cfg->drag_lock_timeout;

	if (gs->drag_state == GS_DRAG_READY) {
		/* if timeout < 0 then tap-dragging will require another tap to break the drag */
		if (lock_timeout >= 0 && timercmp(&gs->time, &gs->move_drag_expire, >=)) {
			LOG_DEBUG_GESTURES("trigger_drag_stop: locked drag expored\n");
			trigger_button_up(gs, 0);
		}
		gs->drag_state = GS_NONE;
		timerclear(&gs->move_drag_expire);
		LOG_DEBUG_GESTURES("trigger_drag_stop: drag canceled\n");
	}
	else if (gs->drag_state == GS_DRAG_ACTIVE) {
		if(lock_timeout == 0){
			reset_drag(gs);
		}
		else{
			/* Tap to drag lock timeout implementaion:
			 * Instead of breaking dragging completely, just take one step back
			 * to ready state and start timer waiting for future dragging */
			gs->drag_state = GS_DRAG_READY;
			timeraddms(&gs->time, lock_timeout, &gs->move_drag_expire);
			LOG_DEBUG_GESTURES("trigger_drag_stop: drag in wait lock\n");
		}
   }
}

/* Return 0 if current time stamp is greater than move_wait time stamp. */
static int can_change_gesture_type(struct Gestures* gs, int desired_gesture){
	if (gs->move_type == desired_gesture)
		return 1;
	if(gs->move_type == GS_NONE || gs->move_type == GS_MOVE)
		return 1;
	return timercmp(&gs->time, &gs->move_wait, >);
}

static void buttons_update(struct Gestures* gs,
			const struct MConfig* cfg,
			const struct HWState* hs,
			struct MTState* ms)
{
	if (!cfg->button_enable || cfg->trackpad_disable >= 3)
		return;

	static bitmask_t button_prev = 0U;
	int i, integrated_down, integrated_up;
	int integrated_just_clicked, integrated_just_released;

	integrated_down = 0;
	integrated_up = 0;
	integrated_just_clicked = GETBIT(hs->button, 0) && !GETBIT(button_prev, 0);
	integrated_just_released = !GETBIT(hs->button, 0) && GETBIT(button_prev, 0);

	for (i = 0; i < 32; i++) {
		if (GETBIT(hs->button, i) == GETBIT(button_prev, i))
			continue;
		if (GETBIT(hs->button, i)) {
			integrated_down++;
		}
		else {
			integrated_up++;
		}
	}
	button_prev = hs->button;

	if (integrated_down) {
		int earliest, latest, lowest;
		gs->move_type = GS_NONE;
		timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);
		earliest = -1;
		latest = -1;
		lowest = -1;
		foreach_bit(i, ms->touch_used) {
			if (!cfg->button_zones && GETBIT(ms->touch[i].flags, MT_INVALID))
				continue;
			if (cfg->button_integrated)
				SETBIT(ms->touch[i].flags, MT_BUTTON); /* Mark all existing touches as physical button press */
			if (lowest == -1 || ms->touch[i].y > ms->touch[lowest].y) /* The logic/naming seems to be inverted here */
				lowest = i;
			if (earliest == -1 || timercmp(&ms->touch[i].down, &ms->touch[earliest].down, <))
				earliest = i;
			if (latest == -1 || timercmp(&ms->touch[i].down, &ms->touch[latest].down, >))
				latest = i;
		}

		if (integrated_just_clicked) {
			LOG_EMULATED("buttons_update: integrated_just_clicked=true\n");
			int is_emulation_triggered = 0;
			if (cfg->button_zones && lowest >= 0) {
				is_emulation_triggered = buttons_zone_update(gs, cfg, ms, lowest);
			}
			if (!is_emulation_triggered) {
				touch_detect_update(gs, cfg, ms, latest);
			}
		}
	}

	if (integrated_just_released) {
		foreach_bit(i, ms->touch_used) {
			if (cfg->button_integrated) { /* <-- I know it can be outide loop, but this is less error prone */
				/* Physical button is no longer down, so unmark touches */
				CLEARBIT(ms->touch[i].flags, MT_BUTTON);
			}
		}
		trigger_button_emulation_end(gs);
	}
}

/*
 * Handle the physical button click in relation to touch properties.
 * It count the number of valid finger touching the pad while clicking it to 
 * emulate the action in the user config. See "ClickFinger#" configuration parameter.
*/
static void touch_detect_update(
	struct Gestures* gs,
	const struct MConfig* cfg,
	const struct MTState* ms,
	int latest)
{
	int i = 0, 
	touching = 0;
	struct timeval expire;

	/*If latest is not set, the finger placement is not valid*/
	if (latest >= 0) {
		foreach_bit(i, ms->touch_used) {
			microtime(&expire);
			timeraddms(&ms->touch[i].down, cfg->button_expire, &expire);
			if ((cfg->button_move || cfg->button_expire == 0 || timercmp(&ms->touch[latest].down, &expire, <)) &&
				!(cfg->ignore_thumb && GETBIT(ms->touch[i].flags, MT_THUMB)) &&
				!(cfg->ignore_palm && GETBIT(ms->touch[i].flags, MT_PALM)) &&
				!(GETBIT(ms->touch[i].flags, MT_EDGE))) {
				touching++;
				LOG_EMULATED("touch_detect_udpate: latest >=0: touching++ =%d\n", touching);
			}
		}
	}
	LOG_EMULATED("touch_detect_udpate: latest=%d: touching=%d\n", latest, touching);
	if (touching == 0 && cfg->button_0touch > 0) {
		/* The integrated physical button have been pressed but no finger are valid
		 * This code path can be reached by enabling the ClickFinger0 in the config file. */
		trigger_button_emulation(gs, cfg->button_0touch - 1);
	}
	else if (touching == 1 && cfg->button_1touch > 0)
		trigger_button_emulation(gs, cfg->button_1touch - 1);
	else if (touching == 2 && cfg->button_2touch > 0)
		trigger_button_emulation(gs, cfg->button_2touch - 1);
	else if (touching == 3 && cfg->button_3touch > 0)
		trigger_button_emulation(gs, cfg->button_3touch - 1);
}

/*
 * Handle the button zone options for triggering button emulation
 * Up to three zones can be setup, one for each button_touch type.
 * This function take into account a maximum height for zone action.
 *
 * Return: "true" if a button_emulation was triggered, else false
*/
static int buttons_zone_update(
	struct Gestures* gs,
	const struct MConfig* cfg,
	const struct MTState* ms,
	int lowest)
{
	int i, zones, left, right, pos_x, pos_y;
	double width;
	double limit_height;

	zones = 0;
	if (cfg->button_first_zone > 0)
		zones++;
	if (cfg->button_second_zone > 0)
		zones++;
	if (cfg->button_third_zone > 0)
		zones++;

	if (zones > 0) {
		limit_height = -1;
		pos_y = 0;

		/* Check if the zone need to be limited in height */
		if(cfg->is_button_zones_height_limited != 0 && cfg->edge_bottom_size != 0) {
			limit_height = cfg->pad_height - cfg->pad_height * ((double)cfg->edge_bottom_size / 100.0);
			pos_y = cfg->pad_height / 2 + ms->touch[lowest].y;
			LOG_EMULATED("button_zone_update: limit_height %f, pos_y %d, pad_height %d, edge_bottom_size %d\n",
				limit_height, pos_y, cfg->pad_height, cfg->edge_bottom_size);
		}
		/* If no height limit is set in the config, it will always be true */
		if(pos_y >= limit_height) {
			width = ((double)cfg->pad_width) / ((double)zones);
			pos_x = cfg->pad_width / 2 + ms->touch[lowest].x; /* Why not mean x of all touches? */
			LOG_EMULATED("button_zone_update: pad width %d, zones %d, zone width %f, zone height %f, x %d\n",
				cfg->pad_width, zones, width, limit_height, pos_x);

			for (i = 0; i < zones; i++) {
				left = width*i;
				right = width*(i+1);
				if ((i == 0 || pos_x >= left) && (i == zones - 1 || pos_x <= right)) {
					LOG_EMULATED("button_zone_update: button %d, left %d, right %d (found)\n", i, left, right);
					break;
				}
				else
					LOG_EMULATED("button_zone_update: button %d, left %d, right %d\n", i, left, right);
			}
			if (i == 0)
				trigger_button_emulation(gs, cfg->button_first_zone - 1);
			else if (i == 1)
				trigger_button_emulation(gs, cfg->button_second_zone - 1);
			else if (i == 2)
				trigger_button_emulation(gs, cfg->button_third_zone - 1);
			return 1;
		}
	}
	return 0;
}

/*
 * So, tapping. Tap begins with 0 fingers on trackpad,
 * then one or more are coming down, stay down for a moment,
 * and then all of them are released more or less simultaneously
 *
 * These 3 steps must be completed in relative short time.
 * Another requirement is that no other gesture can be made duting tapping.
 *
 * What can break taps:
 *  too much time passed by
 *  one of the fingers moved too far
 */
static void abort_tapping(struct Gestures* gs, struct MTState* ms){
	int i;

	gs->tap_touching = 0;
	gs->tap_released = 0;
	timerclear(&gs->tap_timeout);

	foreach_bit(i, ms->touch_used) {
		CLEARBIT(ms->touch[i].flags, MT_TAP);
	}
}

#ifdef DEBUG_GESTURES
#define LOG_TAP LOG_INFO
#else
#define LOG_TAP(...)
#endif

static void tapping_update(struct Gestures* gs,
			const struct MConfig* cfg,
			struct MTState* ms)
{
	int i, dist;
	struct timeval tv_tmp;
	struct Touch* iTouch;
	int final_touch_count;
	int button;

	final_touch_count = 0;

	if (cfg->trackpad_disable >= 1)
		return;

	/* Check conditions for early exit - posibilities to break active tap */
	if (!isepochtime(&gs->tap_timeout)){ /* Tap was started, check exit conditions */
		if (timercmp(&gs->time, &gs->tap_timeout, >=)) {
			/* too much time passed by from first touch, stop waiting for incoming touches */
			abort_tapping(gs, ms);
			LOG_TAP("tapping_update: break: too slow; !isepoch:%d, timercmp:%d\n", !isepochtime(&gs->tap_timeout), timercmp(&gs->time, &gs->tap_timeout, >=));
			return;
		}
		foreach_bit(i, ms->touch_used) {
			iTouch = ms->touch + i;
			if (GETBIT(iTouch->flags, MT_TAP)) {
				/* Finger moved too far. */
				dist = dist2(iTouch->total_dx, iTouch->total_dy);
				if (dist >= SQRVAL(cfg->tap_dist)) {
					abort_tapping(gs, ms);
					LOG_TAP("tapping_update: break: too far\n");
					return;
				}
			}
			if (GETBIT(iTouch->flags, MT_INVALID) ||
					GETBIT(iTouch->flags, MT_BUTTON)){
				/* Invalid or physical button */
				abort_tapping(gs, ms);
				LOG_TAP("tapping_update: break: invalid or button\n");
				return;
			}

			/* If there is touch with any other flag than NEW, TAP, or RELEASED,
			 * tap will be discontinued
			 */
			if (!(GETBIT(iTouch->flags, MT_NEW)||
						GETBIT(iTouch->flags, MT_TAP)||
						GETBIT(iTouch->flags, MT_RELEASED)
						)
			){
				abort_tapping(gs, ms);
				LOG_TAP("tapping_update: break: not NEW, TAP, or RELEASED\n");
				return;
			}
		}
	}

	/* Conditions satisfied, check for new touches/releases
	 * All touches are either in NEW, TAP, or RELEASED state.
	 */
	foreach_bit(i, ms->touch_used) {
		iTouch = ms->touch + i;

		if (GETBIT(iTouch->flags, MT_NEW)) { /* New touch is coming */
			gs->tap_touching += 1;
			SETBIT(iTouch->flags, MT_TAP);
			if(gs->tap_touching == 1){ /* That was first touch, start timer */
				LOG_TAP("tapping_update: start new tap; timeout=%d\n", cfg->tap_timeout);
				timeraddms(&gs->time, cfg->tap_timeout, &gs->tap_timeout);
			}
		}

		if (GETBIT(iTouch->flags, MT_RELEASED)) {
			gs->tap_touching -= 1;
			if(gs->tap_touching == 0){ /* Last finger released, time for decision */
				final_touch_count = MAXVAL(gs->tap_released, gs->tap_touching + 1); /* At least one */
			}else if(gs->tap_touching > 0){
				/* Store how many fingers where down at tap's peak */
				gs->tap_released = MAXVAL(gs->tap_released, gs->tap_touching + 1);
			}
			else{ /* gs->tap_touching is < 0 */
				/* That means finges(s) were down, while tap wasn't active and finger was released */
				gs->tap_touching = 0;
				trigger_drag_stop(gs, cfg);
				return; /* Pretty common situation; do nothing */
			}
			LOG_TAP("tapping_update: touch released; gs->tap_touching=%d, gs->tap_released=%d\n", gs->tap_touching, gs->tap_released);
		}
	}

	if(final_touch_count == 0){ /* Tap is still posible, it's just not finished yet */
		return;
	}

	switch(final_touch_count){
	case 1: button = cfg->tap_1touch - 1; break;
	case 2: button = cfg->tap_2touch - 1; break;
	case 3: button = cfg->tap_3touch - 1; break;
	case 4: button = cfg->tap_4touch - 1; break;
	default:
		LOG_TAP("tapping_update: Something went really bad; final_touch_count=%d\n", final_touch_count);
		button = cfg->tap_4touch - 1;
	}

	timeraddms(&gs->time, cfg->tap_hold, &tv_tmp); /* How long button should be hold down */
	trigger_button_click(gs, button, &tv_tmp);
	if (cfg->drag_enable && button == 0)
		trigger_drag_ready(gs, cfg);

	gs->move_type = GS_NONE;
	timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);
	abort_tapping(gs, ms);
}
#undef LOG_TAP

static void trigger_move(struct Gestures* gs,
			const struct MConfig* cfg,
			int dx, int dy)
{
	if ((gs->move_type == GS_MOVE || timercmp(&gs->time, &gs->move_wait, >=)) && (dx != 0 || dy != 0)) {
		if (trigger_drag_start(gs, cfg, dx, dy)) {
			gs->move_dx = dx*cfg->sensitivity;
			gs->move_dy = dy*cfg->sensitivity;
			break_coasting(gs);
			gs->move_type = GS_MOVE;
			gs->move_dist = 0;
			gs->move_dir = TR_NONE;
			timerclear(&gs->move_wait);
			LOG_DEBUG_GESTURES("trigger_move: %.2f, %.2f\n", gs->move_dx, gs->move_dy);
		}
	}
}

static double get_swipe_dir_n(const struct Touch* touches[DIM_TOUCHES], int count)
{
	if(count > DIM_TOUCHES || count <= 0)
		return TR_NONE;
	if(count == 1)
		return touches[0]->direction;

	int i;
	double avg_dir;
	double angles[DIM_TOUCHES];

	/* Find average direction */
	for (i = 0; i < count; ++i) {
		if (touches[i]->direction == TR_NONE)
			return TR_NONE;
		angles[i] = touches[i]->direction;
	}
	avg_dir = trig_angles_avg(angles, count);

	/* Check if all touches have (almost) same direction */
	for (i = 0; i < count; ++i) {
		if (trig_angles_acute(avg_dir, touches[i]->direction) > 0.5)
			return TR_NONE;
	}
	return avg_dir;
}

static void get_swipe_avg_xy(const struct Touch* touches[DIM_TOUCHES], int count, double* out_x, double* out_y){
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

/* To avoid users' confusion accept diagonal direction only if same button was
 * bound to aligned directions.
 * If different buttons were used and it's diagonal movement we can't decide
 * which button schould be generated.
 * It's better to ignore one gesture than confuse user.
 */
int get_button_for_dir(const struct MConfigSwipe* cfg_swipe, int dir){
	switch (dir){
	case TR_NONE:
		if(cfg_swipe->up_btn == cfg_swipe->lt_btn && cfg_swipe->lt_btn == cfg_swipe->dn_btn && cfg_swipe->dn_btn == cfg_swipe->rt_btn)
			return cfg_swipe->up_btn;
		return -1;
	case TR_DIR_UP:
		return cfg_swipe->up_btn;
	case TR_DIR_DN:
		return cfg_swipe->dn_btn;
	case TR_DIR_LT:
		return cfg_swipe->lt_btn;
	case TR_DIR_RT:
		return cfg_swipe->rt_btn;
	case (8 + TR_DIR_UP + TR_DIR_LT) / 2:
		if(cfg_swipe->up_btn != cfg_swipe->lt_btn)
			return -1;
		return cfg_swipe->up_btn;
	case (TR_DIR_LT + TR_DIR_DN) / 2:
		if(cfg_swipe->lt_btn != cfg_swipe->dn_btn)
			return -1;
		return cfg_swipe->lt_btn;
	case (TR_DIR_DN + TR_DIR_RT) / 2:
		if(cfg_swipe->dn_btn != cfg_swipe->rt_btn)
			return -1;
		return cfg_swipe->dn_btn;
	case (TR_DIR_RT + TR_DIR_UP) / 2:
		if(cfg_swipe->rt_btn != cfg_swipe->up_btn)
			return -1;
		return cfg_swipe->rt_btn;
	default:
		return -1;
	}
}

/* It's called 'unsafe' because caller have to check that all conditions required to
 * trigger swipe were met.
 */
static int trigger_swipe_unsafe(struct Gestures* gs,
			const struct MConfig* cfg, const struct MConfigSwipe* cfg_swipe,
			const struct Touch* touches[4], int touches_count,
			int move_type_to_trigger)
{
	double avg_move_x, avg_move_y;
	int button;
	double dist;
	int dir;
	struct timeval tv_tmp;

	if (touches_count <= 0)
		return 0;
	/** Is that kind of scroll enabled? */
	if (!cfg->scroll_smooth && cfg_swipe->dist <= 0)
		return 0;

	dir = trig_generalize(get_swipe_dir_n(touches, touches_count));
	button = get_button_for_dir(cfg_swipe, dir);
	if(button == -1){
		/* No button? Probably fingers were still down,
		 * but without movement.
		 */
		return 0;
	}

	trigger_drag_stop(gs, cfg);
	get_swipe_avg_xy(touches, touches_count, &avg_move_x, &avg_move_y);
	// hypot(1/n * (x0 + ... + xn); 1/n * (y0 + ... + yn)) <=> 1/n * hypot(x0 + ... + xn; y0 + ... + yn)
	dist = hypot(avg_move_x, avg_move_y);
	if(cfg_swipe->drag_sens){
		gs->move_dx = cfg->sensitivity * avg_move_x * cfg_swipe->drag_sens * 0.001;
		gs->move_dy = cfg->sensitivity * avg_move_y * cfg_swipe->drag_sens * 0.001;
	} else{
		gs->move_dx = gs->move_dy = 0.0;
	}
	if (gs->move_type != move_type_to_trigger){
		trigger_delayed_button_uncond(gs);
		gs->move_dist = 0;
	}
	else if (gs->move_dir != dir){
		gs->move_dist = 0;
	}
	gs->move_type = move_type_to_trigger;
	gs->move_dist += ABSVAL(dist);
	gs->move_dir = dir;
	timeraddms(&gs->time, cfg->gesture_wait + 5 /*bonus from me*/, &gs->move_wait);

	/* Special case for smooth scrolling */
	if(cfg->scroll_smooth && button >= 4 && button <= 7){
		/* Calculate speed vector */

		/* Forbid perpendicular movement: this pre-set will disable h-scroll
		 * while v-scroll is active. See Github #19 */
		gs->scroll_speed_x = 0.0;
		gs->scroll_speed_y = 0.0;
		/* Map selected _button_ (instead of dir) to scroll axis */
		/* Detect 'natural scrolling' - situation when user fipped scroll up and
		 * down buttons. Sign of speed vactor have to match selected button.
		 */
		switch(button){
			case 4: gs->scroll_speed_y = -ABSVAL(avg_move_y); break;  /* scroll up - always negative */
			case 5: gs->scroll_speed_y = ABSVAL(avg_move_y); break;   /* scroll down - always positve */
			case 6: gs->scroll_speed_x = -ABSVAL(avg_move_x); break;  /* scroll left */
			case 7: gs->scroll_speed_x = ABSVAL(avg_move_x); break;   /* scroll right */
		}
		/* Valuator increment=1.0 so I can scale by dist manually here. */
		/* Delta component will be kept in scroll_speed. */
		gs->scroll_speed_x = gs->scroll_speed_x / (double)(cfg_swipe->dist /* * timertoms(&gs->dt) */);
		gs->scroll_speed_y = gs->scroll_speed_y / (double)(cfg_swipe->dist /* * timertoms(&gs->dt) */);

		gs->scroll_speed_valid = 1;
		LOG_INFO2(DISABLED, "smooth scrolling: speed: x: %lf, y: %lf\n", gs->scroll_speed_x, gs->scroll_speed_y);

		/* Reset coasting duration 'to go' ticks. */
		gs->coasting_duration_left = cfg->scroll_coast.duration - 1;

		/* Don't modulo move_dist */
	}
	else if (gs->move_dist >= cfg_swipe->dist) {
		if(cfg_swipe->hold != 0)
			timeraddms(&gs->time, cfg_swipe->hold, &tv_tmp);
		else
			timerclear(&tv_tmp); // wait for gesture end
		gs->move_dist = MODVAL(gs->move_dist, cfg_swipe->dist);
		trigger_button_click(gs, button - 1, &tv_tmp);
	}
	LOG_DEBUG_GESTURES("trigger_swipe_button: swiping %f in direction %d (at %f of %d)\n",
		dist, dir, gs->move_dist, cfg_swipe->dist);
	return 1;
}

/* Return:
 *  0 - it wasn't swipe
 *  1 - it was swipe and was executed
 *  other value - it was swipe, but couldn't be executed
 */
static int trigger_swipe(struct Gestures* gs,
			const struct MConfig* cfg, const struct Touch* touches[4], int touches_count)
{
	int move_type_to_trigger;
	const struct MConfigSwipe* cfg_swipe;
	/* Feature: allow transition from low order swipes into higher ones.
	 * Motivation: it's extremly hard to start fast swipe3 or swipe4 without triggering
	 * swipe2 earlier.
	 */
	int can_transit_swipe;

	can_transit_swipe = FALSE;

	switch(touches_count){
	case 2:
		cfg_swipe = &cfg->scroll;
		move_type_to_trigger = GS_SCROLL;
		break;
	case 3:
		cfg_swipe = &cfg->swipe3;
		move_type_to_trigger = GS_SWIPE3;
		can_transit_swipe = gs->move_type == GS_SCROLL;
		break;
	case 4:
		cfg_swipe = &cfg->swipe4;
		move_type_to_trigger = GS_SWIPE4;
		can_transit_swipe = gs->move_type == GS_SCROLL || gs->move_type == GS_SWIPE3;
		break;
	default:
		goto not_a_swipe;
	}

	if (can_change_gesture_type(gs, move_type_to_trigger) || can_transit_swipe != FALSE){
		if (trigger_swipe_unsafe(gs, cfg, cfg_swipe, touches, touches_count, move_type_to_trigger))
			return 1;
		goto not_a_swipe;
	}

	not_a_swipe:{
		//gs->scroll_speed_x = gs->scroll_speed_y = 0.0;
		//if(is_any_swipe(gs->move_type)){
		//	gs->move_type = GS_NONE;
		//}
		return 0;
	}
}

/* Return:
 *  0 - it wasn't swipe
 *  1 - it was swipe and was executed
 *  other value - it was swipe, but couldn't be executed
 */
static int trigger_edge(struct Gestures* gs, const struct MConfig* cfg,
                        const struct Touch* touch)
{
	int dir = TR_NONE;
	int edge = touch_which_edge(cfg, touch);

	switch (edge){
		case 5:
		case 7: case 8: case 9: /* Always disable scrolling for bottom edge. */
			return 0;
	}

	if (GETBIT(touch->flags, MT_EDGE) == 0)
		return 0;

	dir = trig_generalize(touch->direction);
	if (dir == TR_NONE)
		return 0;
	return trigger_swipe_unsafe(gs, cfg, &cfg->edge_scroll, &touch, 1, GS_SCROLL);
}

/* Compute hypot from x, y and compare it with given value
 */
static int hypot_cmp(int x, int y, int value)
{
	int lhs, rhs;
	lhs = x * x + y * y;
	rhs = value * value;

	if(lhs == rhs)
		return 0;
	if(lhs < rhs)
		return -1;
	return 1;
}

/* Compute hypot from x, y and compare it with given value
 */
int hypot_cmpf(double x, double y, double value)
{
	double lhs, rhs;
	lhs = x * x + y * y;
	rhs = value * value;

	if(lhs == rhs)
		return 0;
	if(lhs < rhs)
		return -1;
	return 1;
}

static int is_touch_stationary(const struct Touch* touch, int max_movement)
{
	return touch->direction == TR_NONE ||
				(hypot_cmp(touch->total_dx, touch->total_dy, max_movement) <= 0);
}

static int can_trigger_hold_move(const struct Gestures* gs,
				const struct Touch* touches[DIM_TOUCHES], int touches_count,
				const struct MConfig* cfg, int max_move)
{
	struct timeval tv_tmp;

	if (touches_count <= 1)
		return 0;

	/* Condition: allow only translation from 'neutral' move type. */
	if(gs->move_type != GS_NONE && gs->move_type != GS_MOVE)
		return 0;

	/* Conditions: was first finger hold in place for some time */
	if (!is_touch_stationary(touches[0], max_move))
		return 0;
	/* todo: make this time configurable */
	timeraddms(&touches[0]->down, MAXVAL(cfg->tap_timeout * 1.2, 350), &tv_tmp);
	if (timercmp(&gs->time, &tv_tmp, <)) /* time < down + wait ?*/
		return 0;

	if (timercmp(&gs->time, &gs->move_wait, <))
		return 0;

	/* Condition: are other fingers making swipe gesture */
	if (get_swipe_dir_n(touches+1, touches_count-1) == TR_NONE)
		return 0;

	return 1;
}

/* Map hold-and-move gesture type to touches */
static int hold_move_gesture_to_touches(int move_type, int real_touches_count){
	switch (move_type){
	case GS_HOLD1_MOVE1:
		return 2;
	case GS_HOLD1_MOVE2:
		return 3;
	case GS_HOLD1_MOVE3:
		return 4;
	}
	return real_touches_count;
}

static int is_hold_move(struct Gestures* gs)
{
	static int invalid_mark = -1;
	return hold_move_gesture_to_touches(gs->move_type, invalid_mark) != invalid_mark;
}

/* Right now only gesture with one stationary finger and one moving finger is supported.
 * Gestures with two or more stationary fingers or two or more moving fingers
 * are not implemented, but I guess it will be easy to extend this function to support
 * them.
 */
static int trigger_hold_move(struct Gestures* gs,
			const struct MConfig* cfg, const struct Touch* touches[DIM_TOUCHES], int touches_count)
{
	int move_type_to_trigger;
	const struct MConfigSwipe* cfg_swipe;
	int stationary_max_move, stationary_btn;

	/* At the moment there's only one Hold1Move* gesture, so this line is fine,
	 * but after addition of other similar gestures it will become invalid, since we
	 * can't initialize stationary button yet (wrom which cfg it should be read?)
	 */
	stationary_btn = cfg->hold1_move1_stationary.button - 1;
	move_type_to_trigger = -1;

	/* Additional code for future features like hold1move{2,3} */
	switch(hold_move_gesture_to_touches(gs->move_type, touches_count)){
		case 0:
		case 1:
			/* Process them later. */
			break;
		case 2:
			move_type_to_trigger = GS_HOLD1_MOVE1;
			stationary_btn = cfg->hold1_move1_stationary.button - 1;
			stationary_max_move = cfg->hold1_move1_stationary.max_move;
			cfg_swipe = &cfg->hold1_move1;
			break;
#if 0
		case 3:
			move_type_to_trigger = GS_HOLD1_MOVE2;
			stationary_btn = cfg->hold1_move2_stationary.button - 1;
			stationary_max_move = cfg->hold1_move2_stationary.max_move;
			cfg_swipe = &cfg->hold1_move2;
			break;
		case 4:
			move_type_to_trigger = GS_HOLD1_MOVE3;
			stationary_btn = cfg->hold1_move3_stationary.button - 1;
			stationary_max_move = cfg->hold1_move3_stationary.max_move;
			cfg_swipe = &cfg->hold1_move3;
			break;
#endif
		default:
			return 0;
	}
	if (stationary_btn < 0){ /* feature disabled, reurn immediately */
		return 0;
	}
	if (is_hold_move(gs)){
		/* Condition: no fingers or stationary just released or stationary moved */
		if (touches_count == 0 ||
				GETBIT(touches[0]->flags, MT_RELEASED) ||
				!is_touch_stationary(touches[0], stationary_max_move)){
			/* Stationary finger released or moved too far */
			gs->move_type = GS_NONE;
			trigger_delayed_button_uncond(gs);
			trigger_button_up(gs, stationary_btn);
			return 1;
		}
		else if (touches_count == 1){
			/* Only one finger is touching, it's stationary.
			 * The gesture was initiated earlier.
			 * Block other actions/movements.
			 */
			return 1;
		}
	}

	if (touches_count <= 1 || move_type_to_trigger == -1)
		return 0;

	if (gs->move_type == move_type_to_trigger){
		return trigger_swipe_unsafe(gs, cfg, cfg_swipe, touches + 1, touches_count - 1,
						move_type_to_trigger);
	}
	else if (can_trigger_hold_move(gs, touches, touches_count, cfg, stationary_max_move)){
		trigger_button_down(gs, stationary_btn);
		return trigger_swipe_unsafe(gs, cfg, cfg_swipe, touches + 1, touches_count - 1,
						move_type_to_trigger);
	}

	return 0;
}

#if defined(DEBUG_GESTURES)
#define LOG_SCALE LOG_INFO
#else
#define LOG_SCALE(...)
#endif


#define ZOOM_IN TR_DIR_UP
#define ZOOM_OUT TR_DIR_DN
#define ZOOM_UNKNOWN TR_NONE
static int calc_scale_dir(const struct Touch* t0, const struct Touch* t1){
	int dist_sq, dist_prev_sq;
	int x0_prev, y0_prev;
	int x1_prev, y1_prev;

	x0_prev = t0->x - t0->dx;
	y0_prev = t0->y - t0->dy;
	x1_prev = t1->x - t1->dx;
	y1_prev = t1->y - t1->dy;

	dist_sq = dist2(t0->x - t1->x, t0->y - t1->y);
	dist_prev_sq = dist2(x0_prev - x1_prev, y0_prev - y1_prev);


	if(dist_prev_sq == dist_sq)
		return ZOOM_UNKNOWN;
	return dist_sq > dist_prev_sq ? ZOOM_IN : ZOOM_OUT;
}

static int trigger_scale(struct Gestures* gs, const struct MConfig* cfg,
													const struct Touch* t0, const struct Touch* t1){
	double d0, d1, angles_diff, max_error;
	int dir;
	int dist;

	max_error = 45; /* = sum of difference of directions, in degrees  */
	max_error = max_error * 8.0/360.0; /* Convert degrees to our from 0 to 8 angle system */
	dir = TR_NONE;

	if (gs->move_type != GS_SCALE && timercmp(&gs->time, &gs->move_wait, <)) {
		return 0;
	}

	d0 = t0->direction;
	d1 = t1->direction;

	if(d0 == TR_NONE && d1 == TR_NONE){
		LOG_SCALE("trigger_scale: no directions\n");
		return 0;
	}
	/* Handle scaling with one finger */
	if(d0 == TR_NONE || d1 == TR_NONE){
		LOG_SCALE("trigger_scale: scaling with one finger\n");
		return 0;
	}

	/* Check if both vectors are on more or less same axis, but with different directions: */
	angles_diff = trig_angles_acute(d0, d1); /* less eq 4.0 */
	angles_diff = trig_angles_sub(4.0, angles_diff);
	if(angles_diff > max_error){
		LOG_SCALE("trigger_scale: movement foo tar from scaling axis, diff: %lf, max_error: %lf\n", angles_diff, max_error);
		return 0;
	}

	dist = dist2(t0->dx,t0->dy) + dist2(t1->dx, t1->dy);
	/* Determinate is it zoom in or zoom out */
	dir = calc_scale_dir(t0, t1);

	struct timeval tv_tmp;
	trigger_drag_stop(gs, cfg);
	if (gs->move_type != GS_SCALE || gs->move_dir != dir)
		gs->move_dist = 0;
	gs->move_dx = gs->move_dy = 0.0;
	gs->move_type = GS_SCALE;
	gs->move_dist += ABSVAL(dist);
	gs->move_dir = dir;
	timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);
	if (gs->move_dist >= cfg->scale_dist) {
		gs->move_dist = MODVAL(gs->move_dist, cfg->scale_dist);
		timeraddms(&gs->time, cfg->gesture_hold, &tv_tmp);
		if (dir == ZOOM_IN)
			trigger_button_click(gs, cfg->scale_up_btn - 1, &tv_tmp);
		else if (dir == ZOOM_OUT)
			trigger_button_click(gs, cfg->scale_dn_btn - 1, &tv_tmp);
	}

	return 1;
}
#undef ZOOM_IN
#undef ZOOM_OUT
#undef ZOOM_UNKNOWN

static void trigger_rotate(struct Gestures* gs,
			const struct MConfig* cfg,
			double dist, int dir)
{
	if (gs->move_type == GS_ROTATE || !timercmp(&gs->time, &gs->move_wait, <)) {
		struct timeval tv_tmp;
		trigger_drag_stop(gs, cfg);
		if (gs->move_type != GS_ROTATE || gs->move_dir != dir)
			gs->move_dist = 0;
		gs->move_dx = 0.0;
		gs->move_dy = 0.0;
		gs->move_type = GS_ROTATE;
		gs->move_dist += ABSVAL(dist);
		gs->move_dir = dir;
		timeraddms(&gs->time, cfg->gesture_wait, &gs->move_wait);
		if (gs->move_dist >= cfg->rotate_dist) {
			gs->move_dist = MODVAL(gs->move_dist, cfg->rotate_dist);
			timeraddms(&gs->time, cfg->gesture_hold, &tv_tmp);
			if (dir == TR_DIR_LT)
				trigger_button_click(gs, cfg->rotate_lt_btn - 1, &tv_tmp);
			else if (dir == TR_DIR_RT)
				trigger_button_click(gs, cfg->rotate_rt_btn - 1, &tv_tmp);
		}
		LOG_DEBUG_GESTURES("trigger_rotate: rotating %f in direction %d (at %f of %d)\n",
			dist, dir, gs->move_dist, cfg->rotate_dist);
	}
}

static void trigger_reset(struct Gestures* gs)
{
	reset_drag(gs);
	gs->move_dx = gs->move_dy = 0.0;
	/* Don't	reset scroll speed cuz it may break things. */
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

static void moving_update(struct Gestures* gs,
			const struct MConfig* cfg,
			struct MTState* ms)
{
	int i, count, btn_count, dx, dy, dir;
	double dist;
	const struct Touch* touches[DIM_TOUCHES];
	const struct Touch* edge_touch;

	edge_touch = NULL;
	count = btn_count = 0;
	dx = dy = 0;
	dir = 0;

	// Reset movement.
	gs->move_dx = gs->move_dy = 0.0;

	// Count touches and aggregate touch movements.
	foreach_bit(i, ms->touch_used) {
		if (GETBIT(ms->touch[i].flags, MT_INVALID)){
			if (GETBIT(ms->touch[i].flags, MT_EDGE))
				edge_touch = &ms->touch[i]; // only valid when count == 1, so could be overwritten here
			continue;
		}
		else if (GETBIT(ms->touch[i].flags, MT_BUTTON)) {
			btn_count++;
			dx += ms->touch[i].dx;
			dy += ms->touch[i].dy;
		}
		else if (!GETBIT(ms->touch[i].flags, MT_TAP)) {
			if (count < DIM_TOUCHES)
				touches[count++] = &ms->touch[i];
		}
	}

	// Determine gesture type.
	if (cfg->trackpad_disable < 1 && trigger_hold_move(gs, cfg, touches, count)){
		/* nothing to do */
	}
	else if (count == 0) {
		if (btn_count >= 1 && cfg->trackpad_disable < 2)
			trigger_move(gs, cfg, dx, dy);
		// one touch down, but count == 0 because it was not counted there
		else if(edge_touch != NULL && trigger_edge(gs, cfg, edge_touch) != 0){ }
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
		if (trigger_swipe(gs, cfg, touches, count)) {
			/* nothing to do */
		}
		else if ((dir = get_rotate_dir(touches[0], touches[1])) != TR_NONE) {
			dist = ABSVAL(hypot(touches[0]->dx, touches[0]->dy)) +
				ABSVAL(hypot(touches[1]->dx, touches[1]->dy));
			trigger_rotate(gs, cfg, dist/2, dir);
		}
		else if(trigger_scale(gs,cfg, touches[0], touches[1])){
			/* nothing to do */
		}
	}
	else if ((count == 3 || count == 4) && cfg->trackpad_disable < 1) {
		if (trigger_swipe(gs, cfg, touches, count)) {
			/* nothing to do */
		}
	}
}

static void dragging_update(struct Gestures* gs, const struct MConfig* cfg)
{
	if (gs->drag_state == GS_DRAG_READY && timercmp(&gs->time, &gs->move_drag_expire, >)) {
		LOG_DEBUG_GESTURES("dragging_update: drag expired\n");
		trigger_drag_stop(gs, cfg);
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
		LOG_DEBUG_GESTURES("delayed_update: %d delay expired, triggering up\n", gs->button_delayed);
		trigger_delayed_button_uncond(gs);
	}
	else {
#ifdef DEBUG_GESTURES
		struct timeval delta;
		timersub(&gs->button_delayed_time, &gs->time, &delta);
		LOG_DEBUG_GESTURES("delayed_update: %d still waiting, new delta %lld ms\n", gs->button_delayed, timertoms(&delta));
#endif
	}
}

void gestures_init(struct MTouch* mt)
{
	memset(&mt->gs, 0, sizeof(struct Gestures));
	timerclear(&mt->gs.tap_timeout);
}

void gestures_extract(struct MTouch* mt)
{
	timersub(&mt->hs.evtime, &mt->gs.time, &mt->gs.dt);
	timercp(&mt->gs.time, &mt->hs.evtime);

	dragging_update(&mt->gs, &mt->cfg);
	buttons_update(&mt->gs, &mt->cfg, &mt->hs, &mt->state);
	tapping_update(&mt->gs, &mt->cfg, &mt->state);
	moving_update(&mt->gs, &mt->cfg, &mt->state);
	delayed_update(&mt->gs);
}

/**
 *  Executed every input time frame, at least once. First time from 'read_input' to check if
 * timer is needed.
 * This function returns timer ID which should be installed/disabled(if negative).
 *
 * Return vale meaning:
 *  - 0 - no delay to handle, don't install timer, do nothing
 *  - MT_TIMER_* - install timer
 *  - -MT_TIMER_* - remove timer
 *  - MT_TIMER_ANY - remove any timer
 */
int gestures_delayed(struct MTouch* mt)
{
	struct Gestures* gs = &mt->gs;
	struct MTState* ms = &mt->state;
	struct timeval now, delta;
	int i, fingers_released, fingers_down;

	// count released fingers
	fingers_released = fingers_down = 0;
	foreach_bit(i, ms->touch_used) {
		if (GETBIT(ms->touch[i].flags, MT_RELEASED))
			++fingers_released;
		else if (!GETBIT(ms->touch[i].flags, MT_INVALID))
			++fingers_down;
	}

	// if there's no delayed button - do nothing
	if(!IS_VALID_BUTTON(gs->button_delayed))
		return MT_TIMER_NONE;

	/* Condition: was finger released and gesture is 'infinite' and it's not hold&move */
	if(fingers_released != 0 && is_timer_infinite(gs) && !is_hold_move(gs)){
		/* Gesture finished - it's time to send "button up" event immediately without
		 * checking for delivery time.
		 */

		trigger_delayed_button_uncond(gs);
		gs->move_dx = gs->move_dy = 0.0;
		gs->move_type = GS_NONE;
		return -MT_TIMER_DELAYED_BUTTON; /* remove delayed button timer */
	}

	if(is_timer_infinite(gs))
		return MT_TIMER_NONE;

	microtime(&now);
	//timersub(&now, &mt->gs.time, &mt->gs.dt);
	//timercp(&mt->gs.time, &now);

	if(timercmp(&gs->button_delayed_time, &now, >)){
		 timersub(&gs->button_delayed_time, &now, &delta);
		 /* That second check may seem unnecessary, but it is not.
		  * Even if button delayed time is > than now time, timertoms may still return 0
		  * because it truncates time to miliseconds. It's important because truncated time
		  * is used to setup timer.
		  */
		 if(timertoms(&delta) > 1){
			LOG_DEBUG_GESTURES("gestures_delayed: %d delayed, new delta: %lld ms\n", gs->button_delayed, timertoms(&delta));

			return MT_TIMER_DELAYED_BUTTON;
		 }
	}

	trigger_delayed_button_uncond(gs);
	gs->move_dx = gs->move_dy = 0.0;
	return -MT_TIMER_DELAYED_BUTTON; /* remove delayed button timer */
}
