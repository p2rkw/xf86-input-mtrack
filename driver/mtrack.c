/***************************************************************************
 *
 * Multitouch X driver
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
 * Copyright (C) 2011 Ryan Bourgeois <bluedragonx@gmail.com>
 * Copyright (C) 2015-2018 Paweł Turkowski <p2rkw0@gmail.com>
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
#include "capabilities.h"
#include "os.h"
#include "trig.h" /* xorg/os.h for timers */

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

#define NUM_AXES 4

#ifdef DEBUG_DRIVER
# define LOG_DEBUG_DRIVER LOG_DEBUG
#else
# define LOG_DEBUG_DRIVER LOG_DISABLED
#endif

#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 12
typedef InputInfoPtr LocalDevicePtr;
#endif

/* button mapping simplified */
#define PROPMAP(m, x, y) m[x] = XIGetKnownProperty(y)

static void pointer_control(DeviceIntPtr dev, PtrCtrl *ctrl)
{
	LOG_DEBUG_DRIVER("pointer_control\n");
}

#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 7
static void init_axes_labels(Atom map[NUM_AXES])
{
	memset(map, 0, NUM_AXES * sizeof(Atom));
	PROPMAP(map, 0, AXIS_LABEL_PROP_REL_X);
	PROPMAP(map, 1, AXIS_LABEL_PROP_REL_Y);
	PROPMAP(map, 2, AXIS_LABEL_PROP_REL_VSCROLL);
	PROPMAP(map, 3, AXIS_LABEL_PROP_REL_HSCROLL);
}

static void init_button_labels(Atom map[DIM_BUTTON])
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

/**
 * How to handle multitouch: http://www.x.org/wiki/Development/Documentation/Multitouch/
 * Howto about xinput: http://www.x.org/wiki/Development/Documentation/XorgInputHOWTO/
 * Example usage: https://gitlab.com/at-home-modifier/at-home-modifier-evdev/commit/d171b3d9194581cb6ed59dbe45d6cbf009dc0eaa?view=parallel
 * Patch were smooth scrolling were introduced: https://lists.x.org/archives/xorg-devel/2011-September/025835.html
 * @param dev
 * @param axnum
 * @param label
 * @param min
 * @param max
 * @param resolution
 */
static void init_axle_absolute(DeviceIntPtr dev, int axnum, Atom* label)
{
	/* Inform server about reported range of axis values. */
	xf86InitValuatorAxisStruct(dev, axnum,
#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 7
		*label,
#endif
		/* minval, maxval */ NO_AXIS_LIMITS, NO_AXIS_LIMITS,
		/*resolution*/ 1,
		/*min res*/ 0,
		/*max res*/ 1
#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 12
		/* mode */, Absolute
#endif
	);
	xf86InitValuatorDefaults(dev, axnum);
}

static void init_axle_relative(DeviceIntPtr dev, int axnum, Atom* label)
{
	xf86InitValuatorAxisStruct(dev, axnum, *label, NO_AXIS_LIMITS, NO_AXIS_LIMITS, 0, 0, 0, Relative);
	xf86InitValuatorDefaults(dev, axnum);
}

static int device_init(DeviceIntPtr dev, LocalDevicePtr local)
{
	struct MTouch *mt = local->private;
	unsigned char btmap[DIM_BUTTON + 1] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
	};
#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 7
	Atom axes_labels[NUM_AXES], btn_labels[DIM_BUTTON];
	init_axes_labels(axes_labels);
	init_button_labels(btn_labels);
#endif

	local->fd = xf86OpenSerial(local->options);
	if (local->fd < 0) {
		LOG_ERROR("cannot open device %s\n", local->name);
		return !Success;
	}
	if (mtouch_configure(mt, local->fd)) {
		LOG_ERROR("cannot configure device %s\n", local->name);
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
				NUM_AXES, axes_labels);
#else
#error "Unsupported ABI_XINPUT_VERSION"
#endif

	init_axle_absolute(dev, 0, &axes_labels[0]);
	init_axle_absolute(dev, 1, &axes_labels[1]);

	init_axle_relative(dev, 2, &axes_labels[2]);
	init_axle_relative(dev, 3, &axes_labels[3]);

	/* Always set valuator distance to 1.0 because it reported values will be
	 * adjusted accordingly by smooth scroll trigger.
	 */
	SetScrollValuator(dev, 2, SCROLL_TYPE_VERTICAL, 1.0, SCROLL_FLAG_PREFERRED);
	SetScrollValuator(dev, 3, SCROLL_TYPE_HORIZONTAL, 1.0, SCROLL_FLAG_NONE);

	mprops_init(&mt->cfg, local);

	XIRegisterPropertyHandler(dev, mprops_set_property, NULL, NULL);

	mt->timer = NULL; /* allocated later */
	mt->timer_kind = 0;
	mt->absolute_mode = FALSE;
	return Success;
}

