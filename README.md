xf86-input-mtrack
=================
###### v0.5.1

An Xorg driver for multitouch trackpads. Supports any trackpad whose kernel
driver uses the slotted multitouch protocol. For more information on the
protocol see the [kernel documentation][1].

This driver is compatible with Xorg server versions 1.7 to 1.19.0 It requires
the [mtdev][4] library to operate.


## License

* Copyright (C) 2015-2018 Paweł Turkowski <p2rkw0@gmail.com>
* Copyright (C) 2011 Ryan Bourgeois <bluedragonx@gmail.com>
* Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>

This software is licensed under the [GPLv2][2] and is a fork of the
[xf86-input-multitouch][3] driver by Henrik Rydberg.


## Building and Installing
This is a standard autoconf package. So:
```
    ./configure
    make && make install
```
It is likely that you will need to change system-dependent paths such as the
xorg module directory. Otherwise mtrack may be not installed in xserver search
path.
See `configure --help` for options.

To build deb package and install in system wide you will usually have to change
installation prefix to /usr like so:
```
    ./configure --prefix=/usr
    dpkg-buildpackage
```
## Bug Reporting

If you found a bug, and you are going to submit it, first run
```
    xinput --list
```
then find your touchpad name/ID on the list and create report with output of
```
    xinput --list-props <your touchpad ID>
```
attached.

## Configuration


The following is a minimal working InputClass section for xorg.conf:
```
    Section "InputClass"
        MatchIsTouchpad "on"
        Identifier      "Touchpads"
        Driver          "mtrack"

#       In case of problems enable/disable this line:
        MatchDevicePath "/dev/input/event*"
    EndSection
```

---
To apply changes made in xorg.conf (or related files) X server have to be restarted.
Changes made with `xinput` are applied immedietly but they are not saved between sessions.

### Options
Configuration options may be defined inside the InputClass section to configure
the driver. See examples/ directory for example configuration files. Feel free to submit
yours, named as your laptop's model.
Available options and their defaults are as follows.

