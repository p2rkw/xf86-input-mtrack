xf86-input-mtrack
=================
###### v0.4.1

An Xorg driver for multitouch trackpads. Supports any trackpad whose kernel
driver uses the slotted multitouch protocol. For more information on the
protocol see the [kernel documentation][1].

This driver is compatible with Xorg server versions 1.7 to 1.19.0 It requires
the [mtdev][4] library to operate.


License
-------

* Copyright (C) 2017 Paweł Turkowski <p2rkw0@gmail.com>
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
    EndSection

Configuration options may be defined inside the InputClass section to configure
the driver. See examples/ directory for example configuration files. Feel free to submit
yours, named as your laptop's model.
Available options and their defaults are as follows.

**TrackpadDisable** -
Disables trackpad touch input. A value of 0 will enable the trackpad. A value
of 1 will disable tapping and gestures but not movement. A value of 2 will
disable all input. A value of 3 will also disable physical buttons. Integer.
Default is 0.

**Sensitivity** - 
Adjusts the sensitivity (movement speed) of the touchpad. This is a real number
greater than or equal to zero. Default is 1. A value of 0 will disable pointer
movement.

**FingerHigh** - 
Defines the pressure at which a finger is detected as a touch. This is a
percentage represented as an integer. Default is 5.

**FingerLow** - 
Defines the pressure at which a finger is detected as a release. This is a
percentage represented as an integer. Default is 5.

**IgnoreThumb** - 
Whether or not to ignore touches that are determined to be thumbs. Boolean
value. Defaults to false.

**IgnorePalm** - 
Whether or not to ignore touches that are determined to be palms. Boolean
value. Defaults to false.

**DisableOnThumb** - 
Whether or not to disable the entire trackpad when a thumb is touching. Boolean
value. Defaults to false.

**DisableOnPalm** - 
Whether or not to disable the entire trackpad when a palm is touching. Boolean
value. Defaults to false.

**ThumbRatio** - 
The width/length ratio of what's considered a thumb. It is expected that a
thumb is longer than it is wide. This tells the driver how much longer.
Percentage represented by an integer. Defaults to 70.

**ThumbSize** - 
The minimum size of what's considered a thumb. It is expected that a thumb
will be larger than other fingers. This is represented as a percentage of the
maximum touch value and is dependent on the trackpad hardware. Integer value.
Defaults to 25.

**PalmSize** - 
The minimum size of what's considered a palm. Palms are expected to be very
large on the trackpad. This is represented as a percentage of the maximum touch
value and is dependent on the trackpad hardware. Integer value. Defaults to 40.

**EdgeSize** -
DEPRECATED.
The size of an area around the trackpad where new touches are ignored (fingers
traveling into this area from above will still be tracked). This is represented
as a percentage of the total trackpad height. Defaults to 0.
Value set here is overwriten by EdgeLeftSize, EdgeRightSize, EdgeTopSize and
EdgeBottomSize

**EdgeTopSize** -
The size of an area at the top of the trackpad where new touches are ignored
(fingers travelling into this area from the bottom will still be tracked). This
is represented as a percentage of the total trackpad height. Defaults to 0.

**EdgeBottomSize** -
The size of an area at the bottom of the trackpad where new touches are ignored
(fingers travelling into this area from the top will still be tracked). This
is represented as a percentage of the total trackpad height. Defaults to 10.

**EdgeLeftSize** -
The size of an area at the left of the trackpad where new touches are ignored
(fingers travelling into this area from the right will still be tracked). This
is represented as a percentage of the total trackpad width. Defaults to 0.

**EdgeRightSize** -
The size of an area at the right of the trackpad where new touches are ignored
(fingers travelling into this area from the left will still be tracked). This
is represented as a percentage of the total trackpad width. Defaults to 0.

**ButtonEnable** - 
Whether or not to enable the physical buttons on or near the trackpad. Boolean
value. Defaults to true.

**ButtonIntegrated** - 
Whether or not the physical buttons are integrated with the trackpad. If you
have a one-piece trackpad like on newer MacBooks, this should be set to true.
Button emulation depends on this value being correct. Boolean value. Defaults
to true.

**ButtonMoveEmulate**
Whether or not to count the moving finger when emulating button clicks.
Useful to disable if you use two hands on trackpad. Boolean value. Defaults to true.

**ButtonZonesEnable** -
Whether or not to enable button zones. If button zones are enabled then the
trackpad will be split into one, two, or three vertical zones. Clicking the
integrated button in one of these zones will send the button event for
ClickFinger1, ClickFinger2, or ClickFinger3. The driver will only add zones for
those ClickFinger values that are enabled. So setting ClickFinger1 to 0 and
enabling the other two will create two zones, one for ClickFinger2 and one for
ClickFinger3. Boolean value. Defaults to false.

**ButtonTouchExpire** - 
How long (in ms) to consider a touching finger as part of button emulation. A
value of 0 will not expire touches. Integer value. Defaults to 100.

**ClickFinger1** - 
Which button to emulate when one finger is touching the trackpad during a
click. Integer value. A value of 0 disables one-touch emulation. Defaults to 3.

