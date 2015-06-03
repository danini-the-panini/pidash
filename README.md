PiDash
======

An interactive dashboard for the Raspberry Pi.

__NOTE__: Linux only, unless someone wants to test and provide instruction for Windows 10.

Idea
----

1. Plug a Raspberry Pi into a TV with a Leap Motion or other hand-gesture recogniser.
2. Provide PiDash with a list of URLs/HTML pages/Shell Commands that the user can swipe through.
3. ????
4. Profit!

Dependencies
-------------

Runtime:
* GTK3+
* webkitgtk-3.0

Development:
* gcc
* pkg-config
* ruby
* rake
* rplusplus


Building
--------

Install dependencies (Ubuntu):

    $ gem install rplusplus
    $ sudo apt-get install libwebkitgtk-3.0-dev

Rake:

    $ rake

Run:

    $ ./pidash

Usage
-----

Use left/right arrow keys to change page, escape to quit

TODO
----

In no _particular_ order
1. Make full screen
1. Make list of URLs read from file
1. Watch the URL file and update on-the-fly
1. Support shell commands (e.g. `iftop`)
1. Use the Leap Motion as input (swipe left/right)
1. Animate swiping
1. Run on startup
1. Add more interesting gestures (swipe up/down) ?

Contributing
------------

1. Fork it
1. Branch it
1. Code it
1. Test it
1. Pull Request it!
