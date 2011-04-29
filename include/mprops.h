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

#include <X11/Xatom.h>
#include <xf86Xinput.h>
#include "mconfig.h"

#ifndef XATOM_FLOAT
#define XATOM_FLOAT "FLOAT"
#endif

#define MTRACK_PROP_SENSITIVITY "Trackpad Sensitivity"	// float, 1 value

struct MProps {
	// Properties Config
	Atom float_type;

	// Adjustable Properties
	Atom sensitivity;
};

void mprops_init(struct MConfig* cfg, InputInfoPtr local);
int mprops_set_property(DeviceIntPtr dev, Atom property, XIPropertyValuePtr prop, BOOL checkonly);

#endif

