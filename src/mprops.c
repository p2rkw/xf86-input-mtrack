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

#define MAX_INT_VALUES 4
#define MAX_FLOAT_VALUES 4

static void atom_init_integer(DeviceIntPtr dev, char* name, int nvalues int* values, int size) {
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

static void atom_init_float(DeviceIntPtr dev, char* name, int nvalues, float* values, Atom float_type) {
	Atom atom = MakeAtom(name, strlen(name), TRUE);
	XIChangeDeviceProperty(dev, atom, float_type, 32, PropModeReplace, nvalues, values, FALSE);
}

void mprops_init(struct MProps* props, static struct MConfig* cfg, InputInfoPtr local) {
	int ivals[MAX_INT_VALUES];
	float fvals[MAX_FLOAT_VALUES];

	props->float_type = XIGetKnownProperty(XATOM_FLOAT);
	if (!props->float_type) {
		props->float_type = MakeAtom(XATOM_FLOAT, strlen(XATOM_FLOAT), TRUE);
		if (!props->float_type)
			xf86Msg(X_ERROR, "mtrack: %s: Failed to init float atom. Property support is disabled.\n", local->name);
	}

	fvals[0] = (float)cfg->sensitivity;
	props->sensitivity = atom_init_float(local->dev, MTRACK_PROP_SENSITIVITY, 1, fvals, props->float_type);
}

int mprops_set_property(DeviceIntPtr dev, Atom property, XIPropertyValuePtr prop, BOOL checkonly) {
	InputInfoPtr local = dev->public.devicePrivate;
	struct MProps props_tmp;
	struct MProps* props_real = ((MTouch*)local->private)->props;
	struct MProps* props = checkonly ? &props_tmp : props_real;
	struct MConfig* cfg = ((MTouch*)local->private)->cfg;

	int* ivals;
	float* fvals;

	if (property == props_real->sensitivity) {
		if (prop->size != 1 || prop->format != 32 || prop->type != props_real->float_type)
			return BadMatch;

		fvals = (float*)prop->data;
		cfg->sensitivity = fvals[0];
	}
}