---
#### Basic
**[TrackpadDisable](#TrackpadDisable)**<a name="TrackpadDisable"></a>
Disables trackpad touch input. A value of 0 will enable the trackpad. A value
of 1 will disable tapping and gestures but not movement. A value of 2 will
disable all input. A value of 3 will also disable physical buttons.  
Integer. Default is 0.

**[ButtonEnable](#ButtonEnable)**<a name="ButtonEnable"></a>
Whether or not to enable the physical buttons on or near the trackpad.  
Boolean value. Defaults to true.

**[ButtonIntegrated](#ButtonIntegrated)**<a name="ButtonIntegrated"></a>
Whether or not the physical buttons are integrated with the trackpad. If you
have a one-piece trackpad like on newer MacBooks, this should be set to true.
Button emulation depends on this value being correct.  
Boolean value. Defaults to true.

---
#### Responsiveness
**[Sensitivity](#Sensitivity)**<a name="Sensitivity"></a>
Adjusts the sensitivity (movement speed) of the touchpad. This is a real number
greater than or equal to zero. A value of 0 will disable pointer movement.  
Float value. Default is 1.

**[FingerHigh](#FingerHigh)**<a name="FingerHigh"></a>
Defines the pressure at which a finger is detected as a touch. This is a
percentage represented as an integer.  
Integer value. Default is 5.

**[FingerLow](#FingerLow)**<a name="FingerLow"></a>
Defines the pressure at which a finger is detected as a release. This is a
percentage represented as an integer.  
Integer value. Default is 5.

**[IgnoreThumb](#IgnoreThumb)**<a name="IgnoreThumb"></a>
Whether or not to ignore touches that are determined to be thumbs.  
Boolean value. Defaults to false.

**[IgnorePalm](#IgnorePalm)**<a name="IgnorePalm"></a>
Whether or not to ignore touches that are determined to be palms.  
Boolean value. Defaults to false.

**[DisableOnThumb](#DisableOnThumb)**<a name="DisableOnThumb"></a>
Whether or not to disable the entire trackpad when a thumb is touching.  
Boolean value. Defaults to false.

**[DisableOnPalm](#DisableOnPalm)**<a name="DisableOnPalm"></a>
Whether or not to disable the entire trackpad when a palm is touching.  
Boolean value. Defaults to false.

**[ThumbSize](#ThumbSize)**<a name="ThumbSize"></a>
The minimum size of what's considered a thumb. It is expected that a thumb
will be larger than other fingers. This is represented as a percentage of the
maximum touch value and is dependent on the trackpad hardware.  
Integer value. Defaults to 25.

**[PalmSize](#PalmSize)**<a name="PalmSize"></a>
The minimum size of what's considered a palm. Palms are expected to be very
large on the trackpad. This is represented as a percentage of the maximum touch
value and is dependent on the trackpad hardware.  
Integer value. Defaults to 40.

**[ThumbRatio](#ThumbRatio)**<a name="ThumbRatio"></a>
The width/length ratio of what's considered a thumb. It is expected that a
thumb is longer than it is wide. This tells the driver how much longer.  
Percentage represented by an integer.  
Integer value. Defaults to 70.

---
#### Zones<a name="Zones"></a>
Divide the touchpad into "zones". Clicking the integrated button in one of 
these zones will send the button event configured for each {First, Second, Third}ZoneButton. 
The driver will only add zones for the ZoneButton values that
are enabled. The zone splitting start from the left to right using the first to third value. 
So enabling only SecondZoneButton and ThirdZoneButton will create two
zones, the left-middle part will fire SecondZoneButton and the middle-right part ThirdZoneButton.  

**[ButtonZonesEnable](#ButtonZonesEnable)**<a name="ButtonZonesEnable"></a>
Whether or not to enable button zones. If button zones are enabled then the
trackpad will be split into one, two, or three vertical zones. 
Boolean value. Defaults to false.

**[FirstZoneButton](#FirstZoneButton)**<a name="FirstZoneButton"></a>
The button to emulate when the zone is pressed. This is the leftmost part of the pad.
Integer value. A value of 0 disables this zone split.  
Integer value. Defaults to 1.

**[SecondZoneButton](#SecondZoneButton)**<a name="SecondZoneButton"></a>
The button to emulate when the zone is pressed. This will float to the right of
the leftmost zone. Integer value. A value of 0 disables this zone split.  
Integer value. Defaults to 2.

**[ThirdZoneButton](#ThirdZoneButton)**<a name="ThirdZoneButton"></a>
The button to emulate when the zone is pressed. This will float to the right of
the leftmost zone. Integer value. A value of 0 disables this zone split.  
Integer value. Defaults to 0.

**[LimitButtonZonesToBottomEdge](#LimitButtonZonesToBottomEdge)**<a name="LimitButtonZonesToBottomEdge"></a>
Restrict button zones inside the [EdgeBottom](#EdgeBottomSize) area. So instead of enabling zones
on the full pad height, the zone is limited to the percentage set for the [EdgeBottom](#EdgeBottomSize).  
Boolean value. Default to false.

---
#### Physical click
**[ClickFinger0](#ClickFinger0)**<a name="ClickFinger0"></a>
Which button to emulate when no valid finger placement is touching the trackpad during a
click, as on "EdgeBottom".  
Integer value. A value of 0 disables one-touch emulation. Defaults to 0.

**[ClickFinger1](#ClickFinger1)**<a name="ClickFinger1"></a>
Which button to emulate when one finger is touching the trackpad during a
click.  
Integer value. A value of 0 disables one-touch emulation. Defaults to 1.

**[ClickFinger2](#ClickFinger2)**<a name="ClickFinger2"></a>
Which button to emulate when two fingers are touching the trackpad during a
click.  
Integer value. A value of 0 disabled one-touch emulation. Defaults to 2.

**[ClickFinger3](#ClickFinger3)**<a name="ClickFinger3"></a>
Which button to emulate when three fingers are touching the trackpad during a
click.  
Integer value. A value of 0 disabled one-touch emulation. Defaults to 3.

**[ButtonMoveEmulate](#ButtonMoveEmulate)**<a name="ButtonMoveEmulate"></a>
Whether or not to count the moving finger when emulating button clicks.
Useful to disable if you use two hands on trackpad.  
Boolean value. Defaults to true.

**[ButtonTouchExpire](#ButtonTouchExpire)**<a name="ButtonTouchExpire"></a>
How long (in ms) to consider a touching finger as part of button emulation. A
value of 0 will not expire touches.  
Integer value. Defaults to 100.

---
#### Tap clicking
**[TapButton1](#TapButton1)**<a name="TapButton1"></a>
Which button to emulate for one-finger tapping. Integer value. A value of 0
disables one-finger tapping.  
Integer value. Defaults to 1.

**[TapButton2](#TapButton2)**<a name="TapButton2"></a>
Which button to emulate for two-finger tapping. Integer value. A value of 0
disables two-finger tapping.  
Integer value. Defaults to 3.

**[TapButton3](#TapButton3)**<a name="TapButton3"></a>
Which button to emulate for three-finger tapping. Integer value. A value of 0
disables three-finger tapping.  
Integer value. Defaults to 2.

**[TapButton4](#TapButton4)**<a name="TapButton4"></a>
Which button to emulate for four-finger tapping. Integer value. A value of 0
disables three-finger tapping.  
Integer value. Defaults to 0.

**[ClickTime](#ClickTime)**<a name="ClickTime"></a>
When tapping, how much time to hold down the emulated button.  
Integer value representing milliseconds. Defaults to 50.

**[MaxTapTime](#MaxTapTime)**<a name="MaxTapTime"></a>
The amount of time to wait for incoming touches after first one before counting
it as emulated button click.  
Integer value representing milliseconds. Defaults to 120.

**[MaxTapMove](#MaxTapMove)**<a name="MaxTapMove"></a>
How far a touch is allowed to move before counting it is no longer considered a
tap.  
Integer value. Defaults to 400.

#### Gesture

---
##### Basic
**[GestureClickTime](#GestureClickTime)**<a name="GestureClickTime"></a>
When a gesture triggers a click, how much time to hold down the emulated button.
Integer value representing milliseconds. Defaults to 10.  

**[GestureWaitTime](#GestureWaitTime)**<a name="GestureWaitTime"></a>
Touches are allowed to transition from one gesture to another. For example, you
may go from scrolling to swiping without releasing your fingers from the pad.
This value is the amount of time you must be performing the new gesture before
it is triggered. This prevents accidental touches from triggering other
gestures.   
Integer value representing milliseconds. Defaults to 100.  

---
##### Two fingers Scrolling<a name="two-finger-scrolling"></a>

**[ScrollDistance](#ScrollDistance)**<a name="ScrollDistance"></a>
For two finger scrolling. How far you must move your fingers before a button
click is triggered.  
Integer value. Defaults to 150.

**[ScrollClickTime](#ScrollClickTime)**<a name="ScrollClickTime"></a>
For two finger scrolling. How long button triggered by scrolling 
will be hold down. A value of 0 will hold button down till end of gesture.
0 - emit button click only once pre "instance" of gesture.
Integer value representing milliseconds.  
Integer value. Defaults to 20.

**[ScrollSensitivity](#ScrollSensitivity)**<a name="ScrollSensitivity"></a>
For two finger scrolling. Sensitivity (movement speed) of pointer during two 
finger scrolling. A value of 0 disables pointer movement during gesture.
Integer value expressed as parts per thousand of normal sensivity.
A value of 1000 results with normal movement speed. 
Integer value. Defaults to 0.

**[ScrollUpButton](#ScrollUpButton)**<a name="ScrollUpButton"></a>
For two finger scrolling. The button that is triggered by scrolling up.  
Integer value. A value of 0 disables scrolling up. Defaults to 4.

**[ScrollDownButton](#ScrollDownButton)**<a name="ScrollDownButton"></a>
For two finger scrolling. The button that is triggered by scrolling down.  
Integer value. A value of 0 disables scrolling down. Defaults to 5.

**[ScrollLeftButton](#ScrollLeftButton)**<a name="ScrollLeftButton"></a>
For two finger scrolling. The button that is triggered by scrolling left.  
Integer value. A value of 0 disables scrolling left. Defaults to 6.

**[ScrollRightButton](#ScrollRightButton)**<a name="ScrollRightButton"></a>
For two finger scrolling. The button that is triggered by scrolling right.  
Integer value. A value of 0 disables scrolling right. Defaults to 7.

**[ScrollSmooth](#ScrollSmooth)**<a name="ScrollSmooth"></a>
For two finger scrolling. Whether to generate high precision scroll events.  
Boolean value. Defaults to 1.
Property: "Trackpad High Smooth Scroll"

**[ScrollCoastDuration](#ScrollCoastDuration)**<a name="ScrollCoastDuration"></a>
How long after finished scrolling movement should be continued. Works only 
with smooth scrolling enabled.  
Floating value representing miliseconds. Defaults to 200.0.
Property: "Trackpad Scroll Coasting"

**[ScrollCoastEnableSpeed](#ScrollCoastEnableSpeed)**<a name="ScrollCoastEnableSpeed"></a>
How fast scroll should be to enable coasting feature.  
Floating value. Defaults to 0.1.
Property: "Trackpad Scroll Coasting"

---
##### Three fingers swipe<a name="three-finger-swipe"></a>
**[SwipeDistance](#SwipeDistance)**<a name="SwipeDistance"></a>
For three finger swiping. How far you must move your fingers before a button
click is triggered.  
Integer value. Defaults to 700.

**[SwipeClickTime](#SwipeClickTime)**<a name="SwipeClickTime"></a>
For three finger swiping. How long button triggered by swiping
will be hold down.  
Integer value representing milliseconds. Defaults to 300.

**[SwipeSensitivity](#SwipeSensitivity)**<a name="SwipeSensitivity"></a>
For three finger scrolling. Sensitivity (movement speed) of pointer during three 
finger scrolling. A value of 0 disables pointer movement during gesture.  
Integer value expressed as parts per thousand of normal sensivity.  
A value of 1000 results with normal movement speed. Defaults to 0.

**[SwipeUpButton](#SwipeUpButton)**<a name="SwipeUpButton"></a>
For three finger swiping. The button that is triggered by swiping up.  
Integer value. A value of 0 disables swiping up. Defaults to 8.

**[SwipeDownButton](#SwipeDownButton)**<a name="SwipeDownButton"></a>
For three finger swiping. The button that is triggered by swiping down.  
Integer value. A value of 0 disables swiping down. Defaults to 9.

**[SwipeLeftButton](#SwipeLeftButton)**<a name="SwipeLeftButton"></a>
For three finger swiping. The button that is triggered by swiping left.  
Integer value. A value of 0 disables swiping left. Defaults to 10.

**[SwipeRightButton](#SwipeRightButton)**<a name="SwipeRightButton"></a>
For three finger swiping. The button that is triggered by swiping right.  
Integer value. A value of 0 disables swiping right. Defaults to 11.

---
##### Four fingers swipe<a name="four-finger-swipe"></a>
**[Swipe4Distance](#Swipe4Distance)**<a name="Swipe4Distance"></a>
For four finger swiping. How far you must move your fingers before a button
click is triggered.  
Integer value. Defaults to 700.

**[Swipe4ClickTime](#Swipe4ClickTime)**<a name="Swipe4ClickTime"></a>
For four finger swiping. How long button triggered by swiping
will be hold down.  
Integer value representing milliseconds. Defaults to 300.

**[Swipe4Sensitivity](#Swipe4Sensitivity)**<a name="Swipe4Sensitivity"></a>
For four finger scrolling. Sensitivity (movement speed) of pointer during four 
finger scrolling. A value of 0 disables pointer movement during gesture.  
Integer value expressed as parts per thousand of normal sensivity.  
A value of 1000 results with normal movement speed. Defaults to 0.

**[Swipe4UpButton](#Swipe4UpButton)**<a name="Swipe4UpButton"></a>
For four finger swiping. The button that is triggered by swiping up.  
Integer value. A value of 0 disables swiping up. Defaults to 8.

**[Swipe4DownButton](#Swipe4DownButton)**<a name="Swipe4DownButton"></a>
For four finger swiping. The button that is triggered by swiping down.  
Integer value. A value of 0 disables swiping down. Defaults to 9.

**[Swipe4LeftButton](#Swipe4LeftButton)**<a name="Swipe4LeftButton"></a>
For four finger swiping. The button that is triggered by swiping left.  
Integer value. A value of 0 disables swiping left. Defaults to 10.

**[Swipe4RightButton](#Swipe4RightButton)**<a name="Swipe4RightButton"></a>
For four finger swiping. The button that is triggered by swiping right.  
Integer value. A value of 0 disables swiping right. Defaults to 11.

---
##### Edge Scrolling - One finger
Should be used in conjuction with the [`Edge disabling`](#EdgeDisabling) options.

**[EdgeScrollDist](#EdgeScrollDist)**<a name="EdgeScrollDist"></a>
For one finger edge scrolling. How far you must move your finger on edge before
a button click is triggered.  
Integer value. Defaults to 105.

**[EdgeScrollClickTime](#EdgeScrollClickTime)**<a name="EdgeScrollClickTime"></a>
For one finger edge scrolling. How long button triggered by edge scrolling
will be hold down. A value of 0 will hold button down till end of gesture.  
0 - emit button click only once pre "instance" of gesture.  
Integer value representing milliseconds. Defaults to 20.

**[EdgeScrollSensitivity](#EdgeScrollSensitivity)**<a name="EdgeScrollSensitivity"></a>
For one finger edge scrolling. Sensitivity (movement speed) of pointer during one
finger scrolling. A value of 0 disables pointer movement during gesture.  
Integer value expressed as parts per thousand of normal sensivity.  
A value of 1000 results with normal movement speed. Defaults to 0.

**[EdgeScrollUpButton](#EdgeScrollUpButton)**<a name="EdgeScrollUpButton"></a>
For one finger edge scrolling. The button that is triggered by edge scrolling up.  
Integer value. A value of 0 disables scrolling up. Defaults to 4.

**[EdgeScrollDownButton](#EdgeScrollDownButton)**<a name="EdgeScrollDownButton"></a>
For one finger edge scrolling. The button that is triggered by edge scrolling down.  
Integer value. A value of 0 disables scrolling down. Defaults to 5.

**[EdgeScrollLeftButton](#EdgeScrollLeftButton)**<a name="EdgeScrollLeftButton"></a>
For one finger edge scrolling. The button that is triggered by edge scrolling left.  
Integer value. A value of 0 disables scrolling left. Defaults to 6.

**[EdgeScrollRightButton](#EdgeScrollRightButton)**<a name="EdgeScrollRightButton"></a>
For one finger edge scrolling. The button that is triggered by edge scrolling right.  
Integer value. A value of 0 disables scrolling right. Defaults to 7.

---
##### Pinch scaling - Two fingers
**[ScaleDistance](#ScaleDistance)**<a name="ScaleDistance"></a>
For pinch scaling. How far you must move your fingers before a button click is
triggered.  
Integer value. Defaults to 150.

**[ScaleUpButton](#ScaleUpButton)**<a name="ScaleUpButton"></a>
For pinch scaling. The button that is triggered by scaling up.  
Integer value. A value of 0 disables scaling up. Defaults to 12.

**[ScaleDownButton](#ScaleDownButton)**<a name="ScaleDownButton"></a>
For pinch scaling. The button that is triggered by scaling down.  
Integer value. A value of 0 disables scaling down. Defaults to 13.

---
##### Rotation - Two fingers
**[RotateDistance](#RotateDistance)**<a name="RotateDistance"></a>
For two finger rotation. How far you must move your fingers before a button
click is triggered.  
Integer value. Defaults to 150.

**[RotateLeftButton](#RotateLeftButton)**<a name="RotateLeftButton"></a>
For two finger rotation. The button that is triggered by rotating left.  
Integer value. A value of 0 disables rotation left. Defaults to 14.

**[RotateRightButton](#RotateRightButton)**<a name="RotateRightButton"></a>
For two finger rotation. The button that is triggered by rotating right.  
Integer value. A value of 0 disables rotation right. Defaults to 15.

---
#### Edge disabling<a name="EdgeDisabling"></a>
~~[EdgeSize](#EdgeSize)~~<a name="EdgeSize"></a>
DEPRECATED.
The size of an area around the trackpad where new touches are ignored (fingers
traveling into this area from above will still be tracked).  
This is represented as a percentage of the total trackpad height.  
Integer value. Defaults to 0.  
Value set here is overwriten by EdgeLeftSize, EdgeRightSize, EdgeTopSize and
EdgeBottomSize

**[EdgeTopSize](#EdgeTopSize)**<a name="EdgeTopSize"></a>
The size of an area at the top of the trackpad where new touches are ignored
(fingers travelling into this area from the bottom will still be tracked).  
Integer value representing a percentage of the total trackpad height. Defaults to 0.

**[EdgeBottomSize](#EdgeBottomSize)**<a name="EdgeBottomSize"></a>
The size of an area at the bottom of the trackpad where new touches are ignored
(fingers travelling into this area from the top will still be tracked).  
Integer value representing a percentage of the total trackpad height. Defaults to 10.

**[EdgeLeftSize](#EdgeLeftSize)**<a name="EdgeLeftSize"></a>
The size of an area at the left of the trackpad where new touches are ignored
(fingers travelling into this area from the right will still be tracked).  
Integer value representing a percentage of the total trackpad width. Defaults to 0.

**[EdgeRightSize](#EdgeRightSize)**<a name="EdgeRightSize"></a>
The size of an area at the right of the trackpad where new touches are ignored
(fingers travelling into this area from the left will still be tracked).  
Integer value representing a percentage of the total trackpad width. Defaults to 0.

---
#### Special features
**[Hold1Move1StationaryButton](#Hold1Move1StationaryButton)**<a name="Hold1Move1StationaryButton"></a>
For two finger hold-and-move functionality. The button that is triggered by 
holding one finger and moving another one.  
Integer value. A value of 0 disables hold-and-move.  
Value of 0 disables this functionality.  
Defaults to 1.

**[Hold1Move1StationaryMaxMove](#Hold1Move1StationaryMaxMove)**<a name="Hold1Move1StationaryMaxMove"></a>
For two finger hold-and-move functionality. Fow far stationary finger can be
moved berfore gesture invalidation.  
Integer value. Default to 20.

**[TapDragEnable](#TapDragEnable)**<a name="TapDragEnable"></a>
Whether or not to enable tap-to-drag functionality.  
Boolean value. Defaults to true.

**[TapDragTime](#TapDragTime)**<a name="TapDragTime"></a>
The tap-to-drag timeout. This is how long the driver will wait after a single
tap for a movement event before sending the click.  
Integer value representing milliseconds. Defaults to 350.

**[TapDragWait](#TapDragWait)**<a name="TapDragWait"></a>
How long after detecting movement to trigger a button down event. During this
time pointer movement will be disabled. Increase this value if you find you're
draggin when you don't wish it.  
Integer value representing milliseconds. Defaults to 40.

**[TapDragDist](#TapDragDist)**<a name="TapDragDist"></a>
How far the finger is allowed to move during drag wait time. If the finger
moves farther than this distance during the wait time then dragging will be
canceled and pointer movement will resume.  
Integer value. Defaults to 200.

**[TapDragLockTimeout](#TapDragLockTimeout)**<a name="TapDragLockTimeout"></a>
This is how long the driver will wait after initial drag in 'drag ready' state
in which it will be able to resume previous drag without additional `up`, `down`
sequence.  
Value of 0 disables this functionality.  
Values of less than zero will make mtrack require additional tap to finish drag
by sending `button up`.  
Integer value representing milliseconds. Defaults to 500.

**[AxisXInvert](#AxisXInvert)**<a name="AxisXInvert"></a>
Whether or not to invert the X axis.  
Boolean value. Defaults to false.

**[AxisYInvert](#AxisYInvert)**<a name="AxisYInvert"></a>
Whether or not to invert the Y axis.  
Boolean value. Defaults to false.

Tips
-------------
#### Swipe to drag
To setup swipe to drag functionality you have to choose which swipe gesture ([Scroll](two-finger-scrolling), [Swipe](#three-finger-swipe), [Swipe4](#four-finger-swipe))
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
This will enable dragging with three fingers. Change [sensitivity](#SwipeSensitivity) for faster/slower movements.
Scroll, and Swipe4 are also supported.

#### Hold and move
Hold down one finger in place to initiate hold-and-move gesture.
Then move another finger to drag configured button.
Gesture will last as long as fist finger (a.k.a. stationary finger) will
be held down in place.

Increase [TapDragDist](#TapDragDist) to give stationary finger more freedom.
Set [Hold1Move1StationaryButton](#Hold1Move1StationaryButton) to 0 to disable, set to other value to send button other 
than "1".

#### Persistent dragging
If you're using lot of tools that require dragging you can make it a little bit
easier by enabling [persistent tap-to-drag](#TapDragLockTimeout):
```
    Option "TapDragLockTimeout" "-1"
```
With that change you will have to perform additional tap when dragging with
tap-to-drag. Other positive values will let you continue yor drag within
specified time.

#### Enabling soft button
##### Basic
If you like to keep a finger on the bottom of the pad to click and use another one to move the cursor, you should enable edge restriction and [ClickFinger0](#ClickFinger0) parameters.
```
    Option "EdgeBottomSize" "20"    # Disable tap and movement detection in the bottom 20% of the pad
    Option "ClickFinger0" "1"       # Enable clicking action "1" when no finger is detected
```
You could also use [ClickFinger0](#ClickFinger0) with [EdgeTop/Right/LeftSize](#EdgeDisabling).

##### Advanced
If you want more than one button in the bottom edge, you need to use a more  
advanced configuration. It will enable you use up to [3 buttons](#Zones) inside that [edge](#EdgeBottomSize).  
```
    Option "ButtonZonesEnable" "true"               # Enable "Zones"
    Option "LimitButtonZonesToBottomEdge" "true"    # Limit the zones to the bottom edge
    Option "EdgeBottomSize" "20"                    # Disable tap and movement detection in the bottom 20% of the pad

    # Zones stack from left to right inside the 20% height defined above
    Option "FirstZoneButton" "1"   # Left part fire click 1
    Option "SecondZoneButton" "3"   # Middle part fire click 3
    Option "ThirdZoneButton" "2"   # Right part fire click 2
```


[1]: http://www.kernel.org/doc/Documentation/input/multi-touch-protocol.txt     "Kernel Multitouch Protocol"
[2]: http://www.gnu.org/licenses/gpl-2.0.html                                   "GNU General Public License, version 2"
[3]: http://bitmath.org/code/multitouch/                                        "xf86-input-multitouch website"
[4]: http://bitmath.org/code/mtdev/                                             "mtdev library website"
