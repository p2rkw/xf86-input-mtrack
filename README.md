xf86-input-mtrack
=================
###### v0.5.0

An Xorg driver for multitouch trackpads. Supports any trackpad whose kernel
driver uses the slotted multitouch protocol. For more information on the
protocol see the [kernel documentation][1].

This driver is compatible with Xorg server versions 1.7 to 1.19.0 It requires
the [mtdev][4] library to operate.


License
-------

* Copyright (C) 2015-2018 Paweł Turkowski <p2rkw0@gmail.com>
* Copyright (C) 2011 Ryan Bourgeois <bluedragonx@gmail.com>
* Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>

This software is licensed under the [GPLv2][2] and is a fork of the
[xf86-input-multitouch][3] driver by Henrik Rydberg.


Building and Installing
-----------------------
This is a standard autoconf package. So:

    ./configure
    make && make install

It is likely that you will need to change system-dependent paths such as the
xorg module directory. Otherwise mtrack may be not installed in xserver search
path.
See `configure --help` for options.

To build deb package and install in system wide you will usually have to change
installation prefix to /usr like so:

    ./configure --prefix=/usr
    dpkg-buildpackage

Bug Reporting
-------------

If you found a bug, and you are going to submit it, first run

    xinput --list

then find your touchpad name/ID on the list and create report with output of

    xinput --list-props <your touchpad ID>

attached.

Configuration
-------------

The following is a minimal working InputClass section for xorg.conf:

    Section "InputClass"
        MatchIsTouchpad "on"
        Identifier      "Touchpads"
        Driver          "mtrack"
#       In case of problems enable/disable this line:
        MatchDevicePath "/dev/input/event*"
    EndSection

Configuration options may be defined inside the InputClass section to configure
the driver. See examples/ directory for example configuration files. Feel free to submit
yours, named as your laptop's model.
Available options and their defaults are as follows.

