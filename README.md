# Summoning Wars
![Join Discord](https://img.shields.io/discord/632161897759637505)
![release](https://flat.badgen.net/github/release/SaintWish/summoning-wars)
![contributors](https://flat.badgen.net/github/contributors/SaintWish/summoning-wars)
![issues-pr](https://flat.badgen.net/github/open-prs/SaintWish/summoning-wars)
![last commit](https://flat.badgen.net/github/last-commit/SaintWish/summoning-wars)
\n
Summoning Wars is an open source role-playing game originally by these wonderful people fusion45, kalimgard, mpreisler, wuha at https://sourceforge.net/projects/sumwars/ as well as the contributors listed on the AUTHORS.md.

## Description
Summoning Wars is an open source role-playing game, featuring both a
single-player and a multiplayer mode for about 2 to 8 players.  It is available
and proved to work for Windows, Mac OS X and GNU/Linux operating systems,
although it might as well work in _BSD_ and similar operating systems.

## Community
* Discord: https://discord.gg/3K7VZbQ

## License
Summoning Wars is free software released under GNU General Public License
version 3 or later (GPLv3+), which means that you can freely use, copy and
modify the software.  Please see LICENSE file for more information about its
license, and AUTHORS.md for copyright holders. To view a full list of all dependencies and their licenses used see DEPENDENCIES.md. If there's a dependencies being used missing from the list feel free to make an issue, or a pull request.

## Future Contributions
If you find something you think you can improve on in our code, please make a pull request. If you find any bugs or want to request a feature to be added, feel free to create an issue or say something in our Discord.\n
If you're addition is denied we will try out best to give you a reason behind our decision.

## General Instructions
Following libraries are required to build Summoning Wars:

* OGRE >= 1.7.3
* OIS
* CEGUI >= 0.7.5
* freealut, openal
* lua5.1
* ogg, vorbis, vorbisfile
* physfs
* POCO (for debug build only)

If you are lucky, all libs may be installed via package manager (apt-get, yum, emerge, pacman, ...).
Have a look at the "OS dependant install" section below.

After installing all required libraries type:
```
cmake -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```

Currently, there is no make install, so you have to start the game in the directory where you compiled it.

## OS Dependent Install Tips
__Ubuntu:__

Ogre 1.7 and CEGUI 0.7 are not included in the standard repositories yet. But you may get them from this additional repo:
https://launchpad.net/~andrewfenn/+archive/ogredev

To install all needed dependencies enter the following command into the terminal:
``sudo apt-get install libogre-dev libcegui-dev libois-dev libalut-dev liblua5.1-0-dev libopenal-dev libvorbis-dev libfreeimage-dev libphysfs-dev``

Then follow the "General Instructions" section above to install the game.

## Troubleshooting
_Problem: Rendering does not work under Windows:_
_Solution: Try replacing ogre.cfg by this (omit the last and the first line of course):_
```
(Begin of ogre.cfg)
Render System=Direct3D9 Rendering Subsystem

[Direct3D9 Rendering Subsystem]
Full Screen=No
Video Mode=1024 x 768 @ 32-bit colour
(End of ogre.cfg9)
```
