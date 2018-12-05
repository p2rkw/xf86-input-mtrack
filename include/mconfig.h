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

#ifndef MCONFIG_H
#define MCONFIG_H

#include "capabilities.h"

#define DEFAULT_TRACKPAD_DISABLE 0
#define DEFAULT_TOUCH_DOWN 5
#define DEFAULT_TOUCH_UP 5
#define DEFAULT_IGNORE_THUMB 0
#define DEFAULT_IGNORE_PALM 0
#define DEFAULT_DISABLE_ON_THUMB 0
#define DEFAULT_DISABLE_ON_PALM 0
#define DEFAULT_THUMB_RATIO 70
#define DEFAULT_THUMB_SIZE 25
#define DEFAULT_PALM_SIZE 40
#define DEFAULT_EDGE_LEFT_SIZE 0
#define DEFAULT_EDGE_RIGHT_SIZE 0
#define DEFAULT_EDGE_TOP_SIZE 0
#define DEFAULT_EDGE_BOTTOM_SIZE 10
#define DEFAULT_BUTTON_ENABLE 1
#define DEFAULT_BUTTON_INTEGRATED 1
#define DEFAULT_BUTTON_ZONES 0 /* Button zones are disabled */
#define DEFAULT_IS_BUTTON_ZONES_HEIGHT_LIMITED 0 /* Button zones are emulated on the whole pad (Not limited to the bottom edge)*/
#define DEFAULT_BUTTON_FIRST_ZONE 1
#define DEFAULT_BUTTON_SECOND_ZONE 2
#define DEFAULT_BUTTON_THIRD_ZONE 0
#define DEFAULT_BUTTON_0TOUCH 0 /* "invalid" finger placement do not emulate any button */
#define DEFAULT_BUTTON_1TOUCH 1
#define DEFAULT_BUTTON_2TOUCH 2
#define DEFAULT_BUTTON_3TOUCH 3
#define DEFAULT_BUTTON_MOVE 1
#define DEFAULT_BUTTON_EXPIRE 100
#define DEFAULT_TAP_1TOUCH 1
#define DEFAULT_TAP_2TOUCH 3
#define DEFAULT_TAP_3TOUCH 2
#define DEFAULT_TAP_4TOUCH 0
#define DEFAULT_TAP_TIMEOUT 120
#define DEFAULT_TAP_HOLD 50
#define DEFAULT_TAP_DIST 400
#define DEFAULT_GESTURE_HOLD 10
#define DEFAULT_GESTURE_WAIT 100
#define DEFAULT_SCROLL_DIST 150
#define DEFAULT_SCROLL_UP_BTN 4
#define DEFAULT_SCROLL_DN_BTN 5
#define DEFAULT_SCROLL_LT_BTN 6
#define DEFAULT_SCROLL_RT_BTN 7
#define DEFAULT_SCROLL_HOLD 20
#define DEFAULT_SCROLL_COAST_MIN_SPEED 0.1f
#define DEFAULT_SCROLL_COAST_DURATION 200
#define DEFAULT_SCROLL_COAST_TICK_MS 30 /* Schould be configurable? */
#define DEFAULT_SWIPE_DIST 700
#define DEFAULT_SWIPE_UP_BTN 8
#define DEFAULT_SWIPE_DN_BTN 9
#define DEFAULT_SWIPE_LT_BTN 10
#define DEFAULT_SWIPE_RT_BTN 11
#define DEFAULT_SWIPE_HOLD 300
#define DEFAULT_SWIPE4_DIST 700
#define DEFAULT_SWIPE4_UP_BTN 0
#define DEFAULT_SWIPE4_DN_BTN 0
#define DEFAULT_SWIPE4_LT_BTN 0
#define DEFAULT_SWIPE4_RT_BTN 0
#define DEFAULT_SWIPE_SENS 0
#define DEFAULT_EDGE_SCROLL_DIST (DEFAULT_SCROLL_DIST*0.7)
#define DEFAULT_SCALE_DIST 150
#define DEFAULT_SCALE_UP_BTN 12
#define DEFAULT_SCALE_DN_BTN 13
#define DEFAULT_ROTATE_DIST 150
#define DEFAULT_ROTATE_LT_BTN 14
#define DEFAULT_ROTATE_RT_BTN 15
#define DEFAULT_HOLD1_MOVE1_STATIONARY_BTN 1
#define DEFAULT_HOLD1_MOVE1_STATIONARY_MAX_MOVE 20
#define DEFAULT_HOLD1_MOVE1_DIST 1
#define DEFAULT_HOLD1_MOVE1_HOLD 0
#define DEFAULT_HOLD1_MOVE1_SENS 1000
#define DEFAULT_HOLD1_MOVE1_BTN 1
#define DEFAULT_HOLD1_MOVE2_STATIONARY_BTN 3
#define DEFAULT_HOLD1_MOVE2_STATIONARY_MAX_MOVE 30
#define DEFAULT_HOLD1_MOVE2_DIST 1
#define DEFAULT_HOLD1_MOVE2_HOLD 0
#define DEFAULT_HOLD1_MOVE2_SENS 1000
#define DEFAULT_HOLD1_MOVE2_BTN 3
#define DEFAULT_HOLD1_MOVE3_STATIONARY_BTN 10
#define DEFAULT_HOLD1_MOVE3_STATIONARY_MAX_MOVE 60
#define DEFAULT_HOLD1_MOVE3_DIST 100
#define DEFAULT_HOLD1_MOVE3_HOLD 50
#define DEFAULT_HOLD1_MOVE3_SENS 0
#define DEFAULT_HOLD1_MOVE3_BTN 11
#define DEFAULT_DRAG_ENABLE 1
#define DEFAULT_DRAG_TIMEOUT 350
#define DEFAULT_DRAG_WAIT 40
#define DEFAULT_DRAG_DIST 200
#define DEFAULT_DRAG_LOCK_TIMEOUT 500
#define DEFAULT_AXIS_X_INVERT 0
#define DEFAULT_AXIS_Y_INVERT 0
#define DEFAULT_SENSITIVITY 1.0
#define DEFAULT_SCROLL_HIGH_PREC 1

