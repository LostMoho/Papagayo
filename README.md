# Papagayo

Lipsync tool for animators


## Papagayo 2.0b1

This is a beta release of Papagayo - please excuse the lack of documentation.
(Documentation is provided for using the application, but not much about building it.)

Papagayo is a lip-sync application for animators. Please see the manual inside the
installer folder.

What you've downloaded here is the source code to Papagayo. You can modify and build it
as you wish, as long as you follow the terms in the License.txt file.

Papagayo is currently built as a Qt application. To build it you need Qt 5.2.1 installed.
Papagayo has been built on Linux, Windows, and Mac OS X.

To build Papagayo, open up the file Papagayo.pro in Qt Creator and press the Build
Project button.


## Building on Linux

Building in Qt Creator on Linux is the same as building in Qt Creator on anything else.

If you want to build from the shell (without the GUI), then use ``qmake`` (this may be called qmake-qt5 on your system, depending on what Qt libraries you have installed).

First, make sure you have Qt5 libraries and headers installed!

Next, use ``qmake-qt5`` to generate a build file from the Qt .pro file:

    $ mkdir build
    $ qmake-qt5 DESTDIR=bld Papagayo.pro

Then make it as usual:

    $ make -j4 

Test what you have built; the binary is inside the ``build`` folder you made. After you confirm that this works, you can install it with ``checkconfig`` or ``makepkg`` or just ``make install``, depending on your distribution and personal preference.


## Development

Papagayo is currently in active development, so please check back for newer versions.

The current version of Papagayo can always be found here:

http://www.lostmarble.com/papagayo/index.shtml





Mike Clifton
July 8, 2014
