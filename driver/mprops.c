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

#include "mprops.h"
#include "common.h"
#include "mtouch.h"

#define MAX_INT_VALUES 4
#define MAX_FLOAT_VALUES 4
#define MAX_BUTTON_VALUES 6

#define VALID_BUTTON(x) (x >= 0 && x <= 32)
#define VALID_BOOL(x) (x == 0 || x == 1)
#define VALID_PCNT(x) (x >= 0 && x <= 100)

struct MProps mprops;

Atom atom_init_integer(DeviceIntPtr dev, char* name, int nvalues, int* values, int size) {
	Atom atom;
	int i;
	uint8_t uvals8[MAX_INT_VALUES];
	uint16_t uvals16[MAX_INT_VALUES];
	uint32_t uvals32[MAX_INT_VALUES];
	pointer uvals;
	nvalues = MINVAL(MAX_INT_VALUES, nvalues);

	switch(size) {
	case 8:
		for (i = 0; i < nvalues; i++) {
			uvals8[i] = values[i];
		}
		uvals = uvals8;
		break;
	case 16:
		for (i = 0; i < nvalues; i++) {
			uvals16[i] = values[i];
		}
		uvals = uvals16;
		break;
	default:
		for (i = 0; i < nvalues; i++) {
			uvals32[i] = values[i];
		}
		uvals = uvals32;
		break;
	}

	atom = MakeAtom(name, strlen(name), TRUE);
	XIChangeDeviceProperty(dev, atom, XA_INTEGER, size, PropModeReplace, nvalues, uvals, FALSE);
	XISetDevicePropertyDeletable(dev, atom, FALSE);
	return atom;
}

Atom atom_init_float(DeviceIntPtr dev, char* name, int nvalues, float* values, Atom float_type) {
	Atom atom = MakeAtom(name, strlen(name), TRUE);
	XIChangeDeviceProperty(dev, atom, float_type, 32, PropModeReplace, nvalues, values, FALSE);
	XISetDevicePropertyDeletable(dev, atom, FALSE);
	return atom;
}

static void init_swipe_props(DeviceIntPtr dev, struct MPropsSwipe* props_swipe,
                             struct MConfigSwipe* cfg_swipe, char const* settings_prop_name,
                             char const* buttons_prop_name){
	int ivals[MAX_INT_VALUES];
	ivals[0] = cfg_swipe->dist;
	ivals[1] = cfg_swipe->hold;
	ivals[2] = cfg_swipe->drag_sens;
	props_swipe->settings = atom_init_integer(dev, (char*)settings_prop_name, 3, ivals, 32);

	ivals[0] = cfg_swipe->up_btn;
	ivals[1] = cfg_swipe->dn_btn;
	ivals[2] = cfg_swipe->lt_btn;
	ivals[3] = cfg_swipe->rt_btn;
	props_swipe->buttons = atom_init_integer(dev, (char*)buttons_prop_name, 4, ivals, 8);
}

