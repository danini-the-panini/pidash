PiDash
======

A (semi-interactive) dashboard for the Raspberry Pi.

__NOTE__: Linux only, unless someone wants to test and provide instruction for Windows 10.

Idea
----

1. Plug a Raspberry Pi into a TV.
2. Provide PiDash with a list of URLs/HTML pages/Shell Commands that the user can cycle through.
3. ????
4. Profit!

Dependencies
-------------

Runtime:
* GTK3+
* webkitgtk-3.0
* vte-2.90
* inotify-tools

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
    $ sudo apt-get install libwebkitgtk-3.0-dev libvte-2.90-dev inotify-tools

Rake:

    $ rake

Run:

    $ ./pidash-run

Usage
-----

Use left/right arrow keys to change page, escape to quit

Modify `pages.txt` while pidash is running will restart pidash with the updates.

TODO
----

In no _particular_ order
1. Figure out the best way to interact with it
1. Animate swiping
1. Run on startup
1. Add more interactions?

Contributing
------------

(To the tune of Daft Punk's Technologic)
1. Fork it
1. Branch it
1. Code it
1. Test it
1. (something)
1. (something)
1. Pull Request it!