#define MCFG_NONE 0
#define MCFG_SCALE 1
#define MCFG_SIZE 2
#define MCFG_PRESSURE_SIZE 3
#define MCFG_SIZE_PRESSURE 4 /* same capabilities as above, but with higher resolution of touches*/
#define MCFG_PRESSURE 5

struct MConfig {
	/* Used by MTState */

	// Set by caps.
	int touch_type;		// How to determine touch? 0 for none, 1 for scale, 2 for size, 3 for pressure
	int touch_minor;	// Does the touchpad report touches as ellipses? 0 or 1
	int touch_min;		// Minimum touch value.
	int touch_max;		// Maximum touch value.
	int pressure_min;		// Minimum pressure value.
	int pressure_max;		// Maximum pressure value.
	int pad_width;		// Width of the touchpad.
	int pad_height;		// Height of the touchpad.

	// Set by config.
	int touch_down;		// When is a finger touching? 0 - 100 (percentage)
	int touch_up;		// When is a finger released? 0 - 100 (percentage)
	int ignore_thumb;	// Whether or not to ignore thumbs. 0 or 1
	int ignore_palm;	// Whether or not to ignore palms. 0 or 1
	int disable_on_thumb;	// Disable the touchpad if thumb detected. 0 or 1
	int disable_on_palm;	// Disable the touchpad if palm detected. 0 or 1
	int thumb_ratio;	// Ratio of width to length that makes a touch a thumb. 0 - 100
	int thumb_size;		// Minimum touch size for a thumb. 0 - 100
	int palm_size;		// Minimum touch size for a palm. 0 - 100
	int edge_left_size;	// Percent of left edge of trackpad to ignore for new touches. 0 - 100
	int edge_right_size;	// Percent of right edge of trackpad to ignore for new touches. 0 - 100
	int edge_top_size;	// Percent of top edge of trackpad to ignore for new touches. 0 - 100
	int edge_bottom_size;	// Percent of bottom edge of trackpad to ignore for new touches. 0 - 100
	int axis_x_invert;	// Whether or not to invert the x axis. 0 or 1.
	int axis_y_invert;	// Whether or not to invert the y axis. 0 or 1.

	/* Used by Gestures */