void mprops_init(struct MConfig* cfg, InputInfoPtr local) {
	int ivals[MAX_INT_VALUES];
	float fvals[MAX_FLOAT_VALUES];

	mprops.float_type = XIGetKnownProperty(XATOM_FLOAT);
	if (!mprops.float_type) {
		mprops.float_type = MakeAtom(XATOM_FLOAT, strlen(XATOM_FLOAT), TRUE);
		if (!mprops.float_type) {
			xf86Msg(X_ERROR, "mtrack: %s: Failed to init float atom. Property support is disabled.\n", local->name);
			return;
		}
	}

	ivals[0] = cfg->trackpad_disable;
	mprops.trackpad_disable = atom_init_integer(local->dev, MTRACK_PROP_TRACKPAD_DISABLE, 1, ivals, 8);

	fvals[0] = (float)cfg->sensitivity;
	mprops.sensitivity = atom_init_float(local->dev, MTRACK_PROP_SENSITIVITY, 1, fvals, mprops.float_type);

	ivals[0] = cfg->touch_down;
	ivals[1] = cfg->touch_up;
	mprops.pressure = atom_init_integer(local->dev, MTRACK_PROP_PRESSURE, 2, ivals, 8);

	ivals[0] = cfg->button_enable;
	ivals[1] = cfg->button_integrated;
	mprops.button_settings = atom_init_integer(local->dev, MTRACK_PROP_BUTTON_SETTINGS, 2, ivals, 8);

	ivals[0] = cfg->button_zones;
	ivals[1] = cfg->button_move;
	ivals[2] = cfg->button_expire;
	mprops.button_emulate_settings = atom_init_integer(local->dev, MTRACK_PROP_BUTTON_EMULATE_SETTINGS, 3, ivals, 16);

	ivals[0] = cfg->button_1touch;
	ivals[1] = cfg->button_2touch;
	ivals[2] = cfg->button_3touch;
	mprops.button_emulate_values = atom_init_integer(local->dev, MTRACK_PROP_BUTTON_EMULATE_VALUES, 3, ivals, 8);

	ivals[0] = cfg->tap_hold;
	ivals[1] = cfg->tap_timeout;
	ivals[2] = cfg->tap_dist;
	mprops.tap_settings = atom_init_integer(local->dev, MTRACK_PROP_TAP_SETTINGS, 3, ivals, 32);

	ivals[0] = cfg->tap_1touch;
	ivals[1] = cfg->tap_2touch;
	ivals[2] = cfg->tap_3touch;
	ivals[3] = cfg->tap_4touch;
	mprops.tap_emulate = atom_init_integer(local->dev, MTRACK_PROP_TAP_EMULATE, 4, ivals, 8);

	ivals[0] = cfg->ignore_thumb;
	ivals[1] = cfg->disable_on_thumb;
	mprops.thumb_detect = atom_init_integer(local->dev, MTRACK_PROP_THUMB_DETECT, 2, ivals, 8);

	ivals[0] = cfg->thumb_size;
	ivals[1] = cfg->thumb_ratio;
	mprops.thumb_size = atom_init_integer(local->dev, MTRACK_PROP_THUMB_SIZE, 2, ivals, 32);

	ivals[0] = cfg->ignore_palm;
	ivals[1] = cfg->disable_on_palm;
	mprops.palm_detect = atom_init_integer(local->dev, MTRACK_PROP_PALM_DETECT, 2, ivals, 8);

	ivals[0] = cfg->palm_size;
	mprops.palm_size = atom_init_integer(local->dev, MTRACK_PROP_PALM_SIZE, 1, ivals, 32);

	ivals[0] = cfg->gesture_hold;
	ivals[1] = cfg->gesture_wait;
	mprops.gesture_settings = atom_init_integer(local->dev, MTRACK_PROP_GESTURE_SETTINGS, 2, ivals, 16);

	ivals[0] = cfg->scroll_smooth;
	mprops.scroll_high_prec = atom_init_integer(local->dev, MTRACK_PROP_SCROLL_HIGH_PRECISION, 1, ivals, 8);

	init_swipe_props(local->dev, &mprops.scroll, &cfg->scroll, MTRACK_PROP_SCROLL_SETTINGS, MTRACK_PROP_SCROLL_BUTTONS);

	init_swipe_props(local->dev, &mprops.swipe3, &cfg->swipe3, MTRACK_PROP_SWIPE_SETTINGS, MTRACK_PROP_SWIPE_BUTTONS);

	init_swipe_props(local->dev, &mprops.swipe4, &cfg->swipe4, MTRACK_PROP_SWIPE4_SETTINGS, MTRACK_PROP_SWIPE4_BUTTONS);

	ivals[0] = cfg->scale_dist;
	mprops.scale_dist = atom_init_integer(local->dev, MTRACK_PROP_SCALE_DIST, 1, ivals, 32);

	ivals[0] = cfg->scale_up_btn;
	ivals[1] = cfg->scale_dn_btn;
	mprops.scale_buttons = atom_init_integer(local->dev, MTRACK_PROP_SCALE_BUTTONS, 2, ivals, 8);

	ivals[0] = cfg->rotate_dist;
	mprops.rotate_dist = atom_init_integer(local->dev, MTRACK_PROP_ROTATE_DIST, 1, ivals, 32);

	ivals[0] = cfg->rotate_lt_btn;
	ivals[1] = cfg->rotate_rt_btn;
	mprops.rotate_buttons = atom_init_integer(local->dev, MTRACK_PROP_ROTATE_BUTTONS, 2, ivals, 8);

	ivals[0] = cfg->hold1_move1_stationary.max_move;
	ivals[1] = cfg->hold1_move1_stationary.button;
	mprops.hold1_move1_stationary = atom_init_integer(local->dev, MTRACK_PROP_HOLD1_MOVE1_STATIONARY_SETTINGS, 2, ivals, 32);

	init_swipe_props(local->dev, &mprops.hold1_move1, &cfg->hold1_move1, MTRACK_PROP_HOLD1_MOVE1_SETTINGS, MTRACK_PROP_HOLD1_MOVE1_BUTTONS);

#if 0
	ivals[0] = cfg->hold1_move2_stationary.max_move;
	ivals[1] = cfg->hold1_move2_stationary.button;
	mprops.hold1_move2_stationary = atom_init_integer(local->dev, MTRACK_PROP_HOLD1_MOVE2_STATIONARY_SETTINGS, 2, ivals, 32);

	init_swipe_props(local->dev, &mprops.hold1_move2, &cfg->hold1_move2, MTRACK_PROP_HOLD1_MOVE2_SETTINGS, MTRACK_PROP_HOLD1_MOVE2_BUTTONS);


	ivals[0] = cfg->hold1_move3_stationary.max_move;
	ivals[1] = cfg->hold1_move3_stationary.button;
	mprops.hold1_move3_stationary = atom_init_integer(local->dev, MTRACK_PROP_HOLD1_MOVE3_STATIONARY_SETTINGS, 2, ivals, 32);

	init_swipe_props(local->dev, &mprops.hold1_move3, &cfg->hold1_move3, MTRACK_PROP_HOLD1_MOVE3_SETTINGS, MTRACK_PROP_HOLD1_MOVE3_BUTTONS);
#endif

	ivals[0] = cfg->drag_enable;
	ivals[1] = cfg->drag_timeout;
	ivals[2] = cfg->drag_wait;
	ivals[3] = cfg->drag_dist;
	mprops.drag_settings = atom_init_integer(local->dev, MTRACK_PROP_DRAG_SETTINGS, 4, ivals, 32);

	ivals[0] = cfg->axis_x_invert;
	ivals[1] = cfg->axis_y_invert;
	mprops.axis_invert = atom_init_integer(local->dev, MTRACK_PROP_AXIS_INVERT, 2, ivals, 8);

	ivals[0] = cfg->edge_size;
	mprops.edge_size = atom_init_integer(local->dev, MTRACK_PROP_EDGE_SIZE, 1, ivals, 8);
}

