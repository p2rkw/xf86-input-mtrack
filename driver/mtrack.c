/***************************************************************************
 *
 * Multitouch X driver
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
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

#include "mtouch.h"
#include "mprops.h"

#include <xf86Module.h>
#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 7
#include <X11/Xatom.h>
#include <xserver-properties.h>
#endif

#define TAP_HOLD 100
#define TAP_TIMEOUT 200
#define TAP_THRESHOLD 0.05
#define TICK_TIMEOUT 50
#define SCROLL_THRESHOLD 0.05
#define SWIPE_THRESHOLD 0.15
#define SCALE_THRESHOLD 0.15
#define ROTATE_THRESHOLD 0.15

#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 12
typedef InputInfoPtr LocalDevicePtr;
#endif

/* button mapping simplified */
#define PROPMAP(m, x, y) m[x] = XIGetKnownProperty(y)

static void pointer_control(DeviceIntPtr dev, PtrCtrl *ctrl)
{
#if DEBUG_DRIVER
	xf86Msg(X_INFO, "pointer_control\n");
#endif
}

#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 7
static void initAxesLabels(Atom map[2])
{
	memset(map, 0, 2 * sizeof(Atom));
	PROPMAP(map, 0, AXIS_LABEL_PROP_REL_X);
	PROPMAP(map, 1, AXIS_LABEL_PROP_REL_Y);
}

static void initButtonLabels(Atom map[DIM_BUTTON])
{
	memset(map, 0, DIM_BUTTON * sizeof(Atom));
	PROPMAP(map, MT_BUTTON_LEFT, BTN_LABEL_PROP_BTN_LEFT);
	PROPMAP(map, MT_BUTTON_MIDDLE, BTN_LABEL_PROP_BTN_MIDDLE);
	PROPMAP(map, MT_BUTTON_RIGHT, BTN_LABEL_PROP_BTN_RIGHT);
	PROPMAP(map, MT_BUTTON_WHEEL_UP, BTN_LABEL_PROP_BTN_WHEEL_UP);
	PROPMAP(map, MT_BUTTON_WHEEL_DOWN, BTN_LABEL_PROP_BTN_WHEEL_DOWN);
	PROPMAP(map, MT_BUTTON_HWHEEL_LEFT, BTN_LABEL_PROP_BTN_HWHEEL_LEFT);
	PROPMAP(map, MT_BUTTON_HWHEEL_RIGHT, BTN_LABEL_PROP_BTN_HWHEEL_RIGHT);
	/* how to map swipe buttons? */
	PROPMAP(map, MT_BUTTON_SWIPE_UP, BTN_LABEL_PROP_BTN_0);
	PROPMAP(map, MT_BUTTON_SWIPE_DOWN, BTN_LABEL_PROP_BTN_1);
	PROPMAP(map, MT_BUTTON_SWIPE_LEFT, BTN_LABEL_PROP_BTN_2);
	PROPMAP(map, MT_BUTTON_SWIPE_RIGHT, BTN_LABEL_PROP_BTN_3);
	/* how to map scale and rotate? */
	PROPMAP(map, MT_BUTTON_SCALE_DOWN, BTN_LABEL_PROP_BTN_4);
	PROPMAP(map, MT_BUTTON_SCALE_UP, BTN_LABEL_PROP_BTN_5);
	PROPMAP(map, MT_BUTTON_ROTATE_LEFT, BTN_LABEL_PROP_BTN_6);
	PROPMAP(map, MT_BUTTON_ROTATE_RIGHT, BTN_LABEL_PROP_BTN_7);
}
#endif

static int device_init(DeviceIntPtr dev, LocalDevicePtr local)
{
	struct MTouch *mt = local->private;
	unsigned char btmap[DIM_BUTTON + 1] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
	};
#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 7
	Atom axes_labels[2], btn_labels[DIM_BUTTON];
	initAxesLabels(axes_labels);
	initButtonLabels(btn_labels);
