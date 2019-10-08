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
 #    Reto Grieder
 #  Description:
 #    Calls a compiler specific script and sets some options.
 #

OPTION(EXTRA_COMPILER_WARNINGS "Enable some extra warnings (heavily pollutes the output)" FALSE)

# Configure the compiler specific build options
IF(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
  INCLUDE(CompilerConfigGCC)
ELSEIF(MSVC)
  INCLUDE(CompilerConfigMSVC)
ELSE()
  MESSAGE(STATUS "Warning: Your compiler is not officially supported.")
ENDIF()

SET(COMPILER_CONFIG_USER_SCRIPT "" CACHE FILEPATH
    "Specify a CMake script if you wish to write your own compiler config.
     See CompilerConfigGCC.cmake or CompilerConfigMSVC.cmake for examples.")
IF(COMPILER_CONFIG_USER_SCRIPT)
  IF(EXISTS ${CMAKE_MODULE_PATH}/${COMPILER_CONFIG_USER_SCRIPT})
    INCLUDE(${CMAKE_MODULE_PATH}/${COMPILER_CONFIG_USER_SCRIPT})
  ENDIF()
ENDIF(COMPILER_CONFIG_USER_SCRIPT)
