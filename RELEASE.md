## v0.5.1
* Add button emulation when no finger placement is valid
* Add dedicated zone button
* Enable the restriction of the zone inside the bottom edge
* Readme configuration options are grouped according to their relationship
* Readme anchor has been moved to the options themselves

## v0.5.0
* Add tap-to-drag lock timeout
* Make edge size configurable for each edge individually
* Add edge scrolling
* Fix movement truncation
* Fix crash during suspend/wake up
* Make movement speed resolution independent
* Lock smooth scroll to one axis at time
* Post button events before move events
* New emulated buttons implementation
* Reduce max recognized fingers/touch points from 32 to 16
* New fancy readme with anchors
* Fix warnings, improve logging
* New examples
* Helper script to parse output of xinput --list-props

## v0.4.1
* Smooth scroll: detect and handle flipped up&down, left&right buttons
* Add examples with my current configuration as first example.
* Proper initialization of scroll axes(2 and 3).
* Setup scroll valuators also during device initialization.
* Do not consider edge clicks when calculating emulated buttons
* Change type of variables holding movement delta to double for better precision.
* Rewrite tap implementation from scratch. Remove 'status' field from Touch structure.
* Clear the MT_BUTTON bit of touches when an integrated button is released.
* Scale gesture reimplementation.

## v0.4.0
* Add smooth scroll
* Add scroll/swipe/swipe4 to drag functionality
* Add scroll coasting
* Add Hold And Move gesture
* Replace EdgeSize with BottomEdge property
* Replace busy waiting with timers
* Fix button down -> up delays
* Initial support for absolute mode devices
* Improve support for pressure based devices
* Many minor improvements/bugfixes

## v0.3.1
* Include a configure script.
* Update the README.
* Add the RELEASE file.