	// Set by config.
	int trackpad_disable;	// Disable the trackpad? 0 or 1
	int button_enable;		// Enable physical buttons? 0 or 1
	int button_integrated;	// Is the button under the touchpad? 0 or 1
	int button_expire;		// How long to consider a touch for button emulation. >= 0
	int button_zones;		// Use button zones for emulation?
	int is_button_zones_height_limited;	// Limit the button zones to the height of the bottom edge?
	int button_first_zone;	// Button to execute when clicking in the leftmost part of the zone (button_#_zones > 0)
	int button_second_zone;	// Button to execute when clicking right of zone_button1 part of the zone (button_#_zones > 0)
	int button_third_zone;	// Button to execute when clicking the rightmost part of the zone (button_#_zones > 0)
	int button_0touch;		// What button to emulate when no finger is on the
							// pad and a click is registered? 0 to 32
	int button_1touch;		// What button to emulate when one finger is on the
							// pad or the first zone is clicked? 0 to 32
	int button_2touch;		// What button to emulate when two fingers are on the
							// pad or the second zone is clicked? 0 to 32
	int button_3touch;		// What button to emulate when three fingers are on the
							// pad or the third zone is clicked? 0 to 32
	int button_move;		// Whether or not to count the moving touch towards button
							// emulation.
	int tap_1touch;			// What button to emulate for one touch taps? 0 to 32
	int tap_2touch;			// What button to emulate for two touch taps? 0 to 32
	int tap_3touch;			// What button to emulate for three touch taps? 0 to 32
	int tap_4touch;			// What button to emulate for four touch taps? 0 to 32
	int tap_timeout;		// Window for touches when counting for the button.
							// How long to wait for incoming touches after first one. > 0
	int tap_hold;			// How long to "hold down" the emulated button on tap. > 0
	int tap_dist;			// How far to allow a touch to move before it's a moving touch. > 0
	int gesture_hold;		// How long to "hold down" the emulated button for gestures. > 0
	int gesture_wait;		// How long after a gesture to wait before movement is allowed. >= 0
	struct MConfigSwipe{
		int dist;		// Distance needed to trigger a button. >= 0, 0 disables
		int hold;		// How long to "hold down" the emulated button for swipe gesture. > 0
		int drag_sens;		// Should this gesture emit movement events? 0 disables movement, 1000 set speed same as during normal movement
		int up_btn;		// Button to use for swipe up. >= 0, 0 is none
		int dn_btn;		// Button to use for swipe down. >= 0, 0 is none
		int lt_btn;		// Button to use for swipe left. >= 0, 0 is none
		int rt_btn;		// Button to use for swipe right. >= 0, 0 is none
	} scroll, swipe3, swipe4/*, swipe5*/;
	struct MConfigSwipeCoasting{
		float min_speed;		// What speed to start scroll coasting at. >= 0
		int tick_ms;		// How fast events will be generated during coasting >= 1
		int duration;		// How long coasting ticks will last >= 0, 0 disables coasting
	} scroll_coast;
	struct MConfigSwipe edge_scroll;
	int scale_dist;			// Distance needed to trigger a button. >= 0, 0 disables
	int scale_up_btn;		// Button to use for scale up. >= 0, 0 is none
	int scale_dn_btn;		// Button to use for scale down. >= 0, 0 is none
	int rotate_dist;		// Distance needed to trigger a button. >= 0, 0 disables
	int rotate_lt_btn;		// Button to use for rotate left. >= 0, 0 is none
	int rotate_rt_btn;		// Button to use for rotate right. >= 0, 0 is none
	struct MConfigStationary{
		int max_move;			// How far stationary finger can move, before interrupting the gesture. >= 0
		int button;		// Button to be pressed/released when gesture starts/ends. >= 0, 0 disables
	} hold1_move1_stationary;
#if 0
	struct MConfigStationary hold1_move2_stationary, hold1_move3_stationary;
#endif
	struct MConfigSwipe		hold1_move1;
#if 0
	struct MConfigSwipe		hold1_move2, hold1_move3;
#endif
	int drag_enable;		// Enable tap-to-drag? 0 or 1
	int drag_timeout;		// How long to wait for a move after tapping? > 0
	int drag_wait;			// How long to wait before triggering button down? >= 0
	int drag_dist;			// How far is the finger allowed to move during wait time? >= 0
	int drag_lock_timeout;  // How long to wait in 'drag ready' state after
	                        // dragging finger was released?
	                        // < 0 - wait of tap to break drag; 0 - disable; > 0 - time in ms

	double sensitivity;		// Mouse movement multiplier. >= 0
	int scroll_smooth;		// Enable high precision (smooth) scrolling. 0 or 1.
};

/* Load the MConfig struct with its defaults.
 */
void mconfig_defaults(struct MConfig* cfg);

/* Initialize the MConfig struct.
 */
void mconfig_init(struct MConfig* cfg,
			const struct Capabilities* caps);

void mconfig_configure(struct MConfig* cfg,
			pointer opts);

#endif

