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
	cfg->edge_size = DEFAULT_EDGE_SIZE;

	// Configure Gestures
	cfg->trackpad_disable = DEFAULT_TRACKPAD_DISABLE;
	cfg->button_enable = DEFAULT_BUTTON_ENABLE;
	cfg->button_integrated = DEFAULT_BUTTON_INTEGRATED;
	cfg->button_expire = DEFAULT_BUTTON_EXPIRE;
	cfg->button_zones = DEFAULT_BUTTON_ZONES;
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
	cfg->scroll_dist = DEFAULT_SCROLL_DIST;
	cfg->scroll_up_btn = DEFAULT_SCROLL_UP_BTN;
	cfg->scroll_dn_btn = DEFAULT_SCROLL_DN_BTN;
	cfg->scroll_lt_btn = DEFAULT_SCROLL_LT_BTN;
	cfg->scroll_rt_btn = DEFAULT_SCROLL_RT_BTN;
	cfg->swipe_dist = DEFAULT_SWIPE_DIST;
	cfg->swipe_up_btn = DEFAULT_SWIPE_UP_BTN;
	cfg->swipe_dn_btn = DEFAULT_SWIPE_DN_BTN;
	cfg->swipe_lt_btn = DEFAULT_SWIPE_LT_BTN;
	cfg->swipe_rt_btn = DEFAULT_SWIPE_RT_BTN;
	cfg->swipe4_dist = DEFAULT_SWIPE4_DIST;
	cfg->swipe4_up_btn = DEFAULT_SWIPE4_UP_BTN;
	cfg->swipe4_dn_btn = DEFAULT_SWIPE4_DN_BTN;
	cfg->swipe4_lt_btn = DEFAULT_SWIPE4_LT_BTN;
	cfg->swipe4_rt_btn = DEFAULT_SWIPE4_RT_BTN;
	cfg->scale_dist = DEFAULT_SCALE_DIST;
	cfg->scale_up_btn = DEFAULT_SCALE_UP_BTN;
	cfg->scale_dn_btn = DEFAULT_SCALE_DN_BTN;
	cfg->rotate_dist = SQRVAL(DEFAULT_ROTATE_DIST);
	cfg->rotate_lt_btn = DEFAULT_ROTATE_LT_BTN;
	cfg->rotate_rt_btn = DEFAULT_ROTATE_RT_BTN;
	cfg->drag_enable = DEFAULT_DRAG_ENABLE;
	cfg->drag_timeout = DEFAULT_DRAG_TIMEOUT;
	cfg->sensitivity = DEFAULT_SENSITIVITY;
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
		xf86Msg(X_INFO, "Touchpad supports regular and approaching touches.\n");
		xf86Msg(X_INFO, "  touch_min = %d, touch_max = %d\n", cfg->touch_min, cfg->touch_max);
	}
	else if (caps->has_abs[MTDEV_TOUCH_MAJOR]) {
		cfg->touch_type = MCFG_SIZE;
		cfg->touch_min = caps->abs[MTDEV_TOUCH_MAJOR].minimum;
		cfg->touch_max = caps->abs[MTDEV_TOUCH_MAJOR].maximum;
		xf86Msg(X_INFO, "Touchpad supports regular touches.\n");
		xf86Msg(X_INFO, "  touch_min = %d, touch_max = %d\n", cfg->touch_min, cfg->touch_max);
	}
	else if (caps->has_abs[MTDEV_PRESSURE]) {
		cfg->touch_type = MCFG_PRESSURE;
		cfg->touch_min = caps->abs[MTDEV_PRESSURE].minimum;
		cfg->touch_max = caps->abs[MTDEV_PRESSURE].maximum;
		xf86Msg(X_INFO, "Touchpad is pressure based.\n");
		xf86Msg(X_INFO, "  touch_min = %d, touch_max = %d\n", cfg->touch_min, cfg->touch_max);
	}
	else {
		cfg->touch_type = MCFG_NONE;
		xf86Msg(X_WARNING, "Touchpad has minimal capabilities. Some features will be unavailable.\n");
	}

	if (cfg->touch_minor)
		xf86Msg(X_INFO, "Touchpad supports minor touch widths.\n");
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
	cfg->edge_size = CLAMPVAL(xf86SetIntOption(opts, "EdgeSize", DEFAULT_EDGE_SIZE), 0, 100);

	// Configure Gestures
	cfg->trackpad_disable = CLAMPVAL(xf86SetIntOption(opts, "TrackpadDisable", DEFAULT_TRACKPAD_DISABLE), 0, 3);
	cfg->button_enable = xf86SetBoolOption(opts, "ButtonEnable", DEFAULT_BUTTON_ENABLE);
	cfg->button_integrated = xf86SetBoolOption(opts, "ButtonIntegrated", DEFAULT_BUTTON_INTEGRATED);
	cfg->button_expire = MAXVAL(xf86SetIntOption(opts, "ButtonTouchExpire", DEFAULT_BUTTON_EXPIRE), 0);
	cfg->button_zones = xf86SetBoolOption(opts, "ButtonZonesEnable", DEFAULT_BUTTON_ZONES);
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
	cfg->scroll_dist = MAXVAL(xf86SetIntOption(opts, "ScrollDistance", DEFAULT_SCROLL_DIST), 1);
	cfg->scroll_up_btn = CLAMPVAL(xf86SetIntOption(opts, "ScrollUpButton", DEFAULT_SCROLL_UP_BTN), 0, 32);
	cfg->scroll_dn_btn = CLAMPVAL(xf86SetIntOption(opts, "ScrollDownButton", DEFAULT_SCROLL_DN_BTN), 0, 32);
	cfg->scroll_lt_btn = CLAMPVAL(xf86SetIntOption(opts, "ScrollLeftButton", DEFAULT_SCROLL_LT_BTN), 0, 32);
	cfg->scroll_rt_btn = CLAMPVAL(xf86SetIntOption(opts, "ScrollRightButton", DEFAULT_SCROLL_RT_BTN), 0, 32);
	cfg->swipe_dist = MAXVAL(xf86SetIntOption(opts, "SwipeDistance", DEFAULT_SWIPE_DIST), 1);
	cfg->swipe_up_btn = CLAMPVAL(xf86SetIntOption(opts, "SwipeUpButton", DEFAULT_SWIPE_UP_BTN), 0, 32);
	cfg->swipe_dn_btn = CLAMPVAL(xf86SetIntOption(opts, "SwipeDownButton", DEFAULT_SWIPE_DN_BTN), 0, 32);
	cfg->swipe_lt_btn = CLAMPVAL(xf86SetIntOption(opts, "SwipeLeftButton", DEFAULT_SWIPE_LT_BTN), 0, 32);
	cfg->swipe_rt_btn = CLAMPVAL(xf86SetIntOption(opts, "SwipeRightButton", DEFAULT_SWIPE_RT_BTN), 0, 32);
	cfg->swipe4_dist = MAXVAL(xf86SetIntOption(opts, "Swipe4Distance", DEFAULT_SWIPE4_DIST), 1);
	cfg->swipe4_up_btn = CLAMPVAL(xf86SetIntOption(opts, "Swipe4UpButton", DEFAULT_SWIPE4_UP_BTN), 0, 32);
	cfg->swipe4_dn_btn = CLAMPVAL(xf86SetIntOption(opts, "Swipe4DownButton", DEFAULT_SWIPE4_DN_BTN), 0, 32);
	cfg->swipe4_lt_btn = CLAMPVAL(xf86SetIntOption(opts, "Swipe4LeftButton", DEFAULT_SWIPE4_LT_BTN), 0, 32);
	cfg->swipe4_rt_btn = CLAMPVAL(xf86SetIntOption(opts, "Swipe4RightButton", DEFAULT_SWIPE4_RT_BTN), 0, 32);
	cfg->scale_dist = MAXVAL(xf86SetIntOption(opts, "ScaleDistance", DEFAULT_SCALE_DIST), 1);
	cfg->scale_up_btn = CLAMPVAL(xf86SetIntOption(opts, "ScaleUpButton", DEFAULT_SCALE_UP_BTN), 0, 32);
	cfg->scale_dn_btn = CLAMPVAL(xf86SetIntOption(opts, "ScaleDownButton", DEFAULT_SCALE_DN_BTN), 0, 32);
	cfg->rotate_dist = MAXVAL(xf86SetIntOption(opts, "RotateDistance", DEFAULT_ROTATE_DIST), 1);
	cfg->rotate_lt_btn = CLAMPVAL(xf86SetIntOption(opts, "RotateLeftButton", DEFAULT_ROTATE_LT_BTN), 0, 32);
	cfg->rotate_rt_btn = CLAMPVAL(xf86SetIntOption(opts, "RotateRightButton", DEFAULT_ROTATE_RT_BTN), 0, 23);
	cfg->drag_enable = xf86SetBoolOption(opts, "TapDragEnable", DEFAULT_DRAG_ENABLE);
	cfg->drag_timeout = MAXVAL(xf86SetIntOption(opts, "TapDragTime", DEFAULT_DRAG_TIMEOUT), 1);
	cfg->drag_wait = MAXVAL(xf86SetIntOption(opts, "TapDragWait", DEFAULT_DRAG_WAIT), 0);
	cfg->drag_dist = MAXVAL(xf86SetIntOption(opts, "TapDragDist", DEFAULT_DRAG_DIST), 0);
	cfg->axis_x_invert = xf86SetBoolOption(opts, "AxisXInvert", DEFAULT_AXIS_X_INVERT);
	cfg->axis_y_invert = xf86SetBoolOption(opts, "AxisYInvert", DEFAULT_AXIS_Y_INVERT);
	cfg->sensitivity = MAXVAL(xf86SetRealOption(opts, "Sensitivity", DEFAULT_SENSITIVITY), 0);
}

