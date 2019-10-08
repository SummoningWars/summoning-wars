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
 #    Function that checks each OGRE plugin for existance. Also looks for debug
 #    versions and sets them accordingly.
 #    All the plugins specified as function arguments have to be found or the
 #    script will issue a fatal error. Additionally, all release plugins have
 #    to be found in the same folder. Analogously for debug plugins.
 #    Output:
 #    OGRE_PLUGINS_FOLDER_DEBUG   Folder with the debug plugins
 #    OGRE_PLUGINS_FOLDER_RELEASE Folder with the release plugins
 #    OGRE_PLUGINS_DEBUG          Names of the debug plugins without extension
 #    OGRE_PLUGINS_RELEASE        Names of the release plugins without ext.
 #  Note:
 #    You must not specify render systems as input. That will be taken care of
 #    automatically.
 #

FUNCTION(CHECK_OGRE_PLUGINS)

  SET(OGRE_PLUGINS ${ARGN})

  IF(WIN32)
    # On Windows we need only *.dll, not *.lib. Especially the MSVC generator doesn't look for *.dll
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .dll)
  ENDIF(WIN32)
  # Do not prefix "lib" on any platform
  SET(CMAKE_FIND_LIBRARY_PREFIXES "")

  SET(OGRE_RENDER_SYSTEMS RenderSystem_GL RenderSystem_Direct3D9)
  SET(OGRE_RENDER_SYSTEM_FOUND FALSE)
  FOREACH(_plugin ${OGRE_PLUGINS} ${OGRE_RENDER_SYSTEMS})
    FIND_LIBRARY(OGRE_PLUGIN_${_plugin}_OPTIMIZED
      NAMES ${_plugin}
      PATHS $ENV{OGRE_HOME} $ENV{OGRE_PLUGIN_DIR}
      PATH_SUFFIXES bin/Release bin/release Release release lib lib/OGRE bin
    )
    FIND_LIBRARY(OGRE_PLUGIN_${_plugin}_DEBUG
      NAMES ${_plugin}d ${_plugin}_d ${_plugin}
      PATHS $ENV{OGRE_HOME} $ENV{OGRE_PLUGIN_DIR}
      PATH_SUFFIXES bin/Debug bin/debug Debug debug lib lib/OGRE bin
    )
    # We only need at least one render system. Check at the end.
    IF(NOT ${_plugin} MATCHES "RenderSystem")
      IF(NOT OGRE_PLUGIN_${_plugin}_OPTIMIZED)
        MESSAGE(FATAL_ERROR "Could not find OGRE plugin named ${_plugin}")
      ENDIF()
    ELSEIF(OGRE_PLUGIN_${_plugin}_OPTIMIZED)
      SET(OGRE_RENDER_SYSTEM_FOUND TRUE)
    ENDIF()

    IF(OGRE_PLUGIN_${_plugin}_OPTIMIZED)
      # If debug version is not available, release will do as well
      IF(OGRE_PLUGIN_${_plugin}_DEBUG STREQUAL OGRE_PLUGIN_${_plugin}_OPTIMIZED)
        # In this case the library finder didn't find real debug versions
        SET(OGRE_PLUGIN_${_plugin}_DEBUG "OGRE_PLUGIN_${_plugin}_DEBUG-NOTFOUND" CACHE STRING "" FORCE)
      ENDIF()
      MARK_AS_ADVANCED(OGRE_PLUGIN_${_plugin}_OPTIMIZED OGRE_PLUGIN_${_plugin}_DEBUG)

      ### Set variables to configure orxonox.ini correctly afterwards in bin/ ###
      # Check and set the folders
      GET_FILENAME_COMPONENT(_plugins_folder ${OGRE_PLUGIN_${_plugin}_OPTIMIZED} PATH)
      IF(OGRE_PLUGINS_FOLDER_RELEASE AND NOT OGRE_PLUGINS_FOLDER_RELEASE STREQUAL _plugins_folder)
        MESSAGE(FATAL_ERROR "Ogre release plugins have to be in the same folder!")
      ENDIF()
      SET(OGRE_PLUGINS_FOLDER_RELEASE ${_plugins_folder})
      IF(OGRE_PLUGIN_${_plugin}_DEBUG)
        GET_FILENAME_COMPONENT(_plugins_folder ${OGRE_PLUGIN_${_plugin}_DEBUG} PATH)
      ENDIF()
      IF(OGRE_PLUGINS_FOLDER_DEBUG AND NOT OGRE_PLUGINS_FOLDER_DEBUG STREQUAL _plugins_folder)
        MESSAGE(FATAL_ERROR "Ogre debug plugins have to be in the same folder!")
      ENDIF()
      SET(OGRE_PLUGINS_FOLDER_DEBUG ${_plugins_folder})

      # Create a list with the plugins for release and debug configurations
      LIST(APPEND OGRE_PLUGINS_RELEASE ${_plugin})
      # Determine debug postfix ("d" or "_d" or none)
      IF(OGRE_PLUGIN_${_plugin}_DEBUG MATCHES "_d\\.|_d$")
        LIST(APPEND OGRE_PLUGINS_DEBUG "${_plugin}_d")
      ELSEIF(OGRE_PLUGIN_${_plugin}_DEBUG MATCHES "d\\.|d$")
        LIST(APPEND OGRE_PLUGINS_DEBUG "${_plugin}d")
      ELSE()
        LIST(APPEND OGRE_PLUGINS_DEBUG "${_plugin}")
      ENDIF()
    ENDIF(OGRE_PLUGIN_${_plugin}_OPTIMIZED)
  ENDFOREACH(_plugin)
  IF(NOT OGRE_RENDER_SYSTEM_FOUND)
      MESSAGE(FATAL_ERROR "Could not find an OGRE render system plugin")
  ENDIF()

  # List has to be comma separated for orxonox.ini
  STRING(REPLACE ";" ", " OGRE_PLUGINS_RELEASE "${OGRE_PLUGINS_RELEASE}")
  STRING(REPLACE ";" ", " OGRE_PLUGINS_DEBUG   "${OGRE_PLUGINS_DEBUG}")

  # Set variables outside function scope
  SET(OGRE_PLUGINS_FOLDER_DEBUG ${OGRE_PLUGINS_FOLDER_DEBUG} PARENT_SCOPE)
  SET(OGRE_PLUGINS_FOLDER_RELEASE ${OGRE_PLUGINS_FOLDER_RELEASE} PARENT_SCOPE)
  SET(OGRE_PLUGINS_RELEASE ${OGRE_PLUGINS_RELEASE} PARENT_SCOPE)
  SET(OGRE_PLUGINS_DEBUG ${OGRE_PLUGINS_DEBUG} PARENT_SCOPE)

ENDFUNCTION(CHECK_OGRE_PLUGINS)