**ClickFinger2** - 
Which button to emulate when two fingers are touching the trackpad during a
click. Integer value. A value of 0 disabled one-touch emulation. Defaults to 2.

**ClickFinger3** - 
Which button to emulate when three fingers are touching the trackpad during a
click. Integer value. A value of 0 disabled one-touch emulation. Defaults to 0.

**TapButton1** - 
Which button to emulate for one-finger tapping. Integer value. A value of 0
disables one-finger tapping. Defaults to 1.

**TapButton2** - 
Which button to emulate for two-finger tapping. Integer value. A value of 0
disables two-finger tapping. Defaults to 3.

**TapButton3** - 
Which button to emulate for three-finger tapping. Integer value. A value of 0
disables three-finger tapping. Defaults to 2.

**TapButton4** - 
Which button to emulate for four-finger tapping. Integer value. A value of 0
disables three-finger tapping. Defaults to 0.

**ClickTime** - 
When tapping, how much time to hold down the emulated button. Integer value
representing milliseconds.
Integer value representing miliseconds. Defaults to 50.

**MaxTapTime** - 
The amount of time to wait for incoming touches after first one before counting
it as emulated button click.
Integer value representing milliseconds. Defaults to 120.

**MaxTapMove** - 
How far a touch is allowed to move before counting it is no longer considered a
tap. Integer value. Defaults to 400.

**GestureClickTime** - 
When a gesture triggers a click, how much time to hold down the emulated button.
Integer value representing milliseconds. Defaults to 10.

**GestureWaitTime** - 
Touches are allowed to transition from one gesture to another. For example, you
may go from scrolling to swiping without releasing your fingers from the pad.
This value is the amount of time you must be performing the new gesture before
it is triggered. This prevents accidental touches from triggering other
gestures. Integer value representing milliseconds. Defaults to 100.

**ScrollDistance** - 
For two finger scrolling. How far you must move your fingers before a button
click is triggered. Integer value. Defaults to 150.

**ScrollClickTime** -
For two finger scrolling. How long button triggered by scrolling 
will be hold down. A value of 0 will hold button down till end of gesture.
0 - emit button click only once pre "instance" of gesture.
Integer value representing milliseconds. Defaults to 20.

**ScrollSensitivity** -
For two finger scrolling. Sensitivity (movement speed) of pointer during two 
finger scrolling. A value of 0 disables pointer movement during gesture.
Integer value expressed as parts per thousand of normal sensivity.
A value of 1000 results with normal movement speed. Defaults to 0.

**ScrollUpButton** - 
For two finger scrolling. The button that is triggered by scrolling up. Integer
value. A value of 0 disables scrolling up. Defaults to 4.

**ScrollDownButton** - 
For two finger scrolling. The button that is triggered by scrolling down.
Integer value. A value of 0 disables scrolling down. Defaults to 5.

**ScrollLeftButton** - 
For two finger scrolling. The button that is triggered by scrolling left.
Integer value. A value of 0 disables scrolling left. Defaults to 6.

**ScrollRightButton** - 
For two finger scrolling. The button that is triggered by scrolling right.
Integer value. A value of 0 disables scrolling right. Defaults to 7.

**ScrollSmooth** -
For two finger scrolling. Whether to generate high precision scroll events.
Boolean value. Defaults to 1.
Property: "Trackpad High Smooth Scroll"

**ScrollCoastDuration** -
How long after finished scrolling movement should be continued. Works only 
with smooth scrolling enabled.
Floating value representing miliseconds. Defaults to 200.0.
Property: "Trackpad Scroll Coasting"

**ScrollCoastEnableSpeed** -
How fast scroll should be to enable coasting feature. Floating value. 
Defaults to 0.1.
Property: "Trackpad Scroll Coasting"

**SwipeDistance** - 
For three finger swiping. How far you must move your fingers before a button
click is triggered. Integer value. Defaults to 700.

**SwipeClickTime** -
For three finger swiping. How long button triggered by swiping
will be hold down. Integer value representing milliseconds. Defaults to 300.

**SwipeSensitivity** -
For three finger scrolling. Sensitivity (movement speed) of pointer during three 
finger scrolling. A value of 0 disables pointer movement during gesture.
Integer value expressed as parts per thousand of normal sensivity.
A value of 1000 results with normal movement speed. Defaults to 0.

**SwipeUpButton** - 
For three finger swiping. The button that is triggered by swiping up. Integer
value. A value of 0 disables swiping up. Defaults to 8.

**SwipeDownButton** - 
For three finger swiping. The button that is triggered by swiping down. Integer
value. A value of 0 disables swiping down. Defaults to 9.

**SwipeLeftButton** - 
For three finger swiping. The button that is triggered by swiping left. Integer
value. A value of 0 disables swiping left. Defaults to 10.

**SwipeRightButton** - 
For three finger swiping. The button that is triggered by swiping right. Integer
value. A value of 0 disables swiping right. Defaults to 11.

**Swipe4Distance** - 
For four finger swiping. How far you must move your fingers before a button
click is triggered. Integer value. Defaults to 700.

