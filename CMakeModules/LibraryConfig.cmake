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
 #    Configures the external libraries. Whenever possible, the find scripts
 #    from the CMake module path are used, but that required some adjustments
 #    for certain libraries (Boost, OpenAL, TCL)
 #

INCLUDE(CompareVersionStrings)
INCLUDE(FindPackageHandleStandardArgs)

# Prevent CMake from finding libraries in the installation folder on Windows.
# There might already be an installation from another compiler
IF(WIN32)
  LIST(REMOVE_ITEM CMAKE_SYSTEM_PREFIX_PATH  "${CMAKE_INSTALL_PREFIX}")
  LIST(REMOVE_ITEM CMAKE_SYSTEM_LIBRARY_PATH "${CMAKE_INSTALL_PREFIX}/bin")
ENDIF()

############## Platform Scripts #################

# On Windows using a package causes way less problems
SET(_option_msg "Set this to true to use precompiled dependecy archives")
IF(WIN32)
  OPTION(DEPENDENCY_PACKAGE_ENABLE "${_option_msg}" ON)
ELSE(WIN32)
  OPTION(DEPENDENCY_PACKAGE_ENABLE "${_option_msg}" FALSE)
ENDIF(WIN32)

# Scripts for specific library and CMake config
INCLUDE(LibraryConfigTardis)
INCLUDE(LibraryConfigApple)

IF(DEPENDENCY_PACKAGE_ENABLE)
  GET_FILENAME_COMPONENT(_dep_dir_1 ${CMAKE_SOURCE_DIR}/../dependencies ABSOLUTE)
  GET_FILENAME_COMPONENT(_dep_dir_2 ${CMAKE_SOURCE_DIR}/../lib_dist/dependencies ABSOLUTE)
  FIND_PATH(DEPENDENCY_PACKAGE_DIR
    NAMES include
    PATHS
      ${CMAKE_SOURCE_DIR}/dependencies
      ${_dep_dir_1}
      ${_dep_dir_2}
    NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH
  )
  IF(NOT DEPENDENCY_PACKAGE_DIR)
    MESSAGE(STATUS "Warning: Could not find dependency directory."
                   "Disable LIBRARY_USE_PACKAGE if you have none intalled.")
  ELSE()
    INCLUDE(PackageConfigMinGW)
    INCLUDE(PackageConfigMSVC)
    INCLUDE(PackageConfig) # For both msvc and mingw
  ENDIF()
ENDIF(DEPENDENCY_PACKAGE_ENABLE)

# User script
SET(LIBRARY_CONFIG_USER_SCRIPT "" CACHE FILEPATH
    "Specify a CMake script if you wish to write your own library path config.
     See LibraryConfigTardis.cmake for an example.")
IF(LIBRARY_CONFIG_USER_SCRIPT)
  IF(EXISTS ${CMAKE_MODULE_PATH}/${LIBRARY_CONFIG_USER_SCRIPT})
    INCLUDE(${CMAKE_MODULE_PATH}/${LIBRARY_CONFIG_USER_SCRIPT})
  ENDIF()
ENDIF(LIBRARY_CONFIG_USER_SCRIPT)


############### Library finding #################
# Performs the search and sets the variables    #

FIND_PACKAGE(OGRE  1.4       REQUIRED)
FIND_PACKAGE(ENet  1.1       REQUIRED)
FIND_PACKAGE(Ogg             REQUIRED)
FIND_PACKAGE(Vorbis          REQUIRED)
FIND_PACKAGE(ALUT            REQUIRED)
FIND_PACKAGE(ZLIB            REQUIRED)
IF(WIN32)
  FIND_PACKAGE(DirectX       REQUIRED)
ENDIF(WIN32)

##### CEGUI #####
# We make use of the CEGUI script module called CEGUILua.
# However there is a small issue with that: We use Tolua, a C++ binding
# generator ourselves. And we also have to use our bindings in the same
# lua state is CEGUILua's. Unfortunately this implies that both lua runtime
# version are equal or else you get segmentation faults.
# In order to match the Lua versions we decided to ship CEGUILua in our
# repository, mainly because there is no way to determine which version of
# Lua CEGUILua was linked against (you'd have to specify yourself) and secondly
# because we can then choose the Lua version. Future plans might involve only
# accepting Lua 5.1.

# Insert all internally supported CEGUILua versions here
SET(CEGUILUA_INTERNAL_SUPPORT 0.5.0 0.6.0 0.6.1 0.6.2)
OPTION(CEGUILUA_USE_EXTERNAL_LIBRARY "Force the use of external CEGUILua library" OFF)
FIND_PACKAGE(CEGUI 0.5 REQUIRED)

##### Lua #####
IF(CEGUILUA_USE_EXTERNAL_LIBRARY)
  COMPARE_VERSION_STRINGS(${CEGUI_VERSION} "0.6" _version_comparison)
  IF(version_comparison LESS 0)
    SET(LUA_VERSION_REQUEST 5.0)
  ELSE()
    SET(LUA_VERSION_REQUEST 5.1)
  ENDIF()
ELSE()
  SET(LUA_VERSION_REQUEST 5)
ENDIF()
FIND_PACKAGE(Lua ${LUA_VERSION_REQUEST} EXACT REQUIRED)

##### OpenAL #####
FIND_PACKAGE(OpenAL REQUIRED)
# Also use parent include dir (without AL/) for ALUT
IF(OPENAL_INCLUDE_DIR MATCHES "/AL$")
  GET_FILENAME_COMPONENT(ALT_OPENAL_INCLUDE_DIR ${OPENAL_INCLUDE_DIR} PATH)
ENDIF()
SET(OPENAL_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR} ${ALT_OPENAL_INCLUDE_DIR})
# Notfiy user
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenAL DEFAULT_MSG OPENAL_LIBRARY OPENAL_INCLUDE_DIR)
# Hide variables created by the script
MARK_AS_ADVANCED(OPENAL_INCLUDE_DIR OPENAL_LIBRARY)

