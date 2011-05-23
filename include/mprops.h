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

#ifndef MTRACK_PROPS_H
#define MTRACK_PROPS_H

#include <xorg-server.h>
#include <xf86Module.h>

#include <X11/Xatom.h>
#include <xf86.h>
#include <xf86Xinput.h>
#include <exevents.h>

#include "mconfig.h"

#ifndef XATOM_FLOAT
#define XATOM_FLOAT "FLOAT"
#endif

// int, 1 value
#define MTRACK_PROP_TRACKPAD_DISABLE "Trackpad Disable Input"
// float, 1 value
#define MTRACK_PROP_SENSITIVITY "Trackpad Sensitivity"
// int, 3 values - enable buttons, has integrated button, emulation touch expiration
#define MTRACK_PROP_BUTTON_SETTINGS "Trackpad Button Settings"
// int, 2 values - button to emulate with 1 touch, 2 touches, 3 touches
#define MTRACK_PROP_BUTTON_EMULATE "Trackpad Button Emulation"
// int, 3 values - click time, touch timeout, invalidate distance
#define MTRACK_PROP_TAP_SETTINGS "Trackpad Tap Settings"
// int, 3 values - 1 touch button, 2 touch button, 3 touch button, 4 touch button
#define MTRACK_PROP_TAP_EMULATE "Trackpad Tap Button Emulation"
// int, 2 values - ignore thumb touches, disable trackpad on thumb touches
#define MTRACK_PROP_THUMB_DETECT "Trackpad Thumb Detection"
// int, 2 values - size, width to length ratio
#define MTRACK_PROP_THUMB_SIZE "Trackpad Thumb Size"
// int, 2 values - ignore palm touches, disable trackpad on palm touches
#define MTRACK_PROP_PALM_DETECT "Trackpad Palm Detection"
// int, 1 value - size
#define MTRACK_PROP_PALM_SIZE "Trackpad Palm Size"
// int, 2 value - button hold, wait time
#define MTRACK_PROP_GESTURE_SETTINGS "Trackpad Gesture Settings"
// int, 1 value - distance before a scroll event is triggered
#define MTRACK_PROP_SCROLL_DIST "Trackpad Scroll Distance"
// int, 4 values - up button, down button, left button, right button
#define MTRACK_PROP_SCROLL_BUTTONS "Trackpad Scroll Buttons"
// int, 1 value - distance before a swipe event is triggered
#define MTRACK_PROP_SWIPE_DIST "Trackpad Swipe Distance"
// int, 4 values - up button, down button, left button, right button
#define MTRACK_PROP_SWIPE_BUTTONS "Trackpad Swipe Buttons"
// int, 1 value - distance before a swipe event is triggered
#define MTRACK_PROP_SWIPE4_DIST "Trackpad Swipe4 Distance"
// int, 4 values - up button, down button, left button, right button
#define MTRACK_PROP_SWIPE4_BUTTONS "Trackpad Swipe4 Buttons"
// int, 1 value - distance before a scale event is triggered
#define MTRACK_PROP_SCALE_DIST "Trackpad Scale Distance"
// int, 2 values - up button, down button
#define MTRACK_PROP_SCALE_BUTTONS "Trackpad Scale Buttons"
// int, 1 value - distance before a rotate event is triggered
#define MTRACK_PROP_ROTATE_DIST "Trackpad Rotate Distance"
// int, 2 values - left button, right button
#define MTRACK_PROP_ROTATE_BUTTONS "Trackpad Rotate Buttons"
// int, 4 values - enable, timeout, wait, dist
#define MTRACK_PROP_DRAG_SETTINGS "Trackpad Drag Settings"

struct MProps {
	// Properties Config
	Atom float_type;

	// Adjustable Properties
	Atom api;
	Atom trackpad_disable;
	Atom sensitivity;
	Atom button_settings;
	Atom button_emulate;
	Atom tap_settings;
	Atom tap_emulate;
	Atom thumb_detect;
	Atom thumb_size;
	Atom palm_detect;
	Atom palm_size;
	Atom gesture_settings;
	Atom scroll_dist;
	Atom scroll_buttons;
	Atom swipe_dist;
	Atom swipe_buttons;
	Atom swipe4_dist;
	Atom swipe4_buttons;
	Atom scale_dist;
	Atom scale_buttons;
	Atom rotate_dist;
	Atom rotate_buttons;
	Atom drag_settings;
};

void mprops_init(struct MConfig* cfg, InputInfoPtr local);
int mprops_set_property(DeviceIntPtr dev, Atom property, XIPropertyValuePtr prop, BOOL checkonly);

#endif

