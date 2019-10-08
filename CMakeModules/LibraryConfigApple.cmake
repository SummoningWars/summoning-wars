 #
 #             ORXONOX - the hottest 3D action shooter ever to exist
 #                             > www.orxonox.net <
 #
 #        This program is free software; you can redistribute it and/or
 #         modify it under the terms of the GNU General Public License
 #        as published by the Free Software Foundation; either version 2
 #            of the License, or (at your option) any later version.
 #
 #       This program is distributed in the hope that it will be useful,
 #        but WITHOUT ANY WARRANTY; without even the implied warranty of
 #        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #                 GNU General Public License for more details.
 #
 #   You should have received a copy of the GNU General Public License along
 #      with this program; if not, write to the Free Software Foundation,
 #     Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 #
 #
 #  Author:
 #    Yuning Chai
 #  Description:
 #    Sets necessary library options and paths on Mac.
 #

IF(APPLE)
  MESSAGE(STATUS "Running on Apple. Using customized paths and options.")

  SET(ENV{OGRE_HOME} "/Developer/SDKs/OgreSDK/Dependencies")
  SET(ENV{CEGUIDIR} "/Developer/SDKs/OgreSDK/Dependencies")

  #MESSAGE(STATUS $ENV{OGRE_HOME})

  SET(CMAKE_FIND_FRAMEWORK "FIRST")

#  SET(OGRE_LIBRARY_OPTIMIZED "/Developer/SDKs/OgreSDK/Dependencies/Ogre.framework/Ogre")

#  SET(CEGUI_LIBRARY_OPTIMIZED "/Developer/SDKs/OgreSDK/Dependencies/CEGUI.framework/CEGUI")

#  SET(OGRE_LIBRARY_OPTIMIZED  "/Developer/SDKs/OgreSDK/Dependencies/Ogre.framework" CACHE STRING "Ogre lib for OSX")
#  SET(CEGUI_LIBRARY_OPTIMIZED  "/Developer/SDKs/OgreSDK/Dependencies/CEGUI.framework" CACHE STRING "CEGUI lib for OSX")

ENDIF(APPLE)
