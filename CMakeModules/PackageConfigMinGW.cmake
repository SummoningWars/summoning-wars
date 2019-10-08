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
 #    Sets the library directories when using precompiled dependency archives
 #  Note:
 #    When setting ENV${} variables, make sure to use quotes for lists
 #

IF(MINGW)

  # 64 bit system?
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(BINARY_POSTFIX x64)
  ELSE()
    SET(BINARY_POSTFIX x86)
  ENDIF()

  SET(DEP_INCLUDE_DIR ${DEPENDENCY_PACKAGE_DIR}/include)
  SET(DEP_LIBRARY_DIR ${DEPENDENCY_PACKAGE_DIR}/lib/mingw-${BINARY_POSTFIX})
  SET(DEP_BINARY_DIR  ${DEPENDENCY_PACKAGE_DIR}/bin/mingw-${BINARY_POSTFIX})

  # Also the directory with the runtime libraries
  SET(RUNTIME_LIBRARY_DIRECTORY ${DEP_BINARY_DIR})

  # Sets the library path for the FIND_LIBRARY
  SET(CMAKE_LIBRARY_PATH ${DEP_LIBRARY_DIR} ${DEP_BINARY_DIR})

  # Certain find scripts don't behave as ecpected to we have
  # to specify the libraries ourselves.
  SET(TCL_LIBRARY  ${DEP_BINARY_DIR}/tcl85.dll CACHE FILEPATH "")
  SET(ZLIB_LIBRARY ${DEP_BINARY_DIR}/zlib1.dll CACHE FILEPATH "")

ENDIF(MINGW)