static int device_on(LocalDevicePtr local)
{
	struct MTouch *mt = local->private;
	local->fd = xf86OpenSerial(local->options);
	if (local->fd < 0) {
		LOG_ERROR("cannot open device\n");
		return !Success;
	}
	if (mtouch_open(mt, local->fd)) {
		LOG_ERROR("cannot grab device\n");
		return !Success;
	}
	/*
	 *  xf86AddEnabledDevice() will add our device's fd to the list of SIGIO handlers.
	 *  When a SIGIO occurs, our read_input will get called.
	 */
	xf86AddEnabledDevice(local);
	if(mt->timer != NULL)
		TimerFree(mt->timer);	// release any existing timer
	mt->timer = NULL;
	mt->timer_kind = 0;
	return Success;
}

static int device_off(LocalDevicePtr local)
{
	struct MTouch *mt = local->private;
	xf86RemoveEnabledDevice(local);
	if (mtouch_close(mt))
		LOG_WARNING("cannot ungrab device\n");
	xf86CloseSerial(local->fd);
	if(mt->timer != NULL)
		TimerFree(mt->timer);	// release any existing timer
	mt->timer = NULL;
	mt->timer_kind = 0;
	return Success;
}

static int device_close(LocalDevicePtr local)
{
	return Success;
}

static void post_button(struct MTouch* mt, int button, int new_state);

void mt_timer_start(struct MTouch *mt, int kind)
{
	struct Gestures* gs = &mt->gs;
	mstime_t timeout; /*< Set this variable to required timeout. */

	if(kind == mt->timer_kind){
		LOG_DEBUG_DRIVER("Timer %i already started\n", kind);
		return;
	}

	mt_timer_stop(mt);

	switch(kind){
	case MT_TIMER_DELAYED_BUTTON:
	{
		struct timeval delta;

		timersub(&gs->button_delayed_time, &gs->time, &delta);
		timeout = timertoms(&delta);
		break;
	}

	case MT_TIMER_COASTING:
		gs->move_dx = gs->move_dy = 0.0;
		gs->move_type = GS_NONE;
		gs->coasting_duration_left = mt->cfg.scroll_coast.duration - 1;
		timeout = mt->cfg.scroll_coast.tick_ms;
		break;

	case MT_TIMER_ANY:
	case MT_TIMER_NONE:
		return; /* do nothing */

	default:
		LOG_INFO("Unimplemented timer, ID: %i\n", kind);
		return;
	}

	LOG_DEBUG_DRIVER("Start timer, ID: %i with timeout: %llu\n", kind, timeout);
	mt->timer = TimerSet(mt->timer, 0, timeout, mt_timer_callback, mt);
	mt->timer_kind = kind;
}

void mt_timer_stop(struct MTouch *mt)
{
	struct Gestures* gs = &mt->gs;

	LOG_DEBUG_DRIVER("Stop timer, ID: %i\n", mt->timer_kind);
	TimerCancel(mt->timer);

	switch(mt->timer_kind){
	case MT_TIMER_DELAYED_BUTTON:
	{
		int button;
		button = trigger_delayed_button_uncond(gs);
		post_button(mt, button, GETBIT(gs->buttons, button));
		gs->move_dx = gs->move_dy = 0.0;
		gs->move_type = GS_NONE;
		break;
	}

	case MT_TIMER_COASTING:
		mt->gs.scroll_speed_x = mt->gs.scroll_speed_y = 0.0;
		break;

	case MT_TIMER_ANY:
	case MT_TIMER_NONE:
		break; /* do nothing */

	default:
		LOG_INFO("Unimplemented timer, ID: %i\n", mt->timer_kind);
		break;
	}
	mt->timer_kind = MT_TIMER_NONE;
}

CARD32 mt_timer_callback(OsTimerPtr timer, CARD32 time, void *arg)
{
	struct MTouch *mt = arg;
	struct Gestures* gs = &mt->gs;
	ValuatorMask* mask = mt->valuator_mask;

	switch(mt->timer_kind){
	case MT_TIMER_DELAYED_BUTTON:
	{
		int button;
		button = trigger_delayed_button_uncond(gs);
		post_button(mt, button, GETBIT(gs->buttons, button));
		gs->move_dx = gs->move_dy = 0.0;
		gs->move_type = GS_NONE;

		mt_timer_stop(mt);
		break;
	}

	case MT_TIMER_COASTING:
	{
		if (gs->coasting_duration_left <= 0)
			return mt_timer_stop(mt), 0;

		mstime_t delta_ms;
		double coasting_progress;

		valuator_mask_zero(mask);
		delta_ms = mt->cfg.scroll_coast.tick_ms;
		coasting_progress = gs->coasting_duration_left / (double)mt->cfg.scroll_coast.duration;

		valuator_mask_set_double(mask, 2, gs->scroll_speed_y * coasting_progress * delta_ms);
		valuator_mask_set_double(mask, 3, gs->scroll_speed_x * coasting_progress * delta_ms);
		gs->coasting_duration_left -= delta_ms;

		xf86PostMotionEventM(mt->local_dev, Relative, mask);

		mt_timer_continue(mt, mt->cfg.scroll_coast.tick_ms);
		break;
	}

	case MT_TIMER_ANY:
	case MT_TIMER_NONE:
		return 0; /* do nothing */

	default:
		LOG_INFO("Unimplemented timer id: %i\n", mt->timer_kind);
		break;
	}
	return 0;
}