#endif

	local->fd = xf86OpenSerial(local->options);
	if (local->fd < 0) {
		xf86Msg(X_ERROR, "mtrack: cannot open device\n");
		return !Success;
	}
	if (mtouch_configure(mt, local->fd)) {
		xf86Msg(X_ERROR, "mtrack: cannot configure device\n");
		return !Success;
	}
	xf86CloseSerial(local->fd);

#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) < 3
	InitPointerDeviceStruct((DevicePtr)dev,
				btmap, DIM_BUTTON,
				GetMotionHistory,
				pointer_control,
				GetMotionHistorySize(),
				2);
#elif GET_ABI_MAJOR(ABI_XINPUT_VERSION) < 7
	InitPointerDeviceStruct((DevicePtr)dev,
				btmap, DIM_BUTTON,
				pointer_control,
				GetMotionHistorySize(),
				2);
#elif GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 7
	InitPointerDeviceStruct((DevicePtr)dev,
				btmap, DIM_BUTTON, btn_labels,
				pointer_control,
				GetMotionHistorySize(),
				2, axes_labels);
#else
#error "Unsupported ABI_XINPUT_VERSION"
#endif

	xf86InitValuatorAxisStruct(dev, 0,
#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 7
				   axes_labels[0],
#endif
				   mt->caps.abs[MTDEV_POSITION_X].minimum,
				   mt->caps.abs[MTDEV_POSITION_X].maximum,
#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 12
				   1, 0, 1, Absolute);
#else
				   1, 0, 1);
#endif
	xf86InitValuatorDefaults(dev, 0);
	xf86InitValuatorAxisStruct(dev, 1,
#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 7
				   axes_labels[1],
#endif
				   mt->caps.abs[MTDEV_POSITION_Y].minimum,
				   mt->caps.abs[MTDEV_POSITION_Y].maximum,
#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 12
				   1, 0, 1, Absolute);
#else
				   1, 0, 1);
#endif
	xf86InitValuatorDefaults(dev, 1);
	mprops_init(&mt->cfg, local);
	XIRegisterPropertyHandler(dev, mprops_set_property, NULL, NULL);

	return Success;
}

static int device_on(LocalDevicePtr local)
{
	struct MTouch *mt = local->private;
	local->fd = xf86OpenSerial(local->options);
	if (local->fd < 0) {
		xf86Msg(X_ERROR, "mtrack: cannot open device\n");
		return !Success;
	}
	if (mtouch_open(mt, local->fd)) {
		xf86Msg(X_ERROR, "mtrack: cannot grab device\n");
		return !Success;
	}
	xf86AddEnabledDevice(local);
	return Success;
}

static int device_off(LocalDevicePtr local)
{
	struct MTouch *mt = local->private;
	xf86RemoveEnabledDevice(local);
	if (mtouch_close(mt))
		xf86Msg(X_WARNING, "mtrack: cannot ungrab device\n");
	xf86CloseSerial(local->fd);
	return Success;
}

static int device_close(LocalDevicePtr local)
{
	return Success;
}

static void handle_gestures(LocalDevicePtr local,
			const struct Gestures* gs)
{
	static bitmask_t buttons_prev = 0U;
	int i;

	for (i = 0; i < 32; i++) {
		if (GETBIT(gs->buttons, i) == GETBIT(buttons_prev, i))
			continue;
		if (GETBIT(gs->buttons, i)) {
			xf86PostButtonEvent(local->dev, FALSE, i+1, 1, 0, 0);
#if DEBUG_DRIVER
			xf86Msg(X_INFO, "button %d down\n", i+1);
#endif
		}
		else {
			xf86PostButtonEvent(local->dev, FALSE, i+1, 0, 0, 0);
#if DEBUG_DRIVER
			xf86Msg(X_INFO, "button %d up\n", i+1);
#endif
		}
	}
	buttons_prev = gs->buttons;

	if (gs->move_dx != 0 || gs->move_dy != 0)
		xf86PostMotionEvent(local->dev, 0, 0, 2, gs->move_dx, gs->move_dy);
}