int check_buttons_property(XIPropertyValuePtr prop, uint8_t** buttons_ret_arr, int buttons_count)
{
	uint8_t* ivals8;
	int i;

	if (prop->size != buttons_count || prop->format != 8 || prop->type != XA_INTEGER)
		return BadMatch;

	ivals8 = (uint8_t*)prop->data;
	for (i = 0; i < buttons_count; ++i) {
		if (!VALID_BUTTON(ivals8[i]))
			return BadMatch;
	}

	*buttons_ret_arr = ivals8;
	return Success;
}

/* Return:
 * 1 - property was recognized and handled with or without error, check error code for details
 * 0 - property not recognized, don't trust returned error code - it's invalid
 */
static int set_swipe_properties(Atom property, BOOL checkonly, XIPropertyValuePtr prop,
                                struct MPropsSwipe* props_swipe,
                                struct MConfigSwipe* cfg_swipe, int* error_code){

	uint8_t* ivals8;
	uint32_t* ivals32;

	*error_code = Success;

	if (property == props_swipe->settings) {
		if (prop->size != 3 || prop->format != 32 || prop->type != XA_INTEGER)
			return *error_code = BadMatch, 1;

		ivals32 = (uint32_t*)prop->data;
		if (ivals32[0] < 1 || (int)(ivals32[1]) < 0)
			return *error_code = BadMatch, 1;

		if (!checkonly) {
			cfg_swipe->dist = ivals32[0];
			cfg_swipe->hold = ivals32[1];
			cfg_swipe->drag_sens = ivals32[2];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set swipe settings: dist: %d hold: %d\n",
				cfg_swipe->dist, cfg_swipe->hold);
#endif
		}
	}
	else if (property == props_swipe->buttons) {
		if (check_buttons_property(prop, &ivals8, 4) == Success){
			if (!checkonly) {
				cfg_swipe->up_btn = ivals8[0];
				cfg_swipe->dn_btn = ivals8[1];
				cfg_swipe->lt_btn = ivals8[2];
				cfg_swipe->rt_btn = ivals8[3];
			}
		}
		else
			return *error_code = BadMatch, 1;
	}
	else{
		return 0;
	}
	return 1;
}

