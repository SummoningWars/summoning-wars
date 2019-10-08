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
 #    Adrian Friedli
 #  Description:
 #    Sets the correct find script paths on the ETH Tardis workstations.
 #    Also sets the TARDIS variable.
 #

IF(UNIX AND NOT APPLE)
  IF(EXISTS /etc/hostname)
    FILE(STRINGS /etc/hostname HOSTNAME LIMIT_COUNT 1)
    IF(${HOSTNAME} MATCHES "^tardis-[a-z][0-9][0-9]$")
      SET (TARDIS ON)
    ENDIF()
  ENDIF()
ENDIF()

IF(TARDIS)
  MESSAGE(STATUS "Running on D-ITET isg.ee Tardis Computer. Using customized paths.")

#  SET(CMAKE_C_COMPILER "gcc-4.1.1")
#  SET(CMAKE_CXX_COMPILER "g++-4.1.1")

  # Note: When setting ENV${} variables, make sure to use quotes when
  #       having multiple directories.
  #SET(BOOST_INCLUDEDIR "/usr/pack/boost-1.42.0-bs/include")
  #SET(BOOST_LIBRARYDIR "/usr/pack/boost-1.42.0-bs/i686-debian-linux4.0/lib")
  SET(Boost_VERSION 104200)
  SET(Boost_INCLUDE_DIRS "/usr/pack/boost-1.42.0-bs/include" CACHE PATH "")
  SET(Boost_SYSTEM_LIBRARY_RELEASE "/usr/pack/boost-1.42.0-bs/i686-debian-linux5.0/lib/libboost_system.so" CACHE PATH "")
  SET(Boost_SYSTEM_LIBRARY_DEBUG "/usr/pack/boost-1.42.0-bs/i686-debian-linux5.0/lib/libboost_system-d.so" CACHE PATH "")
  SET(Boost_DATE_TIME_LIBRARY_RELEASE "/usr/pack/boost-1.42.0-bs/i686-debian-linux5.0/lib/libboost_date_time.so" CACHE PATH "")
  SET(Boost_DATE_TIME_LIBRARY_DEBUG "/usr/pack/boost-1.42.0-bs/i686-debian-linux5.0/lib/libboost_date_time-d.so" CACHE PATH "")
  SET(Boost_FILESYSTEM_LIBRARY_RELEASE "/usr/pack/boost-1.42.0-bs/i686-debian-linux5.0/lib/libboost_filesystem.so" CACHE PATH "")
  SET(Boost_FILESYSTEM_LIBRARY_DEBUG "/usr/pack/boost-1.42.0-bs/i686-debian-linux5.0/lib/libboost_filesystem-d.so" CACHE PATH "")
  SET(Boost_THREAD_LIBRARY_RELEASE "/usr/pack/boost-1.42.0-bs/i686-debian-linux5.0/lib/libboost_thread-mt.so" CACHE PATH "")
  SET(Boost_THREAD_LIBRARY_DEBUG "/usr/pack/boost-1.42.0-bs/i686-debian-linux5.0/lib/libboost_thread-mt-d.so" CACHE PATH "")

  SET(Boost_SYSTEM_LIBRARY "debug;${Boost_SYSTEM_LIBRARY_DEBUG};optimized;${Boost_SYSTEM_LIBRARY_RELEASE}" CACHE PATH "" FORCE)
  SET(Boost_DATE_TIME_LIBRARY "debug;${Boost_DATE_TIME_LIBRARY_DEBUG};optimized;${Boost_DATE_TIME_LIBRARY_RELEASE}" CACHE PATH "" FORCE)
  SET(Boost_FILESYSTEM_LIBRARY "debug;${Boost_FILESYSTEM_LIBRARY_DEBUG};optimized;${Boost_FILESYSTEM_LIBRARY_RELEASE}" CACHE PATH "" FORCE)
  SET(Boost_THREAD_LIBRARY "debug;${Boost_THREAD_LIBRARY_DEBUG};optimized;${Boost_THREAD_LIBRARY_RELEASE}" CACHE PATH "" FORCE)

  MARK_AS_ADVANCED( Boost_INCLUDE_DIRS )
  MARK_AS_ADVANCED( Boost_SYSTEM_LIBRARY Boost_SYSTEM_LIBRARY_RELEASE Boost_SYSTEM_LIBRARY_DEBUG )
  MARK_AS_ADVANCED( Boost_DATE_TIME_LIBRARY Boost_DATE_TIME_LIBRARY_RELEASE Boost_DATE_TIME_LIBRARY_DEBUG )
  MARK_AS_ADVANCED( Boost_FILESYSTEM_LIBRARY Boost_FILESYSTEM_LIBRARY_RELEASE Boost_FILESYSTEM_LIBRARY_DEBUG )
  MARK_AS_ADVANCED( Boost_THREAD_LIBRARY Boost_THREAD_LIBRARY_RELEASE Boost_THREAD_LIBRARY_DEBUG )


  LIST(APPEND CMAKE_INCLUDE_PATH "/usr/pack/cegui-0.6.2-sd")
  LIST(APPEND CMAKE_LIBRARY_PATH "/usr/pack/cegui-0.6.2-sd/i686-debian-linux5.0")
  LIST(APPEND CMAKE_INCLUDE_PATH "/usr/pack/ogre-1.6.1-sd")
  LIST(APPEND CMAKE_LIBRARY_PATH "/usr/pack/ogre-1.6.1-sd/i686-debian-linux5.0")

  #SET(ENV{CEGUIDIR}    "/usr/pack/cegui-0.6.2-sd;/usr/pack/cegui-0.6.2-sd/i686-debian-linux5.0")
  #SET(ENV{ENETDIR}     "/usr/pack/enet-1.2-sd;/usr/pack/enet-1.2-sd/i686-debian-linux4.0")
  #SET(ENV{ALUTDIR}     "/usr/pack/openal-0.0.8-cl;/usr/pack/openal-0.0.8-cl/i686-debian-linux3.1")
  #SET(ENV{OGGDIR}      "/usr/pack/oggvorbis-1.0-ds;/usr/pack/oggvorbis-1.0-ds/i686-debian-linux3.0")
  #SET(ENV{VORBISDIR}   "/usr/pack/oggvorbis-1.0-ds;/usr/pack/oggvorbis-1.0-ds/i686-debian-linux3.0")
  #SET(ENV{LUA_DIR}     "/usr/pack/lua-5.1.4-sd;/usr/pack/lua-5.1.4-sd/i686-debian-linux4.0")
  #SET(ENV{OGRE_HOME}   "/usr/pack/ogre-1.4.9-sd;/usr/pack/ogre-1.4.9-sd/i686-debian-linux4.0")
  #SET(ENV{OPENALDIR}   "/usr/pack/openal-0.0.8-cl;/usr/pack/openal-0.0.8-cl/i686-debian-linux3.1")
  #SET(TCL_INCLUDE_PATH "/usr/pack/tcltk-8.4.9.linux-mo/include")
ENDIF(TARDIS)
