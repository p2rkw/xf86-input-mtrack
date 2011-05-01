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

#define VALID_BUTTON(x) (x >= 0 && x <= 32)
#define VALID_BOOL(x) (x == 0 || x == 1)

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

	ivals[0] = MTRACK_PROP_VERSION;
	mprops.api = atom_init_integer(local->dev, MTRACK_PROP_API, 1, ivals, 32);

	ivals[0] = cfg->trackpad_disable;
	mprops.trackpad_disable = atom_init_integer(local->dev, MTRACK_PROP_TRACKPAD_DISABLE, 1, ivals, 8);

	fvals[0] = (float)cfg->sensitivity;
	mprops.sensitivity = atom_init_float(local->dev, MTRACK_PROP_SENSITIVITY, 1, fvals, mprops.float_type);

	ivals[0] = cfg->button_enable;
	ivals[1] = cfg->button_integrated;
	ivals[2] = cfg->button_expire;
	mprops.button_settings = atom_init_integer(local->dev, MTRACK_PROP_BUTTON_SETTINGS, 3, ivals, 16);

	ivals[0] = cfg->button_1touch;
	ivals[1] = cfg->button_2touch;
	mprops.button_emulate = atom_init_integer(local->dev, MTRACK_PROP_BUTTON_EMULATE, 2, ivals, 8);

	ivals[0] = cfg->tap_hold;
	ivals[1] = cfg->tap_timeout;
	ivals[2] = cfg->tap_dist;
	mprops.tap_settings = atom_init_integer(local->dev, MTRACK_PROP_TAP_SETTINGS, 3, ivals, 32);

	ivals[0] = cfg->tap_1touch;
	ivals[1] = cfg->tap_2touch;
	ivals[2] = cfg->tap_3touch;
	mprops.tap_emulate = atom_init_integer(local->dev, MTRACK_PROP_TAP_EMULATE, 3, ivals, 8);

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

	ivals[0] = cfg->scroll_dist;
	mprops.scroll_dist = atom_init_integer(local->dev, MTRACK_PROP_SCROLL_DIST, 1, ivals, 32);

	ivals[0] = cfg->scroll_up_btn;
	ivals[1] = cfg->scroll_dn_btn;
	ivals[2] = cfg->scroll_lt_btn;
	ivals[3] = cfg->scroll_rt_btn;
	mprops.scroll_buttons = atom_init_integer(local->dev, MTRACK_PROP_SCROLL_BUTTONS, 4, ivals, 8);

	ivals[0] = cfg->swipe_dist;
	mprops.swipe_dist = atom_init_integer(local->dev, MTRACK_PROP_SWIPE_DIST, 1, ivals, 32);

	ivals[0] = cfg->swipe_up_btn;
	ivals[1] = cfg->swipe_dn_btn;
	ivals[2] = cfg->swipe_lt_btn;
	ivals[3] = cfg->swipe_rt_btn;
	mprops.swipe_buttons = atom_init_integer(local->dev, MTRACK_PROP_SWIPE_BUTTONS, 4, ivals, 8);

	ivals[0] = cfg->scale_dist;
	mprops.scale_dist = atom_init_integer(local->dev, MTRACK_PROP_SCALE_DIST, 1, ivals, 32);

	ivals[0] = cfg->scale_up_btn + 1;
	ivals[1] = cfg->scale_dn_btn + 1;
	mprops.scale_buttons = atom_init_integer(local->dev, MTRACK_PROP_SCALE_BUTTONS, 2, ivals, 8);

	ivals[0] = cfg->rotate_dist;
	mprops.rotate_dist = atom_init_integer(local->dev, MTRACK_PROP_ROTATE_DIST, 1, ivals, 32);

	ivals[0] = cfg->rotate_lt_btn + 1;
	ivals[1] = cfg->rotate_rt_btn + 1;
	mprops.rotate_buttons = atom_init_integer(local->dev, MTRACK_PROP_SCALE_BUTTONS, 2, ivals, 8);

	ivals[0] = cfg->drag_enable;
	ivals[1] = cfg->drag_timeout;
	mprops.drag_settings = atom_init_integer(local->dev, MTRACK_PROP_DRAG_SETTINGS, 2, ivals, 16);
}

int mprops_set_property(DeviceIntPtr dev, Atom property, XIPropertyValuePtr prop, BOOL checkonly) {
	InputInfoPtr local = dev->public.devicePrivate;
	struct MConfig* cfg = &((struct MTouch*)local->private)->cfg;

	uint8_t* ivals8;
	uint16_t* ivals16;
	uint32_t* ivals32;
	float* fvals;

	if (property == mprops.api) {
		xf86Msg(X_ERROR, "mtrack: Changing the properties API value is not allowed!");
		return BadMatch;
	}
	else if (property == mprops.trackpad_disable) {
		if (prop->size != 1 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BOOL(ivals8[0]))
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
	else if (property == mprops.button_settings) {
		if (prop->size != 3 || prop->format != 16 || prop->type != XA_INTEGER) 
			return BadMatch;

		ivals16 = (uint16_t*)prop->data;
		if (!VALID_BOOL(ivals16[0]) || !VALID_BOOL(ivals16[1]) || ivals16[2] < 0)
			return BadMatch;

		if (!checkonly) {
			cfg->button_enable = ivals16[0];
			cfg->button_integrated = ivals16[1];
			cfg->button_expire = ivals16[2];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set button settings to %d %d %d\n",
				cfg->button_enable, cfg->button_integrated, cfg->button_expire);
#endif
		}
	}
	else if (property == mprops.button_emulate) {
		if (prop->size != 2 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BUTTON(ivals8[0]) || !VALID_BUTTON(ivals8[1]))
			return BadMatch;

		if (!checkonly) {
			cfg->button_1touch = ivals8[0];
			cfg->button_2touch = ivals8[1];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set button emulation to %d %d\n",
				cfg->button_1touch, cfg->button_2touch);
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
		if (prop->size != 3 || prop->format != 8 || prop->type != XA_INTEGER)
			return BadMatch;

		ivals8 = (uint8_t*)prop->data;
		if (!VALID_BUTTON(ivals8[0]) || !VALID_BUTTON(ivals8[1]) || !VALID_BUTTON(ivals8[2]))
			return BadMatch;

		if (!checkonly) {			
			cfg->tap_1touch = ivals8[0];
			cfg->tap_2touch = ivals8[1];
			cfg->tap_3touch = ivals8[2];
#ifdef DEBUG_PROPS
			xf86Msg(X_INFO, "mtrack: set tap emulation to %d %d %d\n",
				cfg->tap_1touch, cfg->tap_2touch, cfg->tap_3touch);
#endif
		}
	}

	return Success;
}