void mt_timer_start_or_stop(struct MTouch *mt, int kind)
{
	if(kind < 0)
		mt_timer_stop_if(mt, -kind);
	else if(kind > 0)
		mt_timer_start(mt, kind);
}

void mt_timer_stop_if(struct MTouch *mt, int kind)
{
	if(mt->timer_kind == kind || kind == MT_TIMER_ANY || kind == -MT_TIMER_ANY)
		mt_timer_stop(mt);
}

void mt_timer_continue(struct MTouch *mt, mstime_t timeout)
{
	LOG_DEBUG_DRIVER("Continue timer, ID: %i with timeout: %llu\n", mt->timer_kind, timeout);
	mt->timer = TimerSet(mt->timer, 0, timeout, mt_timer_callback, mt);
}

/**
 * @param mt
 * @return
 */
static int can_start_coasting(struct MTouch *mt)
{
	struct Gestures* gs = &mt->gs;
	struct MTState* ms = &mt->state;
	int i;

	if(mt->cfg.scroll_smooth && mt->cfg.scroll_coast.duration > 0 &&
		 mt->cfg.scroll_coast.duration > 0 &&
		 hypot_cmpf(gs->scroll_speed_x, gs->scroll_speed_y, mt->cfg.scroll_coast.min_speed) == 1)
	{
		foreach_bit(i, ms->touch_used) {
			if (GETBIT(ms->touch[i].flags, MT_INVALID) ||
					GETBIT(ms->touch[i].flags, MT_BUTTON) ||
					GETBIT(ms->touch[i].flags, MT_TAP)
				 )
				continue;
			if(GETBIT(mt->state.touch[i].flags, MT_RELEASED))
				return 1;
		} /* foreach */
	}
	return 0;
}

static bitmask_t buttons_posted = 0U;
static void post_gestures(struct MTouch *mt)
{
	struct Gestures* gs = &mt->gs;
	int i;
	const double delta = timertoms(&gs->dt);

	if(mt->absolute_mode == FALSE){
		if (mt->cfg.scroll_smooth){
			/* Never post these buttons in smooth mode. */
			CLEARBIT(gs->buttons, MT_BUTTON_WHEEL_UP);
			CLEARBIT(gs->buttons, MT_BUTTON_WHEEL_DOWN);
			CLEARBIT(gs->buttons, MT_BUTTON_HWHEEL_LEFT);
			CLEARBIT(gs->buttons, MT_BUTTON_HWHEEL_RIGHT);
		}
	}

	for (i = 0; i < 32; i++) {
		post_button(mt, i, GETBIT(gs->buttons, i));
	}

	if(mt->absolute_mode == FALSE){
		if (mt->cfg.scroll_smooth){
			ValuatorMask* mask;	mask = mt->valuator_mask;
			valuator_mask_zero(mask);

			if (gs->move_dx)
				valuator_mask_set_double(mask, 0, gs->move_dx);
			if (gs->move_dy)
				valuator_mask_set_double(mask, 1, gs->move_dy);
			gs->move_dx = gs->move_dy = 0.0;

			/* if is any swipe */
			if(gs->move_type == GS_SWIPE2 || gs->move_type == GS_SWIPE3 || gs->move_type == GS_SWIPE4){
				if(gs->scroll_speed_valid)
				{
					/* Delta encoded in scroll_speed. */
					valuator_mask_set_double(mask, 2, gs->scroll_speed_y);
					valuator_mask_set_double(mask, 3, gs->scroll_speed_x);
				}
				gs->scroll_speed_valid = 0;

				/* Start coasting here if needed. */
				if(delta != 0.0 && can_start_coasting(mt)){
					/* Remove delta component from scroll_speed. */
					gs->scroll_speed_x /= delta;
					gs->scroll_speed_y /= delta;
					mt_timer_start(mt, MT_TIMER_COASTING);
				}
			}

			xf86PostMotionEventM(mt->local_dev, Relative, mask);
		} /* if smooth scroll */
		else{
			// mt->absolute_mode == false
			if (gs->move_dx != 0.0 || gs->move_dy != 0.0)
				xf86PostMotionEvent(mt->local_dev, 0, 0, 2, (int)gs->move_dx, (int)gs->move_dy);
		}
	}
	else{
		/* Give the HW coordinates to Xserver as absolute coordinates, these coordinates
		 * are not scaled, this is oke if the touchscreen has the same resolution as the display.
		 */
		xf86PostMotionEvent(mt->local_dev, 1, 0, 2,
			mt->state.touch[0].x + get_cap_xmid(&mt->caps),
			mt->state.touch[0].y + get_cap_ymid(&mt->caps));
	}
}

