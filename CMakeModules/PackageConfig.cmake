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
 #    General package configuration. Merely sets the include paths.
 #    Library files are treated separately.
 #

# Check package version info
# MAJOR: Breaking change
# MINOR: No breaking changes by the dependency package
#        For example any code running on 3.0 should still run on 3.1
#        But you can specify that the code only runs on 3.1 and higher
#        or 4.0 and higher (so both 3.1 and 4.0 will work).
SET(ALLOWED_MINIMUM_VERSIONS 3.1 4.0 5.0)

IF(NOT EXISTS ${DEPENDENCY_PACKAGE_DIR}/version.txt)
  SET(DEPENDENCY_VERSION 1.0)
ELSE()
  # Get version from file
  FILE(READ ${DEPENDENCY_PACKAGE_DIR}/version.txt _file_content)
  SET(_match)
  STRING(REGEX MATCH "([0-9]+.[0-9]+)" _match ${_file_content})
  IF(_match)
    SET(DEPENDENCY_VERSION ${_match})
  ELSE()
    MESSAGE(FATAL_ERROR "The version.txt file in the dependency file has corrupt version information.")
  ENDIF()
ENDIF()

INCLUDE(CompareVersionStrings)
SET(_version_match FALSE)
FOREACH(_version ${ALLOWED_MINIMUM_VERSIONS})
  # Get major version
  STRING(REGEX REPLACE "^([0-9]+)\\..*$" "\\1" _major_version "${_version}")
  COMPARE_VERSION_STRINGS(${DEPENDENCY_VERSION} ${_major_version} _result TRUE)
  IF(_result EQUAL 0)
    COMPARE_VERSION_STRINGS(${DEPENDENCY_VERSION} ${_version} _result FALSE)
    IF(NOT _result LESS 0)
      SET(_version_match TRUE)
    ENDIF()
  ENDIF()
ENDFOREACH(_version)
IF(NOT _version_match)
  MESSAGE(FATAL_ERROR "Your dependency package version is ${DEPENDENCY_VERSION}\n"
          "Possible required versions: ${ALLOWED_MINIMUM_VERSIONS}\n"
          "You can get a new version from www.orxonox.net")
ENDIF()

IF(NOT _INTERNAL_PACKAGE_MESSAGE)
  MESSAGE(STATUS "Using library package for the dependencies.")
  SET(_INTERNAL_PACKAGE_MESSAGE 1 CACHE INTERNAL "Do not edit!" FORCE)
ENDIF()

# Include paths and other special treatments
SET(ENV{ALUTDIR}               ${DEP_INCLUDE_DIR}/freealut)
SET(ENV{BOOST_ROOT}            ${DEP_INCLUDE_DIR}/boost)
SET(ENV{CEGUIDIR}              ${DEP_INCLUDE_DIR}/cegui)
SET(ENV{DXSDK_DIR}             ${DEP_INCLUDE_DIR}/directx)
SET(ENV{ENETDIR}               ${DEP_INCLUDE_DIR}/enet)
SET(ENV{LUA_DIR}               ${DEP_INCLUDE_DIR}/lua)
SET(ENV{OGGDIR}                ${DEP_INCLUDE_DIR}/libogg)
SET(ENV{VORBISDIR}             ${DEP_INCLUDE_DIR}/libvorbis)
SET(ENV{OGRE_HOME}             ${DEP_INCLUDE_DIR}/ogre)
SET(ENV{OGRE_PLUGIN_DIR}       ${DEP_BINARY_DIR})
SET(ENV{OPENALDIR}             ${DEP_INCLUDE_DIR}/openal)
LIST(APPEND CMAKE_INCLUDE_PATH ${DEP_INCLUDE_DIR}/tcl/include)
LIST(APPEND CMAKE_INCLUDE_PATH ${DEP_INCLUDE_DIR}/zlib/include)

### INSTALL ###

# Tcl script library
INSTALL(
  DIRECTORY ${DEP_LIBRARY_DIR}/tcl/
  DESTINATION lib/tcl
)

# On Windows, DLLs have to be in the executable folder, install them
IF(WIN32 AND DEP_BINARY_DIR)
  ## DEBUG
  # When installing a debug version, we really can't know which libraries
  # are used in released mode because there might be deps of deps.
  # --> Copy all of them, except the debug databases
  INSTALL(
    DIRECTORY ${DEP_BINARY_DIR}/
    DESTINATION bin
    CONFIGURATIONS Debug
    REGEX "^.*\\.pdb$" EXCLUDE
  )

  ## RELEASE
  # Try to filter out all the debug libraries. If the regex doesn't do the
  # job anymore, simply adjust it.
  INSTALL(
    DIRECTORY ${DEP_BINARY_DIR}/
    DESTINATION bin
    CONFIGURATIONS Release RelWithDebInfo MinSizeRel
    REGEX "_[Dd]\\.[a-zA-Z0-9+-]+$|-mt-gd-|^.*\\.pdb$" EXCLUDE
  )
ENDIF()
