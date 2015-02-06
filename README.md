xf86-input-mtrack
=================

An Xorg driver for multitouch trackpads. Supports any trackpad whose kernel
driver uses the slotted multitouch protocol. For more information on the
protocol see the [kernel documentation][1].

This driver is compatible with Xorg server versions 1.7 to 1.12. It requires
the [mtdev][4] library to operate.

License
-------

* Copyright (C) 2011 Ryan Bourgeois <bluedragonx@gmail.com>
* Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>

This software is licensed under the [GPLv2][2] and is a fork of the
[xf86-input-multitouch][3] driver by Henrik Rydberg.

Binaries
--------

* [Gentoo][5]
* [Ubuntu][6]
* [Arch][7]

Arch package contributed and maintained by Mike Zackles.

Configuration
-------------

The following is a minimal working InputClass section for xorg.conf:

    Section "InputClass"
        MatchIsTouchpad "on"
        Identifier      "Touchpads"
        Driver          "mtrack"
    EndSection

Configuration options may be defined inside the InputClass section to configure
the driver. Available options and their defaults are as follows.

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

**BottomEdge** -
The size of an area at the bottom of the trackpad where new touches are ignored
(fingers traveling into this area from above will still be tracked). This is
represented as a percentage of the total trackpad height. Defaults to 10.

**ButtonEnable** - 
Whether or not to enable the physical buttons on or near the trackpad. Boolean
value. Defaults to true.

**ButtonIntegrated** - 
Whether or not the physical buttons are integrated with the trackpad. If you
have a one-piece trackpad like on newer MacBooks, this should be set to true.
Button emulation depends on this value being correct. Boolean value. Defaults
to true.

**ButtonMoveEmulate**
Whether or not to count the moving finger when emulating button clicks. Useful to disable if you use two hands on trackpad. Boolean value. Defaults to true.

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
representing milliseconds. Defaults to 50.

**MaxTapTime** - 
The amount of time to wait for a tap to release before counting it as a move.
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

**SwipeDistance** - 
For three finger swiping. How far you must move your fingers before a button
click is triggered. Integer value. Defaults to 700.

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

**TapDragEnable** - 
Whether or not to enable tap-to-drag functionality. Boolean value. Defaults to
true.

**TapDragTime** - 
The tap-to-drag timeout. This is how long the driver will wait after a single
tap for a movement event before sending the click. Integer value representing
milliseconds. Defaults to 350.

**TapDragWait**
How long after detecting movement to trigger a button down event. During this
time pointer movement will be disabled. Increase this value if you find you're
draggin when you don't wish it. Integer value representing milliseconds.
Defaults to 40.

**TapDragDist**
How far the finger is allowed to move during drag wait time. If the finger
moves farther than this distance during the wait time then dragging will be
canceled and pointer movement will resume. Integer value. Defaults to 200.

**AxisXInvert**
Whether or not to invert the X axis. Boolean value. Defaults to false.

**AxisYInvert**
Whether or not to invert the Y axis. Boolean value. Defaults to false.

[1]: http://www.kernel.org/doc/Documentation/input/multi-touch-protocol.txt     "Kernel Multitouch Protocol"
[2]: http://www.gnu.org/licenses/gpl-2.0.html                                   "GNU General Public License, version 2"
[3]: http://bitmath.org/code/multitouch/                                        "xf86-input-multitouch website"
[4]: http://bitmath.org/code/mtdev/                                             "mtdev library website"
[5]: http://www.dev.fatalmachine.org/xf86-input-mtrack/gentoo                   "Gentoo Ebuilds"
[6]: http://www.dev.fatalmachine.org/xf86-input-mtrack/ubuntu                   "Ubuntu Packages"
[7]: http://aur.archlinux.org/packages.php?ID=48505                             "Arch Package"