/* called for each full received packet from the touchpad */
static void read_input(LocalDevicePtr local)
{
	struct MTouch *mt = local->private;
	while (mtouch_read(mt) > 0)
		handle_gestures(local, &mt->gs);
	if (mtouch_delayed(mt))
		handle_gestures(local, &mt->gs);
}

static Bool device_control(DeviceIntPtr dev, int mode)
{
	LocalDevicePtr local = dev->public.devicePrivate;
	switch (mode) {
	case DEVICE_INIT:
		xf86Msg(X_INFO, "device control: init\n");
		return device_init(dev, local);
	case DEVICE_ON:
		xf86Msg(X_INFO, "device control: on\n");
		return device_on(local);
	case DEVICE_OFF:
		xf86Msg(X_INFO, "device control: off\n");
		return device_off(local);
	case DEVICE_CLOSE:
		xf86Msg(X_INFO, "device control: close\n");
		return device_close(local);
	default:
		xf86Msg(X_INFO, "device control: default\n");
		return BadValue;
	}
}


#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 12
static int preinit(InputDriverPtr drv, InputInfoPtr pInfo, int flags)
{
	struct MTouch *mt;

	mt = calloc(1, sizeof(*mt));
	if (!mt)
		return BadAlloc;

	pInfo->private = mt;
	pInfo->type_name = XI_TOUCHPAD;
	pInfo->device_control = device_control;
	pInfo->read_input = read_input;
	pInfo->switch_mode = 0;

    xf86CollectInputOptions(pInfo, NULL);
    xf86OptionListReport(pInfo->options);
    xf86ProcessCommonOptions(pInfo, pInfo->options);
    mconfig_configure(&mt->cfg, pInfo->options);

	return Success;
}
#else
static InputInfoPtr preinit(InputDriverPtr drv, IDevPtr dev, int flags)
{
	struct MTouch *mt;
	InputInfoPtr local = xf86AllocateInput(drv, 0);
	if (!local)
		goto error;
	mt = calloc(1, sizeof(struct MTouch));
	if (!mt)
		goto error;

	local->name = dev->identifier;
	local->type_name = XI_TOUCHPAD;
	local->device_control = device_control;
	local->read_input = read_input;
	local->private = mt;
	local->flags = XI86_POINTER_CAPABLE | XI86_SEND_DRAG_EVENTS;
	local->conf_idev = dev;

	xf86CollectInputOptions(local, NULL, NULL);
	xf86OptionListReport(local->options);
	xf86ProcessCommonOptions(local, local->options);
	mconfig_configure(&mt->cfg, local->options);

	local->flags |= XI86_CONFIGURED;
 error:
	return local;
}
#endif

static void uninit(InputDriverPtr drv, InputInfoPtr local, int flags)
{
	free(local->private);
	local->private = 0;
	xf86DeleteInput(local, 0);
}

static InputDriverRec MTRACK = {
	1,
	"mtrack",
	NULL,
	preinit,
	uninit,
	NULL,
	0
};

static XF86ModuleVersionInfo moduleVersion = {
	"mtrack",
	MODULEVENDORSTRING,
	MODINFOSTRING1,
	MODINFOSTRING2,
	XORG_VERSION_CURRENT,
	PACKAGE_VERSION_MAJOR, PACKAGE_VERSION_MINOR, PACKAGE_VERSION_PATCHLEVEL,
	ABI_CLASS_XINPUT,
	ABI_XINPUT_VERSION,
	MOD_CLASS_XINPUT,
	{0, 0, 0, 0}
};

static pointer setup(pointer module, pointer options, int *errmaj, int *errmin)
{
	xf86AddInputDriver(&MTRACK, module, 0);
	return module;
}

_X_EXPORT XF86ModuleData mtrackModuleData = {&moduleVersion, &setup, NULL };
