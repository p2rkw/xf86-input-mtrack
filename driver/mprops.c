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
	XISetDeviceProperty(dev, atom, FALSE);
	return atom;
}

Atom atom_init_float(DeviceIntPtr dev, char* name, int nvalues, float* values, Atom float_type) {
	Atom atom = MakeAtom(name, strlen(name), TRUE);
	XIChangeDeviceProperty(dev, atom, float_type, 32, PropModeReplace, nvalues, values, FALSE);
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

	fvals[0] = (float)cfg->sensitivity;
	mprops.sensitivity = atom_init_float(local->dev, MTRACK_PROP_SENSITIVITY, 1, fvals, mprops.float_type);
}

int mprops_set_property(DeviceIntPtr dev, Atom property, XIPropertyValuePtr prop, BOOL checkonly) {
	InputInfoPtr local = dev->public.devicePrivate;
	struct MConfig* cfg = &((struct MTouch*)local->private)->cfg;

	int* ivals;
	float* fvals;

	if (property == mprops.sensitivity) {
		if (prop->size != 1 || prop->format != 32 || prop->type != mprops.float_type)
			return BadMatch;

		if (!checkonly) {
			fvals = (float*)prop->data;
			cfg->sensitivity = fvals[0];
		}
	}

	return Success;
}