**Swipe4ClickTime** -
For four finger swiping. How long button triggered by swiping
will be hold down. Integer value representing milliseconds. Defaults to 300.

**Swipe4Sensitivity** -
For four finger scrolling. Sensitivity (movement speed) of pointer during four 
finger scrolling. A value of 0 disables pointer movement during gesture.
Integer value expressed as parts per thousand of normal sensivity.
A value of 1000 results with normal movement speed. Defaults to 0.

**Swipe4UpButton** - 
For four finger swiping. The button that is triggered by swiping up. Integer
value. A value of 0 disables swiping up. Defaults to 8.

**Swipe4DownButton** - 
For four finger swiping. The button that is triggered by swiping down. Integer
value. A value of 0 disables swiping down. Defaults to 9.

**Swipe4LeftButton** - 
For four finger swiping. The button that is triggered by swiping left. Integer
value. A value of 0 disables swiping left. Defaults to 10.

**Swipe4RightButton** - 
For four finger swiping. The button that is triggered by swiping right. Integer
value. A value of 0 disables swiping right. Defaults to 11.

**EdgeScrollDist** -
For one finger edge scrolling. How far you must move your finger on edge before
a button click is triggered. Integer value. Defaults to ... .

**EdgeScrollClickTime** -
For one finger edge scrolling. How long button triggered by edge scrolling
will be hold down. A value of 0 will hold button down till end of gesture.
0 - emit button click only once pre "instance" of gesture.
Integer value representing milliseconds. Defaults to 20.

**EdgeScrollSensitivity** -
For one finger edge scrolling. Sensitivity (movement speed) of pointer during one
finger scrolling. A value of 0 disables pointer movement during gesture.
Integer value expressed as parts per thousand of normal sensivity.
A value of 1000 results with normal movement speed. Defaults to 0.

**EdgeScrollUpButton** -
For one finger edge scrolling. The button that is triggered by edge scrolling up.
Integer value. A value of 0 disables scrolling up. Defaults to 4.

**EdgeScrollDownButton** -
For one finger edge scrolling. The button that is triggered by edge scrolling down.
Integer value. A value of 0 disables scrolling down. Defaults to 5.

**EdgeScrollLeftButton** -
For one finger edge scrolling. The button that is triggered by edge scrolling left.
Integer value. A value of 0 disables scrolling left. Defaults to 6.

**EdgeScrollRightButton** -
For one finger edge scrolling. The button that is triggered by edge scrolling right.
Integer value. A value of 0 disables scrolling right. Defaults to 7.

**ScaleDistance** - 
For pinch scaling. How far you must move your fingers before a button click is
triggered. Integer value. Defaults to 150.

**ScaleUpButton** - 
For pinch scaling. The button that is triggered by scaling up. Integer value. A
value of 0 disables scaling up. Defaults to 12.

**ScaleDownButton** - 
For pinch scaling. The button that is triggered by scaling down. Integer value.
A value of 0 disables scaling down. Defaults to 13.

**RotateDistance** - 
For two finger rotation. How far you must move your fingers before a button
click is triggered. Integer value. Defaults to 150.

**RotateLeftButton** - 
For two finger rotation. The button that is triggered by rotating left. Integer
value. A value of 0 disables rotation left. Defaults to 14.

**RotateRightButton** - 
For two finger rotation. The button that is triggered by rotating right. Integer
value. A value of 0 disables rotation right. Defaults to 15.

**Hold1Move1StationaryButton** - 
For two finger hold-and-move functionality. The button that is triggered by 
holding one finger and moving another one. Integer value. 
A value of 0 disables hold-and-move. Value of 0 disables this functionality.
Defaults to 1.

**Hold1Move1StationaryMaxMove** -
For two finger hold-and-move functionality. Fow far stationary finger can be
moved berfore gesture invalidation. Integer value.

**TapDragEnable** - 
Whether or not to enable tap-to-drag functionality. Boolean value. Defaults to
true.

**TapDragTime** - 
The tap-to-drag timeout. This is how long the driver will wait after a single
tap for a movement event before sending the click. Integer value representing
milliseconds. Defaults to 350.

**TapDragWait** - 
How long after detecting movement to trigger a button down event. During this
time pointer movement will be disabled. Increase this value if you find you're
draggin when you don't wish it. Integer value representing milliseconds.
Defaults to 40.

**TapDragDist** - 
How far the finger is allowed to move during drag wait time. If the finger
moves farther than this distance during the wait time then dragging will be
canceled and pointer movement will resume. Integer value. Defaults to 200.

**AxisXInvert** - 
Whether or not to invert the X axis. Boolean value. Defaults to false.

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

[1]: http://www.kernel.org/doc/Documentation/input/multi-touch-protocol.txt     "Kernel Multitouch Protocol"
[2]: http://www.gnu.org/licenses/gpl-2.0.html                                   "GNU General Public License, version 2"
[3]: http://bitmath.org/code/multitouch/                                        "xf86-input-multitouch website"
[4]: http://bitmath.org/code/mtdev/                                             "mtdev library website"