int mprops_set_property(DeviceIntPtr dev, Atom property, XIPropertyValuePtr prop, BOOL checkonly) {
	InputInfoPtr local = dev->public.devicePrivate;
	struct MConfig* cfg = &((struct MTouch*)local->private)->cfg;

	uint8_t* ivals8;
	uint16_t* ivals16;
	uint32_t* ivals32;
	float* fvals;

   int error_code;

	if (property == mprops.trackpad_disable) {
		if (prop->size != 1 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (ivals8[0] < 0 || ivals8[0] > 3)
			return BadMatch;

		if (!checkonly) {
			cfg->trackpad_disable = ivals8[0];
#ifdef DEBUG_PROPS
			if (cfg->trackpad_disable)
				xf86Msg(X_INFO, "mtrack: trackpad input disabled\n");
			else
				xf86Msg(X_INFO, "mtrack: trackpad input enabled\n");
#endif
		}
	}
	else if (property == mprops.sensitivity) {
		if (prop->size != 1 || prop->format != 32 || prop->type != mprops.float_type)
			return BadMatch;

		fvals = (float*)prop->data;
		if (fvals[0] < 0)
			return BadMatch;

		if (!checkonly) {
			cfg->sensitivity = fvals[0];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set sensitivity to %f\n", cfg->sensitivity);
#endif
		}
	}
	else if (property == mprops.pressure) {
		if (prop->size != 2 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_PCNT(ivals8[0]) || !VALID_PCNT(ivals8[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->touch_down = ivals8[0];
			cfg->touch_up = ivals8[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set touch pressure to %d %d\n",
				cfg->touch_down, cfg->touch_up);
#endif
		}
	}
	else if (property == mprops.button_settings) {
		if (prop->size != 2 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BOOL(ivals8[0]) || !VALID_BOOL(ivals8[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->button_enable = ivals8[0];
			cfg->button_integrated = ivals8[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set button settings to %d %d\n",
				cfg->button_enable, cfg->button_integrated);
#endif
		}
	}
	else if (property == mprops.button_emulate_settings) {
		if (prop->size != 3 || prop->format != 16 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals16 = (uint16_t*)prop->data;
		if (!VALID_BOOL(ivals16[0]) || !VALID_BOOL(ivals16[1]) || ivals16[2] < 0)
			return BadMatch;

		if (!checkonly) {
			cfg->button_zones = ivals16[0];
			cfg->button_move = ivals16[1];
			cfg->button_expire = ivals16[2];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set button emulate settings to %d %d %d\n",
				cfg->button_zones, cfg->button_move, cfg->button_expire);
#endif
		}
	}
	else if (property == mprops.button_emulate_values) {
		if (prop->size != 3 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BUTTON(ivals8[0]) || !VALID_BUTTON(ivals8[1]) || !VALID_BUTTON(ivals8[2]))
			return BadMatch;

		if (!checkonly) {
			cfg->button_1touch = ivals8[0];
			cfg->button_2touch = ivals8[1];
			cfg->button_3touch = ivals8[2];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set button emulation to %d %d %s\n",
				cfg->button_1touch, cfg->button_2touch, cfg->button_3touch);
#endif
		}
	}
	else if (property == mprops.tap_settings) {
		if (prop->size != 3 || prop->format != 32 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals32 = (uint32_t*)prop->data;
		if (ivals32[0] < 1 || ivals32[1] < 1 || ivals32[2] < 1)
			return BadMatch;

		if (!checkonly) {
			cfg->tap_hold = ivals32[0];
			cfg->tap_timeout = ivals32[1];
			cfg->tap_dist = ivals32[2];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set tap settings to %d %d %d\n",
				cfg->tap_hold, cfg->tap_timeout, cfg->tap_dist);
#endif
		}
	}
	else if (property == mprops.tap_emulate) {
		if (prop->size != 4 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BUTTON(ivals8[0]) || !VALID_BUTTON(ivals8[1]) || !VALID_BUTTON(ivals8[2]) || !VALID_BUTTON(ivals8[3]))
			return BadMatch;

		if (!checkonly) {
			cfg->tap_1touch = ivals8[0];
			cfg->tap_2touch = ivals8[1];
			cfg->tap_3touch = ivals8[2];
			cfg->tap_4touch = ivals8[3];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set tap emulation to %d %d %d %d\n",
				cfg->tap_1touch, cfg->tap_2touch, cfg->tap_3touch, cfg->tap_4touch);
#endif
		}
	}
	else if (property == mprops.thumb_detect) {
		if (prop->size != 2 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BOOL(ivals8[0]) || !VALID_BOOL(ivals8[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->ignore_thumb = ivals8[0];
			cfg->disable_on_thumb = ivals8[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set thumb detect to %d %d\n",
				cfg->ignore_thumb, cfg->disable_on_thumb);
#endif
		}
	}
	else if (property == mprops.thumb_size) {
		if (prop->size != 2 || prop->format != 32 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals32 = (uint32_t*)prop->data;
		if (ivals32[0] < 0 || !VALID_PCNT(ivals32[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->thumb_size = ivals32[0];
			cfg->thumb_ratio = ivals32[0];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set thumb size to %d %d\n",
				cfg->thumb_size, cfg->thumb_ratio);
#endif
		}
	}
	else if (property == mprops.palm_detect) {
		if (prop->size != 2 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BOOL(ivals8[0]) || !VALID_BOOL(ivals8[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->ignore_palm = ivals8[0];
			cfg->disable_on_palm = ivals8[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set palm detect to %d %d\n",
				cfg->ignore_palm, cfg->disable_on_palm);
#endif
		}
	}
	else if (property == mprops.palm_size) {
		if (prop->size != 1 || prop->format != 32 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals32 = (uint32_t*)prop->data;
		if (ivals32[0] < 0)
			return BadMatch;

		if (!checkonly) {
			cfg->palm_size = ivals32[0];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set palm size to %d\n",
				cfg->palm_size);
#endif
		}
	}
	else if (property == mprops.gesture_settings) {
		if (prop->size != 2 || prop->format != 16 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals16 = (uint16_t*)prop->data;
		if (ivals16[0] < 1 || ivals16[1] < 0)
			return BadMatch;

		if (!checkonly) {
			cfg->gesture_hold = ivals16[0];
			cfg->gesture_wait = ivals16[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set gesture settings to %d %d\n",
				cfg->gesture_hold, cfg->gesture_wait);
#endif
		}
	}
	else if (property == mprops.scroll_high_prec) {
		if (prop->size != 1 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BOOL(ivals8[0]))
			return BadMatch;

		if (!checkonly) {
			cfg->scroll_smooth = ivals8[0];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set high precision scrolling to %d\n",
				cfg->scroll_smooth);
#endif
		}
	}
	else if (set_swipe_properties(property, checkonly, prop, &mprops.scroll, &cfg->scroll, &error_code)) {
		return error_code;
	}
	else if (set_swipe_properties(property, checkonly, prop, &mprops.swipe3, &cfg->swipe3, &error_code)) {
		return error_code;
	}
	else if (set_swipe_properties(property, checkonly, prop, &mprops.swipe4, &cfg->swipe4, &error_code)) {
		return error_code;
	}
	else if (property == mprops.scale_dist) {
		if (prop->size != 1 || prop->format != 32 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals32 = (uint32_t*)prop->data;
		if (ivals32[0] < 1)
			return BadMatch;

		if (!checkonly) {
			cfg->scale_dist = ivals32[0];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set scale distance to %d\n",
				cfg->scale_dist);
#endif
		}
	}
	else if (property == mprops.scale_buttons) {
		if (prop->size != 2 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BUTTON(ivals8[0]) || !VALID_BUTTON(ivals8[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->scale_up_btn = ivals8[0];
			cfg->scale_dn_btn = ivals8[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set scale buttons to %d %d\n",
				cfg->scale_up_btn, cfg->scale_dn_btn);
#endif
		}
	}
	else if (property == mprops.rotate_dist) {
		if (prop->size != 1 || prop->format != 32 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals32 = (uint32_t*)prop->data;
		if (ivals32[0] < 1)
			return BadMatch;

		if (!checkonly) {
			cfg->rotate_dist = ivals32[0];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set rotate distance to %d\n",
				cfg->rotate_dist);
#endif
		}
	}
	else if (property == mprops.rotate_buttons) {
		if (prop->size != 2 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BUTTON(ivals8[0]) || !VALID_BUTTON(ivals8[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->rotate_lt_btn = ivals8[0];
			cfg->rotate_rt_btn = ivals8[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set rotate buttons to %d %d\n",
				cfg->rotate_lt_btn, cfg->rotate_rt_btn);
#endif
		}
	}
	else if (property == mprops.hold1_move1_stationary){
		if (prop->size != 2 || prop->format != 32 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals32 = (uint32_t*)prop->data;
		if (ivals32[0] < 0|| !VALID_BUTTON(ivals32[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->hold1_move1_stationary.max_move = ivals32[0];
			cfg->hold1_move1_stationary.button = ivals32[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: hold1_move1: max_move %d; button: %d\n",
				cfg->hold1_move1_stationary.max_move, cfg->hold1_move1_stationary.button);
#endif
		}
	}
	else if (set_swipe_properties(property, checkonly, prop, &mprops.hold1_move1, &cfg->hold1_move1, &error_code)) {
		return error_code;
	}
#if 0
	else if (property == mprops.hold1_move2_stationary){
		if (prop->size != 2 || prop->format != 32 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals32 = (uint32_t*)prop->data;
		if (ivals32[0] < 0|| !VALID_BUTTON(ivals32[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->hold1_move2_stationary.max_move = ivals32[0];
			cfg->hold1_move2_stationary.button = ivals32[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: hold1_move2: max_move %d; button: %d\n",
				cfg->hold1_move2_stationary.max_move, cfg->hold1_move2_stationary.button);
#endif
		}
	}
	else if (set_swipe_properties(property, checkonly, prop, &mprops.hold1_move2, &cfg->hold1_move2, &error_code)) {
		return error_code;
	}
	else if (property == mprops.hold1_move3_stationary){
		if (prop->size != 2 || prop->format != 32 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals32 = (uint32_t*)prop->data;
		if (ivals32[0] < 0|| !VALID_BUTTON(ivals32[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->hold1_move3_stationary.max_move = ivals32[0];
			cfg->hold1_move3_stationary.button = ivals32[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: hold1_move1: max_move %d; button: %d\n",
				cfg->hold1_move3_stationary.max_move, cfg->hold1_move3_stationary.button);
#endif
		}
	}
	else if (set_swipe_properties(property, checkonly, prop, &mprops.hold1_move3, &cfg->hold1_move3, &error_code)) {
		return error_code;
	}
#endif
	else if (property == mprops.drag_settings) {
		if (prop->size != 4 || prop->format != 32 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals32 = (uint32_t*)prop->data;
		if (!VALID_BOOL(ivals32[0]) || ivals32[1] < 1 || (int)(ivals32[2]) < 0 || (int)(ivals32[3]) < 0)
			return BadMatch;

		if (!checkonly) {
			cfg->drag_enable = ivals32[0];
			cfg->drag_timeout = ivals32[1];
			cfg->drag_wait = ivals32[2];
			cfg->drag_dist = ivals32[3];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set drag settings to %d %d %d %d\n",
				cfg->drag_enable, cfg->drag_timeout, cfg->drag_wait, cfg->drag_dist);
#endif
		}
	}
	else if (property == mprops.axis_invert) {
		if (prop->size != 2 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BOOL(ivals8[0]) || !VALID_BOOL(ivals8[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->axis_x_invert = ivals8[0];
			cfg->axis_y_invert = ivals8[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set axis inversion to %d %d\n",
				cfg->axis_x_invert, cfg->axis_y_invert);
#endif
		}
	}
	else if (property == mprops.edge_size) {
		if (prop->size != 1 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_PCNT(ivals8[0]))
			return BadMatch;

		if (!checkonly) {
			cfg->edge_size = ivals8[0];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set edge size to %d\n",
				cfg->edge_size);
#endif
		}
	}

	return Success;
}

