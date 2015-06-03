PiDash
======

An interactive dashboard for the Raspberry Pi.

__NOTE__: Linux only, unless someone can test and provide instruction for Windows 10.

Idea
----

1. Plug a Raspberry Pi into a TV with a Leap Motion or other hand-gesture recogniser.
2. Provide PiDash with a list of URLs/HTML pages/Shell Commands that the user can swipe through.
3. ????
4. Profit!

Dependencies
-------------

Runtime:
* webkit2-3.0

Development:
* gcc
* pkg-config
* ruby
* rake


Building
--------

Install dependencies (Ubuntu):

    $ sudo apt-get install libwebkit2-3.0-dev

Rake:

    $ rake

Run:

    $ ./pidash

Usage
-----

Use left/right arrow keys to change page, escape to quit

TODO
----

1. Make full screen
2. Make list of URLs read from file
3. Watch the URL file and update on-the-fly
4. Support shell commands (e.g. `iftop`)
5. Use the Leap Motion as input (swipe left/right)
6. Run on startup
7. Add more interesting gestures (swipe up/down) ?

Contributing
------------

1. Fork it
2. Branch it
3. Code it
4. Test it
5. Pull Request it!
