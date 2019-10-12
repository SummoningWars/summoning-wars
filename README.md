# Summoning Wars Role-Playing Game
Summoning Wars is an open source role-playing game.

## Description
Summoning Wars is an open source role-playing game, featuring both a
single-player and a multiplayer mode for about 2 to 8 players.  It is available
and proved to work for Windows, Mac OS X and GNU/Linux operating systems,
although it might as well work in __BSD__ and similar operating systems.

## License
Summoning Wars is free software released under GNU General Public License
version 3 or later (GPLv3+), which means that you can freely use, copy and
modify the software.  Please see LICENSE file for more information about its
license, and AUTHORS.md for copyright holders.

## General Instructions
Following libraries are required to build Summoning Wars:

OGRE >= 1.7.3
OIS
CEGUI >= 0.7.5
freealut, openal
lua5.1
ogg, vorbis, vorbisfile
physfs
POCO (for debug build only)

If you are lucky, all libs may be installed via package manager (apt-get, yum, emerge, pacman, ...).
Have a look at the "OS dependant install" section below.

After installing all required libraries type:
cmake -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install

Currently, there is no make install, so you have to start the game in the directory where you compiled it.
Please visit our Website www.sumwars.org to give some feedback or report bugs.

## OS Dependent Install Tips
__Ubuntu:__

Ogre 1.7 and CEGUI 0.7 are not included in the standard repositories yet. But you may get them from this additional repo:
https://launchpad.net/~andrewfenn/+archive/ogredev

To install all needed dependencies enter the following command into the terminal:
sudo apt-get install libogre-dev libcegui-dev libois-dev libalut-dev liblua5.1-0-dev libopenal-dev libvorbis-dev libfreeimage-dev libphysfs-dev

Then follow the "General instructions" section above to install the game.

## Troubleshooting
Problem: Rendering does not work under Windows:
Solution: Try replacing ogre.cfg by this (omit the last and the first line of course):

(Begin of ogre.cfg)
Render System=Direct3D9 Rendering Subsystem

[Direct3D9 Rendering Subsystem]
Full Screen=No
Video Mode=1024 x 768 @ 32-bit colour
(End of ogre.cfg9)
