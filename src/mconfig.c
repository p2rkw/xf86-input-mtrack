/***************************************************************************
 *
 * Multitouch X driver
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

#include "mconfig.h"

void mconfig_defaults(struct MConfig* cfg)
{
	// Configure MTState
	cfg->touch_down = DEFAULT_TOUCH_DOWN;
	cfg->touch_up = DEFAULT_TOUCH_UP;
	cfg->ignore_thumb = DEFAULT_IGNORE_THUMB;
	cfg->ignore_palm = DEFAULT_IGNORE_PALM;
	cfg->disable_on_palm = DEFAULT_DISABLE_ON_PALM;
	cfg->disable_on_thumb = DEFAULT_DISABLE_ON_THUMB;
	cfg->thumb_ratio = DEFAULT_THUMB_RATIO;
	cfg->thumb_size = DEFAULT_THUMB_SIZE;
	cfg->palm_size = DEFAULT_PALM_SIZE;
	cfg->edge_top_size = DEFAULT_EDGE_TOP_SIZE;
	cfg->edge_bottom_size = DEFAULT_EDGE_BOTTOM_SIZE;
	cfg->edge_left_size = DEFAULT_EDGE_LEFT_SIZE;
	cfg->edge_right_size = DEFAULT_EDGE_RIGHT_SIZE;

	// Configure Gestures
	cfg->trackpad_disable = DEFAULT_TRACKPAD_DISABLE;
	cfg->button_enable = DEFAULT_BUTTON_ENABLE;
	cfg->button_integrated = DEFAULT_BUTTON_INTEGRATED;
	cfg->button_expire = DEFAULT_BUTTON_EXPIRE;
	cfg->button_zones = DEFAULT_BUTTON_ZONES;
	cfg->is_button_zones_height_limited = DEFAULT_IS_BUTTON_ZONES_HEIGHT_LIMITED;
	cfg->button_first_zone = DEFAULT_BUTTON_FIRST_ZONE;
	cfg->button_second_zone = DEFAULT_BUTTON_SECOND_ZONE;
	cfg->button_third_zone = DEFAULT_BUTTON_THIRD_ZONE;
	cfg->button_0touch = DEFAULT_BUTTON_0TOUCH;
	cfg->button_1touch = DEFAULT_BUTTON_1TOUCH;
	cfg->button_2touch = DEFAULT_BUTTON_2TOUCH;
	cfg->button_3touch = DEFAULT_BUTTON_3TOUCH;
	cfg->tap_1touch = DEFAULT_TAP_1TOUCH;
	cfg->tap_2touch = DEFAULT_TAP_2TOUCH;
	cfg->tap_3touch = DEFAULT_TAP_3TOUCH;
	cfg->tap_4touch = DEFAULT_TAP_4TOUCH;
	cfg->tap_timeout = DEFAULT_TAP_TIMEOUT;
	cfg->tap_hold = DEFAULT_TAP_HOLD;
	cfg->tap_dist = DEFAULT_TAP_DIST;
	cfg->gesture_hold = DEFAULT_GESTURE_HOLD;
	cfg->gesture_wait = DEFAULT_GESTURE_WAIT;
	cfg->scroll.dist = DEFAULT_SCROLL_DIST;
	cfg->scroll.hold = DEFAULT_SCROLL_HOLD;
	cfg->scroll.drag_sens = DEFAULT_SWIPE_SENS;
	cfg->scroll.up_btn = DEFAULT_SCROLL_UP_BTN;
	cfg->scroll.dn_btn = DEFAULT_SCROLL_DN_BTN;
	cfg->scroll.lt_btn = DEFAULT_SCROLL_LT_BTN;
	cfg->scroll.rt_btn = DEFAULT_SCROLL_RT_BTN;
	cfg->scroll_coast.min_speed = DEFAULT_SCROLL_COAST_MIN_SPEED;
	cfg->scroll_coast.tick_ms = DEFAULT_SCROLL_COAST_TICK_MS;
	cfg->scroll_coast.duration = DEFAULT_SCROLL_COAST_DURATION;
	cfg->swipe3.dist = DEFAULT_SWIPE_DIST;
	cfg->swipe3.hold = DEFAULT_SWIPE_HOLD;
	cfg->swipe3.drag_sens = DEFAULT_SWIPE_SENS;
	cfg->swipe3.up_btn = DEFAULT_SWIPE_UP_BTN;
	cfg->swipe3.dn_btn = DEFAULT_SWIPE_DN_BTN;
	cfg->swipe3.lt_btn = DEFAULT_SWIPE_LT_BTN;
	cfg->swipe3.rt_btn = DEFAULT_SWIPE_RT_BTN;
	cfg->swipe4.dist = DEFAULT_SWIPE4_DIST;
	cfg->swipe4.hold = DEFAULT_SWIPE_HOLD;
	cfg->swipe4.drag_sens = DEFAULT_SWIPE_SENS;
	cfg->swipe4.up_btn = DEFAULT_SWIPE4_UP_BTN;
	cfg->swipe4.dn_btn = DEFAULT_SWIPE4_DN_BTN;
	cfg->swipe4.lt_btn = DEFAULT_SWIPE4_LT_BTN;
	cfg->swipe4.rt_btn = DEFAULT_SWIPE4_RT_BTN;
	cfg->edge_scroll.dist = DEFAULT_EDGE_SCROLL_DIST;
	cfg->edge_scroll.hold = DEFAULT_SCROLL_HOLD;
	cfg->edge_scroll.drag_sens = DEFAULT_SWIPE_SENS;
	cfg->edge_scroll.up_btn = DEFAULT_SCROLL_UP_BTN;
	cfg->edge_scroll.dn_btn = DEFAULT_SCROLL_DN_BTN;
	cfg->edge_scroll.lt_btn = DEFAULT_SCROLL_LT_BTN;
	cfg->edge_scroll.rt_btn = DEFAULT_SCROLL_RT_BTN;
	cfg->scale_dist = DEFAULT_SCALE_DIST;
	cfg->scale_up_btn = DEFAULT_SCALE_UP_BTN;
	cfg->scale_dn_btn = DEFAULT_SCALE_DN_BTN;
	cfg->rotate_dist = SQRVAL(DEFAULT_ROTATE_DIST);
	cfg->rotate_lt_btn = DEFAULT_ROTATE_LT_BTN;
	cfg->rotate_rt_btn = DEFAULT_ROTATE_RT_BTN;

	cfg->hold1_move1_stationary.max_move = DEFAULT_HOLD1_MOVE1_STATIONARY_MAX_MOVE;
	cfg->hold1_move1_stationary.button = DEFAULT_HOLD1_MOVE1_STATIONARY_BTN;
	cfg->hold1_move1.dist = DEFAULT_HOLD1_MOVE1_DIST;
	cfg->hold1_move1.hold = DEFAULT_HOLD1_MOVE1_HOLD;
	cfg->hold1_move1.drag_sens = DEFAULT_HOLD1_MOVE1_SENS;
	cfg->hold1_move1.up_btn = DEFAULT_HOLD1_MOVE1_BTN;
	cfg->hold1_move1.dn_btn = DEFAULT_HOLD1_MOVE1_BTN;
	cfg->hold1_move1.lt_btn = DEFAULT_HOLD1_MOVE1_BTN;
	cfg->hold1_move1.rt_btn = DEFAULT_HOLD1_MOVE1_BTN;

#if 0
	cfg->hold1_move2_stationary.max_move = DEFAULT_HOLD1_MOVE2_STATIONARY_MAX_MOVE;
	cfg->hold1_move2_stationary.button = DEFAULT_HOLD1_MOVE2_STATIONARY_BTN;
	cfg->hold1_move2.dist = DEFAULT_HOLD1_MOVE2_DIST;
	cfg->hold1_move2.hold = DEFAULT_HOLD1_MOVE2_HOLD;
	cfg->hold1_move2.drag_sens = DEFAULT_HOLD1_MOVE2_SENS;
	cfg->hold1_move2.up_btn = DEFAULT_HOLD1_MOVE2_BTN;
	cfg->hold1_move2.dn_btn = DEFAULT_HOLD1_MOVE2_BTN;
	cfg->hold1_move2.lt_btn = DEFAULT_HOLD1_MOVE2_BTN;
	cfg->hold1_move2.rt_btn = DEFAULT_HOLD1_MOVE2_BTN;

	cfg->hold1_move3_stationary.max_move = DEFAULT_HOLD1_MOVE3_STATIONARY_MAX_MOVE;
	cfg->hold1_move3_stationary.button = DEFAULT_HOLD1_MOVE3_STATIONARY_BTN;
	cfg->hold1_move3.dist = DEFAULT_HOLD1_MOVE3_DIST;
	cfg->hold1_move3.hold = DEFAULT_HOLD1_MOVE3_HOLD;
	cfg->hold1_move3.drag_sens = DEFAULT_HOLD1_MOVE3_SENS;
	cfg->hold1_move3.up_btn = DEFAULT_HOLD1_MOVE3_BTN;
	cfg->hold1_move3.dn_btn = DEFAULT_HOLD1_MOVE3_BTN;
	cfg->hold1_move3.lt_btn = DEFAULT_HOLD1_MOVE3_BTN;
	cfg->hold1_move3.rt_btn = DEFAULT_HOLD1_MOVE3_BTN;
#endif

	cfg->drag_enable = DEFAULT_DRAG_ENABLE;
	cfg->drag_timeout = DEFAULT_DRAG_TIMEOUT;
	cfg->sensitivity = DEFAULT_SENSITIVITY;
	cfg->scroll_smooth = DEFAULT_SCROLL_HIGH_PREC;
}

void mconfig_init(struct MConfig* cfg,
			const struct Capabilities* caps)
{
	cfg->touch_minor = caps->has_abs[MTDEV_TOUCH_MINOR];
	cfg->pad_width = get_cap_xsize(caps);
	cfg->pad_height = get_cap_ysize(caps);

	if (caps->has_abs[MTDEV_TOUCH_MAJOR] && caps->has_abs[MTDEV_WIDTH_MAJOR]) {
		cfg->touch_type = MCFG_SCALE;
		cfg->touch_min = caps->abs[MTDEV_TOUCH_MAJOR].minimum;
		cfg->touch_max = caps->abs[MTDEV_TOUCH_MAJOR].maximum;
		LOG_INFO("Touchpad supports regular and approaching touches.\n");
		LOG_INFO("  touch_min = %d, touch_max = %d\n", cfg->touch_min, cfg->touch_max);
	}
	else if (caps->has_abs[MTDEV_TOUCH_MAJOR] && caps->has_abs[MTDEV_PRESSURE]) {
		cfg->touch_min = caps->abs[MTDEV_TOUCH_MAJOR].minimum;
		cfg->touch_max = caps->abs[MTDEV_TOUCH_MAJOR].maximum;
		cfg->pressure_min = caps->abs[MTDEV_PRESSURE].minimum;
		cfg->pressure_max = caps->abs[MTDEV_PRESSURE].maximum;
		/* select source of the events basing on its resolution */
		if(cfg->pressure_max - cfg->pressure_min >= cfg->touch_max - cfg->touch_min)
			cfg->touch_type = MCFG_PRESSURE_SIZE;
		else
			cfg->touch_type = MCFG_SIZE_PRESSURE;
		LOG_INFO("Touchpad is pressure based, but supports regular touches also.\n");
		LOG_INFO("  touch_min = %d, touch_max = %d\n", cfg->touch_min, cfg->touch_max);
		LOG_INFO("  pressure_min = %d, pressure_max = %d\n", cfg->pressure_min, cfg->pressure_max);
	}
	else if (caps->has_abs[MTDEV_TOUCH_MAJOR]) {
		cfg->touch_type = MCFG_SIZE;
		cfg->touch_min = caps->abs[MTDEV_TOUCH_MAJOR].minimum;
		cfg->touch_max = caps->abs[MTDEV_TOUCH_MAJOR].maximum;
		LOG_INFO("Touchpad supports regular touches.\n");
		LOG_INFO("  touch_min = %d, touch_max = %d\n", cfg->touch_min, cfg->touch_max);
	}
	else if (caps->has_abs[MTDEV_PRESSURE]) {
		cfg->touch_type = MCFG_PRESSURE;
		cfg->pressure_min = caps->abs[MTDEV_PRESSURE].minimum;
		cfg->pressure_max = caps->abs[MTDEV_PRESSURE].maximum;
		LOG_INFO("Touchpad is pressure based.\n");
		LOG_INFO("  pressure_min = %d, pressure_max = %d\n", cfg->pressure_min, cfg->pressure_max);
	}
	else {
		cfg->touch_type = MCFG_NONE;
		LOG_WARNING("Touchpad has minimal capabilities. Some features will be unavailable.\n");
	}

	if (cfg->touch_minor)
		LOG_INFO("Touchpad supports minor touch widths.\n");
}