static void post_button(struct MTouch* mt, int button, int new_state)
{
	struct Gestures* gs = &mt->gs;

	if (new_state == GETBIT(buttons_posted, button))
		return;
	if (new_state) {
		xf86PostButtonEvent(mt->local_dev, FALSE, button+1, 1, 0, 0);
		LOG_DEBUG_DRIVER("button %d down\n", button+1);
	}
	else {
		xf86PostButtonEvent(mt->local_dev, FALSE, button+1, 0, 0, 0);
		LOG_DEBUG_DRIVER("button %d up\n", button+1);
	}
	MODBIT(gs->buttons, button, new_state);
	MODBIT(buttons_posted, button, new_state);
}

/*
 *  Called for each full received packet from the touchpad.
 * Any xf86 input event generated by int this callback function fill be queued by
 * xorg server, and fired when control return from this function.
 * So to fire event as early as possible this function should return quickly.
 * For delayed events we can't simply wait in this function, because it will delay
 * all events generated by 'post_gestures'.
 * Moreover we don't know when next input event will occur, so to guarantee proper
 * timing I have to use timer.
 *
 * More on input event processing:
 * http://www.x.org/wiki/Development/Documentation/InputEventProcessing/
 *
 * HowTo:
 * https://www.x.org/wiki/Development/Documentation/XorgInputHOWTO/
 */
static void read_input(LocalDevicePtr local)
{
	struct MTouch *mt = local->private;
	int timer_kind;

	mt->local_dev = local->dev;
	while (mtouch_read(mt) > 0){
		timer_kind = mtouch_delayed(mt);
		mt_timer_start_or_stop(mt, timer_kind);
		post_gestures(mt);
	}
}

static int switch_mode(ClientPtr client, DeviceIntPtr dev, int mode)
{
	LocalDevicePtr local = dev->public.devicePrivate;
	struct MTouch *mt = local->private;

	switch (mode) {
	case Absolute:
		mt->absolute_mode = TRUE;
		LOG_INFO("switch_mode: switing to absolute mode\n");
		break;
	case Relative:
		mt->absolute_mode = FALSE;
		LOG_INFO("switch_mode: switing to relative mode\n");
		break;
	default:
		return XI_BadMode;
	}
	return Success;
}


static Bool device_control(DeviceIntPtr dev, int mode)
{
	LocalDevicePtr local = dev->public.devicePrivate;
	switch (mode) {
	case DEVICE_INIT:
		LOG_INFO("device control: init\n");
		return device_init(dev, local);
	case DEVICE_ON:
		LOG_INFO("device control: on\n");
		return device_on(local);
	case DEVICE_OFF:
		LOG_INFO("device control: off\n");
		return device_off(local);
	case DEVICE_CLOSE:
		LOG_INFO("device control: close\n");
		return device_close(local);
	default:
		LOG_INFO("device control: default\n");
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
	pInfo->switch_mode = switch_mode;

	xf86CollectInputOptions(pInfo, NULL);
	xf86OptionListReport(pInfo->options);
	xf86ProcessCommonOptions(pInfo, pInfo->options);
	mconfig_configure(&mt->cfg, pInfo->options); // set the defaults
	mt->valuator_mask = valuator_mask_new(4);

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
	local->switch_mode = switch_mode;
	local->private = mt;
	local->flags = XI86_POINTER_CAPABLE | XI86_SEND_DRAG_EVENTS;
	local->conf_idev = dev;

	xf86CollectInputOptions(local, NULL, NULL);
	xf86OptionListReport(local->options);
	xf86ProcessCommonOptions(local, local->options);
	mconfig_configure(&mt->cfg, local->options);
	mt->vm = NULL;

	local->flags |= XI86_CONFIGURED;
 error:
	return local;
}
#endif

static void uninit(InputDriverPtr drv, InputInfoPtr local, int flags)
{
	struct MTouch *mt = local->private;

	if (mt->valuator_mask)
		valuator_mask_free(&mt->valuator_mask);
	free(local->private);
	local->private = 0;
	xf86DeleteInput(local, 0);
}

/* About xorg drivers, modules:
 * http://www.x.org/wiki/Development/Documentation/XorgInputHOWTO/
 */
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