##### Tcl #####
# We only require Tcl, so avoid confusing user about other Tcl stuff by
# applying a little workaround
SET(Tclsh_FIND_QUIETLY TRUE)
FIND_PACKAGE(TCL QUIET)
# Display messages separately
SET(TCL_FIND_QUIETLY FALSE)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TCL DEFAULT_MSG TCL_LIBRARY TCL_INCLUDE_PATH)

##### Boost #####
# Expand the next statement if newer boost versions than 1.36.1 are released
SET(Boost_ADDITIONAL_VERSIONS 1.37 1.37.0 1.38 1.38.0 1.39 1.39.0 1.40 1.40.0)
IF( NOT TARDIS )
  FIND_PACKAGE(Boost 1.35 REQUIRED thread filesystem system date_time)
ENDIF()
# No auto linking, so this option is useless anyway
MARK_AS_ADVANCED(Boost_LIB_DIAGNOSTIC_DEFINITIONS)


####### Static/Dynamic linking options ##########

# On Windows dynamically linked libraries need some special treatment
# You may want to edit these settings if you provide your own libraries
# Note: Default option in the libraries vary, but our default option is dynamic
IF(WIN32)
  OPTION(LINK_BOOST_DYNAMIC "Link Boost dynamically on Windows" TRUE)
  OPTION(LINK_CEGUI_DYNAMIC "Link CEGUI dynamicylly on Windows" TRUE)
  OPTION(LINK_ENET_DYNAMIC  "Link ENet dynamically on Windows" TRUE)
  OPTION(LINK_OGRE_DYNAMIC  "Link OGRE dynamically on Windows" TRUE)
  OPTION(LINK_TCL_DYNAMIC   "Link TCL dynamically on Windows" TRUE)
  OPTION(LINK_ZLIB_DYNAMIC  "Link ZLib dynamically on Windows" TRUE)
  COMPARE_VERSION_STRINGS("${LUA_VERSION}" "5.1" _version_comparison)
  IF(_version_comparison LESS 0)
    OPTION(LINK_LUA_DYNAMIC "Link Lua dynamically on Windows" FALSE)
  ELSE(_version_comparison LESS 0)
    OPTION(LINK_LUA_DYNAMIC "Link Lua dynamically on Windows" TRUE)
  ENDIF(_version_comparison LESS 0)

  IF(DEPENDENCY_PACKAGE_ENABLE)
    MARK_AS_ADVANCED(
      LINK_BOOST_DYNAMIC LINK_CEGUI_DYNAMIC LINK_ENET_DYNAMIC
      LINK_OGRE_DYNAMIC  LINK_TCL_DYNAMIC   LINK_ZLIB_DYNAMIC
      LINK_LUA_DYNAMIC
    )
  ENDIF()
ENDIF(WIN32)


################# OGRE Plugins ##################

# More plugins: Plugin_BSPSceneManager, Plugin_OctreeSceneManager
SET(OGRE_PLUGINS_INT Plugin_ParticleFX)
IF(WIN32)
  # CG program manager is probably DirectX related (not available under unix)
  LIST(APPEND OGRE_PLUGINS_INT Plugin_CgProgramManager)
ENDIF(WIN32)
SET(OGRE_PLUGINS ${OGRE_PLUGINS_INT} CACHE STRING
   "Specify which OGRE plugins to load. Existance check is performed.")

# Check the plugins and determine the plugin folder
# You can give a hint by setting the environment variable ENV{OGRE_PLUGIN_DIR}
INCLUDE(CheckOGREPlugins)
CHECK_OGRE_PLUGINS(${OGRE_PLUGINS})