void mconfig_configure(struct MConfig* cfg,
			pointer opts)
{
	// Configure MTState
	cfg->touch_down = CLAMPVAL(xf86SetIntOption(opts, "FingerHigh", DEFAULT_TOUCH_DOWN), 0, 100);
	cfg->touch_up = CLAMPVAL(xf86SetIntOption(opts, "FingerLow", DEFAULT_TOUCH_UP), 0, 100);
	cfg->ignore_thumb = xf86SetBoolOption(opts, "IgnoreThumb", DEFAULT_IGNORE_THUMB);
	cfg->ignore_palm = xf86SetBoolOption(opts, "IgnorePalm", DEFAULT_IGNORE_PALM);
	cfg->disable_on_thumb = xf86SetBoolOption(opts, "DisableOnThumb", DEFAULT_DISABLE_ON_THUMB);
	cfg->disable_on_palm = xf86SetBoolOption(opts, "DisableOnPalm", DEFAULT_DISABLE_ON_PALM);
	cfg->thumb_ratio = CLAMPVAL(xf86SetIntOption(opts, "ThumbRatio", DEFAULT_THUMB_RATIO), 0, 100);
	cfg->thumb_size = CLAMPVAL(xf86SetIntOption(opts, "ThumbSize", DEFAULT_THUMB_SIZE), 0, 100);
	cfg->palm_size = CLAMPVAL(xf86SetIntOption(opts, "PalmSize", DEFAULT_PALM_SIZE), 0, 100);

	// Configure Gestures
	cfg->trackpad_disable = CLAMPVAL(xf86SetIntOption(opts, "TrackpadDisable", DEFAULT_TRACKPAD_DISABLE), 0, 3);
	cfg->button_enable = xf86SetBoolOption(opts, "ButtonEnable", DEFAULT_BUTTON_ENABLE);
	cfg->button_integrated = xf86SetBoolOption(opts, "ButtonIntegrated", DEFAULT_BUTTON_INTEGRATED);
	cfg->button_expire = MAXVAL(xf86SetIntOption(opts, "ButtonTouchExpire", DEFAULT_BUTTON_EXPIRE), 0);
	cfg->button_zones = xf86SetBoolOption(opts, "ButtonZonesEnable", DEFAULT_BUTTON_ZONES);
	cfg->is_button_zones_height_limited = xf86SetBoolOption(opts, "LimitButtonZonesToBottomEdge", DEFAULT_IS_BUTTON_ZONES_HEIGHT_LIMITED);
	cfg->button_first_zone = CLAMPVAL(xf86SetIntOption(opts, "FirstZoneButton", DEFAULT_BUTTON_FIRST_ZONE), 0, 32);
	cfg->button_second_zone = CLAMPVAL(xf86SetIntOption(opts, "SecondZoneButton", DEFAULT_BUTTON_SECOND_ZONE), 0, 32);
	cfg->button_third_zone = CLAMPVAL(xf86SetIntOption(opts, "ThirdZoneButton", DEFAULT_BUTTON_THIRD_ZONE), 0, 32);
	cfg->button_0touch = CLAMPVAL(xf86SetIntOption(opts, "ClickFinger0", DEFAULT_BUTTON_0TOUCH), 0, 32);
	cfg->button_1touch = CLAMPVAL(xf86SetIntOption(opts, "ClickFinger1", DEFAULT_BUTTON_1TOUCH), 0, 32);
	cfg->button_2touch = CLAMPVAL(xf86SetIntOption(opts, "ClickFinger2", DEFAULT_BUTTON_2TOUCH), 0, 32);
	cfg->button_3touch = CLAMPVAL(xf86SetIntOption(opts, "ClickFinger3", DEFAULT_BUTTON_3TOUCH), 0, 32);
	cfg->button_move = xf86SetBoolOption(opts, "ButtonMoveEmulate", DEFAULT_BUTTON_MOVE);
	cfg->tap_1touch = CLAMPVAL(xf86SetIntOption(opts, "TapButton1", DEFAULT_TAP_1TOUCH), 0, 32);
	cfg->tap_2touch = CLAMPVAL(xf86SetIntOption(opts, "TapButton2", DEFAULT_TAP_2TOUCH), 0, 32);
	cfg->tap_3touch = CLAMPVAL(xf86SetIntOption(opts, "TapButton3", DEFAULT_TAP_3TOUCH), 0, 32);
	cfg->tap_4touch = CLAMPVAL(xf86SetIntOption(opts, "TapButton4", DEFAULT_TAP_4TOUCH), 0, 32);
	cfg->tap_hold = MAXVAL(xf86SetIntOption(opts, "ClickTime", DEFAULT_TAP_HOLD), 1);
	cfg->tap_timeout = MAXVAL(xf86SetIntOption(opts, "MaxTapTime", DEFAULT_TAP_TIMEOUT), 1);
	cfg->tap_dist = MAXVAL(xf86SetIntOption(opts, "MaxTapMove", DEFAULT_TAP_DIST), 1);
	cfg->gesture_hold = MAXVAL(xf86SetIntOption(opts, "GestureClickTime", DEFAULT_GESTURE_HOLD), 1);
	cfg->gesture_wait = MAXVAL(xf86SetIntOption(opts, "GestureWaitTime", DEFAULT_GESTURE_WAIT), 0);
	cfg->scroll_smooth = CLAMPVAL(xf86SetIntOption(opts, "ScrollSmooth", DEFAULT_SCROLL_HIGH_PREC), 0, 1);
	cfg->scroll.dist = MAXVAL(xf86SetIntOption(opts, "ScrollDistance", DEFAULT_SCROLL_DIST), 1);
	cfg->scroll.hold = MAXVAL(xf86SetIntOption(opts, "ScrollClickTime", DEFAULT_SCROLL_HOLD), 0);
	cfg->scroll.drag_sens = MAXVAL(xf86SetIntOption(opts, "ScrollSensitivity", DEFAULT_SWIPE_SENS), 0);
	cfg->scroll.up_btn = CLAMPVAL(xf86SetIntOption(opts, "ScrollUpButton", DEFAULT_SCROLL_UP_BTN), 0, 32);
	cfg->scroll.dn_btn = CLAMPVAL(xf86SetIntOption(opts, "ScrollDownButton", DEFAULT_SCROLL_DN_BTN), 0, 32);
	cfg->scroll.lt_btn = CLAMPVAL(xf86SetIntOption(opts, "ScrollLeftButton", DEFAULT_SCROLL_LT_BTN), 0, 32);
	cfg->scroll.rt_btn = CLAMPVAL(xf86SetIntOption(opts, "ScrollRightButton", DEFAULT_SCROLL_RT_BTN), 0, 32);
	cfg->scroll_coast.min_speed = MAXVAL(xf86SetRealOption(opts, "ScrollCoastEnableSpeed", DEFAULT_SCROLL_COAST_MIN_SPEED), 0.0);
	cfg->scroll_coast.tick_ms = MAXVAL(DEFAULT_SCROLL_COAST_TICK_MS, 1);
	cfg->scroll_coast.duration = MAXVAL(xf86SetRealOption(opts, "ScrollCoastDuration", DEFAULT_SCROLL_COAST_DURATION), 0);
	cfg->swipe3.dist = MAXVAL(xf86SetIntOption(opts, "SwipeDistance", DEFAULT_SWIPE_DIST), 1);
	cfg->swipe3.hold = MAXVAL(xf86SetIntOption(opts, "SwipeClickTime", DEFAULT_SWIPE_HOLD), 0);
	cfg->swipe3.drag_sens = MAXVAL(xf86SetIntOption(opts, "SwipeSensitivity", DEFAULT_SWIPE_SENS), 0);
	cfg->swipe3.up_btn = CLAMPVAL(xf86SetIntOption(opts, "SwipeUpButton", DEFAULT_SWIPE_UP_BTN), 0, 32);
	cfg->swipe3.dn_btn = CLAMPVAL(xf86SetIntOption(opts, "SwipeDownButton", DEFAULT_SWIPE_DN_BTN), 0, 32);
	cfg->swipe3.lt_btn = CLAMPVAL(xf86SetIntOption(opts, "SwipeLeftButton", DEFAULT_SWIPE_LT_BTN), 0, 32);
	cfg->swipe3.rt_btn = CLAMPVAL(xf86SetIntOption(opts, "SwipeRightButton", DEFAULT_SWIPE_RT_BTN), 0, 32);
	cfg->swipe4.dist = MAXVAL(xf86SetIntOption(opts, "Swipe4Distance", DEFAULT_SWIPE4_DIST), 1);
	cfg->swipe4.hold = MAXVAL(xf86SetIntOption(opts, "Swipe4ClickTime", DEFAULT_SWIPE_HOLD), 0);
	cfg->swipe4.drag_sens = MAXVAL(xf86SetIntOption(opts, "Swipe4Sensitivity", DEFAULT_SWIPE_SENS), 0);
	cfg->swipe4.up_btn = CLAMPVAL(xf86SetIntOption(opts, "Swipe4UpButton", DEFAULT_SWIPE4_UP_BTN), 0, 32);
	cfg->swipe4.dn_btn = CLAMPVAL(xf86SetIntOption(opts, "Swipe4DownButton", DEFAULT_SWIPE4_DN_BTN), 0, 32);
	cfg->swipe4.lt_btn = CLAMPVAL(xf86SetIntOption(opts, "Swipe4LeftButton", DEFAULT_SWIPE4_LT_BTN), 0, 32);
	cfg->swipe4.rt_btn = CLAMPVAL(xf86SetIntOption(opts, "Swipe4RightButton", DEFAULT_SWIPE4_RT_BTN), 0, 32);
	cfg->edge_scroll.dist = MAXVAL(xf86SetIntOption(opts, "EdgeScrollDist", DEFAULT_EDGE_SCROLL_DIST), 1);
	cfg->edge_scroll.hold = MAXVAL(xf86SetIntOption(opts, "EdgeScrollClickTime", DEFAULT_SCROLL_HOLD), 0);
	cfg->edge_scroll.drag_sens = MAXVAL(xf86SetIntOption(opts, "EdgeScrollSensitivity", DEFAULT_SWIPE_SENS), 0);
	cfg->edge_scroll.up_btn = CLAMPVAL(xf86SetIntOption(opts, "EdgeScrollUpButton", DEFAULT_SCROLL_UP_BTN), 0, 32);
	cfg->edge_scroll.dn_btn = CLAMPVAL(xf86SetIntOption(opts, "EdgeScrollDownButton", DEFAULT_SCROLL_DN_BTN), 0, 32);
	cfg->edge_scroll.lt_btn = CLAMPVAL(xf86SetIntOption(opts, "EdgeScrollLeftButton", DEFAULT_SCROLL_LT_BTN), 0, 32);
	cfg->edge_scroll.rt_btn = CLAMPVAL(xf86SetIntOption(opts, "EdgeScrollRightButton", DEFAULT_SCROLL_RT_BTN), 0, 32);
	int edge_size =	CLAMPVAL(xf86SetIntOption(opts, "EdgeSize", 101), 0, 101);
	if(edge_size < 0 || edge_size > 100){
		cfg->edge_top_size = CLAMPVAL(xf86SetIntOption(opts, "EdgeTopSize", DEFAULT_EDGE_TOP_SIZE), 0, 100);
		cfg->edge_bottom_size = CLAMPVAL(xf86SetIntOption(opts, "EdgeBottomSize", DEFAULT_EDGE_BOTTOM_SIZE), 0, 100);
		cfg->edge_left_size = CLAMPVAL(xf86SetIntOption(opts, "EdgeLeftSize", DEFAULT_EDGE_LEFT_SIZE), 0, 100);
		cfg->edge_right_size = CLAMPVAL(xf86SetIntOption(opts, "EdgeRightSize", DEFAULT_EDGE_RIGHT_SIZE), 0, 100);
	}else{
		xf86Msg(X_WARNING, "mtrack %s:%i: %s", __FILE__, __LINE__, "You' re using DEPRECATED 'EdgeSize' option, it will be removed, plese switch to Edge{Right,Left,Top,Bottom}Size options.\n");
		cfg->edge_top_size = CLAMPVAL(xf86SetIntOption(opts, "EdgeTopSize", edge_size), 0, 100);
		cfg->edge_bottom_size = CLAMPVAL(xf86SetIntOption(opts, "EdgeBottomSize", edge_size), 0, 100);
		cfg->edge_left_size = CLAMPVAL(xf86SetIntOption(opts, "EdgeLeftSize", edge_size), 0, 100);
		cfg->edge_right_size = CLAMPVAL(xf86SetIntOption(opts, "EdgeRightSize", edge_size), 0, 100);
	}

	cfg->scale_dist = MAXVAL(xf86SetIntOption(opts, "ScaleDistance", DEFAULT_SCALE_DIST), 1);
	cfg->scale_up_btn = CLAMPVAL(xf86SetIntOption(opts, "ScaleUpButton", DEFAULT_SCALE_UP_BTN), 0, 32);
	cfg->scale_dn_btn = CLAMPVAL(xf86SetIntOption(opts, "ScaleDownButton", DEFAULT_SCALE_DN_BTN), 0, 32);
	cfg->rotate_dist = MAXVAL(xf86SetIntOption(opts, "RotateDistance", DEFAULT_ROTATE_DIST), 1);
	cfg->rotate_lt_btn = CLAMPVAL(xf86SetIntOption(opts, "RotateLeftButton", DEFAULT_ROTATE_LT_BTN), 0, 32);
	cfg->rotate_rt_btn = CLAMPVAL(xf86SetIntOption(opts, "RotateRightButton", DEFAULT_ROTATE_RT_BTN), 0, 32);

	cfg->hold1_move1_stationary.button = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move1StationaryButton", DEFAULT_HOLD1_MOVE1_STATIONARY_BTN), 0, 32);
	cfg->hold1_move1_stationary.max_move = MAXVAL(xf86SetIntOption(opts, "Hold1Move1StationaryMaxMove", DEFAULT_HOLD1_MOVE1_STATIONARY_MAX_MOVE), 1);
	cfg->hold1_move1.dist = MAXVAL(xf86SetIntOption(opts, "Hold1Move1Distance", DEFAULT_HOLD1_MOVE1_DIST), 1);
	cfg->hold1_move1.hold = MAXVAL(xf86SetIntOption(opts, "Hold1Move1ClickTime", DEFAULT_HOLD1_MOVE1_HOLD), 0);
	cfg->hold1_move1.drag_sens = MAXVAL(xf86SetIntOption(opts, "Hold1Move1Sensitivity", DEFAULT_HOLD1_MOVE1_SENS), 0);
	cfg->hold1_move1.up_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move1UpButton", DEFAULT_HOLD1_MOVE1_BTN), 0, 32);
	cfg->hold1_move1.dn_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move1DownButton", DEFAULT_HOLD1_MOVE1_BTN), 0, 32);
	cfg->hold1_move1.lt_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move1LeftButton", DEFAULT_HOLD1_MOVE1_BTN), 0, 32);
	cfg->hold1_move1.rt_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move1RightButton", DEFAULT_HOLD1_MOVE1_BTN), 0, 32);

#if 0
	cfg->hold1_move2_stationary.button = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move2StationaryButton", DEFAULT_HOLD1_MOVE2_STATIONARY_BTN), 0, 32);
	cfg->hold1_move2_stationary.max_move = MAXVAL(xf86SetIntOption(opts, "Hold1Move2StationaryMaxMove", DEFAULT_HOLD1_MOVE2_STATIONARY_MAX_MOVE), 1);
	cfg->hold1_move2.dist = MAXVAL(xf86SetIntOption(opts, "Hold1Move2Distance", DEFAULT_HOLD1_MOVE2_DIST), 1);
	cfg->hold1_move2.hold = MAXVAL(xf86SetIntOption(opts, "Hold1Move2ClickTime", DEFAULT_HOLD1_MOVE2_HOLD), 0);
	cfg->hold1_move2.drag_sens = MAXVAL(xf86SetIntOption(opts, "Hold1Move2Sensitivity", DEFAULT_HOLD1_MOVE2_SENS), 0);
	cfg->hold1_move2.up_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move2UpButton", DEFAULT_HOLD1_MOVE2_BTN), 0, 32);
	cfg->hold1_move2.dn_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move2DownButton", DEFAULT_HOLD1_MOVE2_BTN), 0, 32);
	cfg->hold1_move2.lt_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move2LeftButton", DEFAULT_HOLD1_MOVE2_BTN), 0, 32);
	cfg->hold1_move2.rt_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move2RightButton", DEFAULT_HOLD1_MOVE2_BTN), 0, 32);

	cfg->hold1_move3_stationary.button = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move3StationaryButton", DEFAULT_HOLD1_MOVE3_STATIONARY_BTN), 0, 32);
	cfg->hold1_move3_stationary.max_move = MAXVAL(xf86SetIntOption(opts, "Hold1Move3StationaryMaxMove", DEFAULT_HOLD1_MOVE3_STATIONARY_MAX_MOVE), 1);
	cfg->hold1_move3.dist = MAXVAL(xf86SetIntOption(opts, "Hold1Move3Distance", DEFAULT_HOLD1_MOVE3_DIST), 1);
	cfg->hold1_move3.hold = MAXVAL(xf86SetIntOption(opts, "Hold1Move3ClickTime", DEFAULT_HOLD1_MOVE3_HOLD), 0);
	cfg->hold1_move3.drag_sens = MAXVAL(xf86SetIntOption(opts, "Hold1Move3Sensitivity", DEFAULT_HOLD1_MOVE3_SENS), 0);
	cfg->hold1_move3.up_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move3UpButton", DEFAULT_HOLD1_MOVE3_BTN), 0, 32);
	cfg->hold1_move3.dn_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move3DownButton", DEFAULT_HOLD1_MOVE3_BTN), 0, 32);
	cfg->hold1_move3.lt_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move3LeftButton", DEFAULT_HOLD1_MOVE3_BTN), 0, 32);
	cfg->hold1_move3.rt_btn = CLAMPVAL(xf86SetIntOption(opts, "Hold1Move3RightButton", DEFAULT_HOLD1_MOVE3_BTN), 0, 32);
#endif

	cfg->drag_enable = xf86SetBoolOption(opts, "TapDragEnable", DEFAULT_DRAG_ENABLE);
	cfg->drag_timeout = MAXVAL(xf86SetIntOption(opts, "TapDragTime", DEFAULT_DRAG_TIMEOUT), 1);
	cfg->drag_wait = MAXVAL(xf86SetIntOption(opts, "TapDragWait", DEFAULT_DRAG_WAIT), 0);
	cfg->drag_dist = MAXVAL(xf86SetIntOption(opts, "TapDragDist", DEFAULT_DRAG_DIST), 0);
	cfg->drag_lock_timeout = xf86SetIntOption(opts, "TapDragLockTimeout", DEFAULT_DRAG_LOCK_TIMEOUT);

	cfg->axis_x_invert = xf86SetBoolOption(opts, "AxisXInvert", DEFAULT_AXIS_X_INVERT);
	cfg->axis_y_invert = xf86SetBoolOption(opts, "AxisYInvert", DEFAULT_AXIS_Y_INVERT);
	cfg->sensitivity = MAXVAL(xf86SetRealOption(opts, "Sensitivity", DEFAULT_SENSITIVITY), 0);
}