[TrackpadDisable](#TrackpadDisable)

[Sensitivity](#Sensitivity)

[FingerHigh](#FingerHigh) &nbsp;&nbsp;&nbsp;
[FingerLow](#FingerLow)

[IgnoreThumb](#IgnoreThumb) &nbsp;&nbsp;&nbsp;
[IgnorePalm](#IgnorePalm)

[DisableOnThumb](#DisableOnThumb) &nbsp;&nbsp;&nbsp;
[DisableOnPalm](#DisableOnPalm)

[ThumbSize](#ThumbSize) &nbsp;&nbsp;&nbsp; [PalmSize](#PalmSize)

[ThumbRatio](#ThumbRatio)

[EdgeSize](#EdgeSize) &nbsp;&nbsp;&nbsp;
[EdgeTopSize](#EdgeTopSize) &nbsp;&nbsp;&nbsp;
[EdgeBottomSize](#EdgeBottomSize) &nbsp;&nbsp;&nbsp;
[EdgeLeftSize](#EdgeLeftSize) &nbsp;&nbsp;&nbsp;
[EdgeRightSize](#EdgeRightSize)

[ButtonEnable](#ButtonEnable)

[ButtonIntegrated](#ButtonIntegrated)

[ButtonMoveEmulate](#ButtonMoveEmulate)

[ButtonZonesEnable](#ButtonZonesEnable)

[ButtonTouchExpire](#ButtonTouchExpire)

[ClickFinger0](#ClickFinger0) &nbsp;&nbsp;&nbsp;
[ClickFinger1](#ClickFinger1) &nbsp;&nbsp;&nbsp;
[ClickFinger2](#ClickFinger2) &nbsp;&nbsp;&nbsp;
[ClickFinger3](#ClickFinger3) &nbsp;&nbsp;&nbsp;

[TapButton1](#TapButton1) &nbsp;&nbsp;&nbsp;
[TapButton2](#TapButton2) &nbsp;&nbsp;&nbsp;
[TapButton3](#TapButton3) &nbsp;&nbsp;&nbsp;
[TapButton4](#TapButton4)

[ClickTime](#ClickTime)

[MaxTapTime](#MaxTapTime)

[MaxTapMove](#MaxTapMove)

[GestureClickTime](#GestureClickTime)

[GestureWaitTime](#GestureWaitTime)

[ScrollDistance](#ScrollDistance) &nbsp;&nbsp;&nbsp;
[ScrollClickTime](#ScrollClickTime) &nbsp;&nbsp;&nbsp;
[ScrollSensitivity](#ScrollSensitivity) &nbsp;&nbsp;&nbsp;

[ScrollUpButton](#ScrollUpButton) &nbsp;&nbsp;&nbsp;
[ScrollDownButton](#ScrollDownButton) &nbsp;&nbsp;&nbsp;
[ScrollLeftButton](#ScrollLeftButton) &nbsp;&nbsp;&nbsp;
[ScrollRightButton](#ScrollRightButton) &nbsp;&nbsp;&nbsp;

[ScrollSmooth](#ScrollSmooth) &nbsp;&nbsp;&nbsp;
[ScrollCoastDuration](#ScrollCoastDuration) &nbsp;&nbsp;&nbsp;
[ScrollCoastEnableSpeed](#ScrollCoastEnableSpeed)

[SwipeDistance](#SwipeDistance) &nbsp;&nbsp;&nbsp;
[SwipeClickTime](#SwipeClickTime) &nbsp;&nbsp;&nbsp;
[SwipeSensitivity](#SwipeSensitivity) &nbsp;&nbsp;&nbsp;

[SwipeUpButton](#SwipeUpButton) &nbsp;&nbsp;&nbsp;
[SwipeDownButton](#SwipeDownButton) &nbsp;&nbsp;&nbsp;
[SwipeLeftButton](#SwipeLeftButton) &nbsp;&nbsp;&nbsp;
[SwipeRightButton](#SwipeRightButton) &nbsp;&nbsp;&nbsp;

[Swipe4Distance](#Swipe4Distance) &nbsp;&nbsp;&nbsp;
[Swipe4ClickTime](#Swipe4ClickTime) &nbsp;&nbsp;&nbsp;
[Swipe4Sensitivity](#Swipe4Sensitivity) &nbsp;&nbsp;&nbsp;

[Swipe4UpButton](#Swipe4UpButton) &nbsp;&nbsp;&nbsp;
[Swipe4DownButton](#Swipe4DownButton) &nbsp;&nbsp;&nbsp;
[Swipe4LeftButton](#Swipe4LeftButton) &nbsp;&nbsp;&nbsp;
[Swipe4RightButton](#Swipe4RightButton) &nbsp;&nbsp;&nbsp;

[EdgeScrollDist](#EdgeScrollDist) &nbsp;&nbsp;&nbsp;
[EdgeScrollClickTime](#EdgeScrollClickTime) &nbsp;&nbsp;&nbsp;
[EdgeScrollSensitivity](#EdgeScrollSensitivity) &nbsp;&nbsp;&nbsp;

[EdgeScrollUpButton](#EdgeScrollUpButton) &nbsp;&nbsp;&nbsp;
[EdgeScrollDownButton](#EdgeScrollDownButton) &nbsp;&nbsp;&nbsp;
[EdgeScrollLeftButton](#EdgeScrollLeftButton) &nbsp;&nbsp;&nbsp;
[EdgeScrollRightButton](#EdgeScrollRightButton) &nbsp;&nbsp;&nbsp;

[ScaleDistance](#ScaleDistance) &nbsp;&nbsp;&nbsp;

[ScaleUpButton](#ScaleUpButton) &nbsp;&nbsp;&nbsp;
[ScaleDownButton](#ScaleDownButton) &nbsp;&nbsp;&nbsp;

[RotateDistance](#RotateDistance) &nbsp;&nbsp;&nbsp;

[RotateLeftButton](#RotateLeftButton) &nbsp;&nbsp;&nbsp;
[RotateRightButton](#RotateRightButton) &nbsp;&nbsp;&nbsp;

[Hold1Move1StationaryButton](#Hold1Move1StationaryButton) &nbsp;&nbsp;&nbsp;
[Hold1Move1StationaryMaxMove](#Hold1Move1StationaryMaxMove) &nbsp;&nbsp;&nbsp;

[TapDragEnable](#TapDragEnable) &nbsp;&nbsp;&nbsp;
[TapDragTime](#TapDragTime) &nbsp;&nbsp;&nbsp;
[TapDragWait](#TapDragWait) &nbsp;&nbsp;&nbsp;
[TapDragDist](#TapDragDist) &nbsp;&nbsp;&nbsp;
[TapDragLockTimeout](#TapDragLockTimeout) &nbsp;&nbsp;&nbsp;

[AxisXInvert](#AxisXInvert) &nbsp;&nbsp;&nbsp;
[AxisYInvert](#AxisYInvert) &nbsp;&nbsp;&nbsp;


<a name="TrackpadDisable"></a>
**TrackpadDisable** -
Disables trackpad touch input. A value of 0 will enable the trackpad. A value
of 1 will disable tapping and gestures but not movement. A value of 2 will
disable all input. A value of 3 will also disable physical buttons. Integer.
Default is 0.

<a name="Sensitivity"></a>
**Sensitivity** - 
Adjusts the sensitivity (movement speed) of the touchpad. This is a real number
greater than or equal to zero. Default is 1. A value of 0 will disable pointer
movement.

<a name="FingerHigh"></a>
**FingerHigh** - 
Defines the pressure at which a finger is detected as a touch. This is a
percentage represented as an integer. Default is 5.

<a name="FingerLow"></a>
**FingerLow** - 
Defines the pressure at which a finger is detected as a release. This is a
percentage represented as an integer. Default is 5.

<a name="IgnoreThumb"></a>
**IgnoreThumb** - 
Whether or not to ignore touches that are determined to be thumbs. Boolean
value. Defaults to false.

<a name="IgnorePalm"></a>
**IgnorePalm** - 
Whether or not to ignore touches that are determined to be palms. Boolean
value. Defaults to false.

<a name="DisableOnThumb"></a>
**DisableOnThumb** - 
Whether or not to disable the entire trackpad when a thumb is touching. Boolean
value. Defaults to false.

<a name="DisableOnPalm"></a>
**DisableOnPalm** - 
Whether or not to disable the entire trackpad when a palm is touching. Boolean
value. Defaults to false.

<a name="ThumbSize"></a>
**ThumbSize** - 
The minimum size of what's considered a thumb. It is expected that a thumb
will be larger than other fingers. This is represented as a percentage of the
maximum touch value and is dependent on the trackpad hardware. Integer value.
Defaults to 25.

<a name="PalmSize"></a>
**PalmSize** - 
The minimum size of what's considered a palm. Palms are expected to be very
large on the trackpad. This is represented as a percentage of the maximum touch
value and is dependent on the trackpad hardware. Integer value. Defaults to 40.

<a name="ThumbRatio"></a>
**ThumbRatio** -
The width/length ratio of what's considered a thumb. It is expected that a
thumb is longer than it is wide. This tells the driver how much longer.
Percentage represented by an integer. Defaults to 70.

<a name="EdgeSize"></a>
**EdgeSize** -
DEPRECATED.
The size of an area around the trackpad where new touches are ignored (fingers
traveling into this area from above will still be tracked). This is represented
as a percentage of the total trackpad height. Defaults to 0.
Value set here is overwriten by EdgeLeftSize, EdgeRightSize, EdgeTopSize and
EdgeBottomSize

<a name="EdgeTopSize"></a>
**EdgeTopSize** -
The size of an area at the top of the trackpad where new touches are ignored
(fingers travelling into this area from the bottom will still be tracked). This
is represented as a percentage of the total trackpad height. Defaults to 0.

<a name="EdgeBottomSize"></a>
**EdgeBottomSize** -
The size of an area at the bottom of the trackpad where new touches are ignored
(fingers travelling into this area from the top will still be tracked). This
is represented as a percentage of the total trackpad height. Defaults to 10.

<a name="EdgeLeftSize"></a>
**EdgeLeftSize** -
The size of an area at the left of the trackpad where new touches are ignored
(fingers travelling into this area from the right will still be tracked). This
is represented as a percentage of the total trackpad width. Defaults to 0.

<a name="EdgeRightSize"></a>
**EdgeRightSize** -
The size of an area at the right of the trackpad where new touches are ignored
(fingers travelling into this area from the left will still be tracked). This
is represented as a percentage of the total trackpad width. Defaults to 0.

<a name="ButtonEnable"></a>
**ButtonEnable** - 
Whether or not to enable the physical buttons on or near the trackpad. Boolean
value. Defaults to true.

<a name="ButtonIntegrated"></a>
**ButtonIntegrated** - 
Whether or not the physical buttons are integrated with the trackpad. If you
have a one-piece trackpad like on newer MacBooks, this should be set to true.
Button emulation depends on this value being correct. Boolean value. Defaults
to true.

<a name="ButtonMoveEmulate"></a>
**ButtonMoveEmulate**
Whether or not to count the moving finger when emulating button clicks.
Useful to disable if you use two hands on trackpad. Boolean value. Defaults to true.

<a name="ButtonZonesEnable"></a>
**ButtonZonesEnable** -
Whether or not to enable button zones. If button zones are enabled then the
trackpad will be split into one, two, or three vertical zones. Clicking the
integrated button in one of these zones will send the button event for
ClickFinger1, ClickFinger2, or ClickFinger3. The driver will only add zones for
those ClickFinger values that are enabled. So setting ClickFinger1 to 0 and
enabling the other two will create two zones, one for ClickFinger2 and one for
ClickFinger3. Boolean value. Defaults to false.

<a name="ButtonTouchExpire"></a>
**ButtonTouchExpire** - 
How long (in ms) to consider a touching finger as part of button emulation. A
value of 0 will not expire touches. Integer value. Defaults to 100.

<a name="ClickFinger0"></a>
**ClickFinger0** - 
Which button to emulate when no valid finger placement is touching the trackpad during a
click, as on "EdgeBottom". Integer value. A value of 0 disables one-touch emulation. Defaults to 0.

<a name="ClickFinger1"></a>
**ClickFinger1** - 
Which button to emulate when one finger is touching the trackpad during a
click. Integer value. A value of 0 disables one-touch emulation. Defaults to 1.

<a name="ClickFinger2"></a>
**ClickFinger2** - 
Which button to emulate when two fingers are touching the trackpad during a
click. Integer value. A value of 0 disabled one-touch emulation. Defaults to 2.

<a name="ClickFinger3"></a>
**ClickFinger3** - 
Which button to emulate when three fingers are touching the trackpad during a
click. Integer value. A value of 0 disabled one-touch emulation. Defaults to 3.

<a name="TapButton1"></a>
**TapButton1** - 
Which button to emulate for one-finger tapping. Integer value. A value of 0
disables one-finger tapping. Defaults to 1.

<a name="TapButton2"></a>
**TapButton2** - 
Which button to emulate for two-finger tapping. Integer value. A value of 0
disables two-finger tapping. Defaults to 3.

<a name="TapButton3"></a>
**TapButton3** - 
Which button to emulate for three-finger tapping. Integer value. A value of 0
disables three-finger tapping. Defaults to 2.

<a name="TapButton4"></a>
**TapButton4** - 
Which button to emulate for four-finger tapping. Integer value. A value of 0
disables three-finger tapping. Defaults to 0.

<a name="ClickTime"></a>
**ClickTime** - 
When tapping, how much time to hold down the emulated button. Integer value
representing milliseconds.
Integer value representing miliseconds. Defaults to 50.

<a name="MaxTapTime"></a>
**MaxTapTime** - 
The amount of time to wait for incoming touches after first one before counting
it as emulated button click.
Integer value representing milliseconds. Defaults to 120.

<a name="MaxTapMove"></a>
**MaxTapMove** - 
How far a touch is allowed to move before counting it is no longer considered a
tap. Integer value. Defaults to 400.

<a name="GestureClickTime"></a>
**GestureClickTime** - 
When a gesture triggers a click, how much time to hold down the emulated button.
Integer value representing milliseconds. Defaults to 10.

<a name="GestureWaitTime"></a>
**GestureWaitTime** - 
Touches are allowed to transition from one gesture to another. For example, you
may go from scrolling to swiping without releasing your fingers from the pad.
This value is the amount of time you must be performing the new gesture before
it is triggered. This prevents accidental touches from triggering other
gestures. Integer value representing milliseconds. Defaults to 100.

<a name="ScrollDistance"></a>
**ScrollDistance** - 
For two finger scrolling. How far you must move your fingers before a button
click is triggered. Integer value. Defaults to 150.

<a name="ScrollClickTime"></a>
**ScrollClickTime** -
For two finger scrolling. How long button triggered by scrolling 
will be hold down. A value of 0 will hold button down till end of gesture.
0 - emit button click only once pre "instance" of gesture.
Integer value representing milliseconds. Defaults to 20.

<a name="ScrollSensitivity"></a>
**ScrollSensitivity** -
For two finger scrolling. Sensitivity (movement speed) of pointer during two 
finger scrolling. A value of 0 disables pointer movement during gesture.
Integer value expressed as parts per thousand of normal sensivity.
A value of 1000 results with normal movement speed. Defaults to 0.

<a name="ScrollUpButton"></a>
**ScrollUpButton** - 
For two finger scrolling. The button that is triggered by scrolling up. Integer
value. A value of 0 disables scrolling up. Defaults to 4.

<a name="ScrollDownButton"></a>
**ScrollDownButton** - 
For two finger scrolling. The button that is triggered by scrolling down.
Integer value. A value of 0 disables scrolling down. Defaults to 5.

<a name="ScrollLeftButton"></a>
**ScrollLeftButton** - 
For two finger scrolling. The button that is triggered by scrolling left.
Integer value. A value of 0 disables scrolling left. Defaults to 6.

<a name="ScrollRightButton"></a>
**ScrollRightButton** - 
For two finger scrolling. The button that is triggered by scrolling right.
Integer value. A value of 0 disables scrolling right. Defaults to 7.

<a name="ScrollSmooth"></a>
**ScrollSmooth** -
For two finger scrolling. Whether to generate high precision scroll events.
Boolean value. Defaults to 1.
Property: "Trackpad High Smooth Scroll"

<a name="ScrollCoastDuration"></a>
**ScrollCoastDuration** -
How long after finished scrolling movement should be continued. Works only 
with smooth scrolling enabled.
Floating value representing miliseconds. Defaults to 200.0.
Property: "Trackpad Scroll Coasting"

<a name="ScrollCoastEnableSpeed"></a>
**ScrollCoastEnableSpeed** -
How fast scroll should be to enable coasting feature. Floating value. 
Defaults to 0.1.
Property: "Trackpad Scroll Coasting"

<a name="SwipeDistance"></a>
**SwipeDistance** - 
For three finger swiping. How far you must move your fingers before a button
click is triggered. Integer value. Defaults to 700.

<a name="SwipeClickTime"></a>
**SwipeClickTime** -
For three finger swiping. How long button triggered by swiping
will be hold down. Integer value representing milliseconds. Defaults to 300.

<a name="SwipeSensitivity"></a>
**SwipeSensitivity** -
For three finger scrolling. Sensitivity (movement speed) of pointer during three 
finger scrolling. A value of 0 disables pointer movement during gesture.
Integer value expressed as parts per thousand of normal sensivity.
A value of 1000 results with normal movement speed. Defaults to 0.

<a name="SwipeUpButton"></a>
**SwipeUpButton** - 
For three finger swiping. The button that is triggered by swiping up. Integer
value. A value of 0 disables swiping up. Defaults to 8.

<a name="SwipeDownButton"></a>
**SwipeDownButton** - 
For three finger swiping. The button that is triggered by swiping down. Integer
value. A value of 0 disables swiping down. Defaults to 9.

<a name="SwipeLeftButton"></a>
**SwipeLeftButton** - 
For three finger swiping. The button that is triggered by swiping left. Integer
value. A value of 0 disables swiping left. Defaults to 10.

<a name="SwipeRightButton"></a>
**SwipeRightButton** - 
For three finger swiping. The button that is triggered by swiping right. Integer
value. A value of 0 disables swiping right. Defaults to 11.

<a name="Swipe4Distance"></a>
**Swipe4Distance** - 
For four finger swiping. How far you must move your fingers before a button
click is triggered. Integer value. Defaults to 700.

<a name="Swipe4ClickTime"></a>
**Swipe4ClickTime** -
For four finger swiping. How long button triggered by swiping
will be hold down. Integer value representing milliseconds. Defaults to 300.

<a name="Swipe4Sensitivity"></a>
**Swipe4Sensitivity** -
For four finger scrolling. Sensitivity (movement speed) of pointer during four 
finger scrolling. A value of 0 disables pointer movement during gesture.
Integer value expressed as parts per thousand of normal sensivity.
A value of 1000 results with normal movement speed. Defaults to 0.

<a name="Swipe4UpButton"></a>
**Swipe4UpButton** - 
For four finger swiping. The button that is triggered by swiping up. Integer
value. A value of 0 disables swiping up. Defaults to 8.

<a name="Swipe4DownButton"></a>
**Swipe4DownButton** - 
For four finger swiping. The button that is triggered by swiping down. Integer
value. A value of 0 disables swiping down. Defaults to 9.

<a name="Swipe4LeftButton"></a>
**Swipe4LeftButton** - 
For four finger swiping. The button that is triggered by swiping left. Integer
value. A value of 0 disables swiping left. Defaults to 10.

<a name="Swipe4RightButton"></a>
**Swipe4RightButton** - 
For four finger swiping. The button that is triggered by swiping right. Integer
value. A value of 0 disables swiping right. Defaults to 11.

<a name="EdgeScrollDist"></a>
**EdgeScrollDist** -
For one finger edge scrolling. How far you must move your finger on edge before
a button click is triggered. Integer value. Defaults to ... .

<a name="EdgeScrollClickTime"></a>
**EdgeScrollClickTime** -
For one finger edge scrolling. How long button triggered by edge scrolling
will be hold down. A value of 0 will hold button down till end of gesture.
0 - emit button click only once pre "instance" of gesture.
Integer value representing milliseconds. Defaults to 20.

<a name="EdgeScrollSensitivity"></a>
**EdgeScrollSensitivity** -
For one finger edge scrolling. Sensitivity (movement speed) of pointer during one
finger scrolling. A value of 0 disables pointer movement during gesture.
Integer value expressed as parts per thousand of normal sensivity.
A value of 1000 results with normal movement speed. Defaults to 0.

<a name="EdgeScrollUpButton"></a>
**EdgeScrollUpButton** -
For one finger edge scrolling. The button that is triggered by edge scrolling up.
Integer value. A value of 0 disables scrolling up. Defaults to 4.

<a name="EdgeScrollDownButton"></a>
**EdgeScrollDownButton** -
For one finger edge scrolling. The button that is triggered by edge scrolling down.
Integer value. A value of 0 disables scrolling down. Defaults to 5.

<a name="EdgeScrollLeftButton"></a>
**EdgeScrollLeftButton** -
For one finger edge scrolling. The button that is triggered by edge scrolling left.
Integer value. A value of 0 disables scrolling left. Defaults to 6.

<a name="EdgeScrollRightButton"></a>
**EdgeScrollRightButton** -
For one finger edge scrolling. The button that is triggered by edge scrolling right.
Integer value. A value of 0 disables scrolling right. Defaults to 7.

<a name="ScaleDistance"></a>
**ScaleDistance** - 
For pinch scaling. How far you must move your fingers before a button click is
triggered. Integer value. Defaults to 150.

<a name="ScaleUpButton"></a>
**ScaleUpButton** - 
For pinch scaling. The button that is triggered by scaling up. Integer value. A
value of 0 disables scaling up. Defaults to 12.

<a name="ScaleDownButton"></a>
**ScaleDownButton** - 
For pinch scaling. The button that is triggered by scaling down. Integer value.
A value of 0 disables scaling down. Defaults to 13.

<a name="RotateDistance"></a>
**RotateDistance** - 
For two finger rotation. How far you must move your fingers before a button
click is triggered. Integer value. Defaults to 150.

<a name="RotateLeftButton"></a>
**RotateLeftButton** - 
For two finger rotation. The button that is triggered by rotating left. Integer
value. A value of 0 disables rotation left. Defaults to 14.

<a name="RotateRightButton"></a>
**RotateRightButton** - 
For two finger rotation. The button that is triggered by rotating right. Integer
value. A value of 0 disables rotation right. Defaults to 15.

<a name="Hold1Move1StationaryButton"></a>
**Hold1Move1StationaryButton** - 
For two finger hold-and-move functionality. The button that is triggered by 
holding one finger and moving another one. Integer value. 
A value of 0 disables hold-and-move. Value of 0 disables this functionality.
Defaults to 1.

<a name="Hold1Move1StationaryMaxMove"></a>
**Hold1Move1StationaryMaxMove** -
For two finger hold-and-move functionality. Fow far stationary finger can be
moved berfore gesture invalidation. Integer value.

<a name="TapDragEnable"></a>
**TapDragEnable** - 
Whether or not to enable tap-to-drag functionality. Boolean value. Defaults to
true.

<a name="TapDragTime"></a>
**TapDragTime** - 
The tap-to-drag timeout. This is how long the driver will wait after a single
tap for a movement event before sending the click. Integer value representing
milliseconds. Defaults to 350.

<a name="TapDragWait"></a>
**TapDragWait** - 
How long after detecting movement to trigger a button down event. During this
time pointer movement will be disabled. Increase this value if you find you're
draggin when you don't wish it. Integer value representing milliseconds.
Defaults to 40.

<a name="TapDragDist"></a>
**TapDragDist** - 
How far the finger is allowed to move during drag wait time. If the finger
moves farther than this distance during the wait time then dragging will be
canceled and pointer movement will resume. Integer value. Defaults to 200.

<a name="TapDragLockTimeout"></a>
**TapDragLockTimeout** -
This is how long the driver will wait after initial drag in 'drag ready' state
in which it will be able to resume previous drag without additional `up`, `down`
sequence.
Value of 0 disables this functionality.
Values less than zero will make mtrack requre additional tap to finish drag
by sending `button up`.
Integer value representing milliseconds. Defaults to 500.

<a name="AxisXInvert"></a>
**AxisXInvert** - 
Whether or not to invert the X axis. Boolean value. Defaults to false.

<a name="AxisYInvert"></a>
**AxisYInvert** - 
Whether or not to invert the Y axis. Boolean value. Defaults to false.

Tips
-------------
##### Swipe to drag
To setup swipe to drag functionality you have to choose which swipe gesture (Scroll, Swipe, Swipe4)
will be used for dragging.
Example configuration for three finger drag:
```
    Option "SwipeDistance" "1"
    Option "SwipeLeftButton" "1"
    Option "SwipeRightButton" "1"
    Option "SwipeUpButton" "1"
    Option "SwipeDownButton" "1"
    Option "SwipeClickTime" "0"
    Option "SwipeSensitivity" "1000"
```
This will enable draging with three fingers. Change sensitivity for faster/slower movements.
Scroll, and Swipe4 are also supported.

##### Hold and move
Hold down one finger in place to initiate hold-and-move gesture.
Then move another finger to drag configured button.
Gesture will last as long as fist finger (a.k.a. stationary finger) will
be held down in place.

Increase TapMaxDist to give stationary finger more freedom.
Set Hold1Move1Button to 0 to disable, set to other value to send button other 
than "1".

##### Persistent dragging
If you're using lot of tools that require dragging you can make it a little bit
easier by enabling persistent tap-to-drag:
```
    Option "TapDragLockTimeout" "-1"
```
With that change you will have to perform additional tap when dragging with
tap-to-drag. Other positive values will let you continue yor drag within
specified time.

##### Enabling soft button
If you enable edge restriction an you still want the integrated button to click inside that edge, you must enable ClickFinger0.
```
    Option "ClickFinger0" "1"
```
You can also set another button instead of "1".

[1]: http://www.kernel.org/doc/Documentation/input/multi-touch-protocol.txt     "Kernel Multitouch Protocol"
[2]: http://www.gnu.org/licenses/gpl-2.0.html                                   "GNU General Public License, version 2"
[3]: http://bitmath.org/code/multitouch/                                        "xf86-input-multitouch website"
[4]: http://bitmath.org/code/mtdev/                                             "mtdev library website"
